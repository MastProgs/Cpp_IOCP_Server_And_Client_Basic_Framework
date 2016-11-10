# Cpp_IOCP_Server_And_Client_Basic_Framework
IOCP 서버 &amp; 클라 C++ 기본 프레임워크

서버의 구조
================
Main.cpp
----------------

[ **IOCP_SERVER_CLASS** ]
* 해당 클래스 생성자에서 모든 기본 셋팅을 진행한다.

IocpServerClass.cpp
----------------

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
  * g_hIocp = CreateIoCompletionPort ( INVALID_HANDLE_VALUE , NULL , NULL , 0 ) ;
    * INVALID_HANDLE_VALUE 옵션을 통해서, IOCP 핸들을 생성하여 해당 핸들을 g_hIocp 변수에 넣어준다.
    * NULL 기존에 다른 IOCP 관련 핸들을 사용할 일이 없기 때문에 NULL 로 넣어준다.
    * NULL 이 부분은 차후 플레이어 index 를 통해 클라이언트를 구분하여 데이터 처리를 한다.
    * 0 쓰레드를 따로 다른 함수 ( IOCP_SERVER_CLASS::IOCP_SERVER_MakeWorkerThreads_and_AcceptThread() ) 에서 생성할 예정이므로 값을 넣지 않는다.

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

[ **IOCP_SERVER_CLASS::IOCP_Server_Initialize()** ] = IOCP 서버 구동 시, 필요한 초기화를 진행하는 함수
* 필요한 초기화를 여기서 진행하게 된다.
  * (예를 들어) 몬스터 AI 위치를 전부 설정하여 준다.
  * (예를 들어) 충돌체크 처리를 위한 맵 폴리곤들을 로딩하여 메모리에 얹는다.
  * 등등.. 다양한 경우가 있을듯...

[ **IOCP_SERVER_CLASS::CheckThisCPUcoreCount()** ] = 서버를 실행시키는 CPU Core 수를 체크한다.
* SYSTEM_INFO si
  * 시스템의 기본적인 정보를 가져오는 변수이다.
  * GetSystemInfo 함수를 통해, 데이터를 해당 변수에 입력받는다.

* int cpuCore
  * 실제 코어 갯수에서 x2 를 한 값을 넣는다.
  * 인텔 CPU 의 하이퍼쓰레딩 기술을 전제로 한 계산 수치이다.
  * 이 값을 기반으로, 추후 IOCP_SERVER_WorkerThread 함수를 생성할 때, cpuCore 만큼 쓰레드를 생성한다.

[ **IOCP_SERVER_CLASS::IOCP_SERVER_MakeWorkerThreads_and_AcceptThread()** ] = 본격적으로 쓰레드를 생성하는 함수
* vector&#60;thread*&#62; worker_threads
  * IOCP_SERVER_WorkerThread 함수를 여러 개 돌리기 위해, 각각의 포인터를 넣는 벡터
  * 전부 다 쓰레드를 할당받아 돌리는 것이므로, 서버 종료 명령어를 통해 ServerShutdown 절차가 진행이 되면, 정상적으로 메모리를 해제하여 메모리 누수를 막는다.

* thread acceptThread
  * acceptThread 는 기본적으로 IOCP_SERVER_WorkerThread 스레드가 준비가 되어야, 클라이언트들을 받을 수 있으므로, 제일 마지막에 join을 시켜준다.

[ **IOCP_SERVER_CLASS::IOCP_SERVER_WorkerThread()** ] = 받은 패킷들을 조립하고, 조립이 완료된 패킷은 작업을 진행한다.
* DWORD key
  * 각 클라이언트에게 부여된 고유한 id 가 곧 key 값으로 인식되게 된다. 클라이언트 벡터에 저장된 곳 ( 서버 클래스 멤버변수 vector&#60;PLAYER_INFO *&#62; clients ) 에서 index 의 값으로도 쓰이게 된다.

* DWORD iosize
  * iosize 변수에는 주고받은 패킷 ( send &amp; recv 모두 포함 됨 ) 크기가 저장되게 된다.
  * iosize 가 0 인 경우 ( 주고받은 패킷이 없을 경우 ) 클라이언트에서 접속을 끊었다고 판단하는 척도로 기준을 삼는다.
  * if (FALSE == result || 0 == iosize) { ... continue; }
    * 클라이언트가 접속을 끊었을 경우, 퇴장 시 필요한 정보들을 각각 다른 클라이언트에게 통보 및 서버 데이터에 적용시켜 준다.
    * 클라이언트 벡터에 저장된 곳 ( 서버 클래스 멤버변수 vector&#60;PLAYER_INFO *&#62; clients ) 에서, 절대로 해당 플레이어를 빼지 않는다.
    * 클라이언트의 key 값 및 index 가 순차적으로 연결되어 있기 때문에, 해당 플레이어 접근 시 걸리는 시간 O ( 1 ) 을 유지할 수 있다.
    * 클라이언트 정보가 담긴 구조체 ( PLAYER_INFO ) 의 변수 connect 만 false 로 셋팅해 두고, 이후 접속이 끊긴 플레이어 상태만 확인 후 skip하여 빠르게 다음 코드로 넘어갈 수 있다.
    * 플레이어 고유의 view list 같은 고유 변수를 조작하게 될 경우, 필히 atomic 한 접근이 필요하다.
    * continue 를 통해, 다시 GetQueuedCompletionStatus 함수 호출을 하여, 다른 패킷 정보들을 처리 할 수 있게 준비해주어야 한다.
    
* OVLP_EX *my_overlap
  * 데이터 송수신 시, 데이터를 담고 있는 OVERLAPPED 주소를 넘겨 받기 위한 포인터
    
* GetQueuedCompletionStatus(g_hIocp, &amp;iosize, &amp;key, reinterpret_cast&#60;LPOVERLAPPED *&#62;(&amp;my_overlap), INFINITE);
  * CreateIoCompletionPort 함수를 통해 return 된 g_hIocp 를 통해 통신을 한다
  * iosize 변수를 통해서, 전체 받아야 하는 패킷 양을 계산하여 조립한다.
  * key 변수를 통해서, 클라이언트에게 부여한 고유의 id 값을 clients vector 의 index 로 접근을 한다.
  * my_overlap 포인터에 데이터 위치의 주소를 연결시켜 준다
  * INFINITE 이벤트가 발생할 때 까지 무한정 대기할 예정이다
  * 함수를 통하여 result 변수에 bool 형태로 실패와 성공 여부를 확인 하게 된다.
  
* my_overlap->operation
  * OVLP_EX ( struct Overlap_ex ) 의 내부 변수 int operation 을 통하여, 현재 Overlap 데이터가 어떤 데이터인지 구분하게 된다.
  * OP_SERVER_RECV
    * 
