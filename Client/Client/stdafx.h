#pragma once
#pragma comment(lib, "ws2_32")

#include<WinSock2.h>
#include<Windows.h>


#include<iostream>
using namespace std;

// ������ �����Ǵ� protocol.h - ������ �������� �ٷ� ����ȴ�.
#include"../../../server/Server/Server/protocol.h"
#define WM_SOCKET (WM_USER + 1)

// client ��� class
#include<fstream>
#include"clientClass.h"

static AsynchronousClientClass g_client;

#ifdef _DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif