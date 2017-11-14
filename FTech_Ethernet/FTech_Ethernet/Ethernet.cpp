// ListenSocket.cpp : implementation file
//

#include "stdafx.h"
#include "Ethernet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// Common Functions.
CString CCommon::HexToData(bool bSTX, CString strMsg, bool bETX, bool bCRLF)
{
	CString strData=_T("");
	
	strData = strMsg;

	strData.MakeUpper();
	strData.Replace(_T(" "),_T(""));	//°ø¹éÀ» ¾ø¾Ú.
	strData.Replace(_T("\r\n"),_T(""));	//ÁÙ³Ñ±èÀ» ¾ø¾Ú.

	if(bSTX == TRUE)
		strData = _T("02") + strData;	// [STX] + Msg

	if(bETX == TRUE)
		strData += _T("03");			// Msg + [ETX]

	if(bCRLF == TRUE)
		strData += _T("0D0A");			// Msg + [CR] + [LF]

	return strData;
}

CString CCommon::AsciiToData(bool bSTX, CString strMsg, bool bETX, bool bCRLF)
{
	CString strData=_T(""), strTmp=_T("");

	int nLen = strMsg.GetLength();
	for(int i = 0; i < nLen; i++)
	{
		strTmp.Format(_T("%X"),strMsg[i]);
		strData += strTmp;
	}

	if(bSTX == TRUE)
		strData = _T("02") + strData;	// [STX] + Msg

	if(bETX == TRUE)
		strData += _T("03");			// Msg + [ETX]

	if(bCRLF == TRUE)
		strData += _T("0D0A");			// Msg + [CR] + [LF]

	return strData;
}

void CCommon::DataToByte(CString strMsg, byte* pbyMsg, int nLen)
{
	CString strHex=_T(""), strTmp=_T("");
	int nLength=0, iBufPos=0;
	BYTE byHigh, byLow;
	BYTE* Send_buff = NULL;

	strHex = strMsg;
	nLength = strMsg.GetLength();

	Send_buff = new BYTE[nLen];
	memset(Send_buff,0,nLen);
	
	for(int i = 0 ; i < nLength; i+=2)
	{
		byHigh = CodeToAsciiValue((char)strMsg[i]);
		byLow  = CodeToAsciiValue((char)strMsg[i+1]);
		Send_buff[iBufPos++] = (byHigh <<4) | byLow;
	}

	memcpy(pbyMsg, Send_buff, nLen);

	delete []Send_buff;
	Send_buff = NULL;
}

BYTE CCommon::CodeToAsciiValue(char cData)
{
	BYTE byAsciiValue;
	if( ( '0' <= cData ) && ( cData <='9' ) )
	{
		byAsciiValue = cData - '0';
	}
	else if( ( 'A' <= cData ) && ( cData <= 'F' ) )
	{
		byAsciiValue = (cData - 'A') + 10;
	}
	else if( ( 'a' <= cData ) && ( cData <= 'f' ) )
	{
		byAsciiValue = (cData - 'a') + 10;
	}
	else
	{
		byAsciiValue = 0;
	}
	return byAsciiValue;
}

//////////////////////////////////////////////////////////////////////////
// Server Socket.
CServerSocket::CServerSocket()
{
	m_tRcvInfo.strIP = _T("");
	m_tRcvInfo.uPort = 0;
	m_tRcvInfo.strHex = _T("");
	m_tRcvInfo.strAscii = _T("");
	m_hWnd = NULL;
	m_isOpened = false;
	m_uPort = 0;
}

CServerSocket::~CServerSocket()
{
	Close();

	m_tRcvInfo.strIP = _T("");
	m_tRcvInfo.uPort = 0;
	m_tRcvInfo.strHex = _T("");
	m_tRcvInfo.strAscii = _T("");
	m_hWnd = NULL;
	m_isOpened = false;
	m_uPort = 0;
}

bool CServerSocket::OnOpen(HWND hWnd, UINT uPort)
{
	BOOL bRet=FALSE;
	bRet = Create(uPort,SOCK_STREAM);
	if (bRet == FALSE)
		return false;

	m_hWnd = hWnd;

	bRet = Listen();
	if (bRet == FALSE)
	{
		Close();
		return false;
	}

	m_isOpened = true;

	m_uPort = uPort;

	return true;
}

