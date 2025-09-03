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
#include "ServerHyperionLibrary/StlCircularQueue.h"
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

	bool SendIO(const shared_ptr< stOverlappedEx > _pInSendOverlappedEx);
	void SendCompleted(const UINT32 _InDataSize);
	bool BindRecv();

	virtual void OnConnect() {}
	virtual void OnClose() {}
	virtual void OnReceive(const UINT32 _InSize) {}

	FORCEINLINE atomic<shared_ptr<stOverlappedEx>>& GetOverlappedEx() { return m_pOverlappedEx; }

	FORCEINLINE StlCircularQueue<Packet>* GetSendPackPool() { return m_pSendPackPool; }
	FORCEINLINE StlCircularQueue<Packet>* GetSendPackQ() { return m_pSendPackQ; }

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
	atomic<bool>						m_bIsSessionIdxSet{ false };
	atomic<UINT32>						m_SessionIdx{ 0 };	// Session Index

	atomic<shared_ptr<stOverlappedEx>>	m_pOverlappedEx;

	StlCircularQueue<Packet>*			m_pSendPackPool;
	StlCircularQueue<Packet>*			m_pSendPackQ;

	StlCircularQueue<stOverlappedEx>*	m_pSendDataPool;
	StlCircularQueue<stOverlappedEx>*	m_pSendDataQ;

	SOCKET m_Sock{ INVALID_SOCKET };

	stOverlappedEx*						m_pRecvOverlappedEx{ nullptr };	//RECV Overlapped I/O작업을 위한 변수	
	FClientRunnable_Send*				m_pClientRunnable_Send{ nullptr };
};

//////////////////////////////////////////////////////////////////////////

class SERVERHYPERION_API FClientRunnable_Send : FRunnable
{
public:
	FClientRunnable_Send(
		UClientSocket*						_pInClientSock,
		StlCircularQueue<stOverlappedEx>*	_InSendDataQ,
		StlCircularQueue<stOverlappedEx>*	_InSendDataPool);

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
	StlCircularQueue<stOverlappedEx>*		m_pSendDataQ;
	StlCircularQueue<stOverlappedEx>*		m_pSendDataPool;

	HANDLE m_IocpHandle{ INVALID_HANDLE_VALUE };

	bool m_bIsRunning{ true };
	FRunnableThread* m_pThread{ nullptr };

	FClientRunnable_IO* m_pClientRunnable_IO{ nullptr };
};

//////////////////////////////////////////////////////////////////////////

class SERVERHYPERION_API FClientRunnable_IO : FRunnable
{
public:
	FClientRunnable_IO(
		UClientSocket*						_pInClientSock,
		HANDLE								_InIocpHandle,
		StlCircularQueue<stOverlappedEx>*	_InSendDataQ,
		StlCircularQueue<stOverlappedEx>*	_InSendDataPool);

	virtual ~FClientRunnable_IO() override;

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;

	void WaitForCompletion() { m_pThread->WaitForCompletion(); }

private:
	void CloseSock(bool _bIsForce = false);

private:
	UClientSocket*							m_pClientSock;
	HANDLE									m_IocpHandle;
	StlCircularQueue<stOverlappedEx>*		m_pSendDataQ;
	StlCircularQueue<stOverlappedEx>*		m_pSendDataPool;

	bool m_bIsRunning{ true };
	FRunnableThread* m_pThread{ nullptr };
};
