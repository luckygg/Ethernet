#pragma once

#define WM_SERVER_RCV_MSG	(WM_USER+1000)	// Server 소켓이 메시지를 수신할 때 발생하는 이벤트.
#define WM_SERVER_CLOSED	(WM_USER+1001)	// Server 소켓이 닫혔을 때 발생하는 이벤트. (Client 소켓에서 감지.)
#define WM_CLIENT_RCV_MSG	(WM_USER+1002)	// Client 소켓이 메시지를 수신할 때 발생하는 이벤트.
#define WM_CLIENT_ACCEPT	(WM_USER+1003)	// Server 소켓에 Client 가 접속됐을 때 발생하는 이벤트.
#define WM_CLIENT_CLOSED	(WM_USER+1004)	// Server 소켓에 Client 가 해제됐을 때 발생하는 이벤트.
#define WM_UDP_RCV_MSG		(WM_USER+1005)	// UDP 소켓이 메시지를 수신할 때 발생하는 이벤트.

#define MAX_BUFFER_SIZE		4096

class CCommon
{
public :
	typedef struct {
		UINT uPort;			// 통신 포트.
		CString strIP;		// 통신 IP.
		CString strHex;		// HEX to CString 문자열.
		CString strAscii;	// Ascii to CString 문자열.
	} tLastRcvInfo;

	void DataToByte(CString strMsg, byte* pbyMsg, int nLen);				// CString 문자열을 byte 형으로 변환시켜주는 함수.
	CString HexToData(bool bSTX, CString strMsg, bool bETX, bool bCRLF);	// byte 형으로 변환시키기 전에, HEX 데이터를 CString 문자열로 변환시켜주는 함수.
	CString AsciiToData(bool bSTX, CString strMsg, bool bETX, bool bCRLF);	// byte 형으로 변환시키기 전에, ASCII 데이터를 Hex -> CString 문자열로 변환시켜주는 함수.
private :
	BYTE CodeToAsciiValue(char cData);	// char 문자를 BYTE 로 반환시키는 함수.
};

// Server Socket
class CServerSocket : public CAsyncSocket, public CCommon
{
public:
	CServerSocket();
	virtual ~CServerSocket();

public :
	//******************************************************************************************************************
	/// \brief				소켓을 생성하는 함수.
	/// \param [in] hwnd	Main 의 HWND 를 입력.
	/// \param [in] uPort	생성할 소켓의 포트를 입력.
	/// \param bool			결과 반환.
	bool OnOpen(HWND hWnd, UINT uPort);
	//******************************************************************************************************************
	/// \brief				소켓을 닫는 함수.
	/// \param bool			결과 반환.
	bool OnClose();
	//******************************************************************************************************************
	/// \brief				소켓이 생성됐는지 확인하는 함수.
	/// \param bool			상태 반환.
	bool IsOpened() { return m_isOpened; }
	//******************************************************************************************************************
	/// \brief				모든 Client 에게 메시지를 보내는 함수.
	/// \param [in] bHex	문자열이 HEX 인지 체크.
	/// \param [in] bSTX	문자열에 STX 가 입력되는지 체크.
	/// \param [in] strMsg	전송할 문자열을 입력.
	/// \param [in] bETX	문자열에 ETX 가 입력되는지 체크.
	/// \param [in] bCRLF	문자열에 CRLF 가 입력되는지 체크.
	/// \param bool			결과 반환.
	bool OnSndMsgToAll(bool bHEX, bool bSTX, CString strMsg, bool bETX, bool bCRLF);
	//******************************************************************************************************************
	/// \brief				특정 Client 에게 메시지를 보내는 함수.
	/// \param [in] strIP	전송할 IP 주소 입력.
	/// \param [in] bHex	문자열이 HEX 인지 체크.
	/// \param [in] bSTX	문자열에 STX 가 입력되는지 체크.
	/// \param [in] strMsg	전송할 문자열을 입력.
	/// \param [in] bETX	문자열에 ETX 가 입력되는지 체크.
	/// \param [in] bCRLF	문자열에 CRLF 가 입력되는지 체크.
	/// \param bool			결과 반환.
	bool OnSndMsg(CString strIP, bool bHEX, bool bSTX, CString strMsg, bool bETX, bool bCRLF);
	//******************************************************************************************************************
	/// \brief				마지막으로 받은 메시지의 IP 주소를 반환하는 함수.
	/// \param CString		IP주소 반환.
	CString GetLastRcvIP() { return m_tRcvInfo.strIP; }
	//******************************************************************************************************************
	/// \brief				마지막으로 받은 메시지의 포트를 반환하는 함수.
	/// \param UINT			포트 반환.
	UINT GetLastRcvPort() { return m_tRcvInfo.uPort; }
	//******************************************************************************************************************
	/// \brief				마지막으로 받은 메시지의 HEX 메시지를 반환하는 함수.
	/// \param CString		메시지 반환.
	CString GetLastRcvMsgHex() { return m_tRcvInfo.strHex; }
	//******************************************************************************************************************
	/// \brief				마지막으로 받은 메시지의 ASCII 메시지를 반환하는 함수.
	/// \param CString		메시지 반환.
	CString GetLastRcvMsgAscii() { return m_tRcvInfo.strAscii; }
	
