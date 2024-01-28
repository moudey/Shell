/////////////////////////////////////////////////////////////////////////////
//
//  Core Detours Functionality (detours.h of detours.lib)
//
//  Microsoft Research Detours Package, Version 4.0.1
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//

#pragma once
#ifndef _DETOURS_H_
#define _DETOURS_H_

#define DETOURS_VERSION     0x4c0c1   // 0xMAJORcMINORcPATCH

//////////////////////////////////////////////////////////////////////////////
//

#ifdef DETOURS_INTERNAL

#define _CRT_STDIO_ARBITRARY_WIDE_SPECIFIERS 1
#define _ARM_WINAPI_PARTITION_DESKTOP_SDK_AVAILABLE 1

#pragma warning(disable:4068) // unknown pragma (suppress)

#if _MSC_VER >= 1900
#pragma warning(push)
#pragma warning(disable:4091) // empty typedef
#endif

// Suppress declspec(dllimport) for the sake of Detours
// users that provide kernel32 functionality themselves.
// This is ok in the mainstream case, it will just cost
// an extra instruction calling some functions, which
// LTCG optimizes away.
//
#define _KERNEL32_ 1
#define _USER32_ 1

#include <windows.h>
#if (_MSC_VER < 1310)
#else
#pragma warning(push)
#if _MSC_VER > 1400
#pragma warning(disable:6102 6103) // /analyze warnings
#endif
#include <strsafe.h>
#include <intsafe.h>
#pragma warning(pop)
#endif
#include <crtdbg.h>

// Allow Detours to cleanly compile with the MingW toolchain.
//
#ifdef __GNUC__
#define __try
#define __except(x) if (0)
#include <strsafe.h>
#include <intsafe.h>
#endif

// From winerror.h, as this error isn't found in some SDKs:
//
// MessageId: ERROR_DYNAMIC_CODE_BLOCKED
//
// MessageText:
//
// The operation was blocked as the process prohibits dynamic code generation.
//
#define ERROR_DYNAMIC_CODE_BLOCKED       1655L

#endif // DETOURS_INTERNAL

//////////////////////////////////////////////////////////////////////////////
//

#undef DETOURS_X64
#undef DETOURS_X86
#undef DETOURS_IA64
#undef DETOURS_ARM
#undef DETOURS_ARM64
#undef DETOURS_BITS
#undef DETOURS_32BIT
#undef DETOURS_64BIT

#if defined(_X86_)
#define DETOURS_X86
#define DETOURS_OPTION_BITS 64

#elif defined(_AMD64_)
#define DETOURS_X64
#define DETOURS_OPTION_BITS 32

#elif defined(_IA64_)
#define DETOURS_IA64
#define DETOURS_OPTION_BITS 32

#elif defined(_ARM_)
#define DETOURS_ARM

#elif defined(_ARM64_)
#define DETOURS_ARM64

#else
#error Unknown architecture (x86, amd64, ia64, arm, arm64)
#endif

#ifdef _WIN64
#undef DETOURS_32BIT
#define DETOURS_64BIT 1
#define DETOURS_BITS 64
// If all 64bit kernels can run one and only one 32bit architecture.
//#define DETOURS_OPTION_BITS 32
#else
#define DETOURS_32BIT 1
#undef DETOURS_64BIT
#define DETOURS_BITS 32
// If all 64bit kernels can run one and only one 32bit architecture.
//#define DETOURS_OPTION_BITS 32
#endif

//////////////////////////////////////////////////////////////////////////////
//

#if (_MSC_VER < 1299) && !defined(__MINGW32__)
typedef LONG LONG_PTR;
typedef ULONG ULONG_PTR;
#endif

///////////////////////////////////////////////// SAL 2.0 Annotations w/o SAL.
//
//  These definitions are include so that Detours will build even if the
//  compiler doesn't have full SAL 2.0 support.
//
#ifndef DETOURS_DONT_REMOVE_SAL_20

