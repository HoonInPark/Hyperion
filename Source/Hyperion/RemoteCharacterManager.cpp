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
	m_pRecvTaskQ = new TQueue <TaskPair>;
}

void URemoteCharacterManager::DeactivateReplication()
{
	if (m_pCurPack)
	{
		delete m_pCurPack;
		m_pCurPack = nullptr;
	}
	if (m_pRecvTaskQ)
	{
		delete m_pRecvTaskQ;
		m_pRecvTaskQ = nullptr;
	}
}

void URemoteCharacterManager::UpdateData()
{
	TaskPair TaskPairToRun;
	while (m_pRecvTaskQ->Dequeue(TaskPairToRun))
	{
		*m_pCurPack = TaskPairToRun.TaskPack;
		TaskPairToRun.TaskFunc(TaskPairToRun.TaskPack);
	}
}

void URemoteCharacterManager::Replicate(const Packet& _InPack) // CAUTION : called in io thread
{
	if (INDEX_NONE == m_RemoteCharIdx.Find(_InPack.GetSessionIdx()))
	{
		m_RemoteCharIdx.Add(_InPack.GetSessionIdx());

		m_pRecvTaskQ->Enqueue({
			[this](const Packet& _InTaskPack)
			{
				UWorld* pWorld = GetWorld();
				check(pWorld);

				auto pRemoteCharacter = pWorld->SpawnActor<ARemoteCharacter>(
					ARemoteCharacter::StaticClass(),
					FVector(_InTaskPack.GetPosX(), _InTaskPack.GetPosY(), _InTaskPack.GetPosZ()),
					FRotator());
				check(pRemoteCharacter);

				pRemoteCharacter->SetSessionIdx(_InTaskPack.GetSessionIdx());

				TScriptInterface<IObserverBase> ObserverInterface;
				ObserverInterface.SetObject(pRemoteCharacter);
				ObserverInterface.SetInterface(Cast<IObserverBase>(pRemoteCharacter));
				Subscribe(ObserverInterface);
			},
			_InPack });
	}
	else
	{
		m_pRecvTaskQ->Enqueue({
			[this](const Packet& _InTaskPack)
			{
				NotifyObservers();
			},
			_InPack });
	}
}
