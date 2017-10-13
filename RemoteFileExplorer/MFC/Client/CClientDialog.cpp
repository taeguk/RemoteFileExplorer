#include "MFC/stdafx.h"

#include <afxdialogex.h>
#include <commoncontrols.h>
#include <shellapi.h>

#include <map>

#include "MFC/Client/CClientDialog.h"

namespace remoteFileExplorer
{
namespace mfc
{
namespace client
{
///////////////////////////////////////////////////////////////////////////////
namespace /*unnamed*/
{
// File List View에서 보여질 Icon에 대한 핸들을 얻는 함수.
HICON GetFileIcon(const CString& fileName, ViewMode viewMode, bool isDir);
} // unnamed namespace

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CClientDialog, CDialogEx)

///////////////////////////////////////////////////////////////////////////////
CClientDialog::CClientDialog(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_CLIENT, pParent)
{
}

///////////////////////////////////////////////////////////////////////////////
CClientDialog::~CClientDialog()
{
}

///////////////////////////////////////////////////////////////////////////////
BOOL CClientDialog::OnInitDialog()
{
    if (!CDialogEx::OnInitDialog())
        return FALSE;

    //
    SHFILEINFO sfi = { 0, };
    SHGetFileInfo(
        _T("C:\\"),
        FILE_ATTRIBUTE_DIRECTORY,
        &sfi,
        sizeof(SHFILEINFO),
        SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | SHGFI_LARGEICON);
    hDriveIcon_ = sfi.hIcon;

    SHGetFileInfo(
        _T("dummy"),
        FILE_ATTRIBUTE_DIRECTORY,
        &sfi,
        sizeof(SHFILEINFO),
        SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | SHGFI_LARGEICON);
    hFolderIcon_ = sfi.hIcon;

    //
    ipAddressCtrl_.SetAddress(127, 0, 0, 1);
    portEdit_.SetWindowTextW(_T("9622"));

    //
    systemFileShow_ = false;
    CheckDlgButton(IDC_CHECK_SHOW_SYSTEM_FILES, systemFileShow_);
    hiddenFileShow_ = false;
    CheckDlgButton(IDC_CHECK_SHOW_SYSTEM_FILES, hiddenFileShow_);

    //
    viewModeComboBox_.AddString(_T("Big Icon"));
    viewModeComboBox_.AddString(_T("Icon"));
    viewModeComboBox_.AddString(_T("Simple"));
    viewModeComboBox_.AddString(_T("Report"));

    viewModeComboBox_.SetCurSel(utils::to_underlying(ViewMode::BigIcon));

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
void CClientDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_IPADDRESS_REMOTE_IP, ipAddressCtrl_);
    DDX_Control(pDX, IDC_EDIT_REMOTE_PORT, portEdit_);
    DDX_Control(pDX, IDC_MFCBUTTON_CLIENT_CONTROL, controlButton_);

    DDX_Control(pDX, IDC_TREE_DIRECTORY, dirTreeControl_);
    DDX_Control(pDX, IDC_LIST_FILE, fileListControl_);

    DDX_Control(pDX, IDC_CHECK_SHOW_SYSTEM_FILES, systemFileCheckBox_);
    DDX_Control(pDX, IDC_CHECK_SHOW_HIDDEN_FILES, hiddenFileCheckBox_);
    DDX_Control(pDX, IDC_COMBO_VIEW_MODE, viewModeComboBox_);
}

///////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CClientDialog, CDialogEx)
    ON_BN_CLICKED(IDC_MFCBUTTON_CLIENT_CONTROL,
        &CClientDialog::OnBnClickedMfcbuttonClientControl)
    ON_BN_CLICKED(IDC_CHECK_SHOW_SYSTEM_FILES,
        &CClientDialog::OnBnClickedCheckShowSystemFiles)
    ON_BN_CLICKED(IDC_CHECK_SHOW_HIDDEN_FILES,
        &CClientDialog::OnBnClickedCheckShowHiddenFiles)
    ON_CBN_SELCHANGE(IDC_COMBO_VIEW_MODE,
        &CClientDialog::OnCbnSelchangeComboViewMode)
    ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DIRECTORY,
        &CClientDialog::OnTvnSelchangedTreeDirectory)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_FILE,
        &CClientDialog::OnNMDblclkListFile)
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
void CClientDialog::OnBnClickedMfcbuttonClientControl()
{
    if (status_ == ClientStatus::Connected)
    {
        DisconnectToServer_();
    }
    else if (status_ == ClientStatus::Disconnected)
    {
        ConnectToServer_();
    }
}

