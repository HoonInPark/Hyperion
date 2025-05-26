// Fill out your copyright notice in the Description page of Project Settings.


#include "UIController.h"

#include "HyperionBase/ObservableBase.h"


// Sets default values
AUIController::AUIController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	m_pObservable = CreateDefaultSubobject<UObservableBase>(TEXT("Observable"));

}

void AUIController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

// Called when the game starts or when spawned
void AUIController::BeginPlay()
{
	Super::BeginPlay();
	
}