#ifdef DETOURS_TEST_REMOVE_SAL_20
#undef _Analysis_assume_
#undef _Benign_race_begin_
#undef _Benign_race_end_
#undef _Field_range_
#undef _Field_size_
#undef _In_
#undef _In_bytecount_
#undef _In_count_
#undef __in_ecount
#undef _In_opt_
#undef _In_opt_bytecount_
#undef _In_opt_count_
#undef _In_opt_z_
#undef _In_range_
#undef _In_reads_
#undef _In_reads_bytes_
#undef _In_reads_opt_
#undef _In_reads_opt_bytes_
#undef _In_reads_or_z_
#undef _In_z_
#undef _Inout_
#undef _Inout_opt_
#undef _Inout_z_count_
#undef _Out_
#undef _Out_opt_
#undef _Out_writes_
#undef _Outptr_result_maybenull_
#undef _Readable_bytes_
#undef _Success_
#undef _Writable_bytes_
#undef _Pre_notnull_
#endif

#if defined(_Deref_out_opt_z_) && !defined(_Outptr_result_maybenull_)
#define _Outptr_result_maybenull_ _Deref_out_opt_z_
#endif

#if defined(_In_count_) && !defined(_In_reads_)
#define _In_reads_(x) _In_count_(x)
#endif

#if defined(_In_opt_count_) && !defined(_In_reads_opt_)
#define _In_reads_opt_(x) _In_opt_count_(x)
#endif

#if defined(_In_opt_bytecount_) && !defined(_In_reads_opt_bytes_)
#define _In_reads_opt_bytes_(x) _In_opt_bytecount_(x)
#endif

#if defined(_In_bytecount_) && !defined(_In_reads_bytes_)
#define _In_reads_bytes_(x) _In_bytecount_(x)
#endif

#ifndef _In_
#define _In_
#endif

#ifndef _In_bytecount_
#define _In_bytecount_(x)
#endif

#ifndef _In_count_
#define _In_count_(x)
#endif

#ifndef __in_ecount
#define __in_ecount(x)
#endif

#ifndef _In_opt_
#define _In_opt_
#endif

#ifndef _In_opt_bytecount_
#define _In_opt_bytecount_(x)
#endif

#ifndef _In_opt_count_
#define _In_opt_count_(x)
#endif

#ifndef _In_opt_z_
#define _In_opt_z_
#endif

#ifndef _In_range_
#define _In_range_(x,y)
#endif

#ifndef _In_reads_
#define _In_reads_(x)
#endif

#ifndef _In_reads_bytes_
#define _In_reads_bytes_(x)
#endif

#ifndef _In_reads_opt_
#define _In_reads_opt_(x)
#endif

#ifndef _In_reads_opt_bytes_
#define _In_reads_opt_bytes_(x)
#endif

#ifndef _In_reads_or_z_
#define _In_reads_or_z_
#endif

#ifndef _In_z_
#define _In_z_
#endif

#ifndef _Inout_
#define _Inout_
#endif

#ifndef _Inout_opt_
#define _Inout_opt_
#endif

#ifndef _Inout_z_count_
#define _Inout_z_count_(x)
#endif

#ifndef _Out_
#define _Out_
#endif

#ifndef _Out_opt_
#define _Out_opt_
#endif

#ifndef _Out_writes_
#define _Out_writes_(x)
#endif

#ifndef _Outptr_result_maybenull_
#define _Outptr_result_maybenull_
#endif

#ifndef _Writable_bytes_
#define _Writable_bytes_(x)
#endif

#ifndef _Readable_bytes_
#define _Readable_bytes_(x)
#endif

#ifndef _Success_
#define _Success_(x)
#endif

#ifndef _Pre_notnull_
#define _Pre_notnull_
#endif

#ifdef DETOURS_INTERNAL

