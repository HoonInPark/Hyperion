// Fill out your copyright notice in the Description page of Project Settings.


#include "HyperionPlayerController.h"

#include "HyperionCharacter.h"
#include "CharacterObservable.h"
#include "HyperionClientSocket.h" 
#include "ServerHyperionLibrary/Packet.h"
#include "RemoteCharacterManager.h"
#include "GameFramework/HUD.h"

AHyperionPlayerController::AHyperionPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;

	m_pClientSock = CreateDefaultSubobject<UHyperionClientSocket>(TEXT("HyperionClientSocket"));
	m_pRemoteCharMng = CreateDefaultSubobject<URemoteCharacterManager>(TEXT("RemoteCharacterManager"));
}

void AHyperionPlayerController::BeginPlay()
{
	Super::BeginPlay();

	
}

void AHyperionPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	m_pRemoteCharMng->UpdateData();
}

void AHyperionPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	// if statement needed for what pawn class is gonna be possessed by for login page

	m_pRemoteCharMng->ActivateReplication();

	if (0 != m_pClientSock->ActivateThreads(aPawn))
		UE_LOG(LogTemp, Error, TEXT("Failed to initialize client socket threads"));
}

void AHyperionPlayerController::OnUnPossess()
{
	Super::OnUnPossess();

	if (0 != m_pClientSock->DeactivateThreads())
		UE_LOG(LogTemp, Error, TEXT("Failed to deinitialize client socket threads"));

	m_pRemoteCharMng->DeactivateReplication();
}

void AHyperionPlayerController::OnNotify_Implementation(UObservableBase* _pInObservable)
{
	if (!m_pClientSock->IsSessionIdxSet()) return;

	auto pObservable = CastChecked<UCharacterObservable>(_pInObservable);

	unique_ptr<Packet> pPack;
	if (!m_pClientSock->GetSendPackPool()->dequeue(pPack))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to Pop from Packet Object Pool"));
		return;
	}

	pPack->SetMsgType(MsgType::MSG_GAME);
	pPack->SetSessionIdx(m_pClientSock->GetSessionIdx());

	pPack->SetIsJumping(pObservable->GetPlayerInAir());

	// Handle the notification from the observable
	if (pObservable->GetPlayerHeader()[static_cast<int32>(ECharStatus::E_WASD)] || pObservable->GetPlayerInAir())
	{
		pPack->SetPosX(pObservable->GetPlayerLoc().X);
		pPack->SetPosY(pObservable->GetPlayerLoc().Y);
		pPack->SetPosZ(pObservable->GetPlayerLoc().Z);
		/*
		UE_LOG(LogTemp, Warning, TEXT("Player Location Updated: %f, %f, %f"), 
			pObservable->GetPlayerLoc().X,
			pObservable->GetPlayerLoc().Y,
			pObservable->GetPlayerLoc().Z);
		*/
	}

	if (pObservable->GetPlayerHeader()[static_cast<int32>(ECharStatus::E_MOUSE)])
	{
		pPack->SetRotPitch(pObservable->GetPlayerRot().Pitch);
		pPack->SetRotRoll(pObservable->GetPlayerRot().Roll);
		pPack->SetRotYaw(pObservable->GetPlayerRot().Yaw);

		/*
		UE_LOG(LogTemp, Warning, TEXT("Player Rotation Updated: %f, %f, %f"), 
			pObservable->GetPlayerRot().Pitch,
			pObservable->GetPlayerRot().Roll,
			pObservable->GetPlayerRot().Yaw);
		*/
	}

	m_pClientSock->GetSendPackQ()->enqueue(pPack);
}
