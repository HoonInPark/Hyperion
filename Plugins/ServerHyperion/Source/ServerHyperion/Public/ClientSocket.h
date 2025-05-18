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

#include "ClientSocket.generated.h"

using namespace std;

class FClientRunnable_Send;
class FClientRunnable_IO;

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

	inline void SendPackQ_Push(shared_ptr<Packet> _pInElem)
	{
		m_SendPackQ.push(_pInElem);
	}

	inline bool SendPackQ_Pop(shared_ptr<Packet>& _pOutElem) 
	{
		if (_pOutElem = m_SendPackQ.front())
		{
			m_SendPackQ.pop();
			return true;
		}

		return false;
	}

	FORCEINLINE ObjPool<Packet>& GetSendPackPool() { return m_SendPackPool; }
	FORCEINLINE queue <shared_ptr< Packet >>& GetSendPackQ() { return m_SendPackQ; }

	FORCEINLINE SOCKET& GetSock() { return m_Sock; }

	bool SendIO();

private:
	queue<stOverlappedEx*> m_SendDataQ;

	ObjPool<Packet> m_SendPackPool;
	queue <shared_ptr< Packet >> m_SendPackQ;

	SOCKET m_Sock{ INVALID_SOCKET };

	FClientRunnable_Send* m_pClientRunnable_Send{ nullptr };
};

//////////////////////////////////////////////////////////////////////////

class SERVERHYPERION_API FClientRunnable_Send : FRunnable
{
public:
	FClientRunnable_Send(
		UClientSocket* _pInClientSock, 
		queue<stOverlappedEx*>& _InSendDataQ);
	~FClientRunnable_Send();

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

private:
	bool InitSock();
	bool Connect();
	bool BindIOCompletionPort(HANDLE _InIocpHandle);

	bool SendMsg(const UINT32 _InSize, char* _pInMsg);

private:
	UClientSocket* m_pClientSock;
	queue<stOverlappedEx*>& m_SendDataQ;

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
		UClientSocket* _pInClientSock,
		queue<stOverlappedEx*>& _InSendDataQ);
	~FClientRunnable_IO();

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

private:
	void SendCompleted(const UINT32 _InDataSize);

private:
	UClientSocket* m_pClientSock;
	queue<stOverlappedEx*>& m_SendDataQ;

	FCriticalSection m_CS;

	bool m_bIsRunning{ true };
	FRunnableThread* m_pThread{ nullptr };

};
