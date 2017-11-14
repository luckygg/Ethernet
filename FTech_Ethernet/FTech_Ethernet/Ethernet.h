#pragma once

#define WM_SERVER_RCV_MSG	(WM_USER+1000)	// Server ������ �޽����� ������ �� �߻��ϴ� �̺�Ʈ.
#define WM_SERVER_CLOSED	(WM_USER+1001)	// Server ������ ������ �� �߻��ϴ� �̺�Ʈ. (Client ���Ͽ��� ����.)
#define WM_CLIENT_RCV_MSG	(WM_USER+1002)	// Client ������ �޽����� ������ �� �߻��ϴ� �̺�Ʈ.
#define WM_CLIENT_ACCEPT	(WM_USER+1003)	// Server ���Ͽ� Client �� ���ӵ��� �� �߻��ϴ� �̺�Ʈ.
#define WM_CLIENT_CLOSED	(WM_USER+1004)	// Server ���Ͽ� Client �� �������� �� �߻��ϴ� �̺�Ʈ.
#define WM_UDP_RCV_MSG		(WM_USER+1005)	// UDP ������ �޽����� ������ �� �߻��ϴ� �̺�Ʈ.

#define MAX_BUFFER_SIZE		4096

class CCommon
{
public :
	typedef struct {
		UINT uPort;			// ��� ��Ʈ.
		CString strIP;		// ��� IP.
		CString strHex;		// HEX to CString ���ڿ�.
		CString strAscii;	// Ascii to CString ���ڿ�.
	} tLastRcvInfo;

	void DataToByte(CString strMsg, byte* pbyMsg, int nLen);				// CString ���ڿ��� byte ������ ��ȯ�����ִ� �Լ�.
	CString HexToData(bool bSTX, CString strMsg, bool bETX, bool bCRLF);	// byte ������ ��ȯ��Ű�� ����, HEX �����͸� CString ���ڿ��� ��ȯ�����ִ� �Լ�.
	CString AsciiToData(bool bSTX, CString strMsg, bool bETX, bool bCRLF);	// byte ������ ��ȯ��Ű�� ����, ASCII �����͸� Hex -> CString ���ڿ��� ��ȯ�����ִ� �Լ�.
private :
	BYTE CodeToAsciiValue(char cData);	// char ���ڸ� BYTE �� ��ȯ��Ű�� �Լ�.
};

// Server Socket
class CServerSocket : public CAsyncSocket, public CCommon
{
public:
	CServerSocket();
	virtual ~CServerSocket();

public :
	//******************************************************************************************************************
	/// \brief				������ �����ϴ� �Լ�.
	/// \param [in] hwnd	Main �� HWND �� �Է�.
	/// \param [in] uPort	������ ������ ��Ʈ�� �Է�.
	/// \param bool			��� ��ȯ.
	bool OnOpen(HWND hWnd, UINT uPort);
	//******************************************************************************************************************
	/// \brief				������ �ݴ� �Լ�.
	/// \param bool			��� ��ȯ.
	bool OnClose();
	//******************************************************************************************************************
	/// \brief				������ �����ƴ��� Ȯ���ϴ� �Լ�.
	/// \param bool			���� ��ȯ.
	bool IsOpened() { return m_isOpened; }
	//******************************************************************************************************************
	/// \brief				��� Client ���� �޽����� ������ �Լ�.
	/// \param [in] bHex	���ڿ��� HEX ���� üũ.
	/// \param [in] bSTX	���ڿ��� STX �� �ԷµǴ��� üũ.
	/// \param [in] strMsg	������ ���ڿ��� �Է�.
	/// \param [in] bETX	���ڿ��� ETX �� �ԷµǴ��� üũ.
	/// \param [in] bCRLF	���ڿ��� CRLF �� �ԷµǴ��� üũ.
	/// \param bool			��� ��ȯ.
	bool OnSndMsgToAll(bool bHEX, bool bSTX, CString strMsg, bool bETX, bool bCRLF);
	//******************************************************************************************************************
	/// \brief				Ư�� Client ���� �޽����� ������ �Լ�.
	/// \param [in] strIP	������ IP �ּ� �Է�.
	/// \param [in] bHex	���ڿ��� HEX ���� üũ.
	/// \param [in] bSTX	���ڿ��� STX �� �ԷµǴ��� üũ.
	/// \param [in] strMsg	������ ���ڿ��� �Է�.
	/// \param [in] bETX	���ڿ��� ETX �� �ԷµǴ��� üũ.
	/// \param [in] bCRLF	���ڿ��� CRLF �� �ԷµǴ��� üũ.
	/// \param bool			��� ��ȯ.
	bool OnSndMsg(CString strIP, bool bHEX, bool bSTX, CString strMsg, bool bETX, bool bCRLF);
	//******************************************************************************************************************
	/// \brief				���������� ���� �޽����� IP �ּҸ� ��ȯ�ϴ� �Լ�.
	/// \param CString		IP�ּ� ��ȯ.
	CString GetLastRcvIP() { return m_tRcvInfo.strIP; }
	//******************************************************************************************************************
	/// \brief				���������� ���� �޽����� ��Ʈ�� ��ȯ�ϴ� �Լ�.
	/// \param UINT			��Ʈ ��ȯ.
	UINT GetLastRcvPort() { return m_tRcvInfo.uPort; }
	//******************************************************************************************************************
	/// \brief				���������� ���� �޽����� HEX �޽����� ��ȯ�ϴ� �Լ�.
	/// \param CString		�޽��� ��ȯ.
	CString GetLastRcvMsgHex() { return m_tRcvInfo.strHex; }
	//******************************************************************************************************************
	/// \brief				���������� ���� �޽����� ASCII �޽����� ��ȯ�ϴ� �Լ�.
	/// \param CString		�޽��� ��ȯ.
	CString GetLastRcvMsgAscii() { return m_tRcvInfo.strAscii; }
	
