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

    // ��������Ʈ ���� (���������� ������ �̺�Ʈ)
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float, NewHealth);

    // ü���� ����� �� ȣ��Ǵ� ��������Ʈ
    FOnHealthChanged OnHealthChanged;

    void ChangeHealth(float Delta)
    {
        Health += Delta;
        OnHealthChanged.Broadcast(Health);
    }

private:
    float Health;
};