#pragma warning(disable:4615) // unknown warning type (suppress with older compilers)

#ifndef _Benign_race_begin_
#define _Benign_race_begin_
#endif

#ifndef _Benign_race_end_
#define _Benign_race_end_
#endif

#ifndef _Field_size_
#define _Field_size_(x)
#endif

#ifndef _Field_range_
#define _Field_range_(x,y)
#endif

#ifndef _Analysis_assume_
#define _Analysis_assume_(x)
#endif

#endif // DETOURS_INTERNAL
#endif // DETOURS_DONT_REMOVE_SAL_20

//////////////////////////////////////////////////////////////////////////////
//
#ifndef GUID_DEFINED
#define GUID_DEFINED
typedef struct  _GUID
{
    DWORD Data1;
    WORD Data2;
    WORD Data3;
    BYTE Data4[ 8 ];
} GUID;

#ifdef INITGUID
#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        const GUID name \
                = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }
#else
#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    const GUID name
#endif // INITGUID
#endif // !GUID_DEFINED

#if defined(__cplusplus)
#ifndef _REFGUID_DEFINED
#define _REFGUID_DEFINED
#define REFGUID             const GUID &
#endif // !_REFGUID_DEFINED
#else // !__cplusplus
#ifndef _REFGUID_DEFINED
#define _REFGUID_DEFINED
#define REFGUID             const GUID * const
#endif // !_REFGUID_DEFINED
#endif // !__cplusplus

#ifndef ARRAYSIZE
#define ARRAYSIZE(x)    (sizeof(x)/sizeof(x[0]))
#endif

//
//////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/////////////////////////////////////////////////// Instruction Target Macros.
//
#define DETOUR_INSTRUCTION_TARGET_NONE          ((PVOID)0)
#define DETOUR_INSTRUCTION_TARGET_DYNAMIC       ((PVOID)(LONG_PTR)-1)
#define DETOUR_SECTION_HEADER_SIGNATURE         0x00727444   // "Dtr\0"

extern const GUID DETOUR_EXE_RESTORE_GUID;
extern const GUID DETOUR_EXE_HELPER_GUID;

#define DETOUR_TRAMPOLINE_SIGNATURE             0x21727444  // Dtr!
typedef struct _DETOUR_TRAMPOLINE DETOUR_TRAMPOLINE, *PDETOUR_TRAMPOLINE;

#ifndef DETOUR_MAX_SUPPORTED_IMAGE_SECTION_HEADERS
#define DETOUR_MAX_SUPPORTED_IMAGE_SECTION_HEADERS      32
#endif // !DETOUR_MAX_SUPPORTED_IMAGE_SECTION_HEADERS
/*
///////////////////////////////////////////////////////////// Binary Typedefs.
//
typedef BOOL (CALLBACK *PF_DETOUR_BINARY_BYWAY_CALLBACK)(
    _In_opt_ PVOID pContext,
    _In_opt_ LPCSTR pszFile,
    _Outptr_result_maybenull_ LPCSTR *ppszOutFile);

typedef BOOL (CALLBACK *PF_DETOUR_BINARY_FILE_CALLBACK)(
    _In_opt_ PVOID pContext,
    _In_ LPCSTR pszOrigFile,
    _In_ LPCSTR pszFile,
    _Outptr_result_maybenull_ LPCSTR *ppszOutFile);

typedef BOOL (CALLBACK *PF_DETOUR_BINARY_SYMBOL_CALLBACK)(
    _In_opt_ PVOID pContext,
    _In_ ULONG nOrigOrdinal,
    _In_ ULONG nOrdinal,
    _Out_ ULONG *pnOutOrdinal,
    _In_opt_ LPCSTR pszOrigSymbol,
    _In_opt_ LPCSTR pszSymbol,
    _Outptr_result_maybenull_ LPCSTR *ppszOutSymbol);

typedef BOOL (CALLBACK *PF_DETOUR_BINARY_COMMIT_CALLBACK)(
    _In_opt_ PVOID pContext);

typedef BOOL (CALLBACK *PF_DETOUR_ENUMERATE_EXPORT_CALLBACK)(_In_opt_ PVOID pContext,
                                                             _In_ ULONG nOrdinal,
                                                             _In_opt_ LPCSTR pszName,
                                                             _In_opt_ PVOID pCode);

typedef BOOL (CALLBACK *PF_DETOUR_IMPORT_FILE_CALLBACK)(_In_opt_ PVOID pContext,
                                                        _In_opt_ HMODULE hModule,
                                                        _In_opt_ LPCSTR pszFile);

typedef BOOL (CALLBACK *PF_DETOUR_IMPORT_FUNC_CALLBACK)(_In_opt_ PVOID pContext,
                                                        _In_ DWORD nOrdinal,
                                                        _In_opt_ LPCSTR pszFunc,
                                                        _In_opt_ PVOID pvFunc);

// Same as PF_DETOUR_IMPORT_FUNC_CALLBACK but extra indirection on last parameter.
typedef BOOL (CALLBACK *PF_DETOUR_IMPORT_FUNC_CALLBACK_EX)(_In_opt_ PVOID pContext,
                                                           _In_ DWORD nOrdinal,
                                                           _In_opt_ LPCSTR pszFunc,
                                                           _In_opt_ PVOID* ppvFunc);
*/
typedef VOID * PDETOUR_BINARY;
typedef VOID * PDETOUR_LOADED_BINARY;

