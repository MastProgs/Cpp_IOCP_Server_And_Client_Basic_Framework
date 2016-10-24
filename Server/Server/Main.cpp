#pragma once
#include"stdafx.h"
using namespace std;

/*
	16.07.06
	서버 기본 틀. 클라이언트를 저장하는 clients 가 원래 array 었으나, vector 포인터 형태로 처음 변경해 보았기 때문에, 버그가 발생할 소지가 있다.
	클라이언트에서 정보를 잘 전달하기 위해, 기본 프로토콜을 같이 적용하여 추후 만들 생각 이다.

	16.07.26
	일부 주석 설명 변경 및 process 함수를 if 문에서 switch 문으로 변경
	몇몇 조건 부분을 가독성을 높이기 위해 TRUE == (!ServerShutdwon) 으로 변경

	16.08.16
	클라이언트 프로그래머가 서버로 통신 보내기 위한 간편한 class SendPacket 추가 - new buf 형태로 할당받아 버퍼를 사용함

	16.08.18
	error_display, error_quit 함수를 protocol.h 로 옮김
	할당받아 사용하던 buf 를 삭제하고, 공용 배열 버퍼를 만들었다.
	기본 서버 통신 확인용 TEST process protocol define -> protocol.h 헤더 참조 ( 18번째 줄 )
	class SendPacket 에 Send_default_test() 함수 추가 -> protocol.h 헤더 참조 ( 100번째 줄 )
	class SendPacket 에 getPacketBuf() 함수 추가 -> protocol.h 헤더 참조 ( 115번째 줄 )
	현재 recv 버퍼를 send 전역 버퍼처럼 만들었지만, 이후 벡터에 넣을 수 있도록 recv 와 해당 queue 를 합칠 예정이다 ( protocol.h - 46, 50 줄 )
	OVLP_EX, PLAYER_INFO 구조체 두개를 protocol.h 에서 ServerMain.cpp 로 선언을 옮겼다. - 서버에서만 활용할 예정이기 때문

	16.09.26
	iocp 서버 클래스로 변환
*/

int main() {

	_wsetlocale(LC_ALL, L"korean");

	// IOCP 서버 class
	IOCP_SERVER_CLASS iocpServer;


}