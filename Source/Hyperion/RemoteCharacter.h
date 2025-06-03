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

};
