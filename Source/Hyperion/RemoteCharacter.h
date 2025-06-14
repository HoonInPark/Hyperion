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
	void SetRotation(float _InRotX, float _InRotY, float _InRotZ);

private:
	float m_InterpSpeed = 5.0f;

	// replicated data...
	FVector m_Dest{ FVector() };
	FRotator m_Rot{ FRotator() };
	bool m_bIsMove{ false };

	FVector m_PrevTickPos;

	INT32 m_SessionIdx;
};
