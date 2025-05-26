// Fill out your copyright notice in the Description page of Project Settings.


#include "ObservableBase.h"

#include "ObserverBase.h"

// Sets default values for this component's properties
UObservableBase::UObservableBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UObservableBase::Subscribe(TScriptInterface<IObserverBase> _Observer)
{
	Observers.Add(_Observer);
}

void UObservableBase::Unsubscribe(TScriptInterface<IObserverBase> _Observer)
{
	Observers.Remove(_Observer);
}

void UObservableBase::NotifyObservers()
{
	for (TScriptInterface<IObserverBase> Observer : Observers)
	{
		if (Observer.GetObject() && Observer.GetObject()->GetClass()->ImplementsInterface(UObserverBase::StaticClass()))
		{
			IObserverBase::Execute_OnNotify(Observer.GetObject(), this);
		}
	}
}
