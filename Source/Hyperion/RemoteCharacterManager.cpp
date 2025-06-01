// Fill out your copyright notice in the Description page of Project Settings.


#include "RemoteCharacterManager.h"
#include "ServerHyperionLibrary/Packet.h"

URemoteCharacterManager::URemoteCharacterManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URemoteCharacterManager::Replicate(Packet* _pInPack)
{
}
