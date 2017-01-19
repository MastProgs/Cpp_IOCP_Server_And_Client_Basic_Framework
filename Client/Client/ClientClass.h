#pragma once

class AsynchronousClientClass
{
public:
	AsynchronousClientClass();
	~AsynchronousClientClass();

	void Init(const HWND&);

	void ProcessWinMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void sendPacket_TEST();

private:
	// member Function
	void inputServerIP();
	void inputServerIP_ReadtxtFile();
	void inputServerIP_cin();

	int recvn();
	void error_display(char *msg, int err_no, int line);
	void error_quit(wchar_t *msg, int err_no);

	SOCKET* getServerSocket();
	void processPacket(const Packet*);

	void sendPacket(const BYTE data_size, const BYTE type, BYTE* data_start_pointer);

	// 윈도우 창 관련 멤버 변수
	WNDCLASS m_wndclass;
	HWND m_hWnd;
	MSG m_msg;

	// 통신 관련 멤버 변수
	WSADATA m_wsadata;
	SOCKET m_sock;
	int m_retval{ 0 };

	CHAR m_serverIP[32]{ 0 };

	// buffer 관련 변수
	WSABUF m_wsa_recvbuf;
	WSABUF m_wsa_sendbuf;

	Packet m_recvbuf[MAX_BUF_SIZE]{ 0 };
	Packet m_sendbuf[MAX_BUF_SIZE]{ 0 };
	Packet m_data_buf[MAX_BUF_SIZE]{ 0 };
	unsigned int m_recvbytes{ 0 };
	unsigned int m_packet_size_current{ 0 };
	unsigned int m_packet_size_previous{ 0 };
};