bool CServerSocket::OnClose()
{
	if (m_isOpened == false) return false;

	POSITION pos;
	pos = m_ptrClientSocketList.GetHeadPosition();
	CDataSocket* pClient = NULL;
	BOOL bRet=FALSE;

	while (pos != NULL)
	{
		pClient = (CDataSocket*)m_ptrClientSocketList.GetNext(pos);

		if (pClient != NULL)
		{
			bRet = pClient->ShutDown();
			if (bRet == FALSE)
				return false;

			pClient->Close();

			delete pClient;
			pClient = NULL;

			m_ptrClientSocketList.RemoveHead();
		}
	}

	Close();

	m_isOpened = false;
	m_uPort = 0;

	return true;
}

void CServerSocket::OnAccept(int nErrorCode)
{
	CDataSocket* pClient = new CDataSocket;

	if (Accept(*pClient) == TRUE)
	{
		pClient->SetServerSocket(this);
		m_ptrClientSocketList.AddTail(pClient);

		CString strIP=_T("");
		UINT uPort=0;

		pClient->GetPeerName(strIP, uPort);
		m_tRcvInfo.strIP = strIP;
		m_tRcvInfo.uPort = uPort;

		::SendMessage(m_hWnd, WM_CLIENT_ACCEPT, (WPARAM)this, NULL);
	}
	else
	{
		delete pClient;
		pClient = NULL;
	}

	CAsyncSocket::OnAccept(nErrorCode);
}

void CServerSocket::CloseClientSocket(CAsyncSocket* pClient)
{
	POSITION pos;
	pos = m_ptrClientSocketList.Find(pClient);
	if (pos != NULL)
	{
		if (pClient != NULL)
		{
			CString strIP=_T("");
			UINT uPort=0;

			pClient->GetPeerName(strIP, uPort);
			m_tRcvInfo.strIP = strIP;
			m_tRcvInfo.uPort = uPort;

			::SendMessage(m_hWnd, WM_CLIENT_CLOSED, (WPARAM)this, NULL);

			pClient->ShutDown();
			pClient->Close();

			delete pClient;
			pClient = NULL;

			m_ptrClientSocketList.RemoveAt(pos);
		}
	}
}

bool CServerSocket::OnSndMsgToAll(bool bHEX, bool bSTX, CString strMsg, bool bETX, bool bCRLF)
{
	POSITION pos;
	pos = m_ptrClientSocketList.GetHeadPosition();
	
	CDataSocket* pClient = NULL;
	int nRet = -1;

	while(pos != NULL)
	{
		pClient = (CDataSocket*)m_ptrClientSocketList.GetNext(pos);
		if (pClient != NULL)
		{
			CString strData = _T("");
			if (bHEX == true)
				strData = HexToData(bSTX, strMsg, bETX, bCRLF);
			else
				strData = AsciiToData(bSTX, strMsg, bETX, bCRLF);

			int nLen = strData.GetLength();

			if (bHEX == true)
				nLen /= 2;

			byte* pbyMsg = new byte[nLen];
			ZeroMemory(pbyMsg, nLen);

			DataToByte(strData, pbyMsg, nLen);

			nRet = pClient->Send(pbyMsg, nLen);

			delete []pbyMsg;
			pbyMsg = NULL;
		}
	}

	if (nRet == -1)
		return false;

	return true;
}

