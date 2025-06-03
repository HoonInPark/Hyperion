// Fill out your copyright notice in the Description page of Project Settings.


#include "RemoteCharacterManager.h"

#include "ServerHyperionLibrary/Packet.h"
#include "RemoteCharacter.h"

URemoteCharacterManager::URemoteCharacterManager()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void URemoteCharacterManager::Replicate(Packet* _pInPack)
{
	int32 IdxFound = m_RemoteCharIdx.Find(_pInPack->GetSessionIdx());
	if (INDEX_NONE == IdxFound)
	{
		UWorld* pWorld = GetWorld();
		check(pWorld);

		auto pRemoteCharacter = pWorld->SpawnActor<ARemoteCharacter>(
			ARemoteCharacter::StaticClass(),
			FVector(_pInPack->GetPosX(), _pInPack->GetPosY(), _pInPack->GetPosZ()),
			FRotator());
		check(pRemoteCharacter);

		TScriptInterface<IObserverBase> ObserverInterface;
		ObserverInterface.SetObject(pRemoteCharacter);
		ObserverInterface.SetInterface(Cast<IObserverBase>(pRemoteCharacter));
		Subscribe(ObserverInterface);

		m_RemoteCharIdx.Add(_pInPack->GetSessionIdx());
	}

	NotifyObservers();
	OnReplicate(_pInPack);
}

void URemoteCharacterManager::OnReplicate(Packet* _pInPack)
{
}
