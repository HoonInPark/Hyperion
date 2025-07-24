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
#include "ServerHyperionLibrary/Packet.h"
#include "ServerHyperionLibrary/ObjPool.h"
#include "ServerHyperionLibrary/Define.h"

#include "ClientSocket.generated.h"

using namespace std;

#define MAX_POOL_SIZE 60
#define MAX_RECV_BUFF_SIZE 256

class FClientRunnable_Send;
class FClientRunnable_IO;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SERVERHYPERION_API UClientSocket : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UClientSocket();
	virtual ~UClientSocket() override;

	virtual int32 ActivateThreads(APawn* aPawn);
	virtual int32 DeactivateThreads();

	inline void SendPackQ_Push(shared_ptr<Packet> _pInElem)
	{
		m_SendPackQ.push(_pInElem);
	}

	inline bool SendPackQ_Pop(shared_ptr<Packet>& _pOutElem)
	{
		if (m_SendPackQ.empty()) return false;
		
		_pOutElem = m_SendPackQ.front();
		m_SendPackQ.pop();

		return true;
	}

	bool SendIO();
	bool BindRecv();

	virtual void OnConnect() {}
	virtual void OnClose() {}
	virtual void OnReceive(const UINT32 _InSize) {}

	FORCEINLINE ObjPool<Packet>& GetSendPackPool() { return m_SendPackPool; }
	FORCEINLINE queue <shared_ptr< Packet >>& GetSendPackQ() { return m_SendPackQ; }

	FORCEINLINE SOCKET& GetSock() { return m_Sock; }
	FORCEINLINE char* GetRecvBuff() { return m_RecvBuff; }

	FORCEINLINE const UINT32 GetSessionIdx() { return m_SessionIdx; }
	FORCEINLINE bool IsSessionIdxSet() const { return m_bIsSessionIdxSet; }

protected:
	FORCEINLINE bool SetSessionIdx(const UINT32 _InSessionIdx) 
	{
		if (m_bIsSessionIdxSet) return false;

		m_bIsSessionIdxSet = true;
		m_SessionIdx = _InSessionIdx;
		return true;
	}

protected:
	char m_RecvBuff[MAX_RECV_BUFF_SIZE];

private:
	atomic<bool> m_bIsSessionIdxSet{ false };
	atomic<UINT32> m_SessionIdx{ 0 };	// Session Index

	queue <shared_ptr< stOverlappedEx >>	m_SendDataQ;
	ObjPool<stOverlappedEx>					m_SendDataPool;

	ObjPool<Packet> m_SendPackPool;
	queue <shared_ptr< Packet >> m_SendPackQ;

	SOCKET m_Sock{ INVALID_SOCKET };

	stOverlappedEx*	m_pRecvOverlappedEx{ nullptr };	//RECV Overlapped I/O작업을 위한 변수	
	FClientRunnable_Send* m_pClientRunnable_Send{ nullptr };
};

//////////////////////////////////////////////////////////////////////////

class SERVERHYPERION_API FClientRunnable_Send : FRunnable
{
public:
	FClientRunnable_Send(
		UClientSocket*							_pInClientSock,
		queue <shared_ptr< stOverlappedEx >>&	_InSendDataQ,
		ObjPool<stOverlappedEx>&				_InSendDataPool);
	virtual ~FClientRunnable_Send() override;

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;

	FORCEINLINE void WaitForCompletion() { m_pThread->WaitForCompletion(); }

private:
	bool InitSock();
	bool Connect();
	bool BindIOCompletionPort(HANDLE _InIocpHandle);

	bool SendMsg(const UINT32 _InSize, char* _pInMsg);

private:
	UClientSocket*							m_pClientSock;
	queue <shared_ptr< stOverlappedEx >>&	m_SendDataQ;
	ObjPool<stOverlappedEx>&				m_SendDataPool;

	HANDLE m_IocpHandle{ INVALID_HANDLE_VALUE };

	FCriticalSection m_CS;

	bool m_bIsRunning{ true };
	FRunnableThread* m_pThread{ nullptr };

	FClientRunnable_IO* m_pClientRunnable_IO{ nullptr };
};

//////////////////////////////////////////////////////////////////////////

class SERVERHYPERION_API FClientRunnable_IO : FRunnable
{
public:
	FClientRunnable_IO(
		UClientSocket*							_pInClientSock,
		HANDLE									_InIocpHandle,
		queue <shared_ptr< stOverlappedEx >>&	_InSendDataQ, 
		ObjPool<stOverlappedEx>&				_InSendDataPool);
	virtual ~FClientRunnable_IO() override;

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;

	void WaitForCompletion() { m_pThread->WaitForCompletion(); }

private:
	void CloseSock(bool _bIsForce = false);
	void SendCompleted(const UINT32 _InDataSize);

private:
	UClientSocket*							m_pClientSock;
	HANDLE									m_IocpHandle;
	queue <shared_ptr< stOverlappedEx >>&	m_SendDataQ;
	ObjPool<stOverlappedEx>&				m_SendDataPool;

	FCriticalSection m_CS;

	bool m_bIsRunning{ true };
	FRunnableThread* m_pThread{ nullptr };
};
