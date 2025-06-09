// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HyperionBase/ObserverBase.h"
#include "RemoteCharacter.generated.h"

class URemoteCharacterManager;
UCLASS()
class HYPERION_API ARemoteCharacter : public ACharacter, public IObserverBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARemoteCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void OnNotify_Implementation(UObservableBase* _pInObservable) override;

	FORCEINLINE void SetSessionIdx(INT32 _InIdx) { m_SessionIdx = _InIdx; }

private:
	void SetDestination(float _InDestX, float _InDestY, float _InDestZ);

private:
	FCriticalSection m_CS;

	float m_InterpSpeed = 5.0f;

	FVector m_Dest{ FVector() };
	bool m_bIsMove{ false };

	INT32 m_SessionIdx;
};
