
// FTech_EthernetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FTech_Ethernet.h"
#include "FTech_EthernetDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFTech_EthernetDlg dialog

CFTech_EthernetDlg::CFTech_EthernetDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFTech_EthernetDlg::IDD, pParent)
	, m_rbtnType(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFTech_EthernetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RBTN_SERVER, m_rbtnType);
}

BEGIN_MESSAGE_MAP(CFTech_EthernetDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_SEND, &CFTech_EthernetDlg::OnBnClickedSend)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_CONNECTION, &CFTech_EthernetDlg::OnBnClickedBtnConnection)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RBTN_SERVER, IDC_RBTN_UDP, &CFTech_EthernetDlg::OnBnClickedRbtnCommType)
	ON_MESSAGE(WM_SERVER_CLOSED, &CFTech_EthernetDlg::OnServerClosed)
	ON_MESSAGE(WM_CLIENT_ACCEPT, &CFTech_EthernetDlg::OnClientAccept)
	ON_MESSAGE(WM_CLIENT_CLOSED, &CFTech_EthernetDlg::OnClientClosed)
	ON_MESSAGE(WM_SERVER_RCV_MSG, &CFTech_EthernetDlg::OnServerRcvMsg)
	ON_MESSAGE(WM_CLIENT_RCV_MSG, &CFTech_EthernetDlg::OnClientRcvMsg)
	ON_MESSAGE(WM_UDP_RCV_MSG, &CFTech_EthernetDlg::OnUDPRcvMsg)
END_MESSAGE_MAP()


// CFTech_EthernetDlg message handlers

BOOL CFTech_EthernetDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CIPAddressCtrl *pCtrl = (CIPAddressCtrl *)GetDlgItem(IDC_IP_CLIENT);
	pCtrl->SetAddress(192,168,0,39);
	SetDlgItemInt(IDC_EDIT_CLIENT_PORT,50000);
	SetDlgItemInt(IDC_EDIT_SERVER_PORT,0);
	pCtrl->EnableWindow(FALSE);

	CComboBox* pCB = (CComboBox*)GetDlgItem(IDC_CB_SENDTYPE);
	pCB->SetCurSel(0);

	pCB = (CComboBox*)GetDlgItem(IDC_CB_RCVTYPE);
	pCB->SetCurSel(0);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFTech_EthernetDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFTech_EthernetDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CFTech_EthernetDlg::OnBnClickedSend()
{
	UpdateData(TRUE);

	CString strMsg;
	GetDlgItemText(IDC_EDIT_SEND, strMsg);

	CComboBox* pCB = (CComboBox*)GetDlgItem(IDC_CB_SENDTYPE);
	int type = pCB->GetCurSel();

	bool bHEX=false;
	type == 0 ? bHEX = true : bHEX = false;

	bool bSTX=false, bETX=false, bCRLF=false;
	int state = IsDlgButtonChecked(IDC_CHK_STX);
	if (state == 1) bSTX = true;

	state = IsDlgButtonChecked(IDC_CHK_ETX);
	if (state == 1) bETX = true;

	state = IsDlgButtonChecked(IDC_CHK_CRLF);
	if (state == 1) bCRLF = true;

	bool bRet = false;
	switch (m_rbtnType)
	{
		// TCP Server
	case 0 :
		bRet = m_Server.OnSndMsgToAll(bHEX, bSTX, strMsg, bETX, bCRLF);
		break;
		// TCP Client
	case 1 :
		bRet = m_Client.OnSndMsg(bHEX, bSTX, strMsg, bETX, bCRLF);
		break;
		// UDP
	case 2 :
		bRet = m_UDP.OnSndMsg(bHEX, bSTX, strMsg, bETX, bCRLF);
		break;
	}

	if (bRet == true)
	{
		CString strLog=_T("");

		SYSTEMTIME SysTime;
		GetLocalTime(&SysTime);

		CString strTime;
		strTime.Format(_T("[%02d:%02d:%02d:%03d] :"),SysTime.wHour,SysTime.wMinute,SysTime.wSecond,SysTime.wMilliseconds);

		if (bSTX == true)
			strMsg = _T("[02]") + strMsg;
		if (bETX == true)
			strMsg = strMsg + _T("[03]");
		if (bCRLF == true)
			strMsg = strMsg + _T("[0D0A]");

		strLog = strTime + strMsg;

		OnLogSndMsg(strLog);
	}

	SetDlgItemText(IDC_EDIT_SEND, _T(""));
	UpdateData(FALSE);
}

void CFTech_EthernetDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	if (m_Server.IsOpened() == true)
	{
		m_Server.OnClose();
	}
}

