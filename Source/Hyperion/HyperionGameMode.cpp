// Copyright Epic Games, Inc. All Rights Reserved.

#include "HyperionGameMode.h"

#include "HyperionCharacter.h"
#include "HyperionPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "HyperionUI/UIController.h"
#include "HyperionUI/UIModel.h"
#include "HyperionUI/UIViewer.h"

AHyperionGameMode::AHyperionGameMode()
	: Super()
{
	//PrimaryActorTick.bCanEverTick = true;
	
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));

	DefaultPawnClass = PlayerPawnClassFinder.Class;
	PlayerControllerClass = AHyperionPlayerController::StaticClass();

	m_pUIController = CreateDefaultSubobject<UUIController>("UIController");
	m_pUIModel = CreateDefaultSubobject<UUIModel>("UIModel");
	m_pUIViewer = CreateDefaultSubobject<UUIViewer>("UIViewer");
}

void AHyperionGameMode::BeginPlay()
{
	Super::BeginPlay();
	

}
