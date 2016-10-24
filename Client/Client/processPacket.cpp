#include"stdafx.h"

void AsynchronousClientClass::processPacket()
{
	switch (m_recvbuf[1])
	{
	case TEST:
#ifdef _DEBUG
		cout << "Server is Running. TEST Packet Recived Successfully.\n";
#endif
		break;
	case KEYINPUT:
		break;
	default:
		break;
	}
}
