// Fill out your copyright notice in the Description page of Project Settings.


#include "ClientSocket.h"

#include <chrono>

// Sets default values for this component's properties
UClientSocket::UClientSocket()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}

UClientSocket::~UClientSocket()
{
}

int32 UClientSocket::ActivateThreads(APawn* aPawn)
{
	m_pAtomicOvlpdEx.store(nullptr);
	m_pInternOvlpdEx = nullptr;

	// TODO : PLZ... fix this filthy init code by Wrap the StlCircularQueue with class name like StlCircularPool... 
	// TODO : its usage is like StlCircularPool<T>(MAX_POOL_SIZE)...
	m_pSendPackPool = new StlCircularQueue<Packet>(MAX_POOL_SIZE);
	for (int i = 0; i < MAX_POOL_SIZE; ++i)
	{
		auto pPack = make_unique<Packet>();
		m_pSendPackPool->enqueue(pPack);
	}
	m_pSendPackQ = new StlCircularQueue<Packet>(MAX_POOL_SIZE);

	m_pSendDataPool = new StlCircularQueue<OverlappedEx>(MAX_POOL_SIZE);
	for (int i = 0; i < MAX_POOL_SIZE; ++i)
	{
		auto pOvlpdEx = make_unique<OverlappedEx>();
		m_pSendDataPool->enqueue(pOvlpdEx);
	}
	m_pSendDataQ = new StlCircularQueue<OverlappedEx>(MAX_POOL_SIZE);

	m_pRecvOverlappedEx = new OverlappedEx;

	m_pClientRunnable_Send = new FClientRunnable_Send(this, m_pSendDataQ, m_pSendDataPool);

	return 0;
}

int32 UClientSocket::DeactivateThreads()
{
	m_pClientRunnable_Send->Stop();
	m_pClientRunnable_Send->WaitForCompletion();

	if (m_pClientRunnable_Send)
		delete m_pClientRunnable_Send;
	if (m_pRecvOverlappedEx)
		delete m_pRecvOverlappedEx;

	return 0;
}

bool UClientSocket::SendIO(const unique_ptr< OverlappedEx >& _pInSendOverlappedEx)
{
	DWORD dwRecvNumBytes = 0;
	int nRet = WSASend(
		m_Sock,
		&(_pInSendOverlappedEx->m_wsaBuf),
		1,
		&dwRecvNumBytes,
		0,
		(LPWSAOVERLAPPED)_pInSendOverlappedEx.get(),
		NULL);

	if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to run WSASend() : %d"), WSAGetLastError());
		return false;
	}

	return true;
}

void UClientSocket::SendCompleted(const UINT32 _InDataSize)
{
	//UE_LOG(LogTemp, Warning, TEXT("Send Complete Data Size : %d"), _InDataSize);

	m_pSendDataPool->enqueue(m_pInternOvlpdEx);

	if (m_pSendDataQ->dequeue(m_pInternOvlpdEx))
	{
		m_pAtomicOvlpdEx.exchange(m_pInternOvlpdEx.get(), memory_order_acq_rel);
		SendIO(m_pInternOvlpdEx);
	}
	else
	{
		m_pInternOvlpdEx = nullptr;
		m_pAtomicOvlpdEx.exchange(nullptr, memory_order_release);
	}
}

