// Copyright Epic Games, Inc. All Rights Reserved.

#include "QTE/AbilityTask_QTEInput.h"

#include "QTE/QTEWidget.h"
#include "GameFramework/PlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AbilityTask_QTEInput)

UAbilityTask_QTEInput::UAbilityTask_QTEInput(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
}

UAbilityTask_QTEInput* UAbilityTask_QTEInput::StartQTESequence(UGameplayAbility* OwningAbility, FQTESequenceData InSequenceData, TSubclassOf<UQTEWidget> InWidgetClass)
{
	UAbilityTask_QTEInput* Task = NewAbilityTask<UAbilityTask_QTEInput>(OwningAbility);
	Task->SequenceData = MoveTemp(InSequenceData);
	Task->WidgetClass = InWidgetClass;
	return Task;
}

APlayerController* UAbilityTask_QTEInput::GetOwnerPlayerController() const
{
	if (!IsValid(Ability))
	{
		return nullptr;
	}

	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	return ActorInfo ? Cast<APlayerController>(ActorInfo->PlayerController.Get()) : nullptr;
}

void UAbilityTask_QTEInput::Activate()
{
	Super::Activate();

	if (SequenceData.KeyCommands.IsEmpty())
	{
		CompleteQTE(EQTEResult::Failed_Cancelled);
		return;
	}

	RemainingTime = SequenceData.TimeLimit;
	CurrentKeyIndex = 0;
	CurrentRound = 0;
	bIsCompleted = false;

	ShowQTEWidget();
}

void UAbilityTask_QTEInput::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (bIsCompleted)
	{
		return;
	}

	RemainingTime -= DeltaTime;

	if (IsValid(ActiveWidget))
	{
		ActiveWidget->UpdateTimer(RemainingTime, SequenceData.TimeLimit);
	}

	if (RemainingTime <= 0.0f)
	{
		CompleteQTE(EQTEResult::Failed_Timeout);
		return;
	}

	ProcessInput();
}

void UAbilityTask_QTEInput::ProcessInput()
{
	APlayerController* PC = GetOwnerPlayerController();
	if (!IsValid(PC) || !PC->PlayerInput)
	{
		return;
	}

	if(!SequenceData.KeyCommands.IsValidIndex(CurrentKeyIndex))
	{
		return;
	}

	const FKey& ExpectedKey = SequenceData.KeyCommands[CurrentKeyIndex].Key;

	if (PC->WasInputKeyJustPressed(ExpectedKey))
	{
		CurrentKeyIndex++;

		if (CurrentKeyIndex >= SequenceData.KeyCommands.Num())
		{
			AdvanceRound();
		}
		else
		{
			OnKeyProgress.Broadcast(CurrentKeyIndex);

			if (IsValid(ActiveWidget))
			{
				ActiveWidget->UpdateKeyProgress(CurrentKeyIndex);
			}
		}
		return;
	}

	if (!SequenceData.bResetOnWrongInput)
	{
		return;
	}

	const bool bWrongKeyPressed = SequenceData.KeyCommands.ContainsByPredicate(
		[&](const FQTEKeyCommand& Cmd)
		{
			return Cmd.Key != ExpectedKey && PC->WasInputKeyJustPressed(Cmd.Key);
		});

	if (bWrongKeyPressed)
	{
		ResetSequence();
	}
}

void UAbilityTask_QTEInput::ResetSequence()
{
	CurrentKeyIndex = 0;
	OnSequenceReset.Broadcast(0);

	if (IsValid(ActiveWidget))
	{
		ActiveWidget->OnSequenceReset();
		ActiveWidget->UpdateKeyProgress(0);
	}
}

void UAbilityTask_QTEInput::AdvanceRound()
{
	CurrentRound++;

	const int32 TotalRounds = SequenceData.GetRequiredRounds();

	OnRoundCompleted.Broadcast(CurrentRound, TotalRounds);

	if (IsValid(ActiveWidget))
	{
		ActiveWidget->OnRoundCompleted(CurrentRound, TotalRounds);
	}

	if (CurrentRound >= TotalRounds)
	{
		CompleteQTE(EQTEResult::Success);
		return;
	}

	CurrentKeyIndex = 0;

	if (IsValid(ActiveWidget))
	{
		ActiveWidget->UpdateKeyProgress(0);
	}
}

void UAbilityTask_QTEInput::CompleteQTE(EQTEResult Result)
{
	bIsCompleted = true;

	if (IsValid(ActiveWidget))
	{
		ActiveWidget->OnQTEResult(Result);
	}

	OnQTECompleted.Broadcast(Result);
}

void UAbilityTask_QTEInput::ShowQTEWidget()
{
	if (!WidgetClass)
	{
		return;
	}

	APlayerController* PC = GetOwnerPlayerController();
	if (!IsValid(PC))
	{
		return;
	}

	if (ActiveWidget = CreateWidget<UQTEWidget>(PC, WidgetClass))
	{
		ActiveWidget->AddToViewport(100);
		ActiveWidget->InitializeQTE(SequenceData);
		ActiveWidget->UpdateKeyProgress(0);
	}
}

void UAbilityTask_QTEInput::RemoveQTEWidget()
{
	if (IsValid(ActiveWidget))
	{
		ActiveWidget->RemoveFromParent();
		ActiveWidget = nullptr;
	}
}

void UAbilityTask_QTEInput::OnDestroy(bool AbilityEnded)
{
	RemoveQTEWidget();

	Super::OnDestroy(AbilityEnded);
}
