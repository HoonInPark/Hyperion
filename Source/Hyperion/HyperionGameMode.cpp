// Copyright Epic Games, Inc. All Rights Reserved.

#include "HyperionGameMode.h"
#include "HyperionCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "HyperionBase/UIHealthBar.h"

AHyperionGameMode::AHyperionGameMode()
	: Super()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));

	DefaultPawnClass = PlayerPawnClassFinder.Class;

}

void AHyperionGameMode::BeginPlay()
{
	Super::BeginPlay();
	
#if OBSERVER_WITH_DELEGATE
	auto pPlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!pPlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is null!"));
		return;
	}

	m_pHealthComp = pPlayerController->GetPawn()->GetComponentByClass<UHealthComponent>();
	if (!m_pHealthComp)
	{
		UE_LOG(LogTemp, Error, TEXT("HealthComponent is null!"));
		return;
	}

	auto pWorld = GetWorld();
	auto pHealthBar = pWorld->SpawnActor<AUIHealthBar>(AUIHealthBar::StaticClass(), FVector(), FRotator());

	pHealthBar->SetHealthComponent(m_pHealthComp);
#else
#endif
}

void AHyperionGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#if OBSERVER_WITH_DELEGATE
	if (m_Timer < 1)
	{
		m_Timer += DeltaTime;
	}
	else
	{
		m_Timer = 0.f;
		m_pHealthComp->ChangeHealth(1.f);
	}
#else
#endif
}
