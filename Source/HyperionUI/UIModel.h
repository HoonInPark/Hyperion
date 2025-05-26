// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HyperionBase/ObserverBase.h"
#include "UIModel.generated.h"

class UObservableBase;

UCLASS()
class HYPERIONUI_API AUIModel : public AActor, public IObserverBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AUIModel();

	virtual void PostInitializeComponents() override;

	//FORCEINLINE void Subscribe(TScriptInterface<IObserverBase> _Observer)
	//{
	//	m_pObservable->Subscribe(_Observer);
	//}
	//FORCEINLINE void Unsubscribe(TScriptInterface<IObserverBase> _Observer)
	//{
	//	m_pObservable->Unsubscribe(_Observer);
	//}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UObservableBase* m_pObservable;

	TMap<FName, FString> m_UIData;
};
