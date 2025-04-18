// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Observer.h"
#include "GameStateSub.generated.h"

/**
 * 
 */
UCLASS()
class HYPERIONBASE_API UGameStateSub : public UObject
{
	GENERATED_BODY()
	
private:
	// 옵저버 리스트 : 등록된 옵저버들이 상태 변화를 수신함
	TArray<TScriptInterface<IObserver>> Observers;

	// 주체의 상태 정보 ( 예: 플레이어 점수 )
	int32 PlayerScore;

public:
	// 생성자 : 초기 점수 설정
	UGameStateSub();

	// 옵저버 등록 : 주체가 상태 변화를 알릴 옵저버를 등록함
	void RegisterObserver(TScriptInterface<IObserver> observer);

	// 옵저버 등록 해제 : 주체가 상태 변화 알림을 중지할 옵저버를 제거함
	void UnregisterObserver(TScriptInterface<IObserver> observer);

	// 상태 변화 발생 시 모든 옵저버에게 알림
	void NotifyObservers();

	// 상태( 점수 )를 변경하는 함수. 상태가 변경되면 NotifyObserver()가 호출됨
	void IncreaseScore(int32 Amount);

	// 현재 점수 반환
	int32 GetScore() const { return PlayerScore; }
};
