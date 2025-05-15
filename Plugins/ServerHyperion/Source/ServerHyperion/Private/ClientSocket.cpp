// Fill out your copyright notice in the Description page of Project Settings.


#include "ClientSocket.h"



// Sets default values for this component's properties
UClientSocket::UClientSocket()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

int32 UClientSocket::ActivateThreads()
{
	m_pClientRunnable_Send = new FClientRunnable_Send(m_Socket_Send, m_SendPackQ);
	
	//m_pClientRunnable_Recv = new FClientRunnable_Recv();

	if (m_pClientRunnable_Send/* && m_pClientRunnable_Recv*/)
		return 0;
	else
		return 1;
}

int32 UClientSocket::DeactivateThreads()
{
	m_pClientRunnable_Send->Stop();

	return 0;
}

//////////////////////////////////////////////////////////////////////////

FClientRunnable_Send::FClientRunnable_Send(SOCKET _InSocket, PackQueue& _InSendQ)
	: m_Socket_Send(_InSocket)
	, m_SendPackQ(_InSendQ)
{
	pThread = FRunnableThread::Create(this, TEXT("ClientSendThread"), 0, TPri_BelowNormal); //windows default = 8mb for thread, could specify more
	auto test = new DynamicObjectPool<Packet>(60);
}

FClientRunnable_Send::~FClientRunnable_Send()
{
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
	m_IocpHandle_Send = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (NULL == m_IocpHandle_Send)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create iocp handle : %d"), GetLastError());

		closesocket(GetSock());
		WSACleanup();

		return false;
	}

	if (!BindIOCompletionPort(m_IocpHandle_Send))
		return false;

	return true;
}

uint32 FClientRunnable_Send::Run()
{
	TSharedPtr<Packet> pPackTmp;
	
	char* StartPtTmp = nullptr;
	UINT8 SizeTmp;

	while (m_bIsRunning)
	{
		m_SendPackQ.Dequeue(pPackTmp);
		SizeTmp = pPackTmp->Write(StartPtTmp);

		SendMsg(SizeTmp, StartPtTmp);

		FPlatformProcess::Sleep(0.004f);
	}

	return 0;
}

void FClientRunnable_Send::Stop() // 
{
	m_bIsRunning = false;
}

void FClientRunnable_Send::Exit() // called when func Run() is returned
{
	pThread->WaitForCompletion();

	delete pThread;
	pThread = nullptr;
	
	delete this;
}

bool FClientRunnable_Send::InitSock()
{
	// socket initialize
	m_Socket_Send = WSASocket(
		AF_INET,
		SOCK_STREAM,
		IPPROTO_TCP,
		NULL,
		NULL,
		WSA_FLAG_OVERLAPPED);

	if (m_Socket_Send == INVALID_SOCKET)
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
	ServerAddr.sin_addr.s_addr = inet_addr("61.79.175.133");

	if (SOCKET_ERROR == connect(m_Socket_Send, (SOCKADDR*)&ServerAddr, sizeof(SOCKADDR)))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to connect : %d"), WSAGetLastError());

		closesocket(GetSock());
		WSACleanup();

		return false;
	}
	else
		return true;
}

bool FClientRunnable_Send::BindIOCompletionPort(HANDLE _InIocpHandle)
{
	auto hIOCP = CreateIoCompletionPort(
		(HANDLE)GetSock(), 
		_InIocpHandle, 
		(ULONG_PTR)(this), 0);

	if (hIOCP == INVALID_HANDLE_VALUE)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to run CreateIoCompletionPort() : %d"), GetLastError());

		CloseHandle(_InIocpHandle);
		closesocket(GetSock());
		WSACleanup();

		return false;
	}

	return true;
}

bool FClientRunnable_Send::SendIO()
{
	auto sendOverlappedEx = m_SendDataQ.front();

	DWORD dwRecvNumBytes = 0;
	int nRet = WSASend(m_Socket_Send,
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

bool FClientRunnable_Send::SendMsg(const UINT32 _InSize, char* _pInMsg)
{
	auto sendOverlappedEx = new stOverlappedEx;
	ZeroMemory(sendOverlappedEx, sizeof(stOverlappedEx));
	sendOverlappedEx->m_wsaBuf.len = _InSize;
	sendOverlappedEx->m_wsaBuf.buf = new char[_InSize];
	CopyMemory(sendOverlappedEx->m_wsaBuf.buf, _pInMsg, _InSize);
	sendOverlappedEx->m_eOperation = IOOperation::SEND;

	//std::lock_guard<std::mutex> guard(mSendLock);
	m_CS_Send.Lock();

	m_SendDataQ.push(sendOverlappedEx);

	if (m_SendDataQ.size() == 1)
	{
		SendIO();
	}

	m_CS_Send.Unlock();

	return true;
}

//////////////////////////////////////////////////////////////////////////

FClientRunnable_Recv::FClientRunnable_Recv()
{
	pThread = FRunnableThread::Create(this, TEXT("ClientRecvThread"), 0, TPri_BelowNormal); //windows default = 8mb for thread, could specify more
}

FClientRunnable_Recv::~FClientRunnable_Recv()
{
}

bool FClientRunnable_Recv::Init()
{
	return false;
}

uint32 FClientRunnable_Recv::Run()
{
	while (m_bIsRunning)
	{

	}

	return uint32();
}

void FClientRunnable_Recv::Stop()
{
}

void FClientRunnable_Recv::Exit()
{
}
