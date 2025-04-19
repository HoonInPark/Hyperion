// Fill out your copyright notice in the Description page of Project Settings.


#include "ObservableBase.h"

#include "ObserverBase.h"

// Sets default values for this component's properties
UObservableBase::UObservableBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UObservableBase::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UObservableBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UObservableBase::Subscribe(TScriptInterface<IObserver> observer)
{
}

void UObservableBase::Unsubscribe(TScriptInterface<IObserver> observer)
{
}

void UObservableBase::NotifyObservers()
{
}
