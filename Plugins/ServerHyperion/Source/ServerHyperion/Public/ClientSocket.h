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

private:
	
	
};
