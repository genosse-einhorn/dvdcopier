// Copyright © 2021 Jonas Kümmerlin <jonas@kuemmerlin.eu>
//
// Permission to use, copy, modify, and distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

#include <shlobj.h>
#include <shobjidl.h>
#include <shellapi.h>

////////////////////////////////////////////
// Compatibility Definitions              //
////////////////////////////////////////////

// older MinGW and older Microsoft SDKs are missing the IFileDialog interface
class FolderBrowseHelper_IFileDialogEvents;

enum FolderBrowseHelper__FILEOPENDIALOGOPTIONS
{
        FolderBrowseHelper_FOS_OVERWRITEPROMPT          = 0x2,
        FolderBrowseHelper_FOS_STRICTFILETYPES          = 0x4,
        FolderBrowseHelper_FOS_NOCHANGEDIR              = 0x8,
        FolderBrowseHelper_FOS_PICKFOLDERS              = 0x20,
        FolderBrowseHelper_FOS_FORCEFILESYSTEM          = 0x40,
        FolderBrowseHelper_FOS_ALLNONSTORAGEITEMS       = 0x80,
        FolderBrowseHelper_FOS_NOVALIDATE               = 0x100,
        FolderBrowseHelper_FOS_ALLOWMULTISELECT         = 0x200,
        FolderBrowseHelper_FOS_PATHMUSTEXIST            = 0x800,
        FolderBrowseHelper_FOS_FILEMUSTEXIST            = 0x1000,
        FolderBrowseHelper_FOS_CREATEPROMPT             = 0x2000,
        FolderBrowseHelper_FOS_SHAREAWARE               = 0x4000,
        FolderBrowseHelper_FOS_NOREADONLYRETURN         = 0x8000,
        FolderBrowseHelper_FOS_NOTESTFILECREATE         = 0x10000,
        FolderBrowseHelper_FOS_HIDEMRUPLACES            = 0x20000,
        FolderBrowseHelper_FOS_HIDEPINNEDPLACES         = 0x40000,
        FolderBrowseHelper_FOS_NODEREFERENCELINKS       = 0x100000,
        FolderBrowseHelper_FOS_OKBUTTONNEEDSINTERACTION = 0x200000,
        FolderBrowseHelper_FOS_DONTADDTORECENT          = 0x2000000,
        FolderBrowseHelper_FOS_FORCESHOWHIDDEN          = 0x10000000,
        FolderBrowseHelper_FOS_DEFAULTNOMINIMODE        = 0x20000000,
        FolderBrowseHelper_FOS_FORCEPREVIEWPANEON       = 0x40000000,
        FolderBrowseHelper_FOS_SUPPORTSTREAMABLEITEMS   = 0x80000000
};
typedef DWORD FolderBrowseHelper_FILEOPENDIALOGOPTIONS;

typedef enum FolderBrowseHelper_FDAP
{
    FolderBrowseHelper_FDAP_BOTTOM  = 0,
    FolderBrowseHelper_FDAP_TOP     = 1
} FolderBrowseHelper_FDAP;

typedef struct FolderBrowseHelper__COMDLG_FILTERSPEC
{
    LPCWSTR pszName;
    LPCWSTR pszSpec;
} FolderBrowseHelper_COMDLG_FILTERSPEC;

MIDL_INTERFACE("42f85136-db7e-439c-85f1-e4075d135fc8")
FolderBrowseHelper_IFileDialog : public IModalWindow
{
public:
    virtual HRESULT STDMETHODCALLTYPE SetFileTypes(UINT cFileTypes, const FolderBrowseHelper_COMDLG_FILTERSPEC *rgFilterSpec) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetFileTypeIndex(UINT iFileType) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFileTypeIndex(UINT *piFileType) = 0;
    virtual HRESULT STDMETHODCALLTYPE Advise(FolderBrowseHelper_IFileDialogEvents *pfde, DWORD *pdwCookie) = 0;
    virtual HRESULT STDMETHODCALLTYPE Unadvise(DWORD dwCookie) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetOptions(FolderBrowseHelper_FILEOPENDIALOGOPTIONS fos) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetOptions(FolderBrowseHelper_FILEOPENDIALOGOPTIONS *pfos) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDefaultFolder(IShellItem *psi) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetFolder(IShellItem *psi) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFolder(IShellItem **ppsi) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentSelection(IShellItem **ppsi) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetFileName(LPCWSTR pszName) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFileName(LPWSTR *pszName) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetTitle(LPCWSTR pszTitle) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetOkButtonLabel(LPCWSTR pszText) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetFileNameLabel(LPCWSTR pszLabel) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetResult(IShellItem **ppsi) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddPlace(IShellItem *psi, FolderBrowseHelper_FDAP fdap) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDefaultExtension(LPCWSTR pszDefaultExtension) = 0;
    virtual HRESULT STDMETHODCALLTYPE Close(HRESULT hr) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetClientGuid(REFGUID guid) = 0;
    virtual HRESULT STDMETHODCALLTYPE ClearClientData(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetFilter(IShellItemFilter *pFilter) = 0;
};

#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL(FolderBrowseHelper_IFileDialog, 0x42f85136, 0xdb7e, 0x439c, 0x85, 0xf1, 0xe4, 0x07, 0x5d, 0x13, 0x5f, 0xc8)
#endif

static const GUID FolderBrowseHelper_CLSID_FileOpenDialog = { 0xdc1c5a9c, 0xe88a, 0x4dde, { 0xa5, 0xa1, 0x60, 0xf8, 0x2a, 0x20, 0xae, 0xf7 } };





////////////////////////////////////////////
// Actual Code                            //
////////////////////////////////////////////

static BOOL
FolderBrowseHelper_IsFileSystemItem(LPITEMIDLIST pidl)
{
    SHFILEINFO sfi;
    ZeroMemory(&sfi, sizeof sfi);

    if (SHGetFileInfo((WCHAR *)pidl, 0, &sfi, sizeof sfi, SHGFI_ATTRIBUTES | SHGFI_PIDL)) {
        return sfi.dwAttributes & SFGAO_FILESYSTEM ? TRUE : FALSE;
    }

    return FALSE;
}

static int CALLBACK
FolderBrowseHelper_BrowseForFolderCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
    (void)lParam;

    if (uMsg == BFFM_INITIALIZED && lpData) {
        SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
    }

    // XXX: BIF_RETURNONLYFSDIRS is broken on XP and possibly earlier
    // when using BIF_NEWDIALOGSTYLE. So we emulate it here.
    // It is reported to work on Win7, but we use IFileDialog there
    if (uMsg == BFFM_SELCHANGED) {
        LPITEMIDLIST pidl = (LPITEMIDLIST)lParam;
        BOOL isFs = FolderBrowseHelper_IsFileSystemItem(pidl);

        SendMessage(hwnd, BFFM_ENABLEOK, 0, (LPARAM)isFs);
    }

    return 0;
}


