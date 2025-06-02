// Fill out your copyright notice in the Description page of Project Settings.


#include "RemoteCharacterManager.h"

#include "ServerHyperionLibrary/Packet.h"

URemoteCharacterManager::URemoteCharacterManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URemoteCharacterManager::Replicate(Packet* _pInPack)
{
	auto pChar = *(m_RemoteChars.Find(_pInPack->GetSessionIdx()));
	if (!pChar)
	{
		//if (!HasAuthority()) return; // 서버만 실행

		UWorld* pWorld = GetWorld();
		check(pWorld);

		FVector SpawnLoc = FVector(
			_pInPack->GetPosX(),
			_pInPack->GetPosY(),
			_pInPack->GetPosZ());
		FRotator SpawnRot = FRotator(
			_pInPack->GetRotX(),
			_pInPack->GetRotY(),
			_pInPack->GetRotZ());

		auto pRemoteCharacter = pWorld->SpawnActor<ARemoteCharacter>(
			ARemoteCharacter::StaticClass(),
			SpawnLoc,
			SpawnRot);
		check(pRemoteCharacter);

		m_RemoteChars.Add(_pInPack->GetSessionIdx(), pRemoteCharacter);
	}



	OnReplicate(_pInPack);
}

void URemoteCharacterManager::OnReplicate(Packet* _pInPack)
{
}
