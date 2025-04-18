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
    // 체력 컴포넌트를 옵저버로 등록하는 메서드
    void SetHealthComponent(UHealthComponent* HealthComp)
    {
        if (HealthComp)
        {
            HealthComp->OnHealthChanged.AddDynamic(this, &AUIHealthBar::UpdateHealthBar);
        }
    }

    // 체력 변화 시 UI를 업데이트하는 메서드
    //UFUNCTION()
    void UpdateHealthBar(float NewHealth)
    {
		UE_LOG(LogTemp, Warning, TEXT("Health changed: %f"), NewHealth);
    }
#else
#endif
};
