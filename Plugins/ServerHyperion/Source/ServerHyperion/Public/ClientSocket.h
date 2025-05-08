// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ClientSocket.generated.h"

class FClientRunnable_Send;
class FClientRunnable_Recv;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SERVERHYPERION_API UClientSocket : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UClientSocket();

	int32 ActivateThreads();
	int32 DeactivateThreads();

private:
	FClientRunnable_Send* m_pClientRunnable_Send{ nullptr };
	FClientRunnable_Recv* m_pClientRunnable_Recv{ nullptr };
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
	bool m_bIsRunning{ true };
	FRunnableThread* pThread{ nullptr };

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
	bool m_bIsRunning{ true };
	FRunnableThread* pThread{ nullptr };

};
