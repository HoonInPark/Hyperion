// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ServerHyperion/Public/ClientSocket.h"
#include "ServerHyperionLibrary/Packet.h"
#include "HyperionClientSocket.generated.h"

/**
 * 
 */
class AHyperionCharacter;

UCLASS()
class SERVERHYPERION_API UHyperionClientSocket : public UClientSocket
{
	GENERATED_BODY()
	
public:
	virtual int32 ActivateThreads(APawn* aPawn) override;
	virtual int32 DeactivateThreads() override;

	virtual void OnConnect() override;
	virtual void OnClose() override;
	virtual void OnReceive(const UINT32 _InSize) override;

private:
	AHyperionCharacter* m_pChar{ nullptr };

	Packet* m_pPack;
};
