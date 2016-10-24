#pragma once

using OVLP_EX = struct Overlap_ex {
	OVERLAPPED original_overlap;
	int operation;
	WSABUF wsabuf;
	Packet iocp_buffer[MAX_BUF_SIZE];
};

using PLAYER_INFO = struct Client_INFO {
	SOCKET s;
	unsigned int id;
	bool connected;
	OVLP_EX recv_overlap;
	int packet_size;
	int previous_size;
	Packet packet_buff[MAX_BUF_SIZE];
};

class IOCP_SERVER_CLASS
{
public:
	IOCP_SERVER_CLASS();
	~IOCP_SERVER_CLASS();

	void IOCP_Server_GetServerIpAddress();
	void IOCP_Server_Initialize();
	void CheckThisCPUcoreCount();

	void IOCP_SERVER_MakeWorkerThreads_and_AcceptThread();
	void IOCP_SERVER_WorkerThread();
	void IOCP_SERVER_AcceptThread();

	void IOCP_SERVER_ProcessPacket(const unsigned int& id, const Packet buf[]);
	void IOCP_SERVER_SendPacket(unsigned int id, const Packet *buf);

	void IOCP_SERVER_ErrorDisplay(char *msg, int err_no, int line);
	void IOCP_SERVER_ErrorQuit(wchar_t *msg, int err_no);

private:
	HANDLE g_hIocp;	
	int cpuCore;

	vector<thread*> worker_threads;
	bool ServerShutdown{ false };

	vector<PLAYER_INFO *> clients;
	unsigned int playerIndex{ UINT_MAX };
};