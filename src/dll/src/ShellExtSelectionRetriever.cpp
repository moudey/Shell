#include <pch.h>
#include "Include/ShellExtSelectionRetriever.h"
#include "Include/ContextMenu.h"
#include <strsafe.h>
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#include <shobjidl.h>
#include <shlobj.h>

using Microsoft::WRL::ComPtr;

extern Logger &_log;

extern HINSTANCE g_hInst;
extern long g_cDllRef;

GlobalSelectionContext g_ShellContext;

const wchar_t shell_ext_selection_retriever_placeholder[] = L"ShellExtSelectionRetriever Placeholder";

HRESULT GetThisPCItem(ComPtr<IShellItem2>& spItem) {
    return SHCreateItemFromParsingName(L"::{20D04FE0-3AEA-1069-A2D8-08002B30309D}", 
                                       NULL, IID_PPV_ARGS(&spItem));
}

ShellExtSelectionRetriever::ShellExtSelectionRetriever(void)
	: m_cRef(1){
    InterlockedIncrement(&g_cDllRef);
}

ShellExtSelectionRetriever::~ShellExtSelectionRetriever(void)
{

    InterlockedDecrement(&g_cDllRef);
}

#pragma region IUnknown

// Query to the interface the component supported.
IFACEMETHODIMP ShellExtSelectionRetriever::QueryInterface(REFIID riid, void **ppv)
{
    __trace(L"In ShellExtSelectionRetriever::QueryInterface");
    static const QITAB qit[] = 
    {
        QITABENT(ShellExtSelectionRetriever, IContextMenu),
        QITABENT(ShellExtSelectionRetriever, IShellExtInit), 
        { 0 },
    };
    return QISearch(this, qit, riid, ppv);
}

// Increase the reference count for an interface on an object.
IFACEMETHODIMP_(ULONG) ShellExtSelectionRetriever::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

// Decrease the reference count for an interface on an object.
IFACEMETHODIMP_(ULONG) ShellExtSelectionRetriever::Release()
{
    ULONG cRef = InterlockedDecrement(&m_cRef);
    if (0 == cRef)
    {
        delete this;
    }

    return cRef;
}

#pragma endregion

#pragma region IShellExtInit
IFACEMETHODIMP ShellExtSelectionRetriever::Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, [[maybe_unused]] HKEY hKeyProgID)
{
    // Reset global context for the current operation
    g_ShellContext = GlobalSelectionContext();

    // 1. Check for Background Context (Right-click on folder background)
    if (pDataObj == NULL && pidlFolder != NULL) 
    {
        g_ShellContext.IsBackground = true;
        __trace(L"[ShellExt] Context: BACKGROUND");

        ComPtr<IShellItem> spItem;
        if (SUCCEEDED(SHCreateItemFromIDList(pidlFolder, IID_PPV_ARGS(&spItem)))) {
            spItem.As(&g_ShellContext.ParentFolder);
            
            // Log background folder path
            PWSTR pszPath = nullptr;
            if (SUCCEEDED(g_ShellContext.ParentFolder->GetDisplayName(SIGDN_FILESYSPATH, &pszPath))) {
                __trace(L"[ShellExt] Background Folder Path: %ls", pszPath);
                CoTaskMemFree(pszPath);
            }
        }
    }
    // 2. Check for Selection Context (Right-click on one or more files/folders)
    else if (pDataObj != NULL) 
    {
        g_ShellContext.IsBackground = false;
        __trace(L"[ShellExt] Context: SELECTION");

        if (SUCCEEDED(SHCreateShellItemArrayFromDataObject(pDataObj, IID_PPV_ARGS(&g_ShellContext.SelectionArray)))) 
        {
            DWORD dwCount = 0;
            g_ShellContext.SelectionArray->GetCount(&dwCount);
            __trace(L"[ShellExt] Total Selected Items: %u", dwCount);

            if (dwCount > 0) {
                ComPtr<IShellItem> spFirstItem;
                ComPtr<IShellItem> spFirstParent;
                bool allSameParent = true;

                // Loop through all selected items for logging and parent verification
                for (DWORD i = 0; i < dwCount; ++i) {
                    ComPtr<IShellItem> spCurrentItem;
                    if (SUCCEEDED(g_ShellContext.SelectionArray->GetItemAt(i, &spCurrentItem))) {
                        // Log individual item path
                        PWSTR pszItemPath = nullptr;
                        // Use SIGDN_FILESYSPATH for local paths, fallback to SIGDN_NORMALDISPLAY for virtual items
                        if (SUCCEEDED(spCurrentItem->GetDisplayName(SIGDN_FILESYSPATH, &pszItemPath))) {
                            __trace(L"[ShellExt]   Item[%u]: %ls", i, pszItemPath);
                            CoTaskMemFree(pszItemPath);
                        }

                        // Parent verification logic
                        if (i == 0) {
                            spCurrentItem->GetParent(&spFirstParent);
                            // Log individual item path
                            PWSTR pszItemPath1 = nullptr;
                            // Use SIGDN_FILESYSPATH for local paths, fallback to SIGDN_NORMALDISPLAY for virtual items
                            if (SUCCEEDED(spFirstParent->GetDisplayName(SIGDN_NORMALDISPLAY, &pszItemPath1))) {
                                __trace(L"[ShellExt]   Parent: %ls", pszItemPath1);
                                CoTaskMemFree(pszItemPath1);
                            }
                        } else if (allSameParent) {
                            ComPtr<IShellItem> spCurrentParent;
                            int compareResult = 0;
                            if (FAILED(spCurrentItem->GetParent(&spCurrentParent)) ||
                                FAILED(spFirstParent->Compare(spCurrentParent.Get(), SICHINT_CANONICAL, &compareResult)) ||
                                compareResult != 0) 
                            {
                                allSameParent = false;
                            }
                        }
                    }
                }

                // Assign the final ParentFolder based on consistency
                if (allSameParent && spFirstParent) {
                    spFirstParent.As(&g_ShellContext.ParentFolder);
                    __trace(L"[ShellExt] Parent Folder is consistent.");
                } else {
                    __trace(L"[ShellExt] Mixed parents or virtual folder; setting Parent to 'This PC'");
                    GetThisPCItem(g_ShellContext.ParentFolder);
                }
            }
        }
    }

    return S_OK;
}
#pragma endregion

