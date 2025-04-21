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
		// Observer 객체가 유효하고 IObserver 인터페이스를 구현하고 있는지 확인
		if (Observer.GetObject() && Observer.GetObject()->GetClass()->ImplementsInterface(UObserverBase::StaticClass()))
		{
			// 옵저버의 OnNotify 함수 호출 ( 점수 변화를 전달 )
			// 실질적인 클래스 인스턴스 -> 함수를 호출
			// 인터페이스 -> 호출
			IObserverBase::Execute_OnNotify(Observer.GetObject(), m_PlayerLoc);
		}
	}
}
