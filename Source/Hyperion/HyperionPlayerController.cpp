// Fill out your copyright notice in the Description page of Project Settings.


#include "HyperionPlayerController.h"

#include "HyperionCharacter.h"
#include "CharacterObservable.h"
#include "ServerHyperion/Public/HyperionClientSocket.h" 
#include "ServerHyperionLibrary/Packet.h"

AHyperionPlayerController::AHyperionPlayerController()
{
	m_pClientSock = CreateDefaultSubobject<UHyperionClientSocket>(TEXT("HyperionClientSocket"));
}

void AHyperionPlayerController::BeginPlay()
{
	Super::BeginPlay();

	
}

void AHyperionPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	// if statement for what pawn class is gonna be possessed


	if (0 != m_pClientSock->ActivateThreads())
		UE_LOG(LogTemp, Error, TEXT("Failed to initialize client socket threads"));
}

void AHyperionPlayerController::OnUnPossess()
{
	Super::OnUnPossess();

	if (0 != m_pClientSock->DeactivateThreads())
		UE_LOG(LogTemp, Error, TEXT("Failed to deinitialize client socket threads"));
}

void AHyperionPlayerController::OnNotify_Implementation(UObservableBase* _pInObservable)
{
	auto pObservable = Cast<UCharacterObservable>(_pInObservable);
	check(pObservable);

	m_CS.Lock();
	
	shared_ptr<Packet> pPack = m_pClientSock->GetSendPackPool().Acquire();

	if (!pPack)
	{
		m_CS.Unlock();
		//UE_LOG(LogTemp, Error, TEXT("Failed to Pop from Packet Object Pool"));
		return;
	}

	m_CS.Unlock();

	pPack->SetSessionIdx(2);
	pPack->SetIsJumping(pObservable->GetbPlayerInAir());

	// Handle the notification from the observable
	if (pObservable->GetPlayerHeader()[static_cast<int32>(ECharStatus::E_WASD)] || pObservable->GetbPlayerInAir())
	{
		pPack->SetPosX(pObservable->GetPlayerLoc().X);
		pPack->SetPosY(pObservable->GetPlayerLoc().Y);
		pPack->SetPosZ(pObservable->GetPlayerLoc().Z);

		//UE_LOG(LogTemp, Warning, TEXT("Player Location Updated: %s"), *(_InNewVec.ToString()));
	}

	if (pObservable->GetPlayerHeader()[static_cast<int32>(ECharStatus::E_MOUSE)])
	{
		pPack->SetRotPitch(pObservable->GetPlayerRot().Pitch);
		pPack->SetRotRoll(pObservable->GetPlayerRot().Roll);
		pPack->SetRotYaw(pObservable->GetPlayerRot().Yaw);

		//UE_LOG(LogTemp, Warning, TEXT("Player Location Updated: %s"), *(_InNewRot.ToString()));
	}

	m_CS.Lock();
	m_pClientSock->SendPackQ_Push(pPack);
	m_CS.Unlock();
}
