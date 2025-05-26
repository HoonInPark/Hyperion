// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "HyperionBase/ObserverBase.h"
#include "UIController.generated.h"

class UObservableBase;
class AUIModel;

UENUM(BlueprintType)
enum class EWindowState : uint8
{
	LOGIN_WINDOW = 0,
	BEF_START_WINDOW,
	ROOM_WINDOW,
	GAME_WINDOW
};

UCLASS()
class HYPERIONUI_API AUIController : public AActor, public IObserverBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUIController();

	virtual void PostInitializeComponents() override;

	FORCEINLINE void Subscribe(TScriptInterface<IObserverBase> _Observer)
	{

	}
	FORCEINLINE void Unsubscribe(TScriptInterface<IObserverBase> _Observer)
	{

	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UObservableBase* m_pObservable;

	

};
