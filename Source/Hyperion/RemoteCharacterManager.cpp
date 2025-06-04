// Fill out your copyright notice in the Description page of Project Settings.


#include "RemoteCharacterManager.h"

#include "RemoteCharacter.h"

URemoteCharacterManager::URemoteCharacterManager()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void URemoteCharacterManager::InitializeComponent()
{
	m_pCurPack = new Packet;
	m_pRecvTaskQ = new TQueue <TaskPair>;
}

void URemoteCharacterManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TaskPair TaskPairToRun;
	while (m_pRecvTaskQ->Dequeue(TaskPairToRun))
	{
		*m_pCurPack = TaskPairToRun.Pack;
		TaskPairToRun.TaskFunc;
	}
}

void URemoteCharacterManager::Replicate(const Packet& _InPack) // CAUTION : called in io thread
{
	if (INDEX_NONE == m_RemoteCharIdx.Find(_InPack.GetSessionIdx()))
	{
		m_RemoteCharIdx.Add(_InPack.GetSessionIdx());

		m_pRecvTaskQ->Enqueue({ [=, this]()
			{
				UWorld* pWorld = GetWorld();
				check(pWorld);

				auto pRemoteCharacter = pWorld->SpawnActor<ARemoteCharacter>(
					ARemoteCharacter::StaticClass(),
					FVector(_InPack.GetPosX(), _InPack.GetPosY(), _InPack.GetPosZ()),
					FRotator());
				check(pRemoteCharacter);

				pRemoteCharacter->SetSessionIdx(_InPack.GetSessionIdx());

				TScriptInterface<IObserverBase> ObserverInterface;
				ObserverInterface.SetObject(pRemoteCharacter);
				ObserverInterface.SetInterface(Cast<IObserverBase>(pRemoteCharacter));
				Subscribe(ObserverInterface);
			},
			_InPack });
	}
	else
	{
		m_pRecvTaskQ->Enqueue({ [this]()
			{
				NotifyObservers();
			},
			_InPack });
	}
}
