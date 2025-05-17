// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Components/ActorComponent.h"

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <mswsock.h>

#pragma comment(lib, "ws2_32")
#pragma comment(lib, "mswsock.lib")

#include <queue>
#include <concurrent_queue.h>
#include "ServerHyperionLibrary/Packet.h"
#include "ServerHyperionLibrary/ObjPool.h"

#include "ClientSocket.generated.h"

using namespace std;
using namespace Concurrency;

class FClientRunnable_Send;
class FClientRunnable_Recv;

enum class IOOperation
{
	ACCEPT,
	RECV,
	SEND
};

//WSAOVERLAPPED구조체를 확장 시켜서 필요한 정보를 더 넣었다.
struct stOverlappedEx
{
	WSAOVERLAPPED m_wsaOverlapped;		//Overlapped I/O구조체
	WSABUF		  m_wsaBuf;				//Overlapped I/O작업 버퍼
	IOOperation   m_eOperation;			//작업 동작 종류
	UINT32		  SessionIndex = 0;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SERVERHYPERION_API UClientSocket : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UClientSocket();
	~UClientSocket();

	int32 ActivateThreads();
	int32 DeactivateThreads();

	ObjPool<Packet>* GetPackPool() { return m_pSendPackPool; }

	inline void SendPackQ_Push(shared_ptr<Packet> _pInElem)
	{
		m_pSendPackQ->push(_pInElem);
	}

	inline bool SendPackQ_Pop(shared_ptr<Packet> _pOutElem) 
	{
		return m_pSendPackQ->try_pop(_pOutElem);
	}

private:
	ObjPool<Packet>* m_pSendPackPool{ nullptr };
	concurrent_queue <shared_ptr< Packet >> *m_pSendPackQ{ nullptr };

	SOCKET m_Socket_Send{ INVALID_SOCKET };
	SOCKET m_Socket_Recv{ INVALID_SOCKET };

	FClientRunnable_Send* m_pClientRunnable_Send{ nullptr };
	FClientRunnable_Recv* m_pClientRunnable_Recv{ nullptr };
};

//////////////////////////////////////////////////////////////////////////

class SERVERHYPERION_API FClientRunnable_Send : FRunnable
{
public:
	FClientRunnable_Send(
		SOCKET _InSocket,
		ObjPool<Packet>* _pInPool,
		concurrent_queue <shared_ptr< Packet >>* _pInQ);

	~FClientRunnable_Send();

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

private:
	bool InitSock();
	bool Connect();
	bool BindIOCompletionPort(HANDLE _InIocpHandle);
	
	bool SendIO();
	bool SendMsg(const UINT32 _InSize, char* _pInMsg);

private:
	SOCKET m_Socket_Send;
	HANDLE m_IocpHandle_Send{ INVALID_HANDLE_VALUE };

	queue<stOverlappedEx*> m_SendDataQ;

	ObjPool<Packet>* m_pPackPool{ nullptr };
	concurrent_queue <shared_ptr< Packet >>* m_pPackQ{ nullptr };

	FCriticalSection CS;

	bool m_bIsRunning{ true };
	FRunnableThread* pThread{ nullptr };

};

//////////////////////////////////////////////////////////////////////////

class SERVERHYPERION_API FClientRunnable_Recv : FRunnable
{
public:
	FClientRunnable_Recv();
	~FClientRunnable_Recv();

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

private:
	//DynamicObjectPool<Packet>* m_pDymPackPool{ nullptr };
	queue<Packet*>* m_pPackQ{ nullptr };

	bool m_bIsRunning{ true };
	FRunnableThread* pThread{ nullptr };

};