///////////////////////////////////////////////////////////////////////////////
void CClientDialog::OnBnClickedCheckShowSystemFiles()
{
    systemFileShow_ = !systemFileShow_;
    CheckDlgButton(IDC_CHECK_SHOW_SYSTEM_FILES, systemFileShow_);

    // 바뀐 설정에 부합하도록 View들을 업데이트 한다.
    UpdateDirTreeViewAll_();
    UpdateFileListView_();
}

///////////////////////////////////////////////////////////////////////////////
void CClientDialog::OnBnClickedCheckShowHiddenFiles()
{
    hiddenFileShow_ = !hiddenFileShow_;
    CheckDlgButton(IDC_CHECK_SHOW_HIDDEN_FILES, hiddenFileShow_);

    // 바뀐 설정에 부합하도록 View들을 업데이트 한다.
    UpdateDirTreeViewAll_();
    UpdateFileListView_();
}

///////////////////////////////////////////////////////////////////////////////
void CClientDialog::OnCbnSelchangeComboViewMode()
{
    // 바뀐 설정에 부합하도록 View들을 업데이트 한다.
    UpdateFileListView_();
}

///////////////////////////////////////////////////////////////////////////////
void CClientDialog::OnTvnSelchangedTreeDirectory(NMHDR *pNMHDR, LRESULT *pResult)
{
    *pResult = 0;

    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    HTREEITEM hItem = pNMTreeView->itemNew.hItem;

    if (hItem == nullptr)
        return;

    FileTree* fileTree = (FileTree*) dirTreeControl_.GetItemData(hItem);

    assert(fileTree->hTreeItem == hItem);

    ClearFileTreeChilds_(fileTree);

    // 원격지 폴더내의 모든 파일 정보를 가져온다.
    // 폴더에 파일이 많을 경우, 한 번에 가져올 수 없으므로,
    // 여러 번에 걸쳐 가져온다.
    common::Directory dir;
    common::file_count_t offset = 0;

    do
    {
        using remoteFileExplorer::client::RPCException;

        try {
            if (remoteFileExplorer_.GetDirectoryInfo(
                fileTree->fullPath,
                offset,
                dir) != 0)
            {
                AfxMessageBox(_T("Fail to get directory information."));
                return;
            }
        }
        catch (const RPCException& e) {
            // RPC가 실패할 경우, server와의 연결을 끊는다.
            AfxMessageBox(CString(e.what()));
            DisconnectToServer_();
            return;
        }

        // 얻은 정보를 바탕으로 FileTree 자료구조를 구축한다.
        for (const auto& file : dir.fileInfos)
        {
            std::unique_ptr<FileTree> newFileTree = std::make_unique<FileTree>();
            newFileTree->fullPath = dir.path + L'\\' + file.fileName;
            newFileTree->f = std::move(file);
            newFileTree->parent = fileTree;

            // . 혹은 .. 디렉토리는 가상디렉토리로 표시해준다.
            if (newFileTree->f.fileName == L".")
                newFileTree->specialBehavior = FileSpecialBehavior::CurrentDirectory;
            else if (newFileTree->f.fileName == L"..")
                newFileTree->specialBehavior = FileSpecialBehavior::ParentDirectory;
            else
                newFileTree->specialBehavior = FileSpecialBehavior::None;

            fileTree->childs.push_back(std::move(newFileTree));
        }
        offset += dir.fileInfos.size();

    } while (dir.fileInfos.size() >= 1);

    // Directory Tree View를 업데이트한다.
    UpdateDirTreeView_(fileTree);
    dirTreeControl_.Expand(hItem, TVE_EXPAND);

    // File List View를 업데이트한다.
    curDirTree_ = fileTree;
    UpdateFileListView_();
}

