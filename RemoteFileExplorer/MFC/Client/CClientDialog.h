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

enum class ClientStatus
{
	Connected,
	Disconnected
};

enum class ViewMode
{
	BigIcon = 0,
	Icon,
	Simple,
	Report
};

struct FileTree
{
	std::wstring fullPath;
	common::FileInformation f;
	HTREEITEM hTreeItem{ nullptr };  // Only for directory.
	std::vector<std::unique_ptr<FileTree>> childs;  // Only for directory.
	bool beingDeleted{ false };
	int isVirtual{ 0 };  // 1 -> ".", 2 -> ".."
	FileTree* parent;
};

// CClientDialog 대화 상자입니다.

class CClientDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CClientDialog)

public:
	CClientDialog(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CClientDialog() override;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLIENT };
#endif

protected:
	virtual BOOL OnInitDialog() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedMfcbuttonClientControl();
	afx_msg void OnBnClickedCheckShowSystemFiles();
	afx_msg void OnBnClickedCheckShowHiddenFiles();
	afx_msg void OnTvnSelchangedTreeDirectory(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeComboViewMode();
	afx_msg void OnNMDblclkListFile(NMHDR *pNMHDR, LRESULT *pResult);

private:
	//
	int InitializeView_();
	void ClearView_();

	//
	void ClearFileTreeChilds(FileTree* parentTree);

	//
	void UpdateDirTreeViewAll();
	void UpdateDirTreeView(FileTree* parentTree);

	//
	void UpdateFileListView();

	//
	remoteFileExplorer::client::RemoteFileExplorer remoteFileExplorer_;
	ClientStatus status_{ ClientStatus::Disconnected };

	//
	CIPAddressCtrl ipAddressCtrl_;
	CEdit portEdit_;
	CMFCButton controlButton_;

	//
	FileTree fileTreeVRoot_;
	CTreeCtrl dirTreeControl_;
	HICON hDriveIcon_;
	HICON hFolderIcon_;

	//
	FileTree* curDirTree_{ nullptr };
	CListCtrl fileListControl_;

	//
	CComboBox viewModeComboBox_;

	//
	CButton systemFileCheckBox_;
	bool systemFileShow_;
	CButton hiddenFileCheckBox_;
	bool hiddenFileShow_;
};

} // namespace client
} // namespace mfc
} // namespace remoteFileExplorer