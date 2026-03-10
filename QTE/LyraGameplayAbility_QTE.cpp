// Copyright Epic Games, Inc. All Rights Reserved.

#include "QTE/LyraGameplayAbility_QTE.h"

#include "QTE/AbilityTask_QTEInput.h"
#include "AbilitySystemComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraGameplayAbility_QTE)

ULyraGameplayAbility_QTE::ULyraGameplayAbility_QTE(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	ActivationPolicy = ELyraAbilityActivationPolicy::OnInputTriggered;
}

void ULyraGameplayAbility_QTE::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	StartQTE();
}

void ULyraGameplayAbility_QTE::StartQTE()
{
	StartQTEWithData(DefaultSequenceData);
}

void ULyraGameplayAbility_QTE::StartQTEWithData(const FQTESequenceData& InSequenceData)
{
	if (IsValid(ActiveQTETask))
	{
		ActiveQTETask->EndTask();
		ActiveQTETask = nullptr;
	}

	ActiveQTETask = UAbilityTask_QTEInput::StartQTESequence(this, InSequenceData, QTEWidgetClass);

	if (IsValid(ActiveQTETask))
	{
		ActiveQTETask->OnQTECompleted.AddDynamic(this, &ThisClass::HandleQTECompleted);
		ActiveQTETask->OnKeyProgress.AddDynamic(this, &ThisClass::HandleQTEKeyProgress);
		ActiveQTETask->OnSequenceReset.AddDynamic(this, &ThisClass::HandleQTEReset);
		ActiveQTETask->OnRoundCompleted.AddDynamic(this, &ThisClass::HandleQTERoundCompleted);
		ActiveQTETask->ReadyForActivation();
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}

void ULyraGameplayAbility_QTE::HandleQTECompleted(EQTEResult Result)
{
	OnQTECompleted(Result);

	if (Result == EQTEResult::Success)
	{
		ApplyGameplayEffectToSelf(SuccessGameplayEffect);
	}
	else
	{
		ApplyGameplayEffectToSelf(FailureGameplayEffect);
	}

	CachedResult = Result;

	if (ResultDisplayDuration > 0.f)
	{
		if (UWorld* const World = GetWorld())
		{
			World->GetTimerManager().SetTimer(ResultDisplayTimerHandle, this, &ThisClass::FinishAbilityAfterDisplay, ResultDisplayDuration, false);
		}
	}
	else
	{
		FinishAbilityAfterDisplay();
	}
}

void ULyraGameplayAbility_QTE::FinishAbilityAfterDisplay()
{
	const bool bWasCancelled = (CachedResult != EQTEResult::Success);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bWasCancelled);
}

void ULyraGameplayAbility_QTE::HandleQTEKeyProgress(int32 CurrentIndex)
{
	OnQTEKeyProgress(CurrentIndex);
}

void ULyraGameplayAbility_QTE::HandleQTEReset(int32 CurrentIndex)
{
	OnQTEReset(CurrentIndex);
}

void ULyraGameplayAbility_QTE::OnQTECompleted_Implementation(EQTEResult Result)
{
}

void ULyraGameplayAbility_QTE::OnQTEKeyProgress_Implementation(int32 CurrentIndex)
{
}

void ULyraGameplayAbility_QTE::OnQTEReset_Implementation(int32 CurrentIndex)
{
}

void ULyraGameplayAbility_QTE::HandleQTERoundCompleted(int32 CompletedRound, int32 TotalRounds)
{
	OnQTERoundCompleted(CompletedRound, TotalRounds);
}

void ULyraGameplayAbility_QTE::OnQTERoundCompleted_Implementation(int32 CompletedRound, int32 TotalRounds)
{
}

void ULyraGameplayAbility_QTE::ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> EffectClass)
{
	if (!EffectClass)
	{
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return;
	}

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(EffectClass, GetAbilityLevel(), MakeEffectContext(CurrentSpecHandle, CurrentActorInfo));

	if (SpecHandle.IsValid())
	{
		ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void ULyraGameplayAbility_QTE::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (UWorld* const World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ResultDisplayTimerHandle);
	}

	if (IsValid(ActiveQTETask))
	{
		ActiveQTETask->EndTask();
		ActiveQTETask = nullptr;
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
