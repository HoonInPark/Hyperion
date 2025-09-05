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
	m_pCurRecvPack = new Packet();

	int32 NumLogicalCores = FPlatformMisc::NumberOfCoresIncludingHyperthreads();
	for (int32 i = 0; i < NumLogicalCores - 3; i++)
	{
		m_ReplicationRunnables.Add(new FReplicationRunnable);
	}
}

void URemoteCharacterManager::DeactivateReplication()
{
	delete m_pCurRecvPack;

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
	m_RecvTaskQ.Enqueue(
		[this, Pack = *_pInPack]() mutable
		{
			TScriptInterface<IObserverBase>* pObserver = m_RemoteCharIdx.Find(Pack.GetSessionIdx());
			if (!pObserver)
			{
				UWorld* pWorld = GetWorld();
				
				FActorSpawnParameters SpawnParam;
				SpawnParam.SpawnCollisionHandlingOverride = 
					ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

				auto pRemoteCharacter = pWorld->SpawnActor<ARemoteCharacter>(
					m_RemoteCharClass,
					FVector(Pack.GetPosX(), Pack.GetPosY(), Pack.GetPosZ()),
					FRotator(),
					SpawnParam);

				pRemoteCharacter->SetSessionIdx(Pack.GetSessionIdx());

				TScriptInterface<IObserverBase> ObserverInterface;
				ObserverInterface.SetObject(pRemoteCharacter);
				ObserverInterface.SetInterface(Cast<IObserverBase>(pRemoteCharacter));
				Subscribe(ObserverInterface);

				m_RemoteCharIdx.Add(Pack.GetSessionIdx(), ObserverInterface);
			}
			
			SetCurPack(&Pack);
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
