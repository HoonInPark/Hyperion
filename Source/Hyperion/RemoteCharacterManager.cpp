// Fill out your copyright notice in the Description page of Project Settings.


#include "RemoteCharacterManager.h"

#include "RemoteCharacter.h"

URemoteCharacterManager::URemoteCharacterManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URemoteCharacterManager::ActivateReplication()
{
	m_pCurPack = new Packet;
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
	if (INDEX_NONE == m_RemoteCharIdx.Find(_pInPack->GetSessionIdx()))
	{
		m_RemoteCharIdx.Add(_pInPack->GetSessionIdx());

		m_RecvTaskQ.Enqueue(
			[this, TaskPackCopied = *_pInPack]() // CAUTION : in TaskPackCopied = *_pInPack, Copy Constructor Called
			{
				UWorld* pWorld = GetWorld();
				check(pWorld);

				auto pRemoteCharacter = pWorld->SpawnActor<ARemoteCharacter>(
					m_RemoteCharClass,
					FVector(TaskPackCopied.GetPosX(), TaskPackCopied.GetPosY(), TaskPackCopied.GetPosZ()),
					FRotator());
				check(pRemoteCharacter);

				pRemoteCharacter->SetSessionIdx(TaskPackCopied.GetSessionIdx());

				SetCurPack(TaskPackCopied);

				TScriptInterface<IObserverBase> ObserverInterface;
				ObserverInterface.SetObject(pRemoteCharacter);
				ObserverInterface.SetInterface(Cast<IObserverBase>(pRemoteCharacter));
				Subscribe(ObserverInterface);
			});
	}
	else
	{
		m_RecvTaskQ.Enqueue(
			[this, TaskPackCopied = *_pInPack]()
			{
				SetCurPack(TaskPackCopied);
				NotifyObservers();
			});
	}
}
