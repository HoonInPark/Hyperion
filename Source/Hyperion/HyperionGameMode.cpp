// Copyright Epic Games, Inc. All Rights Reserved.

#include "HyperionGameMode.h"
#include "HyperionCharacter.h"
#include "HyperionPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "HyperionUI/UIModel.h"
#include "HyperionUI/UIController.h"

AHyperionGameMode::AHyperionGameMode()
	: Super()
{
	//PrimaryActorTick.bCanEverTick = true;
	
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));

	DefaultPawnClass = PlayerPawnClassFinder.Class;
	PlayerControllerClass = AHyperionPlayerController::StaticClass();
}

void AHyperionGameMode::BeginPlay()
{
	Super::BeginPlay();
	/*
	auto pWorld = GetWorld();
	check(pWorld);
	
	auto pUICtrl = pWorld->SpawnActor<AUIController>(AUIController::StaticClass(), FVector(), FRotator(), FActorSpawnParameters());
	auto pUIModel = pWorld->SpawnActor<AUIModel>(AUIModel::StaticClass(), FVector(), FRotator(), FActorSpawnParameters());

	check(pUICtrl);
	check(pUIModel);
	*/

}