bool UClientSocket::BindRecv()
{
	DWORD dwFlag = 0;
	DWORD dwRecvNumBytes = 0;

	//Overlapped I/O을 위해 각 정보를 셋팅해 준다.
	m_pRecvOverlappedEx->m_wsaBuf.len = MAX_RECV_BUFF_SIZE;
	m_pRecvOverlappedEx->m_wsaBuf.buf = m_RecvBuff;
	m_pRecvOverlappedEx->m_eOperation = IOOperation::IO_RECV;

	int nRet = WSARecv(
		m_Sock,
		&(m_pRecvOverlappedEx->m_wsaBuf),
		1,
		&dwRecvNumBytes,
		&dwFlag,
		(LPWSAOVERLAPPED)m_pRecvOverlappedEx,
		NULL);

	//socket_error이면 client socket이 끊어진걸로 처리한다.
	if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to run WSARecv() : %d"), WSAGetLastError());
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

FClientRunnable_Send::FClientRunnable_Send(
	UClientSocket*						_pInClientSock,
	StlCircularQueue<OverlappedEx>*	_InSendDataQ,
	StlCircularQueue<OverlappedEx>*	_InSendDataPool)
	: m_pClientSock		(_pInClientSock)
	, m_pSendDataQ		(_InSendDataQ)
	, m_pSendDataPool	(_InSendDataPool)
{
	m_pThread = FRunnableThread::Create(this, TEXT("ClientThread_Send"), 0, TPri_BelowNormal); //windows default = 8mb for thread, could specify more
}

FClientRunnable_Send::~FClientRunnable_Send()
{
	if (!m_pThread) return;

	delete m_pThread;
	m_pThread = nullptr;
}

bool FClientRunnable_Send::Init() // func Init also called in outside of thread. u can check it while in debugging session.
{
	WSADATA wsaData; // for initialize winsock dll, it is needed

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		UE_LOG(LogTemp, Error, TEXT("WSAStartup() Error : %d"), WSAGetLastError());
		WSACleanup();
		return false;
	}

	if (!InitSock())
		return false;

	if (!Connect())
		return false;

	// create iocp handle 
	m_IocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (NULL == m_IocpHandle)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create iocp handle : %d"), GetLastError());
		closesocket(m_pClientSock->GetSock());
		WSACleanup();
		return false;
	}

	if (!BindIOCompletionPort(m_IocpHandle))
		return false;

	m_pClientRunnable_IO = new FClientRunnable_IO(m_pClientSock, m_IocpHandle, m_pSendDataQ, m_pSendDataPool);

	return true;
}

uint32 FClientRunnable_Send::Run()
{
	unique_ptr<Packet> pPack = nullptr;
	char* pStart = nullptr;
	UINT8 Size;

	while (m_bIsRunning)
	{
		FPlatformProcess::Sleep(0.003f);

		if (m_pClientSock->GetSendPackQ()->dequeue(pPack))
		{
			Size = pPack->Write(pStart);
			m_pClientSock->GetSendPackPool()->enqueue(pPack);

			SendMsg(Size, pStart);
			pPack = nullptr;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("FClientRunnable_Send::Run() Is Returned Safely"));
	return 0;
}

void FClientRunnable_Send::Stop() // 
{
	m_bIsRunning = false;

	if (!m_pClientRunnable_IO) return;

	m_pClientRunnable_IO->Stop();
	CloseHandle(m_IocpHandle);
	m_pClientRunnable_IO->WaitForCompletion();

	delete m_pClientRunnable_IO;
	m_pClientRunnable_IO = nullptr;
}

bool FClientRunnable_Send::InitSock()
{
	// socket initialize
	m_pClientSock->GetSock() = WSASocket(
		AF_INET,
		SOCK_STREAM,
		IPPROTO_TCP,
		NULL,
		NULL,
		WSA_FLAG_OVERLAPPED);

	if (m_pClientSock->GetSock() == INVALID_SOCKET)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to initialize client socket object"));
		WSACleanup();
		return false;
	}
	else
		return true;
}

bool FClientRunnable_Send::Connect()
{
	SOCKADDR_IN ServerAddr;
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(11021);
	ServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (SOCKET_ERROR == connect(m_pClientSock->GetSock(), (SOCKADDR*)&ServerAddr, sizeof(SOCKADDR)))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to connect : %d"), WSAGetLastError());
		closesocket(m_pClientSock->GetSock());
		WSACleanup();
		return false;
	}
	else
	{
		m_pClientSock->OnConnect();
		return true;
	}
}

bool FClientRunnable_Send::BindIOCompletionPort(HANDLE _InIocpHandle)
{
	auto hIOCP = CreateIoCompletionPort(
		(HANDLE)m_pClientSock->GetSock(),
		_InIocpHandle,
		(ULONG_PTR)(m_pClientRunnable_IO),
		0);

	if (hIOCP == INVALID_HANDLE_VALUE)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to run CreateIoCompletionPort() : %d"), GetLastError());
		CloseHandle(_InIocpHandle);
		closesocket(m_pClientSock->GetSock());
		WSACleanup();
		return false;
	}

	return true;
}

