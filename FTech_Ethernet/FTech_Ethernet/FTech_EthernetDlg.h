
// FTech_EthernetDlg.h : header file
//

#pragma once
#include "resource.h"
#include "afxwin.h"
#include "Ethernet.h"

// CFTech_EthernetDlg dialog
class CFTech_EthernetDlg : public CDialogEx
{
// Construction
public:
	CFTech_EthernetDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_FTech_Ethernet_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	CServerSocket m_Server;
	CClientSocket m_Client;
	CUDPSocket m_UDP;
	int m_rbtnType;

	void OnLogSndMsg(CString strMsg);
	void OnLogRcvMsg(CString strMsg);
	void OnLogSysMsg(CString strMsg);
	CString ConvertToHex(CString strData);
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSend();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnConnection();
	afx_msg void OnBnClickedRbtnCommType(UINT ID);

	afx_msg LRESULT OnServerClosed(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClientAccept(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClientClosed(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnServerRcvMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClientRcvMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUDPRcvMsg(WPARAM wParam, LPARAM lParam);
};
