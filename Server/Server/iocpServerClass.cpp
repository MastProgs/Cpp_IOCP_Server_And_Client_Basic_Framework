#pragma once
#include"stdafx.h"

IOCP_SERVER_CLASS::IOCP_SERVER_CLASS()
{	
	IOCP_Server_GetServerIpAddress();
	CheckThisCPUcoreCount();

	IOCP_Server_Initialize();
	
	IOCP_SERVER_MakeWorkerThreads_and_AcceptThread();
}

IOCP_SERVER_CLASS::~IOCP_SERVER_CLASS()
{
	WSACleanup();
}

void IOCP_SERVER_CLASS::IOCP_Server_GetServerIpAddress()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	PHOSTENT hostinfo;
	char hostname[50];
	char ipaddr[50];
	memset(hostname, 0, sizeof(hostname));
	memset(ipaddr, 0, sizeof(ipaddr));

	int nError = gethostname(hostname, sizeof(hostname));
	if (nError == 0)
	{
		hostinfo = gethostbyname(hostname);
		strcpy(ipaddr, inet_ntoa(*reinterpret_cast<struct in_addr*>(hostinfo->h_addr_list[0])));
	}
	WSACleanup();
	printf("This Server's IP address : %s\n", ipaddr);
}

void IOCP_SERVER_CLASS::IOCP_Server_Initialize()
{
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	g_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	if (g_hIocp == NULL) {
		int err_no = WSAGetLastError();
		IOCP_SERVER_ErrorQuit(L"IOCP_SERVER_CLASS::IOCP_Server_Initialize", err_no);
	}
}

void IOCP_SERVER_CLASS::CheckThisCPUcoreCount()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	cpuCore = static_cast<int>(si.dwNumberOfProcessors) * 2;
	printf("CPU Core Count = %d, threads = %d\n", cpuCore / 2, cpuCore);
}

void IOCP_SERVER_CLASS::IOCP_SERVER_MakeWorkerThreads_and_AcceptThread()
{
	worker_threads.reserve(cpuCore);

	for (int i = 0; i < cpuCore; ++i)
	{
		worker_threads.push_back(new thread{ &IOCP_SERVER_CLASS::IOCP_SERVER_WorkerThread, this });
	}

	thread acceptThread{ &IOCP_SERVER_CLASS::IOCP_SERVER_AcceptThread, this };
	while (ServerShutdown) { Sleep(1000); }

	for (auto thread : worker_threads) {
		thread->join();
		delete thread;
	}

	acceptThread.join();
}

void IOCP_SERVER_CLASS::IOCP_SERVER_WorkerThread()
{
	while (TRUE == (!ServerShutdown)) {
		DWORD key;
		DWORD iosize;
		OVLP_EX *my_overlap;

		BOOL result = GetQueuedCompletionStatus(g_hIocp, &iosize, &key, reinterpret_cast<LPOVERLAPPED *>(&my_overlap), INFINITE);

		// 클라이언트가 접속을 끊었을 경우
		if (FALSE == result || 0 == iosize) {
			if (FALSE == result) {
				int err_no = WSAGetLastError();
				IOCP_SERVER_ErrorDisplay("WorkerThreadStart::GetQueuedCompletionStatus", err_no, __LINE__);
			}

			closesocket(clients[key]->s);
			clients[key]->connected = false;
			printf("[ No. %3u ] Disconnected\n", key);

			/* view list 에서 빼주자 */
			/* 모든 클라이언트에게, 현재 클라이언트가 끊겼다고 알려주자 */

			/*Packet p[11];
			p[0] = 11;
			p[1] = DISCONNECTED;
			*((int *)(&p[2])) = key;*/

			//for (int i = 0; i < clients.size(); ++i) {
			//	if (false == clients[i]->connected) { continue; }
			//	//if (i == playerIndex) { continue; }

			//	SendPacket(i, p);
			//}
			continue;
		}
		else if (OP_SERVER_RECV == my_overlap->operation) {
			// 클라이언트로 부터 데이터를 받았을 경우
			Packet *buf_ptr = clients[key]->recv_overlap.iocp_buffer;
			int remained = iosize;
			while (0 < remained) {
				if (0 == clients[key]->packet_size) { clients[key]->packet_size = buf_ptr[0]; }

				int required = clients[key]->packet_size - clients[key]->previous_size;

				if (remained >= required) {
					memcpy(clients[key]->packet_buff + clients[key]->previous_size, buf_ptr, required);

					// 아래 함수에서 패킷을 처리하게 된다.
					IOCP_SERVER_ProcessPacket(key, clients[key]->packet_buff);

					buf_ptr += required;
					remained -= required;

					clients[key]->packet_size = 0;
					clients[key]->previous_size = 0;
				}
				else {
					memcpy(clients[key]->packet_buff + clients[key]->previous_size, buf_ptr, remained);
					buf_ptr += remained;
					clients[key]->previous_size += remained;
					remained = 0;
				}
			}
			DWORD flags = 0;
			int retval = WSARecv(clients[key]->s, &clients[key]->recv_overlap.wsabuf, 1, NULL, &flags, &clients[key]->recv_overlap.original_overlap, NULL);
			if (SOCKET_ERROR == retval) {
				int err_no = WSAGetLastError();
				if (ERROR_IO_PENDING != err_no) {
					IOCP_SERVER_ErrorDisplay("WorkerThreadStart::WSARecv", err_no, __LINE__);
				}
				continue;
			}
		}
		else if (OP_SERVER_SEND == my_overlap->operation) {
			// 서버에서 메세지를 보냈으면, 메모리를 해제해 준다.
			delete my_overlap;
		}
		else {
			cout << "Unknown IOCP event !!\n";
			exit(-1);
		}
	}
}

