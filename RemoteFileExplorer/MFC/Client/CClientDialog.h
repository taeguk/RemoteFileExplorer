#pragma once

#include <afxwin.h>
#include <afxcmn.h>
#include <afxbutton.h>

#include "MFC/resource.h"
#include "Client/RemoteFileExplorer.h"

namespace remoteFileExplorer
{
namespace mfc
{
namespace client
{
///////////////////////////////////////////////////////////////////////////////
enum class ClientStatus
{
    Connected,
    Disconnected
};

///////////////////////////////////////////////////////////////////////////////
enum class ViewMode
{
    BigIcon = 0,
    Icon,
    Simple,
    Report
};

///////////////////////////////////////////////////////////////////////////////
enum class FileSpecialBehavior
{
    None,
    CurrentDirectory,  // "." Directory
    ParentDirectory    // ".." Directory
};

///////////////////////////////////////////////////////////////////////////////
enum class FileIconType
{
    Big,
    Medium,
    Small,
};

///////////////////////////////////////////////////////////////////////////////
// 트리 구조로 디렉토리/파일 데이터를관리하기 위한 자료구조이다.
// Directory Tree Control (=Directory Tree View) 와
//   File List Box Control (=File List View) 를 위한
//   backbone 자료구조 역할을 한다.
// Dir Tree View와 File List View는 모두 이 자료구조를 바탕으로 형성된다.
struct FileTree
{
    std::wstring fullPath;
    common::FileInformation f;

    FileTree* parent;
    std::vector<std::unique_ptr<FileTree>> childs; // Only for directory.

    // Directory Tree Control에서의 item handle을 나타낸다.
    // 만약 값이 nullptr이라면, tree view에 보여지고 있지 않음을 의미한다.
    // 파일은 항상 이 값이 nullptr이다. (tree view에 보여지지 않으므로.)
    HTREEITEM hTreeItem{ nullptr }; // Only for directory.
    FileSpecialBehavior specialBehavior;
};

///////////////////////////////////////////////////////////////////////////////
// Client의 UI를 담당한다.
class CClientDialog : public CDialogEx
{
    DECLARE_DYNAMIC(CClientDialog)

public:
    CClientDialog(CWnd* pParent = nullptr);
    virtual ~CClientDialog() override;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_CLIENT };
#endif

protected:
    virtual BOOL OnInitDialog() override;
    virtual void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV 지원입니다.

    DECLARE_MESSAGE_MAP()
    afx_msg void OnBnClickedMfcbuttonClientControl();
    afx_msg void OnBnClickedCheckShowSystemFiles();
    afx_msg void OnBnClickedCheckShowHiddenFiles();
    afx_msg void OnCbnSelchangeComboViewMode();
    // Dir Tree View에서 선택된 item이 바뀌었을 때 호출되는 함수.
    afx_msg void OnTvnSelchangedTreeDirectory(NMHDR *pNMHDR, LRESULT *pResult);
    // File List View에서 더블클릭이 발생했을 때 호출되는 함수.
    afx_msg void OnNMDblclkListFile(NMHDR *pNMHDR, LRESULT *pResult);

private:
    ///////////////////////////////////////////////////////////////////////////////
    void ConnectToServer_();
    void DisconnectToServer_();

    // 모든 View를 초기화하는 함수.
    int InitializeView_();
    // 모든 View를 비우는 함수.
    void ClearView_();

    // parentTree의 자식트리들을 전부 삭제하는 함수.
    // (자기자신은 삭제하지 않는다.)
    void ClearFileTreeChilds_(FileTree* parentTree);

    // Dir Tree View를 root부터 몽땅 업데이트하는 함수.
    void UpdateDirTreeViewAll_();
    // Dir Tree View에서 parentTree에 해당하는 item밑으로 업데이트하는 함수.
    void UpdateDirTreeView_(FileTree* parentTree);
    // File List View를 업데이트하는 함수.
    void UpdateFileListView_();

    // 파일이 View들에서 보여져야하는 지 아닌지를 반환하는 함수.
    bool CheckFileShouldBeShown_(FileTree* fileTree);

    // File List Control의 ImageList에서 file icon image의 index를 얻는 함수.
    int GetFileIconImageIndex_(
        const CString& fileName,
        FileIconType fileIconSizeType,
        bool isDir);

    ///////////////////////////////////////////////////////////////////////////////
    remoteFileExplorer::client::RemoteFileExplorer remoteFileExplorer_;
    ClientStatus status_{ ClientStatus::Disconnected };

    CIPAddressCtrl ipAddressCtrl_;
    CEdit portEdit_;
    CMFCButton controlButton_;

    CTreeCtrl dirTreeControl_;
    // 가상의 root. 이 root 밑에는 drive들이 위치한다.
    FileTree fileTreeVRoot_;
    // Dir Tree Control의 ImageList에서 drive icon image의 index.
    int nImageDrive_;
    // Dir Tree Control의 ImageList에서 directory icon image의 index.
    int nImageDir_;

    CListCtrl fileListControl_;
    // File List Control에서 사용되는 ImageList들.
    // 각각 큰 아이콘, 보통 아이콘, 작은 아이콘을 위한 ImageList이다.
    CImageList* bigFileImageList_;
    CImageList* mediumFileImageList_;
    CImageList* smallFileImageList_;

    // 현재 File List View가 나타내고 있는 directory를 의미한다.
    FileTree* curDirTree_{ nullptr };

    CComboBox viewModeComboBox_;

    CButton systemFileCheckBox_;
    bool systemFileShow_;
    CButton hiddenFileCheckBox_;
    bool hiddenFileShow_;
};

} // namespace client
} // namespace mfc
} // namespace remoteFileExplorer