///////////////////////////////////////////////////////////////////////////////
void CClientDialog::OnNMDblclkListFile(NMHDR *pNMHDR, LRESULT *pResult)
{
    *pResult = 0;

    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    int nItem = pNMItemActivate->iItem;
    
    // 올바르지 않은 경우,
    // 예시) 파일이 아닌 흰바탕에 더블클릭이 발생했을 때
    if (nItem < 0 || nItem >= fileListControl_.GetItemCount())
        return;

    FileTree* fileTree = (FileTree*) fileListControl_.GetItemData(nItem);
    
    assert(fileTree != nullptr);

    // 파일에 대해선 아무런 행동도 하지 않는다.
    if (fileTree->f.fileType != common::FileType::Directory)
        return;

    if (fileTree->specialBehavior == FileSpecialBehavior::None)
    {
        assert(fileTree->hTreeItem);
        dirTreeControl_.SelectItem(fileTree->hTreeItem);
    }
    else
    {
        // ".." 디렉토리를 의미한다.
        if (fileTree->specialBehavior == FileSpecialBehavior::ParentDirectory)
        {
            assert(fileTree->parent != nullptr);
            assert(fileTree->parent->parent != nullptr);
            dirTreeControl_.SelectItem(fileTree->parent->parent->hTreeItem);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void CClientDialog::ConnectToServer_()
{
    std::uint8_t ipAddress[4];
    std::int16_t port;

    // Get IP Address
    {
        ipAddressCtrl_.GetAddress(
            ipAddress[0], ipAddress[1], ipAddress[2], ipAddress[3]);
    }

    // Get Port Number
    {
        CString cstr;
        portEdit_.GetWindowTextW(cstr);

        int len = cstr.GetLength();
        if (len == 0 || len > 5)
        {
            AfxMessageBox(_T("Invalid Port Number!"));
            return;
        }
        int num = _ttoi(cstr.GetBuffer());
        if (num > UINT16_MAX)
        {
            AfxMessageBox(_T("Invalid Port Number!"));
            return;
        }
        port = static_cast<std::uint16_t>(num);
    }

    if (remoteFileExplorer_.Connect(ipAddress, port) != 0)
    {
        AfxMessageBox(_T("Fail to connect the server!"));
        return;
    }

    controlButton_.SetWindowTextW(_T("Disconnect"));
    status_ = ClientStatus::Connected;

    // 서버와 연결을 한 후, 서버에 맞게 View를 초기화한다.
    if (InitializeView_() != 0)
        DisconnectToServer_();
}

///////////////////////////////////////////////////////////////////////////////
void CClientDialog::DisconnectToServer_()
{
    controlButton_.SetWindowTextW(_T("Connect"));
    status_ = ClientStatus::Disconnected;
    ClearView_();
    (void) remoteFileExplorer_.Disconnect(); // 반드시 맨 마지막에 하기.
}

///////////////////////////////////////////////////////////////////////////////
int CClientDialog::InitializeView_()
{
    using remoteFileExplorer::client::RPCException;

    std::vector<common::LogicalDrive> drives;

    try {
        if (remoteFileExplorer_.GetLogicalDriveInfo(drives) != 0)
        {
            AfxMessageBox(_T("Fail to get logical drive informations."));
            return -1;
        }
    }
    catch (const RPCException& e) {
        AfxMessageBox(CString(e.what()));
        return -1;
    }

    fileTreeVRoot_.childs.clear();
    dirTreeControl_.DeleteAllItems();

    auto numDrives = drives.size();

    CImageList* imageListTree = new CImageList;
    imageListTree->Create(16, 16, ILC_COLOR32, numDrives, 0);
    imageListTree->SetBkColor(RGB(255, 255, 255));

    for (auto i = 0; i < numDrives; ++i)
    {
        CString drivePath = CString(drives[i].letter) + ":";
        CString driveName(drives[i].driveName.c_str());
        
        CString text = driveName + _T('(') + drives[i].letter + _T(":)");

        imageListTree->Add(hDriveIcon_);
        HTREEITEM hItem = dirTreeControl_.InsertItem(text, i, i);

        std::unique_ptr<FileTree> fileTree = std::make_unique<FileTree>();
        fileTree->f.fileAttr = common::FileAttribute::NoFlag;
        fileTree->f.fileType = common::FileType::Directory;
        fileTree->f.fileName = drivePath;
        fileTree->f.modifiedDate = 0;
        fileTree->fullPath = drivePath;
        fileTree->hTreeItem = hItem;

        dirTreeControl_.SetItemData(hItem, (DWORD_PTR) fileTree.get());
        fileTreeVRoot_.childs.push_back(std::move(fileTree));
    }

    dirTreeControl_.SetImageList(imageListTree, TVSIL_NORMAL);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
void CClientDialog::ClearView_()
{
    ClearFileTreeChilds_(&fileTreeVRoot_);
    curDirTree_ = nullptr;
    UpdateFileListView_();
}

///////////////////////////////////////////////////////////////////////////////
void CClientDialog::ClearFileTreeChilds_(FileTree* parentTree)
{
    assert(parentTree != nullptr);

    CImageList* imageList = dirTreeControl_.GetImageList(TVSIL_NORMAL);

    for (const auto& child : parentTree->childs)
    {
        ClearFileTreeChilds_(child.get());

        if (child->hTreeItem != nullptr)
        {
            if (child->hTreeItem == dirTreeControl_.GetSelectedItem())
            {
                curDirTree_ = nullptr;
                dirTreeControl_.SelectItem(nullptr);
                UpdateFileListView_();
            }
            // 이미지 리스트에서 item에 해당하는 이미지를 지운다.
            int nImage, nSelectedImage;
            if (dirTreeControl_.GetItemImage(
                child->hTreeItem,
                nImage,
                nSelectedImage))
            {
                imageList->Remove(nImage);
            }

            // item을 삭제한다.
            dirTreeControl_.DeleteItem(child->hTreeItem);
        }
    }
    parentTree->childs.clear();
}

///////////////////////////////////////////////////////////////////////////////
void CClientDialog::UpdateDirTreeViewAll_()
{
    for (auto& drive : fileTreeVRoot_.childs)
    {
        UpdateDirTreeView_(drive.get());
    }
}

///////////////////////////////////////////////////////////////////////////////
void CClientDialog::UpdateDirTreeView_(FileTree* parentTree)
{
    assert(parentTree->hTreeItem != nullptr);

    CImageList* imageList = dirTreeControl_.GetImageList(TVSIL_NORMAL);

    HTREEITEM hParent = parentTree->hTreeItem;
    HTREEITEM hInsertAfter = TVI_FIRST;
    auto childCount = parentTree->childs.size();

    for (int i = 0; i < childCount; ++i)
    {
        FileTree* childTree = parentTree->childs[i].get();

        if (childTree->f.fileType != common::FileType::Directory)
            continue;

        // 가상 디렉토리는 tree view에 출력하지 않는다.
        if (childTree->specialBehavior != FileSpecialBehavior::None)
            continue;

        bool show = CheckFileShouldBeShown_(childTree);

        // 보여져야하는데 숨겨져 있는 경우,
        if (show && childTree->hTreeItem == nullptr)
        {
            auto nImage = imageList->Add(hFolderIcon_);
            auto hItem =
                dirTreeControl_.InsertItem(
                    CString(childTree->f.fileName.c_str()),
                    nImage, nImage, hParent, hInsertAfter);
            assert(hItem != nullptr);
            childTree->hTreeItem = hItem;
            dirTreeControl_.SetItemData(hItem, (DWORD_PTR) childTree);
        }
        // 숨겨져야하는데 보여져 있는 경우,
        else if (!show && childTree->hTreeItem != nullptr)
        {
            // 자식들을 모두 삭제한다.
            ClearFileTreeChilds_(childTree);

            // 이미지 리스트에서 item에 해당하는 이미지를 지운다.
            int nImage, nSelectedImage;
            if (dirTreeControl_.GetItemImage(
                childTree->hTreeItem,
                nImage,
                nSelectedImage))
            {
                imageList->Remove(nImage);
            }

            // item을 삭제한다.
            dirTreeControl_.DeleteItem(childTree->hTreeItem);
            childTree->hTreeItem = nullptr;
        }

        // 보여지는 경우, 자식들에 대해서도 재귀적으로 알고리즘을 수행해야 한다.
        if (show)
        {
            // 자식들에 대해서도 재귀적인 알고리즘 수행.
            UpdateDirTreeView_(childTree);
            hInsertAfter = childTree->hTreeItem;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void CClientDialog::UpdateFileListView_()
{
    // List Control의 Column과 Item들을 모두 지운다.
    auto headerControl = fileListControl_.GetHeaderCtrl();
    if (headerControl != nullptr)
    {
        while (headerControl->GetItemCount() > 0)
            fileListControl_.DeleteColumn(0);
    }
    fileListControl_.DeleteAllItems();

    if (curDirTree_ == nullptr)
        return;

    ViewMode viewMode =
        static_cast<ViewMode>(viewModeComboBox_.GetCurSel());
    
    // 큰 아이콘 / 아이콘 / 간단히 모드,
    if (viewMode == ViewMode::BigIcon ||
        viewMode == ViewMode::Icon ||
        viewMode == ViewMode::Simple)
    {
        UINT styleFlag;
        int imageSize;
        int nImageList;

        switch (viewMode)
        {
        case ViewMode::BigIcon:
            imageSize = 64;
            styleFlag = LVS_ICON;
            nImageList = TVSIL_NORMAL;
            break;
        case ViewMode::Icon:
            imageSize = 32;
            styleFlag = LVS_ICON;
            nImageList = TVSIL_NORMAL;
            break;
        case ViewMode::Simple:
        default:
            imageSize = 16;
            styleFlag = LVS_LIST;
            nImageList = LVSIL_SMALL;
        }

        fileListControl_.ModifyStyle(
            LVS_ICON | LVS_REPORT | LVS_SMALLICON | LVS_LIST, styleFlag);

        CImageList* imageList = new CImageList;
        // Create가 실패한다면, GDI Object leak을 의심해봐야 한다.
        assert(imageList->Create(
            imageSize,
            imageSize,
            ILC_COLOR32,
            curDirTree_->childs.size(),
            0));
        imageList->SetBkColor(RGB(255, 255, 255));

        int nItem = 0;
        for (const auto& childTree : curDirTree_->childs)
        {
            if (!CheckFileShouldBeShown_(childTree.get()))
                continue;

            CString fileName(childTree->f.fileName.c_str());
            HICON hIcon = GetFileIcon(
                fileName,
                viewMode,
                childTree->f.fileType == common::FileType::Directory);

            int nImage = imageList->Add(hIcon);

            fileListControl_.InsertItem(nItem, fileName, nImage);
            fileListControl_.SetItemData(nItem++, (DWORD_PTR) childTree.get());
        }
        fileListControl_.SetImageList(imageList, nImageList);
    }
    // 자세히 모드.
    else // ViewMode::Report
    {
        fileListControl_.ModifyStyle(
            LVS_ICON | LVS_REPORT | LVS_SMALLICON | LVS_LIST, LVS_REPORT);

        fileListControl_.InsertColumn(0, _T("Name"), LVCFMT_LEFT, 90);
        fileListControl_.InsertColumn(1, _T("Modified Date"), LVCFMT_LEFT, 90);
        fileListControl_.InsertColumn(2, _T("Type"), LVCFMT_LEFT, 90);
        fileListControl_.InsertColumn(3, _T("Size"), LVCFMT_LEFT, 90);

        CImageList* imageList = new CImageList;
        // Create가 실패한다면, GDI Object leak을 의심해봐야 한다.
        assert(imageList->Create(
            16,
            16,
            ILC_COLOR32,
            curDirTree_->childs.size(),
            0));
        imageList->SetBkColor(RGB(255, 255, 255));

        int nItem = 0;
        for (const auto& childTree : curDirTree_->childs)
        {
            if (!CheckFileShouldBeShown_(childTree.get()))
                continue;

            bool isDir = childTree->f.fileType == common::FileType::Directory;
            CString fileName(childTree->f.fileName.c_str());
            HICON hIcon = GetFileIcon(fileName, viewMode, isDir);
            int nImage = imageList->Add(hIcon);

            auto dateString = CString(std::strtok(std::asctime(std::localtime(
                &childTree->f.modifiedDate)), "\n"));
            auto typeString = isDir ? _T("Directory") : _T("File");
            auto sizeString = isDir ? CString(_T("")) :
                CString((std::to_string(childTree->f.fileSize) + " B").c_str());

            int nIndex = fileListControl_.InsertItem(nItem, fileName, nImage);
            fileListControl_.SetItemText(nIndex, 1, dateString);
            fileListControl_.SetItemText(nIndex, 2, typeString);
            fileListControl_.SetItemText(nIndex, 3, sizeString);
            fileListControl_.SetItemData(nItem++, (DWORD_PTR) childTree.get());
        }
        fileListControl_.SetImageList(imageList, LVSIL_SMALL);

        // Column Width 조정.
        assert(fileListControl_.GetHeaderCtrl()->GetItemCount() == 4);
        const double rate[4] = { 0.40, 0.25, 0.15, 0.2 };
        CRect rect;
        fileListControl_.GetClientRect(rect);
        for (int i = 0; i < 4; ++i)
            fileListControl_.SetColumnWidth(i, rect.Width() * rate[i]);

        fileListControl_.SetRedraw(true);
        fileListControl_.UpdateWindow();
    }
}

///////////////////////////////////////////////////////////////////////////////
bool CClientDialog::CheckFileShouldBeShown_(FileTree* fileTree)
{
    bool show = true;

    if (!systemFileShow_ &&
        static_cast<bool>(fileTree->f.fileAttr & common::FileAttribute::System))
        show = false;

    if (!hiddenFileShow_ &&
        static_cast<bool>(fileTree->f.fileAttr & common::FileAttribute::Hidden))
        show = false;

    return show;
}

///////////////////////////////////////////////////////////////////////////////
namespace /*unnamed*/
{
HICON GetFileIcon(const CString& fileName, ViewMode viewMode, bool isDir)
{
    // 과도한 GDI Object 생성을 막기 위해, icon handle pool을 만들어 쓴다.
    // (extension, icon level, isDir) -> icon handle 로의 mapping.
    static std::map<std::pair<CString, UINT>, HICON> fileIconMap;
    static std::map<UINT, HICON> dirIconMap;

    // icon level은 icon의 선명도(크기)를 의미한다.
    // 0 에 가까울 수록 선명도가 좋다. (=아이콘 크기가 크다.)
    static const UINT IconSizeFlagMax = 3;
    static const UINT IconSizeFlag[IconSizeFlagMax] = {
        SHGFI_SYSICONINDEX, SHGFI_LARGEICON, SHGFI_SMALLICON
    };
    UINT orgIconLevel;

    // view mode에 따라 icon level를 달리한다.
    switch (viewMode)
    {
    case ViewMode::BigIcon:
        orgIconLevel = 0;
        break;
    case ViewMode::Icon:
        orgIconLevel = 1;
        break;
    case ViewMode::Simple:
    case ViewMode::Report:
    default:
        orgIconLevel = 2;
    }

    // icon을 얻어 올 때는, 전체파일명이 아니라 확장자명만 있으면 된다.
    CString extension = _T("dummy");

    if (!isDir)
    {
        // Parse extension from fileName
        int pos = fileName.ReverseFind('.');
        if (pos != -1)
            extension = fileName.Right(fileName.GetLength() - pos).MakeLower();
    }

    HICON hIcon = nullptr;
    SHFILEINFO sfi = { 0, };
    UINT flag = SHGFI_ICON | SHGFI_USEFILEATTRIBUTES;

    CoInitialize(nullptr);

    // 최종적으로 얻어진 icon의 level을 의미한다.
    UINT gotIconLevel = IconSizeFlagMax;

    // 원래 해당되는 icon level에 해당하는 icon을 얻으면 좋겠지만,
    //   시스템에 따라 실패를 할 수도 있다.
    // 따라서 실패를 하면, 좀 더 낮은 화질의 icon이라도 얻기 위해
    //   반복적으로 시도한다.
    for (auto iconLevel = orgIconLevel;
        hIcon == nullptr && iconLevel <= IconSizeFlagMax;
        ++iconLevel)
    {
        if (isDir)
        {
            auto it = dirIconMap.find(iconLevel);
            if (it != std::end(dirIconMap))
            {
                hIcon = it->second;
                gotIconLevel = iconLevel;
                break;
            }
        }
        else
        {
            auto it = fileIconMap.find(std::make_pair(extension, iconLevel));
            if (it != std::end(fileIconMap))
            {
                hIcon = it->second;
                gotIconLevel = iconLevel;
                break;
            }
        }

        HRESULT hResult = SHGetFileInfo(
            extension,
            (isDir ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL),
            &sfi,
            sizeof(sfi),
            flag);

        if (FAILED(hResult))
            continue;

        if (IconSizeFlag[iconLevel] == SHGFI_SYSICONINDEX) {
            // System image list를 얻는다.
            HIMAGELIST* imageList;
            hResult = SHGetImageList(
                SHIL_JUMBO,
                IID_IImageList,
                (void**)&imageList);

            if (FAILED(hResult))
            {
                DestroyIcon(sfi.hIcon);
                continue;
            }

            // System image list으로 부터, icon을 얻어온다.
            hResult = ((IImageList*)imageList)->GetIcon(
                sfi.iIcon,
                ILD_TRANSPARENT,
                &hIcon);

            if (FAILED(hResult))
            {
                DestroyIcon(sfi.hIcon);
                continue;
            }
        }
        else {
            hIcon = sfi.hIcon;
        }

        gotIconLevel = iconLevel;
        if (isDir)
            dirIconMap[iconLevel] = hIcon;
        else
            fileIconMap[std::make_pair(extension, iconLevel)] = hIcon;
    }

    // 최종적으로 얻은 icon level부터 원래 얻으려했던 icon level에까지,
    //   전부 다 cache 해놓는다.
    while (gotIconLevel-- > orgIconLevel)
    {
        if (isDir)
            dirIconMap[gotIconLevel] = hIcon;
        else
            fileIconMap[std::make_pair(extension, gotIconLevel)] = hIcon;
    }

    return hIcon;
}
} // unnamed namespace

} // namespace client
} // namespace mfc
} // namespace remoteFileExplorer