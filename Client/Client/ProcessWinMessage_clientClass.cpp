#include"stdafx.h"

void AsynchronousClientClass::ProcessWinMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	// ���� �߻� ���� Ȯ��
	if (WSAGETSELECTERROR(lParam)) {
		error_display("ProcessWinMessage::", WSAGETSELECTERROR(lParam), __LINE__);
		return;
	}

	// �޼��� ó��
	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_READ: {	// ������ ����

		// m_retval = recvn();
		DWORD ioByteSize{ 0 };
		DWORD ioFlag{ 0 };

		m_retval = WSARecv(m_sock, &m_wsa_recvbuf, 1, &ioByteSize, &ioFlag, NULL, NULL);
		if (SOCKET_ERROR == m_retval) {
			int err_no = WSAGetLastError();
			error_display("FD_READ::", err_no, __LINE__);
			return;
		}
		m_recvbytes = m_retval;

		// recvbuf �� ���� ������ ���� ������ ó��
		processPacket();
	}
		break;
	case FD_WRITE:	// ������ �۽�
		break;
	case FD_CLOSE:	// �������� ����� ����
		break;
	default:
		break;
	}
}