	// Data Socket ���� ���Ǵ� �Լ�.
	void OnRcvMsgEvent(tLastRcvInfo *info);
	void CloseClientSocket(CAsyncSocket* pClient);
private:
	CPtrList	m_ptrClientSocketList;	// Server �� ����� Client ���� pointer list.
	UINT		m_uPort;				// Server �� ��Ʈ.
	HWND		m_hWnd;					// Main �� HWND.
	bool		m_isOpened;				// Server �� ���� ���� Ȯ��.
	tLastRcvInfo m_tRcvInfo;			// ���� �������� ���� �޽����� ����.

	virtual void OnAccept(int nErrorCode);		// Client ���� �� �߻��ϴ� �޽���.
};

// Server ���ϰ� Client ���ϰ��� ������ ��ȯ�� ���ִ� ����.
class CDataSocket : public CServerSocket
{
public:
	CDataSocket();
	virtual ~CDataSocket();

public :
	void SetServerSocket(CAsyncSocket* pSocket);

private:
	CAsyncSocket* m_pSocket;
	virtual void OnClose(int nErrorCode);	// Client �� ���� ���� �� �߻�.
	virtual void OnReceive(int nErrorCode);	// Client �� Server �� �޽��� ���� �� �߻�.
};

// Client Socket
class CClientSocket : public CAsyncSocket, public CCommon
{
public :
	CClientSocket();
	virtual ~CClientSocket();

public:
	//******************************************************************************************************************
	/// \brief							������ �����ϴ� �Լ�.
	/// \param [in] hwnd				Main �� HWND �� �Է�.
	/// \param [in] strIP				������ ������ IP �ּҸ� �Է�.
	/// \param [in] uPort				������ ������ ��Ʈ�� �Է�.
	/// \param [in] nTimeoutMilliSec	���� �� Ÿ�Ӿƿ��� ����.
	/// \param bool						��� ��ȯ.
	bool OnOpen(HWND hWnd, CString strIP, UINT uPort, int nTimeoutMilliSec);
	//******************************************************************************************************************
	/// \brief				������ �ݴ� �Լ�.
	/// \param void
	void OnClose();
	//******************************************************************************************************************
	/// \brief				�޽����� ������ �Լ�.
	/// \param [in] bHex	���ڿ��� HEX ���� üũ.
	/// \param [in] bSTX	���ڿ��� STX �� �ԷµǴ��� üũ.
	/// \param [in] strMsg	������ ���ڿ��� �Է�.
	/// \param [in] bETX	���ڿ��� ETX �� �ԷµǴ��� üũ.
	/// \param [in] bCRLF	���ڿ��� CRLF �� �ԷµǴ��� üũ.
	/// \param bool			��� ��ȯ.
	bool OnSndMsg(bool bHEX, bool bSTX, CString strMsg, bool bETX, bool bCRLF);
	//******************************************************************************************************************
	/// \brief				������ �����ƴ��� Ȯ���ϴ� �Լ�.
	/// \param bool			���� ��ȯ.
	bool IsOpened() { return m_isOpened; }
	//******************************************************************************************************************
	/// \brief				���������� ���� �޽����� IP �ּҸ� ��ȯ�ϴ� �Լ�.
	/// \param CString		IP�ּ� ��ȯ.
	CString GetLastRcvIP() { return m_tRcvInfo.strIP; }
	//******************************************************************************************************************
	/// \brief				���������� ���� �޽����� ��Ʈ�� ��ȯ�ϴ� �Լ�.
	/// \param UINT			��Ʈ ��ȯ.
	UINT GetLastRcvPort() { return m_tRcvInfo.uPort; }
	//******************************************************************************************************************
	/// \brief				���������� ���� �޽����� HEX �޽����� ��ȯ�ϴ� �Լ�.
	/// \param CString		�޽��� ��ȯ.
	CString GetLastRcvMsgHex() { return m_tRcvInfo.strHex; }
	//******************************************************************************************************************
	/// \brief				���������� ���� �޽����� ASCII �޽����� ��ȯ�ϴ� �Լ�.
	/// \param CString		�޽��� ��ȯ.
	CString GetLastRcvMsgAscii() { return m_tRcvInfo.strAscii; }

private :
	tLastRcvInfo	m_tRcvInfo;	// ���� �������� ���� �޽����� ����.
	HWND			m_hWnd;		// Main �� HWND.
	bool			m_isOpened;	// Server �� ���� ���� Ȯ��.
	
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
};

