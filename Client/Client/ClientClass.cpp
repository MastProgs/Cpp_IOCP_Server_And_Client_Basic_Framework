#include"stdafx.h"

AsynchronousClientClass::AsynchronousClientClass()
{

}

AsynchronousClientClass::~AsynchronousClientClass()
{
	closesocket(m_sock);
	WSACleanup();
}

void AsynchronousClientClass::Init(const HWND& hwnd)
{
	inputServerIP();

	// init Winsock
	if (WSAStartup(MAKEWORD(2, 2), &m_wsadata) != 0) {
		int err_no = WSAGetLastError();
		error_quit(L"WSAStartup ERROR", err_no);
	}

	m_sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	if (INVALID_SOCKET == m_sock) {
		int err_no = WSAGetLastError();
		error_quit(L"socket()", err_no);
	}
		
	// connect
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(m_serverIP);
	serveraddr.sin_port = htons(SERVERPORT);

	m_retval = WSAConnect(m_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr), NULL, NULL, NULL, NULL);
	if (SOCKET_ERROR == m_retval) {
		// �񵿱� �����̶� �׳� ����, �˻� ���־�� ��
		if (WSAGetLastError() != WSAEWOULDBLOCK) {
			int err_no = WSAGetLastError();
			error_quit(L"connect()", err_no);
		}
	}

	// WSAAsyncSelect - �ͺ��ŷ ���� �ڵ� ��ȯ
	m_hWnd = hwnd;

	if (NULL == m_hWnd) {
#ifdef _DEBUG
		printf("Cannot find Consol Window, ERROR : %d\n", __LINE__);
#endif
		exit(-1);
	}
	m_retval = WSAAsyncSelect(m_sock, m_hWnd, WM_SOCKET, FD_READ | FD_CLOSE);
	if (SOCKET_ERROR == m_retval) {
		int err_no = WSAGetLastError();
		error_quit(L"ioctlsocket()", err_no);
	}

	m_wsa_recvbuf.buf = reinterpret_cast<CHAR*>(m_recvbuf);
	m_wsa_recvbuf.len = MAX_BUF_SIZE;
	m_wsa_sendbuf.buf = reinterpret_cast<CHAR*>(m_sendbuf);
	m_wsa_sendbuf.len = MAX_BUF_SIZE;
}

void AsynchronousClientClass::inputServerIP_ReadtxtFile()
{
	ifstream getServerIP;
	getServerIP.open("ServerIP.txt", ios::in);
	getServerIP.getline(m_serverIP, 32);
	getServerIP.close();
}

void AsynchronousClientClass::inputServerIP_cin()
{
	cout << "\nexample 127.0.0.1\nInput Server's IP : ";
	cin >> m_serverIP;
}

void AsynchronousClientClass::inputServerIP()
{
#ifdef _DEBUG
	cout << "choose Server to connect.\n\n0. Read Server IP in .txt File\n1. Input Server IP, directly\n\nchoice method : ";
	short cmd{ 0 };
	cin >> cmd;

	if (cmd & 1) { inputServerIP_cin(); }
	else { inputServerIP_ReadtxtFile(); }

	cout << "\n\n\t--==** Connecting Server, Please Wait **==--\n\n\n";
#else
	inputServerIP_ReadtxtFile();
#endif
}

SOCKET* AsynchronousClientClass::getServerSocket()
{
	return &m_sock;
}

int AsynchronousClientClass::recvn()
{
	int received;
	Packet *ptr = m_recvbuf;
	int left = MAX_BUF_SIZE;

	while (left > 0) {
		received = recv(m_sock, reinterpret_cast<char*>(&ptr), left, 0);
		if (SOCKET_ERROR == received)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (MAX_BUF_SIZE - left);
}

void AsynchronousClientClass::error_display(char *msg, int err_no, int line)
{
	WCHAR *lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
#ifdef _DEBUG
	printf("[ %s - %d ]", msg, line);
	wprintf(L"���� %s\n", lpMsgBuf);
#endif
	LocalFree(lpMsgBuf);
}

void AsynchronousClientClass::error_quit(wchar_t *msg, int err_no)
{
	WCHAR *lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(-1);
}

// sendPacket ( sizeof( data ), type �� , data �� �ּҰ� );
void AsynchronousClientClass::sendPacket(const BYTE data_size, const BYTE type, BYTE* data_start_pointer) {
	// ���� �ִ� ���� ������ ���� ������ ���̰� Ŀ���� �ȵȴ�.
	if (MAX_BUF_SIZE < (data_size + 2)) {
#ifdef _DEBUG
		// �Ʒ��� ���� ������ �߻��ϰ� �ȴٸ�, ���� ����� �ǵ帮�� ���� ���� ������ ũ�⸦ ������ �ٿ� ���� �� �ֵ��� ����
		printf("[ code LINE %d ] [ code FUNCTION %s ] SendPacket class ERROR :: data size overed MAX_BUF_SIZE\n", __LINE__, __FUNCTION__);
#endif
	}
	else {
		// ��Ŷ ���� ���� ���� ����
		m_sendbuf[0] = data_size + 2;
		m_sendbuf[1] = type;
		memcpy(&m_sendbuf[2], data_start_pointer, m_sendbuf[0]);

		m_wsa_sendbuf.len = m_sendbuf[0];
		DWORD ioByteSize;
		m_retval = WSASend(m_sock, &m_wsa_sendbuf, 1, &ioByteSize, 0, NULL, NULL);
		if (SOCKET_ERROR == m_retval) {
			// �񵿱� �����̶� �׳� ����, �˻� ���־�� ��
			if (WSAGetLastError() != WSAEWOULDBLOCK) {
				int err_no = WSAGetLastError();
				error_quit(L"connect()", err_no);
			}
		}
	}
}

// ������ ����ִ���, ������ �ϴ����� ���� �⺻ �׽�Ʈ �Լ�. 3 Bytes �� ������, �⺻ Ÿ���� TEST, �����ʹ� 1�� ������.
void AsynchronousClientClass::sendPacket_TEST() {
	m_sendbuf[0] = 3;
	m_sendbuf[1] = TEST;
	m_sendbuf[2] = 1;		// true

	m_wsa_sendbuf.len = m_sendbuf[0];

	DWORD ioByteSize;
	m_retval = WSASend(m_sock, &m_wsa_sendbuf, 1, &ioByteSize, 0, NULL, NULL);
	//int retval = send(sock, reinterpret_cast<char*>(&sendbuf), sendbuf[0], 0);
	if (SOCKET_ERROR == m_retval) {
		// �񵿱� �����̶� �׳� ����, �˻� ���־�� ��
		if (WSAGetLastError() != WSAEWOULDBLOCK) {
			int err_no = WSAGetLastError();
			error_quit(L"connect()", err_no);
		}
	}
}