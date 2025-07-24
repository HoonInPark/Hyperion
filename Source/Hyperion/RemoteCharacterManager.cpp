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
	m_pCurPack = new Packet;
	//m_RecvTaskQ = TCircularQueue<TFunction<void()>>(12);
	m_PackPool = ObjPool<Packet>(MAX_POOL_SIZE * 5);

	int32 NumLogicalCores = FPlatformMisc::NumberOfCoresIncludingHyperthreads();
	for (int32 i = 0; i < NumLogicalCores - 3; i++)
	{
		m_ReplicationRunnables.Add(new FReplicationRunnable);
	}
}

void URemoteCharacterManager::DeactivateReplication()
{
	if (m_pCurPack)
	{
		delete m_pCurPack;
		m_pCurPack = nullptr;
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
	m_CS.Lock();
	shared_ptr<Packet> pPack = m_PackPool.Acquire();
	m_CS.Unlock();

	if (!pPack)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to Acquire while Replicating..."));
		return;
	}

	*pPack = *_pInPack;

	m_RecvTaskQ.Enqueue(
		[this, pPack]() mutable
		{
			TScriptInterface<IObserverBase>* pObserver = m_RemoteCharIdx.Find(pPack->GetSessionIdx());
			if (!pObserver)
			{
				UWorld* pWorld = GetWorld();
				
				FActorSpawnParameters SpawnParam;
				SpawnParam.SpawnCollisionHandlingOverride = 
					ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

				auto pRemoteCharacter = pWorld->SpawnActor<ARemoteCharacter>(
					m_RemoteCharClass,
					FVector(pPack->GetPosX(), pPack->GetPosY(), pPack->GetPosZ()),
					FRotator(),
					SpawnParam);

				pRemoteCharacter->SetSessionIdx(pPack->GetSessionIdx());

				TScriptInterface<IObserverBase> ObserverInterface;
				ObserverInterface.SetObject(pRemoteCharacter);
				ObserverInterface.SetInterface(Cast<IObserverBase>(pRemoteCharacter));
				Subscribe(ObserverInterface);

				m_RemoteCharIdx.Add(pPack->GetSessionIdx(), ObserverInterface);
			}
			
			SetCurPack(pPack.get());

			m_CS.Lock();
			m_PackPool.Return(pPack);
			m_CS.Unlock();

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
