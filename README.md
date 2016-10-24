# Cpp_IOCP_Server_And_Client_Basic_Framework
IOCP 서버 &amp; 클라 C++ 기본 프레임워크

서버의 구조
================
`Main.cpp`

[ **IOCP_SERVER_CLASS** ]
* 해당 클래스 생성자에서 모든 기본 셋팅을 진행한다.

`IocpServerClass.cpp`

[ **IOCP_SERVER_CLASS::IOCP_SERVER_CLASS()** ] = 생성자
* IOCP_Server_GetServerIpAddress()
  * 현재 서버의 IP를 받아 온다.
  * 그냥 h_addr_list[0] 첫번째 리스트의 값만 가져오므로, 하나면 표시된다.
  * WSADATA 를 할당하고 삭제하기 때문에, 비효율 적일 수는 있다.

* CheckThisCPUcoreCount()
  * 현재 서버의 CPU의 Core 갯수를 가져온다.
  * 인텔의 하이퍼쓰레딩 기술을 쓴다는 전제하에, 코어의 2배 쓰레드를 생성한다.
 
* IOCP_Server_Initialize()
  * IOCP 서버의 초기화 기본 작업을 진행해 준다. 
  * 통신 쪽을 제외한, 다른 변수들 초기화 작업도 여기서 진행해주어야 한다.
  * CreateIoCompletionPort 함수를 통해, 핸들 이벤트를 운영체제에 등록한다.

* IOCP_SERVER_MakeWorkerThreads_and_AcceptThread
  * 서버 구동을 위해, 각각의 스레드를 만들어 준다.
  * WorkerThread 는 코어의 2배의 갯수로 생성한다.
  * AcceptThread 는 1 개의 쓰레드만 생성하여 준다.
  * WorkerThread 는 어차피 알아서 return 되고 값을 넘기지 않으므로 delete
  * ServerShutdown 의 bool 변수를 통해, 서버를 명령어를 통해 종료하여 작업 도중 예기치 못한 강제 종료를 막는다.

[ **IOCP_SERVER_CLASS::~IOCP_SERVER_CLASS()** ] = 소멸자
* 할당 받았던 WSADATA 해재
  * WSACleanup()

[ **IOCP_SERVER_CLASS::IOCP_Server_GetServerIpAddress()** ] = IP console window 에 출력
* 자세한 설명은 생성자 쪽 IOCP_Server_GetServerIpAddress() 참조

[ **IOCP_SERVER_CLASS::IOCP_Server_Initialize** ] = IOCP 서버 구동 시, 필요한 초기화를 진행하는 함수
* 필요한 초기화를 여기서 진행하게 된다.
  * (예를 들어) 몬스터 AI 위치를 전부 설정하여 준다.
  * (예를 들어) 충돌체크 처리를 위한 맵 폴리곤들을 로딩하여 메모리에 얹는다.
  * 등등.. 다양한 경우가 있을듯...

[ **IOCP_SERVER_CLASS::CheckThisCPUcoreCount** ] = 서버를 실행시키는 CPU Core 수를 체크한다.
* SYSTEM_INFO si
  * 시스템의 기본적인 정보를 가져오는 변수이다.
  * GetSystemInfo 함수를 통해, 데이터를 해당 변수에 입력받는다.

* int cpuCore
  * 실제 코어 갯수에서 x2 를 한 값을 넣는다.
  * 인텔 CPU 의 하이퍼쓰레딩 기술을 전제로 한 계산 수치이다.
  * 이 값을 기반으로, 추후 IOCP_SERVER_WorkerThread 함수를 생성할 때, cpuCore 만큼 쓰레드를 생성한다.

[ **IOCP_SERVER_CLASS::IOCP_SERVER_MakeWorkerThreads_and_AcceptThread** ] = 본격적으로 쓰레드를 생성하는 함수
* vector<thread*> worker_threads
  * IOCP_SERVER_WorkerThread 함수를 여러 개 돌리기 위해, 각각의 포인터를 넣는 벡터
  * 전부 다 쓰레드를 할당받아 돌리는 것이므로, 서버 종료 명령어를 통해 ServerShutdown 절차가 진행이 되면, 정상적으로 메모리를 해제하여 메모리 누수를 막는다.

* thread acceptThread
  * acceptThread 는 기본적으로 IOCP_SERVER_WorkerThread 스레드가 준비가 되어야, 클라이언트들을 받을 수 있으므로, 제일 마지막에 join을 시켜준다.

