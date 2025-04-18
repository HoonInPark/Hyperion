// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

#define OBSERVER_WITH_DELEGATE 0

/*
* game mode changes the health data in every sec
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HYPERIONBASE_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    UHealthComponent();

#if OBSERVER_WITH_DELEGATE

    // ��������Ʈ ���� (���������� ������ �̺�Ʈ)
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float, NewHealth);

    // ü���� ����� �� ȣ��Ǵ� ��������Ʈ
    FOnHealthChanged OnHealthChanged;

    void ChangeHealth(float Delta)
    {
        m_Health += Delta;
        OnHealthChanged.Broadcast(m_Health);
    }

private:
    float m_Health;
#else
#endif
};
