#pragma once
#include"stdafx.h"

void IOCP_SERVER_CLASS::IOCP_SERVER_ProcessPacket(const unsigned int& id, const Packet buf[])
{
	// packet[0] = packet size		> 0번째 자리에는 무조건, 패킷의 크기가 들어가야만 한다.
	// packet[1] = type				> 1번째 자리에는 현재 패킷이 무슨 패킷인지 속성을 정해주는 값이다.
	// packet[...] = data			> 2번째 부터는 속성에 맞는 순대로 처리를 해준다.

	// buf[1] 번째의 속성으로 분류를 한 뒤에, 내부에서 2번째 부터 데이터를 처리하기 시작한다.
	switch (buf[1])
	{
	case TEST:
	{
		// 받은 패킷을 그대로 돌려준다.
		printf("[ No. %3u ] TEST Packet Recived !!\n", id);
		printf("buf[0] = %d, buf[1] = %d, buf[2] = %d\n\n", buf[0], buf[1], buf[2]);
		IOCP_SERVER_SendPacket(id, buf);
	}
	break;
	default:
	{
		// 클라이언트로 부터 알수 없는 데이터가 왔을 경우, 해킹 방지를 위해 서버를 강제 종료. 해당 클라이언트의 고유 번호와 타입 번호를 알려준다.
		printf("ERROR, Unknown signal -> [ %u ] protocol num = %d\n", id, buf[1]);
		exit(-1);
	}
	break;
	}
}