// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealthComponent.h"
#include "UIHealthBar.generated.h"

UCLASS()
class HYPERIONBASE_API AUIHealthBar : public AActor
{
	GENERATED_BODY()
	
public:
    AUIHealthBar();

#if OBSERVER_WITH_DELEGATE
    // ü�� ������Ʈ�� �������� ����ϴ� �޼���
    void SetHealthComponent(UHealthComponent* HealthComp)
    {
        if (HealthComp)
        {
            HealthComp->OnHealthChanged.AddDynamic(this, &AUIHealthBar::UpdateHealthBar);
        }
    }

    // ü�� ��ȭ �� UI�� ������Ʈ�ϴ� �޼���
    //UFUNCTION()
    void UpdateHealthBar(float NewHealth)
    {
		UE_LOG(LogTemp, Warning, TEXT("Health changed: %f"), NewHealth);
    }
#else
#endif
};