bool CServerSocket::OnSndMsg(CString strIP, bool bHEX, bool bSTX, CString strMsg, bool bETX, bool bCRLF)
{
	POSITION pos;
	pos = m_ptrClientSocketList.GetHeadPosition();

	CDataSocket* pClient = NULL;
	int nRet = -1;

	while(pos != NULL)
	{
		pClient = (CDataSocket*)m_ptrClientSocketList.GetNext(pos);
		if (pClient != NULL)
		{
			CString strSckIP=_T("");
			UINT uPort=0;
			pClient->GetPeerName(strSckIP, uPort);

			if (strIP == strSckIP)
			{
				CString strData = _T("");
				if (bHEX == true)
					strData = HexToData(bSTX, strMsg, bETX, bCRLF);
				else
					strData = AsciiToData(bSTX, strMsg, bETX, bCRLF);

				int nLen = strData.GetLength();

				if (bHEX == true)
					nLen /= 2;

				byte* pbyMsg = new byte[nLen];
				ZeroMemory(pbyMsg, nLen);

				DataToByte(strData, pbyMsg, nLen);

				nRet = pClient->Send(pbyMsg, nLen);

				delete []pbyMsg;
				pbyMsg = NULL;

				break;
			}
		}
	}

	if (nRet == -1)
		return false;

	return true;
}

void CServerSocket::OnRcvMsgEvent(tLastRcvInfo *info)
{
	if (m_hWnd != NULL)
	{
		m_tRcvInfo.strIP = info->strIP;
		m_tRcvInfo.uPort = info->uPort;
		m_tRcvInfo.strHex = info->strHex;
		m_tRcvInfo.strAscii = info->strAscii;

		::SendMessage(m_hWnd, WM_SERVER_RCV_MSG, (WPARAM)this, NULL);
	}
}

//////////////////////////////////////////////////////////////////////////
// Data Socket.
CDataSocket::CDataSocket()
{
	m_pSocket = NULL;
}

CDataSocket::~CDataSocket()
{
}

void CDataSocket::SetServerSocket(CAsyncSocket* pSocket)
{
	m_pSocket = pSocket;
}

void CDataSocket::OnClose(int nErrorCode)
{
	CAsyncSocket::OnClose(nErrorCode);

	CServerSocket* pServerSocket = (CServerSocket*)m_pSocket;
	pServerSocket->CloseClientSocket(this);
}


void CDataSocket::OnReceive(int nErrorCode)
{
	char szBuffer[MAX_BUFFER_SIZE];
	::ZeroMemory(szBuffer, sizeof(szBuffer));

	CString strIP=_T(""), strMsg=_T(""), strHex=_T(""), strAscii=_T("");
	UINT uPort = 0;

	GetPeerName(strIP, uPort);

	int nLen = Receive(szBuffer, sizeof(szBuffer));

	char* pBuff = new char[nLen*5+10];
	ZeroMemory(pBuff, nLen*5+10);

	if (nLen != SOCKET_ERROR)
	{
		int n=0;
		for (int i=0; i<nLen; i++)
		{
			n += sprintf_s (pBuff+n, nLen*5+10-n, "%02X", (unsigned int)(unsigned char)szBuffer[i]);
			if (i+1 < nLen) n += sprintf_s (pBuff+n, nLen*5+10-n, " "); //Hex 2byte ¶ç¾î¾²±â
		}
		
		strAscii = (char*)szBuffer;
		strHex = (CString)pBuff;
		delete []pBuff;
		pBuff = NULL;
	}

	tLastRcvInfo info;
	info.strIP = strIP;
	info.uPort = uPort;
	info.strHex = strHex;
	info.strAscii = strAscii;

	CServerSocket* pServerSocket = (CServerSocket*)m_pSocket;
	pServerSocket->OnRcvMsgEvent(&info);

	CAsyncSocket::OnReceive(nErrorCode);
}

//////////////////////////////////////////////////////////////////////////
// Client Socket.
CClientSocket::CClientSocket()
{
	m_tRcvInfo.strIP = _T("");
	m_tRcvInfo.uPort = 0;
	m_tRcvInfo.strHex = _T("");
	m_tRcvInfo.strAscii = _T("");
	m_hWnd = NULL;
	m_isOpened = false;
}

CClientSocket::~CClientSocket()
{
	Close();

	m_tRcvInfo.strIP = _T("");
	m_tRcvInfo.uPort = 0;
	m_tRcvInfo.strHex = _T("");
	m_tRcvInfo.strAscii = _T("");
	m_hWnd = NULL;
	m_isOpened = false;
}