//////////////////////////////////////////////////////////// Transaction APIs.
//
LONG WINAPI DetourTransactionBegin(VOID);
LONG WINAPI DetourTransactionAbort(VOID);
LONG WINAPI DetourTransactionCommit(VOID);
LONG WINAPI DetourTransactionCommitEx(_Out_opt_ PVOID **pppFailedPointer);

LONG WINAPI DetourUpdateThread(_In_ HANDLE hThread);

LONG WINAPI DetourAttach(_Inout_ PVOID *ppPointer,
                         _In_ PVOID pDetour);

LONG WINAPI DetourAttachEx(_Inout_ PVOID *ppPointer,
                           _In_ PVOID pDetour,
                           _Out_opt_ PDETOUR_TRAMPOLINE *ppRealTrampoline,
                           _Out_opt_ PVOID *ppRealTarget,
                           _Out_opt_ PVOID *ppRealDetour);

LONG WINAPI DetourDetach(_Inout_ PVOID *ppPointer,
                         _In_ PVOID pDetour);

BOOL WINAPI DetourSetIgnoreTooSmall(_In_ BOOL fIgnore);
BOOL WINAPI DetourSetRetainRegions(_In_ BOOL fRetain);
PVOID WINAPI DetourSetSystemRegionLowerBound(_In_ PVOID pSystemRegionLowerBound);
PVOID WINAPI DetourSetSystemRegionUpperBound(_In_ PVOID pSystemRegionUpperBound);

////////////////////////////////////////////////////////////// Code Functions.
//
PVOID WINAPI DetourFindFunction(_In_ LPCWSTR pszModule,
                                _In_ LPCSTR pszFunction);
PVOID WINAPI DetourCodeFromPointer(_In_ PVOID pPointer,
                                   _Out_opt_ PVOID *ppGlobals);
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
PVOID WINAPI DetourCopyInstruction(_In_opt_ PVOID pDst,
                                   _Inout_opt_ PVOID *ppDstPool,
                                   _In_ PVOID pSrc,
                                   _Out_opt_ PVOID *ppTarget,
                                   _Out_opt_ LONG *plExtra);
BOOL WINAPI DetourSetCodeModule(_In_ HMODULE hModule,
                                _In_ BOOL fLimitReferencesToModule);
