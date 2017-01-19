#include"stdafx.h"

void AsynchronousClientClass::ProcessWinMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	// 오류 발생 여부 확인
	if (WSAGETSELECTERROR(lParam)) {
		error_display("ProcessWinMessage::", WSAGETSELECTERROR(lParam), __LINE__);
		return;
	}

	// 메세지 처리
	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_READ: {	// 데이터 수신

		// m_retval = recvn();
		DWORD ioByteSize{ 0 };
		DWORD ioFlag{ 0 };

		m_retval = WSARecv(m_sock, &m_wsa_recvbuf, 1, &ioByteSize, &ioFlag, NULL, NULL);
		if (SOCKET_ERROR == m_retval) {
			int err_no = WSAGetLastError();
			error_display("FD_READ::", err_no, __LINE__);
			return;
		}
		Packet *buf = m_recvbuf;

		int current_data_processing = ioByteSize;
		while (0 < current_data_processing) {
			if (0 == m_packet_size_current) {
				m_packet_size_current = buf[0];
				if (buf[0] > MAX_BUF_SIZE) {
					cout << "AsynchronousClientClass::ProcessWinMessage() Error, recvbuf[0] is out of MAX_BUF_SIZE\n";
					exit(-1);
				}
			}
			int need_to_build = m_packet_size_current - m_packet_size_previous;
			if (need_to_build <= current_data_processing) {
				// Packet building Complete & Process Packet
				memcpy(m_data_buf + m_packet_size_previous, buf, need_to_build);

				// recvbuf 에 받은 내용을 토대로 데이터 처리
				processPacket(m_data_buf);

				m_packet_size_current = 0;
				m_packet_size_previous = 0;
				current_data_processing -= need_to_build;
				buf += need_to_build;
			}
			else {
				// Packet build continue
				memcpy(m_data_buf + m_packet_size_previous, buf, current_data_processing);
				m_packet_size_previous += current_data_processing;
				current_data_processing = 0;
				buf += current_data_processing;
			}
		}
	}
		break;
	case FD_WRITE:	// 데이터 송신
		break;
	case FD_CLOSE:	// 서버에서 통신을 끊음
		break;
	default:
		break;
	}
}