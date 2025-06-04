// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HyperionBase/ObservableBase.h"
#include "ServerHyperionLibrary/Packet.h"
#include "RemoteCharacterManager.generated.h"

/**
 *
 */
UCLASS()
class HYPERION_API URemoteCharacterManager : public UObservableBase
{
	GENERATED_BODY()

	struct TaskPair
	{
		TFunction<void()> TaskFunc; 
		Packet Pack;
	};

public:
	URemoteCharacterManager();

	virtual void InitializeComponent() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Replicate(const Packet& _InPack); // CAUTION : called in io thread of cli sock class
	FORCEINLINE Packet* GetCurPack() { return m_pCurPack; }

private:
	TArray<int32> m_RemoteCharIdx;

	Packet* m_pCurPack{ nullptr };
	TQueue <TaskPair>* m_pRecvTaskQ{ nullptr };
};
