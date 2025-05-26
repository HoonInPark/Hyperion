// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObservableBase.generated.h"

class IObserverBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HYPERIONBASE_API UObservableBase : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UObservableBase();

public:
	virtual void Subscribe(TScriptInterface<IObserverBase> _Observer);
	virtual void Unsubscribe(TScriptInterface<IObserverBase> _Observer);

	void NotifyObservers();

private:
	TArray<TScriptInterface<IObserverBase>> Observers;
};
