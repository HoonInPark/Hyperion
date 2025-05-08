// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ClientSocket.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SERVERHYPERION_API UClientSocket : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UClientSocket();

	virtual void Activate(bool bReset = false) override;

private:
	
	
};

//////////////////////////////////////////////////////////////////////////

class SERVERHYPERION_API FClientRunnable_Send : FRunnable
{
public:
	FClientRunnable_Send();
	~FClientRunnable_Send();

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

private:
	bool m_bIsRunning{ false };

};

//////////////////////////////////////////////////////////////////////////

class SERVERHYPERION_API FClientRunnable_Recv : FRunnable
{
public:
	FClientRunnable_Recv();
	~FClientRunnable_Recv();

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

private:
	bool m_bIsRunning{ false };

};