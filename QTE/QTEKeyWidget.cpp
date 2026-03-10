// Copyright Epic Games, Inc. All Rights Reserved.

#include "QTE/QTEKeyWidget.h"

#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(QTEKeyWidget)

void UQTEKeyWidget::SetupKey(const FText& Label, UTexture2D* Icon)
{
	if (KeyText)
	{
		KeyText->SetText(Label);
	}

	if (KeyImage)
	{
		if (Icon)
		{
			KeyImage->SetBrushFromTexture(Icon);
			KeyImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			KeyImage->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UQTEKeyWidget::SetKeyState(EQTEKeyState NewState)
{
	FLinearColor BorderColor;
	FLinearColor TintColor;

	switch (NewState)
	{
	case EQTEKeyState::Current:
		BorderColor = CurrentBorderColor;
		TintColor = CurrentTintColor;
		break;
	case EQTEKeyState::Completed:
		BorderColor = CompletedBorderColor;
		TintColor = CompletedTintColor;
		break;
	case EQTEKeyState::Failed:
		BorderColor = FailedBorderColor;
		TintColor = FailedTintColor;
		break;
	default: // Pending
		BorderColor = PendingBorderColor;
		TintColor = PendingTintColor;
		break;
	}

	if (KeyBorder)
	{
		KeyBorder->SetBrushColor(BorderColor);
	}

	if (KeyText)
	{
		KeyText->SetColorAndOpacity(FSlateColor(TintColor));
	}

	if (KeyImage)
	{
		KeyImage->SetColorAndOpacity(TintColor);
	}
}
