#pragma once
#pragma comment(lib, "ws2_32")

#include<WinSock2.h>
#include<Windows.h>

#include<iostream>
#include<thread>
#include<vector>
#include<cstdlib>
using namespace std;


// Ŭ���̾�Ʈ�� ������ ����ϱ� ���� �⺻���� ��� ��Ģ ( ���� �� Ŭ�󿡼� ���� ��, Ŭ���̾�Ʈ�� ����ȭ�Ǿ� ������ ��ģ�� )
#include"protocol.h"

// iocp ������ ������ �Ǵ� ���� �ٽ����� Ŭ������ ������ ��� �ִ�. ( iocp �� boost �� �ϳ��� ���� �ȴ� )
#include"iocpServerClass.h"

// boost asio �������� ���̺귯�� ( iocp �� boost �� �ϳ��� ���� �ȴ� )
//#include"boost_asio.h"