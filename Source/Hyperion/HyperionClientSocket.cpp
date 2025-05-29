// Fill out your copyright notice in the Description page of Project Settings.


#include "HyperionClientSocket.h"

#include "HyperionCharacter.h"
#include "RemoteCharacterManager.h"

int32 UHyperionClientSocket::ActivateThreads(APawn* aPawn)
{
	if (AHyperionCharacter* pChar = Cast<AHyperionCharacter>(aPawn))
	{
		m_pRemoteCharMng = pChar->GetRemoteCharMng();
		return Super::ActivateThreads(aPawn);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to cast Pawn to AHyperionCharacter"));
		return -1; // Return an error code if casting fails
	}
}

int32 UHyperionClientSocket::DeactivateThreads()
{
	return Super::DeactivateThreads();
}

void UHyperionClientSocket::OnConnect()
{
	BindRecv();

	m_pPack = new Packet;
}

void UHyperionClientSocket::OnClose()
{
	if (m_pPack)
	{
		delete m_pPack;
		m_pPack = nullptr;
	}
}

void UHyperionClientSocket::OnReceive(const UINT32 _InSize)
{
	m_pPack->Read(m_RecvBuff, _InSize);

	/*
	UE_LOG(LogTemp, Warning, TEXT("Echoing Player %d : %f, %f, %f || %f, %f, %f"), 
		m_pPack->GetSessionIdx(),
		m_pPack->GetPosX(),
		m_pPack->GetPosY(),
		m_pPack->GetPosZ(),
		m_pPack->GetRotX(),
		m_pPack->GetRotY(),
		m_pPack->GetRotZ());
	*/

	
}