void CFTech_EthernetDlg::OnBnClickedBtnConnection()
{
	CString caption;
	GetDlgItemText(IDC_BTN_CONNECTION, caption);

	if (caption == _T("Open"))
	{
		CIPAddressCtrl *pCtrl = (CIPAddressCtrl *)GetDlgItem(IDC_IP_CLIENT);
		BYTE addr[4] = {0,};
		pCtrl->GetAddress(addr[0],addr[1],addr[2],addr[3]);

		int nPort = GetDlgItemInt(IDC_EDIT_CLIENT_PORT);
		CString strIP=_T("");
		strIP.Format(_T("%d.%d.%d.%d"), addr[0],addr[1],addr[2],addr[3]);
		
		bool bRet=false;
		switch (m_rbtnType)
		{
			// TCP Server
		case 0 :
			bRet = m_Server.OnOpen(this->GetSafeHwnd(), nPort);
			break;
			// TCP Client
		case 1 :
			bRet = m_Client.OnOpen(this->GetSafeHwnd(), strIP, nPort, 1000);
			break;
			// UDP
		case 2 :
			bRet = m_UDP.OnOpen(this->GetSafeHwnd(), strIP, nPort);
			break;
		}

		if (bRet == true)
		{
			SetDlgItemText(IDC_BTN_CONNECTION, _T("Close"));
			GetDlgItem(IDC_IP_CLIENT)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_CLIENT_PORT)->EnableWindow(FALSE);
		}
	}
	else
	{
		switch (m_rbtnType)
		{
			// TCP Server
		case 0 :
			m_Server.OnClose();
			break;
			// TCP Client
		case 1 :
			m_Client.OnClose();
			break;
			// UDP
		case 2 :
			m_UDP.OnClose();	
			break;
		}

		SetDlgItemText(IDC_BTN_CONNECTION, _T("Open"));
		GetDlgItem(IDC_IP_CLIENT)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_CLIENT_PORT)->EnableWindow(TRUE);
	}
}

CString CFTech_EthernetDlg::ConvertToHex(CString strData)
{
	CString returnvalue;
	for (int x = 0; x < strData.GetLength(); x++)
	{
		CString temporary;
		int value = (int)(strData[x]);
		temporary.Format(_T("%02X "), value);
		returnvalue += temporary;
	}
	return returnvalue;
}

void CFTech_EthernetDlg::OnBnClickedRbtnCommType(UINT ID)
{
	UpdateData(TRUE);

	switch (m_rbtnType)
	{
		// TCP Server
	case 0 :
		GetDlgItem(IDC_IP)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_PORT)->EnableWindow(TRUE);
		break;
		// TCP Client
	case 1 :
		GetDlgItem(IDC_IP)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_PORT)->EnableWindow(TRUE);
		break;
		// UDP
	case 2 :
		GetDlgItem(IDC_IP)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_PORT)->EnableWindow(TRUE);
		break;
	}
}

LRESULT CFTech_EthernetDlg::OnServerClosed(WPARAM wParam, LPARAM lParam)
{
	SetDlgItemText(IDC_BTN_CONNECTION, _T("Open"));
	GetDlgItem(IDC_IP_CLIENT)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_CLIENT_PORT)->EnableWindow(TRUE);

	UINT uPort = GetDlgItemInt(IDC_EDIT_PORT);

	CString strLog=_T(""), strInfo=_T(""), strMsg=_T("");
	strInfo.Format(_T("[%d] "), uPort);

	SYSTEMTIME SysTime;
	GetLocalTime(&SysTime);

	CString strTime;
	strTime.Format(_T("[%02d:%02d:%02d:%03d] :"),SysTime.wHour,SysTime.wMinute,SysTime.wSecond,SysTime.wMilliseconds);

	strMsg = _T("The Server is closed.");
	strLog = strTime + strInfo + strMsg;

	OnLogSysMsg(strLog);

	return 0;
}

LRESULT CFTech_EthernetDlg::OnClientAccept(WPARAM wParam, LPARAM lParam)
{
	CServerSocket *pSocket = (CServerSocket*)wParam;

	CString strIP = pSocket->GetLastRcvIP();
	UINT uPort = pSocket->GetLastRcvPort();

	CString strLog=_T(""), strInfo=_T(""), strMsg=_T("");
	strInfo.Format(_T("[%s:%d] "), strIP, uPort);

	SYSTEMTIME SysTime;
	GetLocalTime(&SysTime);

	CString strTime;
	strTime.Format(_T("[%02d:%02d:%02d:%03d] :"),SysTime.wHour,SysTime.wMinute,SysTime.wSecond,SysTime.wMilliseconds);

	strMsg = _T("The Client is accpeted.");
	strLog = strTime + strInfo + strMsg;

	OnLogSysMsg(strLog);
	
	return 0;
}

LRESULT CFTech_EthernetDlg::OnClientClosed(WPARAM wParam, LPARAM lParam)
{
	CServerSocket *pSocket = (CServerSocket*)wParam;

	CString strIP = pSocket->GetLastRcvIP();
	UINT uPort = pSocket->GetLastRcvPort();

	CString strLog=_T(""), strInfo=_T(""), strMsg=_T("");
	strInfo.Format(_T("[%s:%d] "), strIP, uPort);

	SYSTEMTIME SysTime;
	GetLocalTime(&SysTime);

	CString strTime;
	strTime.Format(_T("[%02d:%02d:%02d:%03d] :"),SysTime.wHour,SysTime.wMinute,SysTime.wSecond,SysTime.wMilliseconds);

	strMsg = _T("The Client is closed.");
	strLog = strTime + strInfo + strMsg;

	OnLogSysMsg(strLog);

	return 0;
}

