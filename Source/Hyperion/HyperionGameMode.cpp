// Copyright Epic Games, Inc. All Rights Reserved.

#include "HyperionGameMode.h"
#include "HyperionCharacter.h"
#include "HyperionPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "HyperionMulti/HyperionPlayerState.h"

AHyperionGameMode::AHyperionGameMode()
	: Super()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));

	DefaultPawnClass = PlayerPawnClassFinder.Class;
	PlayerControllerClass = AHyperionPlayerController::StaticClass();
	PlayerStateClass = AHyperionPlayerState::StaticClass();
}

void AHyperionGameMode::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHyperionGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
