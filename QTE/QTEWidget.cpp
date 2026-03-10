// Copyright Epic Games, Inc. All Rights Reserved.

#include "QTE/QTEWidget.h"

#include "QTE/QTEKeyWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Engine/DataTable.h"
#include "Engine/Texture2D.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(QTEWidget)

void UQTEWidget::InitializeQTE(const FQTESequenceData& SequenceData)
{
	ClearKeyWidgets();

	if (!KeyContainer || !KeyWidgetClass)
	{
		return;
	}

	for (const FQTEKeyCommand& Cmd : SequenceData.KeyCommands)
	{
		UQTEKeyWidget* KeyWidget = CreateWidget<UQTEKeyWidget>(this, KeyWidgetClass);
		if (!IsValid(KeyWidget))
		{
			continue;
		}

		KeyWidget->SetupKey(GetKeyDisplayName(Cmd), FindKeyIcon(Cmd));
		KeyWidget->SetKeyState(EQTEKeyState::Pending);

		UHorizontalBoxSlot* KeySlot = KeyContainer->AddChildToHorizontalBox(KeyWidget);
		KeySlot->SetPadding(FMargin(4.f, 0.f));
		KeySlot->SetHorizontalAlignment(HAlign_Center);
		KeySlot->SetVerticalAlignment(VAlign_Center);

		KeyWidgetInstances.Add(KeyWidget);
	}

	if (StatusText)
	{
		StatusText->SetVisibility(ESlateVisibility::Collapsed);
	}

	UpdateRoundCounter(0, SequenceData.GetRequiredRounds());
}

void UQTEWidget::UpdateKeyProgress(int32 CurrentIndex)
{
	for (int32 i = 0; i < KeyWidgetInstances.Num(); ++i)
	{
		UQTEKeyWidget* KW = KeyWidgetInstances[i];
		if (!IsValid(KW))
		{
			continue;
		}

		if (i < CurrentIndex)
		{
			KW->SetKeyState(EQTEKeyState::Completed);
		}
		else if (i == CurrentIndex)
		{
			KW->SetKeyState(EQTEKeyState::Current);
		}
		else
		{
			KW->SetKeyState(EQTEKeyState::Pending);
		}
	}
}

void UQTEWidget::UpdateTimer(float RemainingTime, float TotalTime)
{
	if (TotalTime <= 0.f)
	{
		return;
	}

	const float Pct = FMath::Clamp(RemainingTime / TotalTime, 0.f, 1.f);
	const FLinearColor Color = GetTimerColor(Pct);

	if (TimerBar)
	{
		TimerBar->SetPercent(Pct);
		TimerBar->SetFillColorAndOpacity(Color);
	}

	if (TimerText)
	{
		TimerText->SetText(FText::FromString(
			FString::Printf(TEXT("%.1fs"), FMath::Max(0.f, RemainingTime))));
		TimerText->SetColorAndOpacity(FSlateColor(Color));
	}
}

void UQTEWidget::OnSequenceReset()
{
	SetAllKeyStates(EQTEKeyState::Failed);
}

void UQTEWidget::OnQTEResult(EQTEResult Result)
{
	if (StatusText)
	{
		const bool bSuccess = (Result == EQTEResult::Success);

		StatusText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		StatusText->SetText(FText::FromString(bSuccess ? TEXT("SUCCESS!") : TEXT("FAILED")));
		StatusText->SetColorAndOpacity(FSlateColor(bSuccess ? SuccessTextColor : FailTextColor));
	}

	SetAllKeyStates(Result == EQTEResult::Success ? EQTEKeyState::Completed : EQTEKeyState::Failed);
}

void UQTEWidget::UpdateRoundCounter(int32 CurrentRound, int32 TotalRounds)
{
	if (!CountText)
	{
		return;
	}

	if (TotalRounds <= 1)
	{
		CountText->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		CountText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		CountText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentRound, TotalRounds)));
	}
}

void UQTEWidget::OnRoundCompleted(int32 CompletedRound, int32 TotalRounds)
{
	SetAllKeyStates(EQTEKeyState::Completed);
	UpdateRoundCounter(CompletedRound, TotalRounds);
}

UTexture2D* UQTEWidget::FindKeyIcon(const FQTEKeyCommand& Command) const
{
	if (!Command.KeyIconOverride.IsNull())
	{
		return Command.KeyIconOverride.LoadSynchronous();
	}

	if (IsValid(KeyIconTable))
	{
		const FName RowName = Command.Key.GetFName();
		if (const FQTEKeyIconRow* Row = KeyIconTable->FindRow<FQTEKeyIconRow>(RowName, TEXT("QTEWidget")))
		{
			if (!Row->KeyIcon.IsNull())
			{
				return Row->KeyIcon.LoadSynchronous();
			}
		}
	}

	return nullptr;
}

FText UQTEWidget::GetKeyDisplayName(const FQTEKeyCommand& Command) const
{
	if (!Command.DisplayName.IsEmpty())
	{
		return Command.DisplayName;
	}

	if (IsValid(KeyIconTable))
	{
		const FName RowName = Command.Key.GetFName();
		if (const FQTEKeyIconRow* Row = KeyIconTable->FindRow<FQTEKeyIconRow>(RowName, TEXT("QTEWidget")))
		{
			if (!Row->DisplayName.IsEmpty())
			{
				return Row->DisplayName;
			}
		}
	}

	return Command.Key.GetDisplayName(false);
}

void UQTEWidget::ClearKeyWidgets()
{
	if (KeyContainer)
	{
		KeyContainer->ClearChildren();
	}

	KeyWidgetInstances.Empty();
}

void UQTEWidget::SetAllKeyStates(EQTEKeyState State)
{
	for (UQTEKeyWidget* KW : KeyWidgetInstances)
	{
		if (IsValid(KW))
		{
			KW->SetKeyState(State);
		}
	}
}

FLinearColor UQTEWidget::GetTimerColor(float Percent) const
{
	if (Percent > 0.5f)
	{
		return TimerGreenColor;
	}
	if (Percent > 0.25f)
	{
		return TimerYellowColor;
	}
	return TimerRedColor;
}
