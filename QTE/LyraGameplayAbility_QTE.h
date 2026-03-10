// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystem/Abilities/LyraGameplayAbility.h"
#include "QTE/QTETypes.h"

#include "LyraGameplayAbility_QTE.generated.h"

class UQTEWidget;
class UAbilityTask_QTEInput;
class UGameplayEffect;

// ---------------------------------------------------------------------------
// Gameplay Ability — QTE 베이스 어빌리티
// ---------------------------------------------------------------------------

/** 제한 시간 내 키 시퀀스를 순서대로 입력하면 성공. 잘못된 키 입력 시 처음부터 다시 입력. Blueprint에서 QTEWidgetClass, DefaultSequenceData, 성공/실패 GE 설정 */
UCLASS(Abstract, Blueprintable)
class LYRAGAME_API ULyraGameplayAbility_QTE : public ULyraGameplayAbility
{
	GENERATED_BODY()

public:
	ULyraGameplayAbility_QTE(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	UFUNCTION(BlueprintCallable, Category = "QTE")
	void StartQTE();
	UFUNCTION(BlueprintCallable, Category = "QTE")
	void StartQTEWithData(const FQTESequenceData& InSequenceData);
	UFUNCTION(BlueprintNativeEvent, Category = "QTE")
	void OnQTECompleted(EQTEResult Result);
	UFUNCTION(BlueprintNativeEvent, Category = "QTE")
	void OnQTEKeyProgress(int32 CurrentIndex);
	UFUNCTION(BlueprintNativeEvent, Category = "QTE")
	void OnQTEReset(int32 CurrentIndex);
	UFUNCTION(BlueprintNativeEvent, Category = "QTE")
	void OnQTERoundCompleted(int32 CompletedRound, int32 TotalRounds);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "QTE")
	FQTESequenceData DefaultSequenceData;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "QTE")
	TSubclassOf<UQTEWidget> QTEWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "QTE", meta = (ClampMin = "0.0"))
	float ResultDisplayDuration = 1.5f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "QTE|Effects")
	TSubclassOf<UGameplayEffect> SuccessGameplayEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "QTE|Effects")
	TSubclassOf<UGameplayEffect> FailureGameplayEffect;

private:
	UFUNCTION()
	void HandleQTECompleted(EQTEResult Result);
	UFUNCTION()
	void HandleQTEKeyProgress(int32 CurrentIndex);
	UFUNCTION()
	void HandleQTEReset(int32 CurrentIndex);
	UFUNCTION()
	void HandleQTERoundCompleted(int32 CompletedRound, int32 TotalRounds);
	void ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> EffectClass);
	void FinishAbilityAfterDisplay();
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_QTEInput> ActiveQTETask;
	
	FTimerHandle ResultDisplayTimerHandle;
	EQTEResult CachedResult = EQTEResult::Failed_Cancelled;
};
