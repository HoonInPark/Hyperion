// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "HyperionBase/ObservableBase.h"
#include "ServerHyperionLibrary/Packet.h"
#include "ServerHyperionLibrary/StlCircularQueue.h"
#include "RemoteCharacterManager.generated.h"

/**
 *
 */
class ARemoteCharacter;
class FReplicationRunnable;
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

	FORCEINLINE void SetCurPack(Packet* _InPack) { *m_pCurRecvPack = *_InPack; }
	FORCEINLINE Packet* GetCurPack() { return m_pCurRecvPack; }

public:
	UPROPERTY(EditAnywhere, Category = "RemoteCharacter")
	TSubclassOf<ARemoteCharacter> m_RemoteCharClass;

private:
	TArray<FReplicationRunnable*> m_ReplicationRunnables;

	TMap<int32, TScriptInterface<IObserverBase>> m_RemoteCharIdx;

	Packet* m_pCurRecvPack{ nullptr };
	TQueue <TFunction<void()>> m_RecvTaskQ;

	StlCircularQueue<Packet>* m_pPackPool;
};

//////////////////////////////////////////////////////////////////////////

class HYPERION_API FReplicationRunnable : FRunnable
{
public:
	FReplicationRunnable();
	virtual ~FReplicationRunnable() override;

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	//virtual void Exit() override;

	FORCEINLINE bool EnqueueTask(TFunction<void()> _InTask)
	{ 
		return m_TaskQ.Enqueue(_InTask); 
	}

private:
	FRunnableThread* m_pThread{ nullptr };
	TCircularQueue<TFunction<void()>> m_TaskQ;
};
