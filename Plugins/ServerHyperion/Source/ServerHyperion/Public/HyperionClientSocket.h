// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ClientSocket.h"
#include "HyperionClientSocket.generated.h"

/**
 * 
 */
UCLASS()
class SERVERHYPERION_API UHyperionClientSocket : public UClientSocket
{
	GENERATED_BODY()
	
public:
	virtual void OnConnect() override;
	virtual void OnClose() override;
	virtual void OnReceive(const UINT32 _InSize) override;
};
