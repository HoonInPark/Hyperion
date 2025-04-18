// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HYPERIONBASE_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
    UHealthComponent();

    // 델리게이트 선언 (옵저버들이 구독할 이벤트)
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float, NewHealth);

    // 체력이 변경될 때 호출되는 델리게이트
    FOnHealthChanged OnHealthChanged;

    void ChangeHealth(float Delta)
    {
        Health += Delta;
        OnHealthChanged.Broadcast(Health);
    }

private:
    float Health;
};