static HRESULT
FolderBrowseHelper_PidlToPath(LPITEMIDLIST pidl, WCHAR **pPath)
{
    *pPath = (WCHAR *)CoTaskMemAlloc(MAX_PATH * sizeof(WCHAR));
    if (!*pPath) {
        return E_OUTOFMEMORY;
    }

    if (!SHGetPathFromIDList(pidl, *pPath)) {
        CoTaskMemFree(*pPath);
        *pPath = NULL;
        return E_FAIL;
    }

    return S_OK;
}


static HRESULT
FolderBrowseHelper_CreateItemFromParsingName(const WCHAR *pszPath,
                                             IBindCtx    *pbc,
                                             REFIID       riid,
                                             void       **ppv)
{
    static HRESULT (WINAPI *cifpn)(const WCHAR *, IBindCtx *, REFIID, void **) = NULL;

    if (!cifpn) {
        HMODULE shell32 = GetModuleHandle(L"SHELL32.DLL");
        cifpn = (HRESULT(WINAPI*)(const WCHAR *, IBindCtx *, REFIID, void **))(void*)GetProcAddress(shell32, "SHCreateItemFromParsingName");
    }

    if (cifpn) {
        return cifpn(pszPath, pbc, riid, ppv);
    } else {
        *ppv = NULL;
        return E_NOTIMPL;
    }
}


HRESULT
FolderBrowseHelper_BrowseForFolder(HWND hwndParent, const WCHAR *initial, WCHAR **pResult)
{
    FolderBrowseHelper_IFileDialog *pfd = NULL;
    HRESULT hr = S_OK;

    *pResult = NULL;

    if (SUCCEEDED(CoCreateInstance(FolderBrowseHelper_CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd)))) {
        // new-style dialog for vista or newer
        DWORD dwOptions;
        if (SUCCEEDED(pfd->GetOptions(&dwOptions))) {
            pfd->SetOptions(dwOptions | FolderBrowseHelper_FOS_PICKFOLDERS | FolderBrowseHelper_FOS_FORCEFILESYSTEM);
        }

        if (initial) {
            IShellItem *psi = NULL;
            hr = FolderBrowseHelper_CreateItemFromParsingName(initial, NULL, IID_PPV_ARGS(&psi));
            if (SUCCEEDED(hr) && psi) {
                pfd->SetFolder(psi);
                psi->Release();
            }
        }

        hr = pfd->Show(hwndParent);
        if (SUCCEEDED(hr))
        {
            IShellItem *psi = NULL;
            hr = pfd->GetResult(&psi);
            if (SUCCEEDED(hr) && psi)
            {
                hr = psi->GetDisplayName(SIGDN_FILESYSPATH, pResult);
                psi->Release();
            }
        }
        pfd->Release();
    } else {
        // old-style dialog for pre-Vista OS

        BROWSEINFO bi;
        ZeroMemory(&bi, sizeof bi);

        bi.hwndOwner = hwndParent;
        bi.ulFlags = BIF_NEWDIALOGSTYLE;
        bi.lpfn = FolderBrowseHelper_BrowseForFolderCallbackProc;
        bi.lParam = (LPARAM)initial;

        LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
        if (pidl) {
            hr = FolderBrowseHelper_PidlToPath(pidl, pResult);
            CoTaskMemFree(pidl);
        } else {
            hr = HRESULT_FROM_WIN32(ERROR_CANCELLED);
        }
    }

    return hr;
}
