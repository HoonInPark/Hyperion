// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "HyperionBase/ObservableBase.h"
#include "ServerHyperionLibrary/Packet.h"
#include "ServerHyperionLibrary/ObjPool.h"
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
	void DestroyReplicant(Packet* _pInPack);

	FORCEINLINE void SetCurPack(Packet* _InPack) { *m_pCurPack = *_InPack; }
	FORCEINLINE Packet* GetCurPack() { return m_pCurPack; }

public:
	UPROPERTY(EditAnywhere, Category = "RemoteCharacter")
	TSubclassOf<ARemoteCharacter> m_RemoteCharClass;

private:
	TMap<int32, TScriptInterface<IObserverBase>> m_RemoteCharIdx;

	Packet* m_pCurPack{ nullptr };
	TQueue <TFunction<void()>> m_RecvTaskQ;

	FCriticalSection m_CS;
	ObjPool<Packet> m_PackPool;
};
