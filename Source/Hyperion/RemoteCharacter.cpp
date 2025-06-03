// Fill out your copyright notice in the Description page of Project Settings.


#include "RemoteCharacter.h"

#include "RemoteCharacterManager.h"

// Sets default values
ARemoteCharacter::ARemoteCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ARemoteCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARemoteCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARemoteCharacter::OnNotify_Implementation(UObservableBase* _pInObservable)
{
}