PVOID WINAPI DetourAllocateRegionWithinJumpBounds(_In_ LPCVOID pbTarget,
                                                  _Out_ PDWORD pcbAllocatedSize);
BOOL WINAPI DetourIsFunctionImported(_In_ PBYTE pbCode,
                                     _In_ PBYTE pbAddress);
#ifdef __cplusplus
}
#endif // __cplusplus

//
//////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
}
#endif // __cplusplus

/////////////////////////////////////////////////// Type-safe overloads for C++
//
#if __cplusplus >= 201103L || _MSVC_LANG >= 201103L
#include <type_traits>

template<typename T>
struct DetoursIsFunctionPointer : std::false_type {};

template<typename T>
struct DetoursIsFunctionPointer<T*> : std::is_function<typename std::remove_pointer<T>::type> {};

template<
    typename T,
    typename std::enable_if<DetoursIsFunctionPointer<T>::value, int>::type = 0>
LONG DetourAttach(_Inout_ T *ppPointer,
                  _In_ T pDetour) noexcept
{
    return DetourAttach(
        reinterpret_cast<void**>(ppPointer),
        reinterpret_cast<void*>(pDetour));
}

template<
    typename T,
    typename std::enable_if<DetoursIsFunctionPointer<T>::value, int>::type = 0>
LONG DetourAttachEx(_Inout_ T *ppPointer,
                    _In_ T pDetour,
                    _Out_opt_ PDETOUR_TRAMPOLINE *ppRealTrampoline,
                    _Out_opt_ T *ppRealTarget,
                    _Out_opt_ T *ppRealDetour) noexcept
{
    return DetourAttachEx(
        reinterpret_cast<void**>(ppPointer),
        reinterpret_cast<void*>(pDetour),
        ppRealTrampoline,
        reinterpret_cast<void**>(ppRealTarget),
        reinterpret_cast<void**>(ppRealDetour));
}

template<
    typename T,
    typename std::enable_if<DetoursIsFunctionPointer<T>::value, int>::type = 0>
LONG DetourDetach(_Inout_ T *ppPointer,
                  _In_ T pDetour) noexcept
{
    return DetourDetach(
        reinterpret_cast<void**>(ppPointer),
        reinterpret_cast<void*>(pDetour));
}

#endif // __cplusplus >= 201103L || _MSVC_LANG >= 201103L
//
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////// Detours Internal Definitions.
//
#ifdef __cplusplus
#ifdef DETOURS_INTERNAL

#define NOTHROW
// #define NOTHROW (nothrow)

//////////////////////////////////////////////////////////////////////////////
//
#if (_MSC_VER < 1299) && !defined(__GNUC__)
#include <imagehlp.h>
typedef IMAGEHLP_MODULE IMAGEHLP_MODULE64;
typedef PIMAGEHLP_MODULE PIMAGEHLP_MODULE64;
typedef IMAGEHLP_SYMBOL SYMBOL_INFO;
typedef PIMAGEHLP_SYMBOL PSYMBOL_INFO;

static inline
LONG InterlockedCompareExchange(_Inout_ LONG *ptr, _In_ LONG nval, _In_ LONG oval)
{
    return (LONG)::InterlockedCompareExchange((PVOID*)ptr, (PVOID)nval, (PVOID)oval);
}
#else
#pragma warning(push)
#pragma warning(disable:4091) // empty typedef
#include <dbghelp.h>
#pragma warning(pop)
#endif

#if defined(_INC_STDIO) && !defined(_CRT_STDIO_ARBITRARY_WIDE_SPECIFIERS)
#error detours.h must be included before stdio.h (or at least define _CRT_STDIO_ARBITRARY_WIDE_SPECIFIERS earlier)
#endif
#define _CRT_STDIO_ARBITRARY_WIDE_SPECIFIERS 1


#if 1 || defined(DETOURS_IA64)

//
// IA64 instructions are 41 bits, 3 per bundle, plus 5 bit bundle template => 128 bits per bundle.
//

