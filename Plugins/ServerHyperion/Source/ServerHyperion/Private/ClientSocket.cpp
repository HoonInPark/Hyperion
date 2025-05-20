// Fill out your copyright notice in the Description page of Project Settings.


#include "ClientSocket.h"

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

int32 UClientSocket::ActivateThreads()
{
	m_SendPackPool = ObjPool<Packet>(60);
	m_pClientRunnable_Send = new FClientRunnable_Send(this, m_SendDataQ);

	return 0;
}

int32 UClientSocket::DeactivateThreads()
{
	m_pClientRunnable_Send->Stop();
	m_pClientRunnable_Send->WaitForCompletion();

	delete m_pClientRunnable_Send;

	return 0;
}

bool UClientSocket::SendIO()
{
	auto sendOverlappedEx = m_SendDataQ.front();

	DWORD dwRecvNumBytes = 0;
	int nRet = WSASend(
		GetSock(),
		&(sendOverlappedEx->m_wsaBuf),
		1,
		&dwRecvNumBytes,
		0,
		(LPWSAOVERLAPPED)sendOverlappedEx,
		NULL);

	if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to run WSASend() : %d"), WSAGetLastError());

		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

FClientRunnable_Send::FClientRunnable_Send(
	UClientSocket* _pInClientSock,
	queue<stOverlappedEx*>& _InSendDataQ)
	: m_pClientSock(_pInClientSock)
	, m_SendDataQ(_InSendDataQ)
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

	m_pClientRunnable_IO = new FClientRunnable_IO(m_pClientSock, m_IocpHandle, m_SendDataQ);

	return true;
}

uint32 FClientRunnable_Send::Run()
{
	shared_ptr<Packet> pPack = nullptr;
	char* pStart = nullptr;
	UINT8 Size;

	while (m_bIsRunning)
	{
		FPlatformProcess::Sleep(0.005f);
		m_CS.Lock();

		queue <shared_ptr< Packet >>& SendPackQ = m_pClientSock->GetSendPackQ();

		if (!SendPackQ.empty())
		{
			pPack = SendPackQ.front();

			Size = pPack->Write(pStart);
			SendMsg(Size, pStart);
			SendPackQ.pop();

			m_pClientSock->GetSendPackPool().Return(pPack);

			m_CS.Unlock();
		}
		else
		{
			m_CS.Unlock();

			continue;
		}
	}

	return 0;
}

void FClientRunnable_Send::Stop() // 
{
	m_bIsRunning = false;

	m_pClientRunnable_IO->Stop();
	m_pClientRunnable_IO->WaitForCompletion();

	delete m_pClientRunnable_IO;
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
	ServerAddr.sin_addr.s_addr = inet_addr("115.23.150.83");

	if (SOCKET_ERROR == connect(m_pClientSock->GetSock(), (SOCKADDR*)&ServerAddr, sizeof(SOCKADDR)))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to connect : %d"), WSAGetLastError());

		closesocket(m_pClientSock->GetSock());
		WSACleanup();

		return false;
	}
	else
		return true;
}

bool FClientRunnable_Send::BindIOCompletionPort(HANDLE _InIocpHandle)
{
	auto hIOCP = CreateIoCompletionPort(
		(HANDLE)m_pClientSock->GetSock(),
		_InIocpHandle,
		(ULONG_PTR)(m_pClientRunnable_IO), 0);

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
	auto sendOverlappedEx = new stOverlappedEx;
	ZeroMemory(sendOverlappedEx, sizeof(stOverlappedEx));
	sendOverlappedEx->m_wsaBuf.len = _InSize;
	sendOverlappedEx->m_wsaBuf.buf = new char[_InSize];
	CopyMemory(sendOverlappedEx->m_wsaBuf.buf, _pInMsg, _InSize);
	sendOverlappedEx->m_eOperation = IOOperation::SEND;

	m_CS.Lock();

	m_SendDataQ.push(sendOverlappedEx);

	if (m_SendDataQ.size() == 1)
	{
		m_pClientSock->SendIO();
	}

	m_CS.Unlock();

	return true;
}

//////////////////////////////////////////////////////////////////////////

FClientRunnable_IO::FClientRunnable_IO(
	UClientSocket* _pInClientSock,
	HANDLE _InIocpHandle,
	queue<stOverlappedEx*>& _InSendDataQ)
	: m_pClientSock(_pInClientSock)
	, m_IocpHandle(_InIocpHandle)
	, m_SendDataQ(_InSendDataQ)
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

		auto pOverlappedEx = (stOverlappedEx*)lpOverlapped;

		if (FALSE == bSuccess || (0 == dwIoSize && IOOperation::ACCEPT != pOverlappedEx->m_eOperation))
		{
			//CloseSocket(pClientInfo);
			continue;
		}

		switch (pOverlappedEx->m_eOperation)
		{
		case IOOperation::SEND:
		{
			SendCompleted(dwIoSize);

			break;
		}
		default:
			break;
		}
	}

	return 0;
}

void FClientRunnable_IO::Stop()
{
	m_bIsRunning = false;
}

void FClientRunnable_IO::SendCompleted(const UINT32 _InDataSize)
{
	UE_LOG(LogTemp, Warning, TEXT("Send Complete Data Size : %d"), _InDataSize);

	m_CS.Lock();

	delete[] m_SendDataQ.front()->m_wsaBuf.buf;
	delete m_SendDataQ.front();

	m_SendDataQ.pop();

	if (m_SendDataQ.empty() == false)
	{
		m_pClientSock->SendIO();
	}

	m_CS.Unlock();
}