#pragma region IContextMenu
IFACEMETHODIMP ShellExtSelectionRetriever::QueryContextMenu(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, [[maybe_unused]] UINT idCmdLast, UINT uFlags)
{
    // If CMF_DEFAULTONLY is set, we should not add any items
    if (uFlags & CMF_DEFAULTONLY) return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0);

    HWND hWnd = GetActiveWindow();
    // Create a dummy ContextMenu to check if is excluded
    Nilesoft::Shell::ContextMenu dummyContextMenu = Nilesoft::Shell::ContextMenu(hWnd, hmenu, {0, 0});
    dummyContextMenu.Initialize(true);
    if (dummyContextMenu.is_excluded())
    {
        __trace(L"In QueryContextMenu: is excluded.");
        return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0);
    }

    // Insert the placeholder menu item
    // idCmdFirst is the starting ID for your commands
    InsertMenu(hmenu, indexMenu, MF_BYPOSITION | MF_STRING, idCmdFirst, shell_ext_selection_retriever_placeholder);

    // Return the number of items added (1)
    return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 1);
}


//
//   FUNCTION: ShellExtSelectionRetriever::InvokeCommand
//
//   PURPOSE: This method is called when a user clicks a menu item to tell 
//            the handler to run the associated command. The lpcmi parameter 
//            points to a structure that contains the needed information.
//
IFACEMETHODIMP ShellExtSelectionRetriever::InvokeCommand(LPCMINVOKECOMMANDINFO lpici)
{
    if (LOWORD(lpici->lpVerb) == 0) 
    {
        __trace(L"[ShellExt] Placeholder clicked.");
        return S_OK;
    }
    return E_FAIL;
}


//
//   FUNCTION: CShellExtSelectionRetriever::GetCommandString
//
//   PURPOSE: If a user highlights one of the items added by a context menu 
//            handler, the handler's IContextMenu::GetCommandString method is 
//            called to request a Help text string that will be displayed on 
//            the Windows Explorer status bar. This method can also be called 
//            to request the verb string that is assigned to a command. 
//            Either ANSI or Unicode verb strings can be requested. This 
//            example only implements support for the Unicode values of 
//            uFlags, because only those have been used in Windows Explorer 
//            since Windows 2000.
//
IFACEMETHODIMP ShellExtSelectionRetriever::GetCommandString([[maybe_unused]] UINT_PTR idCommand, 
    [[maybe_unused]] UINT uFlags, [[maybe_unused]] UINT *pwReserved, [[maybe_unused]] LPSTR pszName, [[maybe_unused]] UINT cchMax)
{
    return E_NOTIMPL;
}

#pragma endregion