#define DETOUR_IA64_INSTRUCTIONS_PER_BUNDLE (3)

#define DETOUR_IA64_TEMPLATE_OFFSET (0)
#define DETOUR_IA64_TEMPLATE_SIZE   (5)

#define DETOUR_IA64_INSTRUCTION_SIZE (41)
#define DETOUR_IA64_INSTRUCTION0_OFFSET (DETOUR_IA64_TEMPLATE_SIZE)
#define DETOUR_IA64_INSTRUCTION1_OFFSET (DETOUR_IA64_TEMPLATE_SIZE + DETOUR_IA64_INSTRUCTION_SIZE)
#define DETOUR_IA64_INSTRUCTION2_OFFSET (DETOUR_IA64_TEMPLATE_SIZE + DETOUR_IA64_INSTRUCTION_SIZE + DETOUR_IA64_INSTRUCTION_SIZE)

C_ASSERT(DETOUR_IA64_TEMPLATE_SIZE + DETOUR_IA64_INSTRUCTIONS_PER_BUNDLE * DETOUR_IA64_INSTRUCTION_SIZE == 128);

__declspec(align(16)) struct DETOUR_IA64_BUNDLE
{
  public:
    union
    {
        BYTE    data[16];
        UINT64  wide[2];
    };

    enum {
        A_UNIT  = 1u,
        I_UNIT  = 2u,
        M_UNIT  = 3u,
        B_UNIT  = 4u,
        F_UNIT  = 5u,
        L_UNIT  = 6u,
        X_UNIT  = 7u,
    };
    struct DETOUR_IA64_METADATA
    {
        ULONG       nTemplate       : 8;    // Instruction template.
        ULONG       nUnit0          : 4;    // Unit for slot 0
        ULONG       nUnit1          : 4;    // Unit for slot 1
        ULONG       nUnit2          : 4;    // Unit for slot 2
    };

  protected:
    static const DETOUR_IA64_METADATA s_rceCopyTable[33];

    UINT RelocateBundle(_Inout_ DETOUR_IA64_BUNDLE* pDst, _Inout_opt_ DETOUR_IA64_BUNDLE* pBundleExtra) const;

    bool RelocateInstruction(_Inout_ DETOUR_IA64_BUNDLE* pDst,
                             _In_ BYTE slot,
                             _Inout_opt_ DETOUR_IA64_BUNDLE* pBundleExtra) const;

    // 120 112 104 96 88 80 72 64 56 48 40 32 24 16  8  0
    //  f.  e.  d. c. b. a. 9. 8. 7. 6. 5. 4. 3. 2. 1. 0.

    //                                      00
    // f.e. d.c. b.a. 9.8. 7.6. 5.4. 3.2. 1.0.
    // 0000 0000 0000 0000 0000 0000 0000 001f : Template [4..0]
    // 0000 0000 0000 0000 0000 03ff ffff ffe0 : Zero [ 41..  5]
    // 0000 0000 0000 0000 0000 3c00 0000 0000 : Zero [ 45.. 42]
    // 0000 0000 0007 ffff ffff c000 0000 0000 : One  [ 82.. 46]
    // 0000 0000 0078 0000 0000 0000 0000 0000 : One  [ 86.. 83]
    // 0fff ffff ff80 0000 0000 0000 0000 0000 : Two  [123.. 87]
    // f000 0000 0000 0000 0000 0000 0000 0000 : Two  [127..124]
    BYTE    GetTemplate() const;
    // Get 4 bit opcodes.
    BYTE    GetInst0() const;
    BYTE    GetInst1() const;
    BYTE    GetInst2() const;
    BYTE    GetUnit(BYTE slot) const;
    BYTE    GetUnit0() const;
    BYTE    GetUnit1() const;
    BYTE    GetUnit2() const;
    // Get 37 bit data.
    UINT64  GetData0() const;
    UINT64  GetData1() const;
    UINT64  GetData2() const;

    // Get/set the full 41 bit instructions.
    UINT64  GetInstruction(BYTE slot) const;
    UINT64  GetInstruction0() const;
    UINT64  GetInstruction1() const;
    UINT64  GetInstruction2() const;
    void    SetInstruction(BYTE slot, UINT64 instruction);
    void    SetInstruction0(UINT64 instruction);
    void    SetInstruction1(UINT64 instruction);
    void    SetInstruction2(UINT64 instruction);

    // Get/set bitfields.
    static UINT64 GetBits(UINT64 Value, UINT64 Offset, UINT64 Count);
    static UINT64 SetBits(UINT64 Value, UINT64 Offset, UINT64 Count, UINT64 Field);

    // Get specific read-only fields.
    static UINT64 GetOpcode(UINT64 instruction); // 4bit opcode
    static UINT64 GetX(UINT64 instruction); // 1bit opcode extension
    static UINT64 GetX3(UINT64 instruction); // 3bit opcode extension
    static UINT64 GetX6(UINT64 instruction); // 6bit opcode extension

    // Get/set specific fields.
    static UINT64 GetImm7a(UINT64 instruction);
    static UINT64 SetImm7a(UINT64 instruction, UINT64 imm7a);
    static UINT64 GetImm13c(UINT64 instruction);
    static UINT64 SetImm13c(UINT64 instruction, UINT64 imm13c);
    static UINT64 GetSignBit(UINT64 instruction);
    static UINT64 SetSignBit(UINT64 instruction, UINT64 signBit);
    static UINT64 GetImm20a(UINT64 instruction);
    static UINT64 SetImm20a(UINT64 instruction, UINT64 imm20a);
    static UINT64 GetImm20b(UINT64 instruction);
    static UINT64 SetImm20b(UINT64 instruction, UINT64 imm20b);

    static UINT64 SignExtend(UINT64 Value, UINT64 Offset);

    BOOL    IsMovlGp() const;

    VOID    SetInst(BYTE Slot, BYTE nInst);
    VOID    SetInst0(BYTE nInst);
    VOID    SetInst1(BYTE nInst);
    VOID    SetInst2(BYTE nInst);
    VOID    SetData(BYTE Slot, UINT64 nData);
    VOID    SetData0(UINT64 nData);
    VOID    SetData1(UINT64 nData);
    VOID    SetData2(UINT64 nData);
    BOOL    SetNop(BYTE Slot);
    BOOL    SetNop0();
    BOOL    SetNop1();
    BOOL    SetNop2();

  public:
    BOOL    IsBrl() const;
    VOID    SetBrl();
    VOID    SetBrl(UINT64 target);
    UINT64  GetBrlTarget() const;
    VOID    SetBrlTarget(UINT64 target);
    VOID    SetBrlImm(UINT64 imm);
    UINT64  GetBrlImm() const;

    UINT64  GetMovlGp() const;
    VOID    SetMovlGp(UINT64 gp);

    VOID    SetStop();

    UINT    Copy(_Out_ DETOUR_IA64_BUNDLE *pDst, _Inout_opt_ DETOUR_IA64_BUNDLE* pBundleExtra = NULL) const;
};
#endif // DETOURS_IA64

#ifdef DETOURS_ARM

#define DETOURS_PFUNC_TO_PBYTE(p)  ((PBYTE)(((ULONG_PTR)(p)) & ~(ULONG_PTR)1))
#define DETOURS_PBYTE_TO_PFUNC(p)  ((PBYTE)(((ULONG_PTR)(p)) | (ULONG_PTR)1))

#endif // DETOURS_ARM

//////////////////////////////////////////////////////////////////////////////

#endif // DETOURS_INTERNAL
#endif // __cplusplus

#endif // _DETOURS_H_
//
////////////////////////////////////////////////////////////////  End of File.
