// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HyperionBase/ObservableBase.h"
#include "RemoteCharacter.h"
#include "RemoteCharacterManager.generated.h"

/**
 * 
 */

class Packet;
UCLASS()
class HYPERION_API URemoteCharacterManager : public UObservableBase
{
	GENERATED_BODY()
	
public:
	URemoteCharacterManager();

	void Replicate(Packet* _pInPack);
	
private:
	virtual void OnReplicate(Packet* _pInPack);
	
private:
	TMap<UINT32, ARemoteCharacter*> m_RemoteChars;
};
