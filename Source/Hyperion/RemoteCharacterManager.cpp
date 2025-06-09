// Fill out your copyright notice in the Description page of Project Settings.


#include "RemoteCharacterManager.h"

#include "RemoteCharacter.h"
#include "ServerHyperion/Public/ClientSocket.h"

URemoteCharacterManager::URemoteCharacterManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URemoteCharacterManager::ActivateReplication()
{
	m_pCurPack = new Packet;
	m_PackPool = ObjPool<Packet>(MAX_POOL_SIZE);
}

void URemoteCharacterManager::DeactivateReplication()
{
	if (m_pCurPack)
	{
		delete m_pCurPack;
		m_pCurPack = nullptr;
	}
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
		UE_LOG(LogTemp, Error, TEXT("Failed to Acquire while Replicating..."));

	*pPack = *_pInPack;

	if (INDEX_NONE == m_RemoteCharIdx.Find(_pInPack->GetSessionIdx()))
	{
		m_RemoteCharIdx.Add(_pInPack->GetSessionIdx());

		m_RecvTaskQ.Enqueue(
			[this, pPack]() mutable
			{
				UWorld* pWorld = GetWorld();
				//check(pWorld);

				auto pRemoteCharacter = pWorld->SpawnActor<ARemoteCharacter>(
					m_RemoteCharClass,
					FVector(pPack->GetPosX(), pPack->GetPosY(), pPack->GetPosZ()),
					FRotator());
				//check(pRemoteCharacter);

				pRemoteCharacter->SetSessionIdx(pPack->GetSessionIdx());

				SetCurPack(pPack.get());

				m_CS.Lock();
				m_PackPool.Return(pPack);
				m_CS.Unlock();

				TScriptInterface<IObserverBase> ObserverInterface;
				ObserverInterface.SetObject(pRemoteCharacter);
				ObserverInterface.SetInterface(Cast<IObserverBase>(pRemoteCharacter));
				Subscribe(ObserverInterface);

				NotifyObservers();
			});
	}
	else
	{
		m_RecvTaskQ.Enqueue(
			[this, pPack]() mutable
			{
				SetCurPack(pPack.get());
				
				m_CS.Lock();
				m_PackPool.Return(pPack);
				m_CS.Unlock();
				
				NotifyObservers();
			});
	}
}
