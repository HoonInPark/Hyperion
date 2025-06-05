// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HyperionBase/ObservableBase.h"
#include "ServerHyperionLibrary/Packet.h"
#include "RemoteCharacterManager.generated.h"

/**
 *
 */
class ARemoteCharacter;
UCLASS()
class HYPERION_API URemoteCharacterManager : public UObservableBase
{
	GENERATED_BODY()

public:
	URemoteCharacterManager();

	void ActivateReplication();
	void DeactivateReplication();
	void UpdateData();

	void Replicate(Packet* _pInPack); // CAUTION : called in io thread of cli sock class

	FORCEINLINE void SetCurPack(const Packet& _InPack) { *m_pCurPack = _InPack; }
	FORCEINLINE Packet* GetCurPack() { return m_pCurPack; }

public:
	UPROPERTY(EditAnywhere, Category = "RemoteCharacter")
	TSubclassOf<ARemoteCharacter> m_RemoteCharClass;

private:
	TArray<int32> m_RemoteCharIdx;

	Packet* m_pCurPack{ nullptr };
	TQueue <TFunction<void()>> m_RecvTaskQ;
};