bool FClientRunnable_Send::SendMsg(const UINT32 _InSize, char* _pInMsg)
{
	unique_ptr<OverlappedEx> pSendOvlpdEx;
	if (!m_pSendDataPool->dequeue(pSendOvlpdEx))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to run Acquire() From Send Data Pool"));
		return false;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// write byte
	////////////////////////////////////////////////////////////////////////////////
	pSendOvlpdEx->Init();
	pSendOvlpdEx->m_wsaBuf.len = _InSize;
	CopyMemory(pSendOvlpdEx->m_wsaBuf.buf, _pInMsg, _InSize);
	pSendOvlpdEx->m_eOperation = IOOperation::IO_SEND;
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////

	m_pSendDataQ->enqueue(pSendOvlpdEx);
	// If there are no messages currently in the process of being sent
	if (nullptr == m_pClientSock->GetAtomicOvlpdEx().load(memory_order_relaxed))
	{
		if (m_pSendDataQ->dequeue(m_pClientSock->GetInternOvlpdEx()))
		{
			m_pClientSock->GetAtomicOvlpdEx().store(m_pClientSock->GetInternOvlpdEx().get(), memory_order_release);
			m_pClientSock->SendIO(m_pClientSock->GetInternOvlpdEx());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[SendMsg] : Error while dequeue from send buf q, data race is suspected"));
			return false;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

FClientRunnable_IO::FClientRunnable_IO(
	UClientSocket*							_pInClientSock,
	HANDLE									_InIocpHandle,
	StlCircularQueue<OverlappedEx>*		_pInSendDataQ,
	StlCircularQueue<OverlappedEx>*		_pInSendDataPool)
	: m_pClientSock		(_pInClientSock)
	, m_IocpHandle		(_InIocpHandle)
	, m_pSendDataQ		(_pInSendDataQ)
	, m_pSendDataPool	(_pInSendDataPool)
{
	m_pThread = FRunnableThread::Create(this, TEXT("ClientThread_IO"), 0, TPri_BelowNormal); //windows default = 8mb for thread, could specify more
}

FClientRunnable_IO::~FClientRunnable_IO()
{
	if (!m_pThread) return;

	delete m_pThread;
	m_pThread = nullptr;
}

bool FClientRunnable_IO::Init()
{
	return true;
}

uint32 FClientRunnable_IO::Run()
{
	//함수 호출 성공 여부
	BOOL bSuccess = TRUE;
	//Overlapped I/O작업에서 전송된 데이터 크기
	DWORD dwIoSize = 0;
	//I/O 작업을 위해 요청한 Overlapped 구조체를 받을 포인터
	LPOVERLAPPED lpOverlapped = NULL;

	FClientRunnable_IO* ThisPtr = this;

	while (m_bIsRunning)
	{
		bSuccess = GetQueuedCompletionStatus(
			m_IocpHandle,
			&dwIoSize,					// 실제로 전송된 바이트
			(PULONG_PTR)&ThisPtr,		// CompletionKey
			&lpOverlapped,				// Overlapped IO 객체
			INFINITE);					// 대기할 시간

		if (TRUE == bSuccess && 0 == dwIoSize && NULL == lpOverlapped)
		{
			m_bIsRunning = false;
			continue;
		}

		if (NULL == lpOverlapped)
		{
			continue;
		}

		auto pOverlappedEx = (OverlappedEx*)lpOverlapped;

		if (FALSE == bSuccess || (0 == dwIoSize && IOOperation::IO_ACCEPT != pOverlappedEx->m_eOperation))
		{
			CloseSock();
			continue;
		}

		switch (pOverlappedEx->m_eOperation)
		{
		case IOOperation::IO_RECV:
		{
			m_pClientSock->OnReceive(dwIoSize);
			m_pClientSock->BindRecv();

			break;
		}
		case IOOperation::IO_SEND:
		{
			m_pClientSock->SendCompleted(dwIoSize);

			break;
		}
		default:
			break;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("FClientRunnable_IO::Run() Is Returned Safely"));
	return 0;
}

void FClientRunnable_IO::Stop()
{
	m_bIsRunning = false;
}

void FClientRunnable_IO::CloseSock(bool _bIsForce)
{
	struct linger stLinger = { 0, 0 };	// SO_DONTLINGER로 설정

	// bIsForce가 true이면 SO_LINGER, timeout = 0으로 설정하여 강제 종료 시킨다. 주의 : 데이터 손실이 있을수 있음 
	if (true == _bIsForce)
	{
		stLinger.l_onoff = 1;
	}

	//socketClose소켓의 데이터 송수신을 모두 중단 시킨다.
	shutdown(m_pClientSock->GetSock(), SD_BOTH);

	//소켓 옵션을 설정한다.
	setsockopt(m_pClientSock->GetSock(), SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger));

	//m_LatestClosedTimeSec = chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
	//소켓 연결을 종료 시킨다.
	closesocket(m_pClientSock->GetSock());
	m_pClientSock->GetSock() = INVALID_SOCKET;

	m_pClientSock->OnClose();
}
