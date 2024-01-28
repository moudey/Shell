#pragma once
/*
 * EAT-based hooking for x86/x64.
 *
 * Big thanks to ez (https://github.com/ezdiy/) for making this!
 *
 * Creates "hooks" by modifying the module's export address table.
 * The procedure works in three main parts:
 *
 * 1. Reading the module's PE file and getting all exported functions.
 * 2. Finding the right function to "hook" by simple address lookup
 * 3. Modify the entry to point to the hook.
 *
 * The idea is based on the fact that the export table allows forwarding imports:
 *
 * https://en.wikibooks.org/wiki/X86_Disassembly/Windows_Executable_Files#Forwarding
 *
 * You can also find some reference material on the same page to understand this code better.
 *
 */

#include <windows.h>

 // PE format uses RVAs (Relative Virtual Addresses) to save addresses relative to the base of the module
 // More info: https://en.wikibooks.org/wiki/X86_Disassembly/Windows_Executable_Files#Relative_Virtual_Addressing_(RVA)
 //
 // This helper macro converts the saved RVA to a fully valid pointer to the data in the PE file
#define RVA2PTR(t,base,rva) ((t)(((PCHAR) base) + rva))

// A helper function to write into protected memory
inline int vpmemcpy(void *dst, void *src, size_t sz)
{
	DWORD oldp;
	// Make the memory page writeable
	if(!::VirtualProtect(dst, sz, PAGE_READWRITE, &oldp))
		return 1;
	::memcpy(dst, src, sz);
	// Restore the protection level
	::VirtualProtect(dst, sz, oldp, &oldp);
	return 0;
}


/**
 * \brief Replaces the specified function entry in the EAT with a forward to a custom one, thus creating the "hook" effect.
 * \param hostDll The address of the module to hook.
 * \param originalFunction Address of the original function.
 * \param forwardFunctionEntry Name of the function to add a forward to. Must be of form `dll.API`.
 * \return TRUE, if hooking succeeded, otherwise, FALSE.
 */
inline BOOL ezHook(HMODULE hostDll, void *originalFunction, char *forwardFunctionEntry)
{
	/*
	 * Note that we are not doing any trampoline magic or editing the assembly!
	 *
	 * Instead, we are reading the module's PE file, find the original function's entry in the export address table (EAT),
	 * and replace it with a forward import.
	 *
	 * This ultimately will fool the game/executable to call our hook, while keeping original function intact.
	 *
	 * Thus, in order to work, the proxy DLL has to export the hook, because we are essentially
	 * asking the game to call our hook without ever going to the original function (unlike with trampolines).
	 */

	size_t fwdlen = strlen(forwardFunctionEntry);

	// The module always starts with a DOS (or "MZ") header
	IMAGE_DOS_HEADER *mz = (PIMAGE_DOS_HEADER)hostDll;

	// Next, get the NT headers. The offset to them is saved in e_lfanew
	IMAGE_NT_HEADERS *nt = RVA2PTR(PIMAGE_NT_HEADERS, mz, mz->e_lfanew);

	// Get the pointer to the data directory of the exports
	IMAGE_DATA_DIRECTORY *edirp = &nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
	IMAGE_DATA_DIRECTORY edir = *edirp;

	// Finally, the virtual address in the data direcotry tells the location of the exports table
	IMAGE_EXPORT_DIRECTORY *exports = RVA2PTR(PIMAGE_EXPORT_DIRECTORY, mz, edir.VirtualAddress);

	// Read the addrress of the function list and the address of function names
	DWORD *addrs = RVA2PTR(DWORD *, mz, exports->AddressOfFunctions);
	//	DWORD* names = RVA2PTR(DWORD*, mz, exports->AddressOfNames);

	// Iterate through all functions
	for(unsigned i = 0; i < exports->NumberOfFunctions; i++)
	{
		//char* name = RVA2PTR(char*, mz, names[i]); // Name of the exported function
		void *addr = RVA2PTR(void *, mz, addrs[i]); // Address of the exported function

		// Check if we have the function we need to modify
		if(addr == originalFunction)
		{
			DWORD fptr = edir.VirtualAddress + edir.Size;
			int err = 0;

			// Update the entry to go the the last entry (which we will populate in the next memcpy)
			err |= vpmemcpy(&addrs[i], &fptr, sizeof(fptr));

			// Add the forwarding import to our function at the end of the EAT
			err |= vpmemcpy(((char *)exports + edir.Size), forwardFunctionEntry, fwdlen);

			// Increment the size of the export data directory
			// and write the new export data directory
			edir.Size += fwdlen + 1;
			err |= vpmemcpy(edirp, &edir, sizeof(edir));
			return err == 0;
		}
	}
	return FALSE;
}

/**
 * \brief Hooks the given function through the Import Address Table.
		  This is a simplified version that doesn't does lookup directly in the initialized IAT.
		  This is usable to hook system DLLs like kernel32.dll assuming the process wasn't already hooked.
 * \param dll Module to hook
 * \param targetFunction Address of the target function to hook
 * \param detourFunction Address of the detour function
 * \return TRUE if successful, otherwise FALSE
 */
inline BOOL iat_hook(HMODULE dll, char const *targetDLL, void *targetFunction, void *detourFunction)
{
	IMAGE_DOS_HEADER *mz = (PIMAGE_DOS_HEADER)dll;
	IMAGE_NT_HEADERS *nt = RVA2PTR(PIMAGE_NT_HEADERS, mz, mz->e_lfanew);
	IMAGE_IMPORT_DESCRIPTOR *imports = RVA2PTR(PIMAGE_IMPORT_DESCRIPTOR, 
											   mz, 
											   nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
	for(int i = 0; imports[i].Characteristics; i++)
	{
		char *name = RVA2PTR(char *, mz, imports[i].Name);

		if(lstrcmpiA(name, targetDLL) != 0)
			continue;

		void **thunk = RVA2PTR(void **, mz, imports[i].FirstThunk);

		for(; thunk; thunk++)
		{
			void *import = *thunk;

			if(import != targetFunction)
				continue;

			DWORD oldState;
			if(!::VirtualProtect(thunk, sizeof(void *), PAGE_READWRITE, &oldState))
				return FALSE;

			*thunk = (void *)detourFunction;

			::VirtualProtect(thunk, sizeof(void *), oldState, &oldState);

			return TRUE;
		}
	}

	return FALSE;
}
