// Fill out your copyright notice in the Description page of Project Settings.


#include "RemoteCharacterManager.h"

#include "RemoteCharacter.h"
#include "ServerHyperion/Public/ClientSocket.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef GetObject

URemoteCharacterManager::URemoteCharacterManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URemoteCharacterManager::ActivateReplication()
{
	m_pCurRecvPack = new Packet;
	m_pPackPool = new StlCircularQueue<Packet>(MAX_POOL_SIZE * 5);
	for (int i = 0; i < MAX_POOL_SIZE * 5; ++i)
	{
		auto pPack = make_unique<Packet>();
		m_pPackPool->enqueue(pPack);
	}
	
	int32 NumLogicalCores = FPlatformMisc::NumberOfCoresIncludingHyperthreads();
	for (int32 i = 0; i < NumLogicalCores - 3; i++)
	{
		m_ReplicationRunnables.Add(new FReplicationRunnable);
	}
}

void URemoteCharacterManager::DeactivateReplication()
{
	if (m_pCurRecvPack)
	{
		delete m_pCurRecvPack;
		m_pCurRecvPack = nullptr;
	}

	if (m_pPackPool)
	{
		delete m_pPackPool;
		m_pPackPool = nullptr;
	}

	for (auto ReplicationRunnable : m_ReplicationRunnables)
	{
		delete ReplicationRunnable;
	}
	m_ReplicationRunnables.Empty();
}

void URemoteCharacterManager::UpdateData()
{
	TFunction<void()> Task;
	while (m_RecvTaskQ.Dequeue(Task))
	{
		Task();
	}
}

void URemoteCharacterManager::Replicate(Packet* _pInPack) // CAUTION : called in io thread
{
	unique_ptr<Packet> pPack;
	if (!m_pPackPool->dequeue(pPack))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to Acquire while Replicating..."));
		return;
	}

	*pPack = *_pInPack;

	m_RecvTaskQ.Enqueue(
		[this, pPackMoved = move(pPack)]() mutable
		{
			TScriptInterface<IObserverBase>* pObserver = m_RemoteCharIdx.Find(pPackMoved->GetSessionIdx());
			if (!pObserver)
			{
				UWorld* pWorld = GetWorld();
				
				FActorSpawnParameters SpawnParam;
				SpawnParam.SpawnCollisionHandlingOverride = 
					ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

				auto pRemoteCharacter = pWorld->SpawnActor<ARemoteCharacter>(
					m_RemoteCharClass,
					FVector(pPackMoved->GetPosX(), pPackMoved->GetPosY(), pPackMoved->GetPosZ()),
					FRotator(),
					SpawnParam);

				pRemoteCharacter->SetSessionIdx(pPackMoved->GetSessionIdx());

				TScriptInterface<IObserverBase> ObserverInterface;
				ObserverInterface.SetObject(pRemoteCharacter);
				ObserverInterface.SetInterface(Cast<IObserverBase>(pRemoteCharacter));
				Subscribe(ObserverInterface);

				m_RemoteCharIdx.Add(pPackMoved->GetSessionIdx(), ObserverInterface);
			}
			
			// TODO : TaskQ에서 하나씩 꺼내서 실행을 한다 해도, 람다식의 끝나는 속도가 제각각일 수 있음. 중요한 건 람다식의 실행 후 반환이 순서대로 이루어져야 한다는 거.
			SetCurPack(pPackMoved.get());
			m_pPackPool->enqueue(pPackMoved);

			NotifyObservers();
		});
}

void URemoteCharacterManager::DestroyReplicant(Packet* _pInPack)
{
	m_RecvTaskQ.Enqueue(
		[this, SessionIdx = _pInPack->GetSessionIdx()]() mutable
		{
			TScriptInterface<IObserverBase>* pObserver = m_RemoteCharIdx.Find(SessionIdx);
			check(pObserver);

			Unsubscribe(*pObserver);
			m_RemoteCharIdx.Remove(SessionIdx);

			auto pRemoteChar = Cast<ARemoteCharacter>(pObserver->GetObject());
			pRemoteChar->Destroy();
		});
}

//////////////////////////////////////////////////////////////////////////

FReplicationRunnable::FReplicationRunnable()
	: m_TaskQ(TCircularQueue<TFunction<void()>>(20))
{
	m_pThread = FRunnableThread::Create(this, TEXT("ReplicationThread"), 0, TPri_BelowNormal); //windows default = 8mb for thread, could specify more
}

FReplicationRunnable::~FReplicationRunnable()
{
	if (!m_pThread) return;

	delete m_pThread;
	m_pThread = nullptr;
}

bool FReplicationRunnable::Init()
{
	return false;
}

uint32 FReplicationRunnable::Run()
{
	return uint32();
}

void FReplicationRunnable::Stop()
{
}
