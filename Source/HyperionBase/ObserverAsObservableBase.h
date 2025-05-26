// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObservableBase.h"
#include "ObserverBase.h"
#include "ObserverAsObservableBase.generated.h"

/**
 * 
 */
UCLASS()
class HYPERIONBASE_API UObserverAsObservableBase : public UObservableBase, public IObserverBase
{
	GENERATED_BODY()
	
public:
	virtual void OnNotify_Implementation(UObservableBase* _pInObservable) override;
	
};