LRESULT CFTech_EthernetDlg::OnServerRcvMsg(WPARAM wParam, LPARAM lParam)
{
	CServerSocket *pSocket = (CServerSocket*)wParam;
	
	CString strMsg=_T("");
	CString strIP = pSocket->GetLastRcvIP();
	UINT uPort = pSocket->GetLastRcvPort();

	CString strLog=_T(""), strInfo=_T("");
	strInfo.Format(_T("[%s:%d] "), strIP, uPort);

	SYSTEMTIME SysTime;
	GetLocalTime(&SysTime);

	CString strTime;
	strTime.Format(_T("[%02d:%02d:%02d:%03d] :"),SysTime.wHour,SysTime.wMinute,SysTime.wSecond,SysTime.wMilliseconds);

	CComboBox *pCB = (CComboBox*)GetDlgItem(IDC_CB_RCVTYPE);
	int type = pCB->GetCurSel();

	if (type == 0)
		strMsg = pSocket->GetLastRcvMsgHex();
	else
		strMsg = pSocket->GetLastRcvMsgAscii();

	strLog = strTime + strInfo + strMsg;

	OnLogRcvMsg(strLog);

	return 0;
}

LRESULT CFTech_EthernetDlg::OnClientRcvMsg(WPARAM wParam, LPARAM lParam)
{
	CClientSocket *pSocket = (CClientSocket*)wParam;

	CString strMsg = _T("");
	CString strIP = pSocket->GetLastRcvIP();
	UINT uPort = pSocket->GetLastRcvPort();

	CString strLog=_T(""), strInfo=_T("");
	strInfo.Format(_T("[%s:%d] "), strIP, uPort);
	
	SYSTEMTIME SysTime;
	GetLocalTime(&SysTime);

	CString strTime;
	strTime.Format(_T("[%02d:%02d:%02d:%03d] :"),SysTime.wHour,SysTime.wMinute,SysTime.wSecond,SysTime.wMilliseconds);

	CComboBox *pCB = (CComboBox*)GetDlgItem(IDC_CB_RCVTYPE);
	int type = pCB->GetCurSel();

	if (type == 0)
		strMsg = pSocket->GetLastRcvMsgHex();
	else
		strMsg = pSocket->GetLastRcvMsgAscii();

	strLog = strTime + strInfo + strMsg;

	OnLogRcvMsg(strLog);

	return 0;
}

LRESULT CFTech_EthernetDlg::OnUDPRcvMsg(WPARAM wParam, LPARAM lParam)
{
	CUDPSocket *pSocket = (CUDPSocket*)wParam;

	CString strMsg = _T("");
	CString strIP = pSocket->GetLastRcvIP();
	UINT uPort = pSocket->GetLastRcvPort();

	CString strLog=_T(""), strInfo=_T("");
	strInfo.Format(_T("[%s:%d] "), strIP, uPort);

	SYSTEMTIME SysTime;
	GetLocalTime(&SysTime);

	CString strTime;
	strTime.Format(_T("[%02d:%02d:%02d:%03d] :"),SysTime.wHour,SysTime.wMinute,SysTime.wSecond,SysTime.wMilliseconds);

	CComboBox *pCB = (CComboBox*)GetDlgItem(IDC_CB_RCVTYPE);
	int type = pCB->GetCurSel();

	if (type == 0)
		strMsg = pSocket->GetLastRcvMsgHex();
	else
		strMsg = pSocket->GetLastRcvMsgAscii();

	strLog = strTime + strInfo + strMsg;

	OnLogRcvMsg(strLog);

	return 0;
}

void CFTech_EthernetDlg::OnLogSndMsg(CString strMsg)
{
	CListBox *pLtb = NULL;
	pLtb = (CListBox*)GetDlgItem(IDC_LTB_SEND);

	pLtb->AddString(strMsg);

	pLtb->SetCurSel(pLtb->GetCount()-1);
}

void CFTech_EthernetDlg::OnLogRcvMsg(CString strMsg)
{
	CListBox *pLtb = NULL;
	pLtb = (CListBox*)GetDlgItem(IDC_LTB_RCV);

	pLtb->AddString(strMsg);

	pLtb->SetCurSel(pLtb->GetCount()-1);
}

void CFTech_EthernetDlg::OnLogSysMsg(CString strMsg)
{
	CListBox *pLtb = NULL;
	pLtb = (CListBox*)GetDlgItem(IDC_LTB_SYSTEM);

	pLtb->AddString(strMsg);

	pLtb->SetCurSel(pLtb->GetCount()-1);
}