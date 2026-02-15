#pragma once

#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

// Global structure to share data with existing Parse logic
struct GlobalSelectionContext {
    bool IsBackground = false;
    ComPtr<IShellItem2> ParentFolder;
    ComPtr<IShellItemArray> SelectionArray;
};

extern GlobalSelectionContext g_ShellContext;
extern const wchar_t shell_ext_selection_retriever_placeholder[];

class ShellExtSelectionRetriever : public IShellExtInit, public IContextMenu
{
public:
    // IUnknown
    IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv);
    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();

    // IShellExtInit
    IFACEMETHODIMP Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hKeyProgID);

    // IContextMenu
    IFACEMETHODIMP QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags);
    IFACEMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO pici);
    IFACEMETHODIMP GetCommandString(UINT_PTR idCommand, UINT uFlags, UINT *pwReserved, LPSTR pszName, UINT cchMax);
	
    ShellExtSelectionRetriever(void);

protected:
    ~ShellExtSelectionRetriever(void);

private:
    // Reference count of component.
    long m_cRef;
};