void CClientSocket::OnClose()
{
	m_tRcvInfo.strIP = _T("");
	m_tRcvInfo.uPort = 0;
	m_tRcvInfo.strHex = _T("");
	m_tRcvInfo.strAscii = _T("");
	m_hWnd = NULL;
	m_isOpened = false;

	CAsyncSocket::Close();
}

void CClientSocket::OnClose(int nErrorCode)
{
	CAsyncSocket::OnClose(nErrorCode);

	::SendMessage(m_hWnd, WM_SERVER_CLOSED, (WPARAM)this, NULL);

	m_tRcvInfo.uPort = 0;
	m_tRcvInfo.strIP = _T("");
	m_hWnd = NULL;
	m_isOpened = false;

	CAsyncSocket::Close();
}

bool CClientSocket::OnSndMsg(bool bHEX, bool bSTX, CString strMsg, bool bETX, bool bCRLF)
{
	int nRet = -1;
	CString strData = _T("");
	if (bHEX == true)
		strData = HexToData(bSTX, strMsg, bETX, bCRLF);
	else
		strData = AsciiToData(bSTX, strMsg, bETX, bCRLF);

	int nLen = strData.GetLength();

	if (bHEX == true)
		nLen /= 2;

	byte* pbyMsg = new byte[nLen];
	ZeroMemory(pbyMsg, nLen);

	DataToByte(strData, pbyMsg, nLen);

	nRet = Send(pbyMsg, nLen);

	delete []pbyMsg;
	pbyMsg = NULL;

	if (nRet == -1)
		return false;
	
	return true;
}

void CClientSocket::OnReceive(int nErrorCode)
{
	BYTE szBuffer[MAX_BUFFER_SIZE];
	::ZeroMemory(szBuffer,sizeof(szBuffer));

	int nLen = Receive(szBuffer, sizeof(szBuffer));
	
	char* pBuff = new char[nLen*5+10];
	ZeroMemory(pBuff, nLen*5+10);

	if (nLen != SOCKET_ERROR)
	{
		int n=0;
		for (int i=0; i<nLen; i++)
		{
			n += sprintf_s (pBuff+n, nLen*5+10-n, "%02X", (unsigned int)(unsigned char)szBuffer[i]);
			if (i+1 < nLen) n += sprintf_s (pBuff+n, nLen*5+10-n, " "); //Hex 2byte ¶ç¾î¾²±â
		}

		m_tRcvInfo.strAscii = (char*)szBuffer;
		m_tRcvInfo.strHex = (CString)pBuff;
		delete []pBuff;
		pBuff = NULL;
	}

	if (m_hWnd != NULL)
		::SendMessage(m_hWnd, WM_CLIENT_RCV_MSG, (WPARAM)this, NULL);

	CAsyncSocket::OnReceive(nErrorCode);
}

bool CClientSocket::OnOpen(HWND hWnd, CString strIP, UINT uPort, int nTimeoutMilliSec)
{
	BOOL bRet = Create();
	if (bRet == FALSE)
		return false;

	TIMEVAL Timeout;
	Timeout.tv_sec = 0;
	Timeout.tv_usec = nTimeoutMilliSec*1000;

	wchar_t* wchar_str;     
	char*    char_str;      
	int      char_str_len;  
	wchar_str = strIP.GetBuffer(strIP.GetLength());

	char_str_len = WideCharToMultiByte(CP_ACP, 0, wchar_str, -1, NULL, 0, NULL, NULL);
	char_str = new char[char_str_len];
	WideCharToMultiByte(CP_ACP, 0, wchar_str, -1, char_str, char_str_len, 0,0);  

	sockaddr_in address; 
	address.sin_addr.s_addr = inet_addr(char_str);
	address.sin_port = htons(uPort); 
	address.sin_family = AF_INET;

	delete char_str;
	char_str = NULL;

	unsigned long iMode = 1;
	int iResult = ioctlsocket(m_hSocket, FIONBIO, &iMode);
	if (iResult != NO_ERROR)
		return false;

	if(connect(m_hSocket,(struct sockaddr *)&address,sizeof(address))==false)
	{
		Close();
		return false;
	}

	fd_set Write, Err;
	FD_ZERO(&Write);
	FD_ZERO(&Err);
	FD_SET(m_hSocket, &Write);
	FD_SET(m_hSocket, &Err);

	select(0,NULL,&Write,&Err,&Timeout);			
	if(FD_ISSET(m_hSocket, &Write)) 
	{
		m_hWnd = hWnd;
		m_tRcvInfo.uPort = uPort;
		m_tRcvInfo.strIP = strIP;
		m_isOpened = true;

		return true;
	}

	Close();
	return false;
}