void IOCP_SERVER_CLASS::IOCP_SERVER_AcceptThread()
{
	int retval{ 0 };

	// socket() - IPv4 ( AF_INET )
	SOCKET listen_sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (listen_sock == INVALID_SOCKET) {
		int err_no = WSAGetLastError();
		IOCP_SERVER_ErrorQuit(L"socket()", err_no);
	};

	// bind()
	struct sockaddr_in serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = ::bind(listen_sock, reinterpret_cast<struct sockaddr *>(&serveraddr), sizeof(serveraddr));
	if (retval == SOCKET_ERROR) {
		int err_no = WSAGetLastError();
		IOCP_SERVER_ErrorQuit(L"socket()", err_no);
	}

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) {
		int err_no = WSAGetLastError();
		IOCP_SERVER_ErrorQuit(L"socket()", err_no);
	}

	while (TRUE == (!ServerShutdown)) {
		// accept()
		struct sockaddr_in clientaddr;
		int addrlen = sizeof(clientaddr);
		SOCKET client_sock = WSAAccept(listen_sock, reinterpret_cast<sockaddr *>(&clientaddr), &addrlen, NULL, NULL);
		if (INVALID_SOCKET == client_sock) {
			int err_no = WSAGetLastError();
			IOCP_SERVER_ErrorDisplay("Accept::WSAAccept", err_no, __LINE__);
			while (true);
		}

		/* DB 관련 login 기능이 여기에 추가되어야 한다. 로그인이 번호가 제대로 맞으면 통과, 아니면 클라이언트 연결을 끊는다. 로그인을 하면 DB에서 정보를 가져온다 */

		playerIndex += 1;
		printf("[ No. %3u ] Client IP = %s, Port = %d is Connected\n", playerIndex, inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(client_sock), g_hIocp, playerIndex, 0);

		PLAYER_INFO *user = new PLAYER_INFO;

		user->s = client_sock;
		user->connected = true;
		user->id = playerIndex;
		user->packet_size = 0;
		user->previous_size = 0;
		memset(&user->recv_overlap.original_overlap, 0, sizeof(user->recv_overlap.original_overlap));
		user->recv_overlap.operation = OP_SERVER_RECV;
		user->recv_overlap.wsabuf.buf = reinterpret_cast<char*>(&user->recv_overlap.iocp_buffer);
		user->recv_overlap.wsabuf.len = sizeof(user->recv_overlap.iocp_buffer);

		/* 추가로 데이터가 필요한 경우라면, PLAYER_INFO 구조체 내용을 수정하고, 추가로 초기화 값이나 불러오는 내용을 대입하여 넣어주어야 한다. */

		clients.push_back(move(user));

		/* 주변 클라이언트에 대해 뿌릴 정보 뿌리고, 시야 리스트나 처리해야 할 정보들도 함께 넣는다. */

		// 클라이언트에서 응답오길 기다리기
		DWORD flags{ 0 };
		retval = WSARecv(client_sock, &clients[playerIndex]->recv_overlap.wsabuf, 1, NULL, &flags, &clients[playerIndex]->recv_overlap.original_overlap, NULL);
		if (SOCKET_ERROR == retval) {
			int err_no = WSAGetLastError();
			if (ERROR_IO_PENDING != err_no) {
				IOCP_SERVER_ErrorDisplay("Accept::WSARecv", err_no, __LINE__);
			}
		}
	}
}

void IOCP_SERVER_CLASS::IOCP_SERVER_SendPacket(unsigned int id, const Packet *packet)
{
	OVLP_EX *over = new OVLP_EX;
	memset(over, 0, sizeof(OVLP_EX));
	over->operation = OP_SERVER_SEND;
	over->wsabuf.buf = reinterpret_cast<char *>(over->iocp_buffer);
	over->wsabuf.len = packet[0];
	memcpy(over->iocp_buffer, packet, packet[0]);

	DWORD flags{ 0 };
	int retval = WSASend(clients[id]->s, &over->wsabuf, 1, NULL, flags, &over->original_overlap, NULL);
	if (SOCKET_ERROR == retval) {
		int err_no = WSAGetLastError();
		if (ERROR_IO_PENDING != err_no) {
			IOCP_SERVER_ErrorDisplay("SendPacket::WSASend", err_no, __LINE__);
			while (true);
		}
	}
}


void IOCP_SERVER_CLASS::IOCP_SERVER_ErrorDisplay(char *msg, int err_no, int line)
{
	WCHAR *lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[ %s - %d ]", msg, line);
	wprintf(L"에러 %s\n", lpMsgBuf);
	LocalFree(lpMsgBuf);
}

void IOCP_SERVER_CLASS::IOCP_SERVER_ErrorQuit(wchar_t *msg, int err_no)
{
	WCHAR *lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(-1);
}