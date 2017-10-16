#include "MFC/stdafx.h"

#include <afxdialogex.h>
#include <commoncontrols.h>
#include <shellapi.h>

#include <cassert>
#include <algorithm>
#include <map>

#include "MFC/Client/CClientDialog.h"

namespace remoteFileExplorer
{
namespace mfc
{
namespace client
{
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

    //
    SHFILEINFO sfi = { 0, };
    SHGetFileInfo(
        _T("C:\\"),
        FILE_ATTRIBUTE_DIRECTORY,
        &sfi,
        sizeof(SHFILEINFO),
        SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | SHGFI_LARGEICON);
    // Dir Tree View에서 drive에 보여질 아이콘.
    HICON hDriveIcon = sfi.hIcon;

    SHGetFileInfo(
        _T("dummy"),
        FILE_ATTRIBUTE_DIRECTORY,
        &sfi,
        sizeof(SHFILEINFO),
        SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | SHGFI_LARGEICON);
    // Dir Tree View에서 directory에 보여질 아이콘.
    HICON hDirIcon = sfi.hIcon;

    CImageList* dirTreeImageList = new CImageList;
    dirTreeImageList->Create(16, 16, ILC_COLOR32, 2, 0);
    dirTreeImageList->SetBkColor(RGB(255, 255, 255));
    nImageDrive_ = dirTreeImageList->Add(hDriveIcon);
    nImageDir_ = dirTreeImageList->Add(hDirIcon);
    dirTreeControl_.SetImageList(dirTreeImageList, TVSIL_NORMAL);

    DestroyIcon(hDriveIcon);
    DestroyIcon(hDirIcon);

    //
    bigFileImageList_ = new CImageList;
    bigFileImageList_->Create(64, 64, ILC_COLOR32, 2, 0);
    bigFileImageList_->SetBkColor(RGB(255, 255, 255));

    mediumFileImageList_ = new CImageList;
    mediumFileImageList_->Create(32, 32, ILC_COLOR32, 2, 0);
    mediumFileImageList_->SetBkColor(RGB(255, 255, 255));

    smallFileImageList_ = new CImageList;
    smallFileImageList_->Create(16, 16, ILC_COLOR32, 2, 0);
    smallFileImageList_->SetBkColor(RGB(255, 255, 255));

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
BOOL CClientDialog::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
        return TRUE;