// UDP Socket.
class CUDPSocket : public CAsyncSocket, public CCommon
{
public:
	CUDPSocket();
	virtual ~CUDPSocket();
	
public :
	//******************************************************************************************************************
	/// \brief							������ �����ϴ� �Լ�.
	/// \param [in] hwnd				Main �� HWND �� �Է�.
	/// \param [in] strIP				������ ������ IP �ּҸ� �Է�.
	/// \param [in] uPort				������ ������ ��Ʈ�� �Է�.
	/// \param bool						��� ��ȯ.
	bool OnOpen(HWND hWnd, CString strIP, UINT uPort);
	//******************************************************************************************************************
	/// \brief				������ �ݴ� �Լ�.
	/// \param void
	void OnClose();
	//******************************************************************************************************************
	/// \brief				�޽����� ������ �Լ�.
	/// \param [in] bHex	���ڿ��� HEX ���� üũ.
	/// \param [in] bSTX	���ڿ��� STX �� �ԷµǴ��� üũ.
	/// \param [in] strMsg	������ ���ڿ��� �Է�.
	/// \param [in] bETX	���ڿ��� ETX �� �ԷµǴ��� üũ.
	/// \param [in] bCRLF	���ڿ��� CRLF �� �ԷµǴ��� üũ.
	/// \param bool			��� ��ȯ.
	bool OnSndMsg(bool bHEX, bool bSTX, CString strMsg, bool bETX, bool bCRLF);
	//******************************************************************************************************************
	/// \brief				������ �����ƴ��� Ȯ���ϴ� �Լ�.
	/// \param bool			���� ��ȯ.
	bool IsOpened() { return m_isOpened; }
	//******************************************************************************************************************
	/// \brief				���������� ���� �޽����� IP �ּҸ� ��ȯ�ϴ� �Լ�.
	/// \param CString		IP�ּ� ��ȯ.
	CString GetLastRcvIP() { return m_tRcvInfo.strIP; }
	//******************************************************************************************************************
	/// \brief				���������� ���� �޽����� ��Ʈ�� ��ȯ�ϴ� �Լ�.
	/// \param UINT			��Ʈ ��ȯ.
	UINT GetLastRcvPort() { return m_tRcvInfo.uPort; }
	//******************************************************************************************************************
	/// \brief				���������� ���� �޽����� HEX �޽����� ��ȯ�ϴ� �Լ�.
	/// \param CString		�޽��� ��ȯ.
	CString GetLastRcvMsgHex() { return m_tRcvInfo.strHex; }
	//******************************************************************************************************************
	/// \brief				���������� ���� �޽����� ASCII �޽����� ��ȯ�ϴ� �Լ�.
	/// \param CString		�޽��� ��ȯ.
	CString GetLastRcvMsgAscii() { return m_tRcvInfo.strAscii; }

private:
	CString		m_strIP;		// ���� ����� IP �ּ�.
	UINT		m_uPort;		// ���� ����� ��Ʈ.
	HWND		m_hWnd;			// Main �� HWND.
	bool		m_isOpened;		// Server �� ���� ���� Ȯ��.
	tLastRcvInfo m_tRcvInfo;	// ���� �������� ���� �޽����� ����.

	void OnReceive(int nErrorCode);
};
