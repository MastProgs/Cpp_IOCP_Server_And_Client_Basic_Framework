#pragma once
#include"stdafx.h"
using namespace std;

/*
	16.07.06
	���� �⺻ Ʋ. Ŭ���̾�Ʈ�� �����ϴ� clients �� ���� array ������, vector ������ ���·� ó�� ������ ���ұ� ������, ���װ� �߻��� ������ �ִ�.
	Ŭ���̾�Ʈ���� ������ �� �����ϱ� ����, �⺻ ���������� ���� �����Ͽ� ���� ���� ���� �̴�.

	16.07.26
	�Ϻ� �ּ� ���� ���� �� process �Լ��� if ������ switch ������ ����
	��� ���� �κ��� �������� ���̱� ���� TRUE == (!ServerShutdwon) ���� ����

	16.08.16
	Ŭ���̾�Ʈ ���α׷��Ӱ� ������ ��� ������ ���� ������ class SendPacket �߰� - new buf ���·� �Ҵ�޾� ���۸� �����

	16.08.18
	error_display, error_quit �Լ��� protocol.h �� �ű�
	�Ҵ�޾� ����ϴ� buf �� �����ϰ�, ���� �迭 ���۸� �������.
	�⺻ ���� ��� Ȯ�ο� TEST process protocol define -> protocol.h ��� ���� ( 18��° �� )
	class SendPacket �� Send_default_test() �Լ� �߰� -> protocol.h ��� ���� ( 100��° �� )
	class SendPacket �� getPacketBuf() �Լ� �߰� -> protocol.h ��� ���� ( 115��° �� )
	���� recv ���۸� send ���� ����ó�� ���������, ���� ���Ϳ� ���� �� �ֵ��� recv �� �ش� queue �� ��ĥ �����̴� ( protocol.h - 46, 50 �� )
	OVLP_EX, PLAYER_INFO ����ü �ΰ��� protocol.h ���� ServerMain.cpp �� ������ �Ű��. - ���������� Ȱ���� �����̱� ����

	16.09.26
	iocp ���� Ŭ������ ��ȯ
*/

int main() {

	_wsetlocale(LC_ALL, L"korean");

	// IOCP ���� class
	IOCP_SERVER_CLASS iocpServer;


}