    return CDialogEx::PreTranslateMessage(pMsg);
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
    ON_NOTIFY(HDN_ITEMCLICK, 0,
        &CClientDialog::OnHdnItemclickListFile)
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
void CClientDialog::OnHdnItemclickListFile(NMHDR *pNMHDR, LRESULT *pResult)
{
    *pResult = 0;

    LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
    ViewMode viewMode =
        static_cast<ViewMode>(viewModeComboBox_.GetCurSel());

    // Report 모드가 아니면 무시한다.
    if (viewMode != ViewMode::Report)
        return;

    // 현재 File List View가 비워져 있으면 무시한다.
    if (curDirTree_ == nullptr)
        return;

    int nColumn = phdr->iItem;

    if (nColumn < 0 || nColumn > 3)
        return;

    fileListSortAscendFlags_[nColumn] = !fileListSortAscendFlags_[nColumn];

    FileInformationCompare_ compare;

    // 비교 함수를 만든다. (일단, 오름차순이라는 가정하에)
    switch (nColumn)
    {
    case 0:  // Name
        compare = [](const auto& f1, const auto& f2) -> int
        {
            return CString(f1.fileName.c_str()).CompareNoCase(f2.fileName.c_str());
        };
        break;
    case 1:  // Modified Date
        compare = [](const auto& f1, const auto& f2) -> int
        {
            if (f1.modifiedDate < f2.modifiedDate)
                return -1;
            else if (f1.modifiedDate > f2.modifiedDate)
                return 1;
            else
                return 0;
        };
        break;
    case 2:  // Type
        compare = [](const auto& f1, const auto& f2) -> int
        {
            if (f1.fileType < f2.fileType)
                return -1;
            else if (f1.fileType > f2.fileType)
                return 1;
            else
                return 0;
        };
        break;
    case 3:  // Size
        compare = [](const auto& f1, const auto& f2) -> int
        {
            if (f1.fileSize < f2.fileSize)
                return -1;
            else if (f1.fileSize > f2.fileSize)
                return 1;
            else
                return 0;
        };
        break;
    default:
        assert(false);
    }

    // 내림차순으로 정렬해야 하는 경우, 위에서 만든 비교함수를 '뒤집는다'.
    if (!fileListSortAscendFlags_[nColumn])
    {
        compare = [compare](const auto& f1, const auto& f2) -> int
        {
            return compare(f2, f1);
        };
    }

    // Item들을 정렬한다.
    fileListControl_.SortItems(&FileListCompareFunc_, (LPARAM) &compare);
}

///////////////////////////////////////////////////////////////////////////////
/*static*/ int CALLBACK CClientDialog::FileListCompareFunc_(
    LPARAM lParam1,
    LPARAM lParam2,
    LPARAM lParamSort)
{
    FileInformationCompare_* compare = (FileInformationCompare_*) lParamSort;

    FileTree* fileTree1 = (FileTree*)lParam1;
    FileTree* fileTree2 = (FileTree*)lParam2;

    assert(fileTree1 != nullptr && fileTree2 != nullptr);

    return (*compare)(fileTree1->f, fileTree2->f);
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
    for (auto i = 0; i < numDrives; ++i)
    {
        CString drivePath = CString(drives[i].letter) + ":";
        CString driveName(drives[i].driveName.c_str());
        
        CString text = driveName + _T('(') + drives[i].letter + _T(":)");

        HTREEITEM hItem = dirTreeControl_.InsertItem(
            text,
            nImageDrive_,
            nImageDrive_);

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
            auto hItem = dirTreeControl_.InsertItem(
                CString(childTree->f.fileName.c_str()),
                nImageDir_,
                nImageDir_,
                hParent,
                hInsertAfter);

            assert(hItem != nullptr);
            childTree->hTreeItem = hItem;
            dirTreeControl_.SetItemData(hItem, (DWORD_PTR) childTree);
        }
        // 숨겨져야하는데 보여져 있는 경우,
        else if (!show && childTree->hTreeItem != nullptr)
        {
            // 자식들을 모두 삭제한다.
            ClearFileTreeChilds_(childTree);

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
        FileIconType fileIconType;
        int imageSize;
        UINT styleFlag;
        CImageList* imageList;
        int nImageListType;

        switch (viewMode)
        {
        case ViewMode::BigIcon:
            fileIconType = FileIconType::Big;
            imageSize = 64;
            styleFlag = LVS_ICON;
            imageList = bigFileImageList_;
            nImageListType = TVSIL_NORMAL;
            break;
        case ViewMode::Icon:
            fileIconType = FileIconType::Medium;
            imageSize = 32;
            styleFlag = LVS_ICON;
            imageList = mediumFileImageList_;
            nImageListType = TVSIL_NORMAL;
            break;
        case ViewMode::Simple:
        default:
            fileIconType = FileIconType::Small;
            imageSize = 16;
            styleFlag = LVS_LIST;
            imageList = smallFileImageList_;
            nImageListType = LVSIL_SMALL;
        }

        fileListControl_.ModifyStyle(
            LVS_ICON | LVS_REPORT | LVS_SMALLICON | LVS_LIST, styleFlag);
        fileListControl_.SetImageList(imageList, nImageListType);

        int nItem = 0;
        for (const auto& childTree : curDirTree_->childs)
        {
            // 보여져야하는 파일이 아니면 무시한다.
            if (!CheckFileShouldBeShown_(childTree.get()))
                continue;

            CString fileName(childTree->f.fileName.c_str());
            bool isDir = childTree->f.fileType == common::FileType::Directory;
            int nImage = GetFileIconImageIndex_(fileName, fileIconType, isDir);

            fileListControl_.InsertItem(nItem, fileName, nImage);
            fileListControl_.SetItemData(nItem++, (DWORD_PTR) childTree.get());
        }
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
        fileListControl_.SetImageList(smallFileImageList_, LVSIL_SMALL);

        // 파일 이름은 처음에 이미 오름차순으로 정렬되있다.
        fileListSortAscendFlags_[0] = true;
        fileListSortAscendFlags_[1] = fileListSortAscendFlags_[2] =
            fileListSortAscendFlags_[3] = false;

        int nItem = 0;
        for (const auto& childTree : curDirTree_->childs)
        {
            // 보여져야하는 파일이 아니면 무시한다.
            if (!CheckFileShouldBeShown_(childTree.get()))
                continue;

            CString fileName(childTree->f.fileName.c_str());
            bool isDir = childTree->f.fileType == common::FileType::Directory;
            int nImage = GetFileIconImageIndex_(
                fileName,
                FileIconType::Small,
                isDir);

            tm* tm = std::localtime(&childTree->f.modifiedDate);

            char dateStringBuffer[30];
            snprintf(dateStringBuffer, 30, "%d-%02d-%02d %02d:%02d:%02d",
                tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
                tm->tm_hour, tm->tm_min, tm->tm_sec);

            auto dateString = CString(dateStringBuffer);
            auto typeString = isDir ? _T("Directory") : _T("File");
            auto sizeString = isDir ? CString(_T("")) :
                CString((std::to_string(childTree->f.fileSize) + " B").c_str());

            int nIndex = fileListControl_.InsertItem(nItem, fileName, nImage);
            fileListControl_.SetItemText(nIndex, 1, dateString);
            fileListControl_.SetItemText(nIndex, 2, typeString);
            fileListControl_.SetItemText(nIndex, 3, sizeString);
            fileListControl_.SetItemData(nItem++, (DWORD_PTR) childTree.get());
        }

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
int CClientDialog::GetFileIconImageIndex_(
    const CString& fileName,
    FileIconType fileIconType,
    bool isDir)
{
    // CString -> file extension.
    // FileIconType -> file icon type.
    // bool -> is directory?
    using MapKey = std::tuple<CString, FileIconType, bool>;

    // 과도한 GDI Object 생성과 ImageList.Add()를 막기 위해,
    //   icon image index pool(cache)을 만들어 쓴다.
    // MapKey -> icon image index 로의 mapping.
    static std::map<MapKey, int> fileImageMap;
    // MapKeyType -> icon handle 로의 mapping.
    //static std::map<MapKey, HICON> fileIconMap;

    // icon을 얻어 올 때는, 전체파일명이 아니라 확장자명만 있으면 된다.
    CString extension = _T("dummy");

    if (!isDir)
    {
        // Parse extension from fileName
        int pos = fileName.ReverseFind('.');
        if (pos != -1)
            extension = fileName.Right(fileName.GetLength() - pos).MakeLower();
    }

    // 필요한 icon image가 cache에 이미 존재하는지를 확인한다.
    MapKey mapKey = std::make_tuple(extension, fileIconType, isDir);
    auto it = fileImageMap.find(mapKey);
    if (it != std::end(fileImageMap))
        return it->second;

    HICON hIcon = nullptr;
    SHFILEINFO sfi = { 0, };
    UINT flag = SHGFI_ICON | SHGFI_USEFILEATTRIBUTES;

    switch (fileIconType)
    {
    case FileIconType::Big:
    case FileIconType::Medium:
        flag |= SHGFI_LARGEICON;
        break;
    case FileIconType::Small:
        flag |= SHGFI_SMALLICON;
        break;
    default:
        assert(false);
    }

    CoInitialize(nullptr);

    // medium icon 혹은 small icon을 구한다.
    HRESULT hResult = SHGetFileInfo(
        extension,
        (isDir ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL),
        &sfi,
        sizeof(sfi),
        flag);

    if (FAILED(hResult))
        return -1;

    hIcon = sfi.hIcon;

    // Big icon이 요청된 경우,
    if (fileIconType == FileIconType::Big)
    {
        // Medium icon이 이미 구해진 상태니,
        //   이걸 image list와 cache에 저장한다.
        {
            MapKey mediumMapKey =
                std::make_tuple(extension, FileIconType::Medium, isDir);
            int nImage = mediumFileImageList_->Add(hIcon);
            assert(nImage >= 0);
            fileImageMap[mediumMapKey] = nImage;
        }

        // System image list를 얻는다.
        HIMAGELIST* systemImageList;
        hResult = SHGetImageList(
            SHIL_JUMBO,
            IID_IImageList,
            (void**) &systemImageList);

        if (SUCCEEDED(hResult))
        {
            // System image list으로 부터, icon을 얻어온다.
            hResult = ((IImageList*) systemImageList)->GetIcon(
                sfi.iIcon,
                ILD_TRANSPARENT,
                &hIcon);

            // Big icon을 구하는 데 성공했으므로,
            //   이전에 구한 medium icon은 삭제한다.
            if (SUCCEEDED(hResult))
                DestroyIcon(sfi.hIcon);
        }
    }

    // 구한 icon을 image list와 cache에 저장한다.
    int nImage;
    switch (fileIconType)
    {
    case FileIconType::Big:
        nImage = bigFileImageList_->Add(hIcon);
        break;
    case FileIconType::Medium:
        nImage = mediumFileImageList_->Add(hIcon);
        break;
    case FileIconType::Small:
        nImage = smallFileImageList_->Add(hIcon);
        break;
    default:
        assert(false);
    }
    fileImageMap[mapKey] = nImage;
    DestroyIcon(hIcon);

    return nImage;
}

} // namespace client
} // namespace mfc
} // namespace remoteFileExplorer
