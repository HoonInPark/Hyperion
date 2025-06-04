// Fill out your copyright notice in the Description page of Project Settings.


#include "RemoteCharacterManager.h"

#include "RemoteCharacter.h"

URemoteCharacterManager::URemoteCharacterManager()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void URemoteCharacterManager::InitializeComponent()
{
	//m_pCurPack = new Packet;
}

void URemoteCharacterManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TPair<TFunction<void()>, Packet> TaskPair;

	while (m_RecvTaskQ.Dequeue(TaskPair))
	{
		m_CurPack = TaskPair.Value;
		TaskPair.Key;
	}
}

void URemoteCharacterManager::Replicate(const Packet& _InPack) // CAUTION : called in io thread
{
	if (INDEX_NONE == m_RemoteCharIdx.Find(_InPack.GetSessionIdx()))
	{
		m_RemoteCharIdx.Add(_InPack.GetSessionIdx());

		m_RecvTaskQ.Enqueue({ [=, this]()
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
		m_RecvTaskQ.Enqueue({ [this]()
			{
				NotifyObservers();
			},
			_InPack });
	}
}
