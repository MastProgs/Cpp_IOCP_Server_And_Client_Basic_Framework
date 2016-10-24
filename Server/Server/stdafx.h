#pragma once
#pragma comment(lib, "ws2_32")

#include<WinSock2.h>
#include<Windows.h>

#include<iostream>
#include<thread>
#include<vector>
#include<cstdlib>
using namespace std;


// 클라이언트와 서버가 통신하기 위한 기본적인 통신 규칙 ( 서버 및 클라에서 수정 시, 클라이언트와 동기화되어 영향을 미친다 )
#include"protocol.h"

// iocp 서버의 몸통이 되는 가장 핵심적인 클래스의 선언이 들어 있다. ( iocp 와 boost 중 하나만 쓰면 된다 )
#include"iocpServerClass.h"

// boost asio 서버관련 라이브러리 ( iocp 와 boost 중 하나만 쓰면 된다 )
//#include"boost_asio.h"