//////////////////////////////////////////////////////////////////////////
// UDP Socket.
CUDPSocket::CUDPSocket()
{
	m_tRcvInfo.strIP = _T("");
	m_tRcvInfo.uPort = 0;
	m_tRcvInfo.strHex = _T("");
	m_tRcvInfo.strAscii = _T("");
	m_strIP = _T("");
	m_hWnd	= NULL;
	m_uPort = 0;
}

CUDPSocket::~CUDPSocket()
{
	OnClose();
}

bool CUDPSocket::OnOpen(HWND hWnd, CString strIP, UINT uPort)
{ 
	bool bRet=false;
	m_strIP = strIP;
	m_uPort = uPort;
	m_hWnd = hWnd;
	if (CAsyncSocket::Create(m_uPort, SOCK_DGRAM) == TRUE)
	{
		m_isOpened = true;
		return true;
	}
	else
		return false;
}

void CUDPSocket::OnClose()
{
	m_tRcvInfo.strIP = _T("");
	m_tRcvInfo.uPort = 0;
	m_tRcvInfo.strHex = _T("");
	m_tRcvInfo.strAscii = _T("");
	m_strIP = _T("");
	m_hWnd	= NULL;
	m_uPort = 0;
	m_isOpened = false;

	Close();
}
void CUDPSocket::OnReceive(int nErrorCode)
{
	CString strIP = _T("");
	UINT uPort = 0;
	
	BYTE szBuffer[MAX_BUFFER_SIZE];
	::ZeroMemory(szBuffer,sizeof(szBuffer));
	
	int nLen = ReceiveFrom(szBuffer, MAX_BUFFER_SIZE, strIP, uPort); 
	
	char* pBuff = new char[nLen*5+10];
	ZeroMemory(pBuff, nLen*5+10);

	if (nLen != SOCKET_ERROR)
	{
		int n=0;
		for (int i=0; i<nLen; i++)
		{
			n += sprintf_s (pBuff+n, nLen*5+10-n, "%02X", (unsigned int)(unsigned char)szBuffer[i]);
			if (i+1 < nLen) n += sprintf_s (pBuff+n, nLen*5+10-n, " "); //Hex 2byte ¶ç¾î¾²±â
		}

		m_tRcvInfo.strIP = strIP;
		m_tRcvInfo.uPort = uPort;
		m_tRcvInfo.strAscii = (char*)szBuffer;
		m_tRcvInfo.strHex = (CString)pBuff;
		delete []pBuff;
		pBuff = NULL;
	}

	if (m_hWnd != NULL)
		::SendMessage(m_hWnd, WM_UDP_RCV_MSG, (WPARAM)this, NULL);

	CAsyncSocket::OnReceive(nErrorCode);
}

bool CUDPSocket::OnSndMsg(bool bHEX, bool bSTX, CString strMsg, bool bETX, bool bCRLF)
{
	int nRet = -1;
	CString strData = _T("");
	if (bHEX == true)
		strData = HexToData(bSTX, strMsg, bETX, bCRLF);
	else
		strData = AsciiToData(bSTX, strMsg, bETX, bCRLF);

	int nLen = strData.GetLength();

	if (bHEX == true)
		nLen /= 2;

	byte* pbyMsg = new byte[nLen];
	ZeroMemory(pbyMsg, nLen);

	DataToByte(strData, pbyMsg, nLen);

	nRet = SendTo(pbyMsg, nLen, m_uPort, m_strIP);

	delete []pbyMsg;
	pbyMsg = NULL;

	if (nRet == -1)
		return false;

	return true;
}

