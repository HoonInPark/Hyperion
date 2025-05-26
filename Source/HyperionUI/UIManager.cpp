// Fill out your copyright notice in the Description page of Project Settings.


#include "UIManager.h"

#include "HyperionBase/ObservableBase.h"


// Sets default values
AUIManager::AUIManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	m_pObservable = CreateDefaultSubobject<UObservableBase>(TEXT("Observable"));

}

// Called when the game starts or when spawned
void AUIManager::BeginPlay()
{
	Super::BeginPlay();
	
}