	// Data Socket 에서 사용되는 함수.
	void OnRcvMsgEvent(tLastRcvInfo *info);
	void CloseClientSocket(CAsyncSocket* pClient);
private:
	CPtrList	m_ptrClientSocketList;	// Server 에 연결된 Client 들의 pointer list.
	UINT		m_uPort;				// Server 의 포트.
	HWND		m_hWnd;					// Main 의 HWND.
	bool		m_isOpened;				// Server 의 생성 여부 확인.
	tLastRcvInfo m_tRcvInfo;			// 가장 마지막에 받은 메시지의 정보.

	virtual void OnAccept(int nErrorCode);		// Client 접속 시 발생하는 메시지.
};

// Server 소켓과 Client 소켓과의 데이터 교환을 해주는 역할.
class CDataSocket : public CServerSocket
{
public:
	CDataSocket();
	virtual ~CDataSocket();

public :
	void SetServerSocket(CAsyncSocket* pSocket);

private:
	CAsyncSocket* m_pSocket;
	virtual void OnClose(int nErrorCode);	// Client 가 접속 해제 시 발생.
	virtual void OnReceive(int nErrorCode);	// Client 가 Server 로 메시지 전송 시 발생.
};

// Client Socket
class CClientSocket : public CAsyncSocket, public CCommon
{
public :
	CClientSocket();
	virtual ~CClientSocket();

public:
	//******************************************************************************************************************
	/// \brief							소켓을 생성하는 함수.
	/// \param [in] hwnd				Main 의 HWND 를 입력.
	/// \param [in] strIP				연결할 소켓의 IP 주소를 입력.
	/// \param [in] uPort				연결할 소켓의 포트를 입력.
	/// \param [in] nTimeoutMilliSec	연결 시 타임아웃을 설정.
	/// \param bool						결과 반환.
	bool OnOpen(HWND hWnd, CString strIP, UINT uPort, int nTimeoutMilliSec);
	//******************************************************************************************************************
	/// \brief				소켓을 닫는 함수.
	/// \param void
	void OnClose();
	//******************************************************************************************************************
	/// \brief				메시지를 보내는 함수.
	/// \param [in] bHex	문자열이 HEX 인지 체크.
	/// \param [in] bSTX	문자열에 STX 가 입력되는지 체크.
	/// \param [in] strMsg	전송할 문자열을 입력.
	/// \param [in] bETX	문자열에 ETX 가 입력되는지 체크.
	/// \param [in] bCRLF	문자열에 CRLF 가 입력되는지 체크.
	/// \param bool			결과 반환.
	bool OnSndMsg(bool bHEX, bool bSTX, CString strMsg, bool bETX, bool bCRLF);
	//******************************************************************************************************************
	/// \brief				소켓이 생성됐는지 확인하는 함수.
	/// \param bool			상태 반환.
	bool IsOpened() { return m_isOpened; }
	//******************************************************************************************************************
	/// \brief				마지막으로 받은 메시지의 IP 주소를 반환하는 함수.
	/// \param CString		IP주소 반환.
	CString GetLastRcvIP() { return m_tRcvInfo.strIP; }
	//******************************************************************************************************************
	/// \brief				마지막으로 받은 메시지의 포트를 반환하는 함수.
	/// \param UINT			포트 반환.
	UINT GetLastRcvPort() { return m_tRcvInfo.uPort; }
	//******************************************************************************************************************
	/// \brief				마지막으로 받은 메시지의 HEX 메시지를 반환하는 함수.
	/// \param CString		메시지 반환.
	CString GetLastRcvMsgHex() { return m_tRcvInfo.strHex; }
	//******************************************************************************************************************
	/// \brief				마지막으로 받은 메시지의 ASCII 메시지를 반환하는 함수.
	/// \param CString		메시지 반환.
	CString GetLastRcvMsgAscii() { return m_tRcvInfo.strAscii; }

private :
	tLastRcvInfo	m_tRcvInfo;	// 가장 마지막에 받은 메시지의 정보.
	HWND			m_hWnd;		// Main 의 HWND.
	bool			m_isOpened;	// Server 의 생성 여부 확인.
	
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
	/// \brief							소켓을 생성하는 함수.
	/// \param [in] hwnd				Main 의 HWND 를 입력.
	/// \param [in] strIP				연결할 소켓의 IP 주소를 입력.
	/// \param [in] uPort				연결할 소켓의 포트를 입력.
	/// \param bool						결과 반환.
	bool OnOpen(HWND hWnd, CString strIP, UINT uPort);
	//******************************************************************************************************************
	/// \brief				소켓을 닫는 함수.
	/// \param void
	void OnClose();
	//******************************************************************************************************************
	/// \brief				메시지를 보내는 함수.
	/// \param [in] bHex	문자열이 HEX 인지 체크.
	/// \param [in] bSTX	문자열에 STX 가 입력되는지 체크.
	/// \param [in] strMsg	전송할 문자열을 입력.
	/// \param [in] bETX	문자열에 ETX 가 입력되는지 체크.
	/// \param [in] bCRLF	문자열에 CRLF 가 입력되는지 체크.
	/// \param bool			결과 반환.
	bool OnSndMsg(bool bHEX, bool bSTX, CString strMsg, bool bETX, bool bCRLF);
	//******************************************************************************************************************
	/// \brief				소켓이 생성됐는지 확인하는 함수.
	/// \param bool			상태 반환.
	bool IsOpened() { return m_isOpened; }
	//******************************************************************************************************************
	/// \brief				마지막으로 받은 메시지의 IP 주소를 반환하는 함수.
	/// \param CString		IP주소 반환.
	CString GetLastRcvIP() { return m_tRcvInfo.strIP; }
	//******************************************************************************************************************
	/// \brief				마지막으로 받은 메시지의 포트를 반환하는 함수.
	/// \param UINT			포트 반환.
	UINT GetLastRcvPort() { return m_tRcvInfo.uPort; }
	//******************************************************************************************************************
	/// \brief				마지막으로 받은 메시지의 HEX 메시지를 반환하는 함수.
	/// \param CString		메시지 반환.
	CString GetLastRcvMsgHex() { return m_tRcvInfo.strHex; }
	//******************************************************************************************************************
	/// \brief				마지막으로 받은 메시지의 ASCII 메시지를 반환하는 함수.
	/// \param CString		메시지 반환.
	CString GetLastRcvMsgAscii() { return m_tRcvInfo.strAscii; }

private:
	CString		m_strIP;		// 연결 대상의 IP 주소.
	UINT		m_uPort;		// 연결 대상의 포트.
	HWND		m_hWnd;			// Main 의 HWND.
	bool		m_isOpened;		// Server 의 생성 여부 확인.
	tLastRcvInfo m_tRcvInfo;	// 가장 마지막에 받은 메시지의 정보.

	void OnReceive(int nErrorCode);
};
