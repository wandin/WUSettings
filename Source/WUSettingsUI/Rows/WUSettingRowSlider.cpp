// Copyright (c) 2026 Wanderson Lopes. All rights reserved.


#include "WUSettingRowSlider.h"

#include "Components/Slider.h"
#include "Components/TextBlock.h"

void UWUSettingRowSlider::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// USlider::SetValue doesn't fire OnValueChanged, so refreshing from code never commits
	Slider->OnValueChanged.AddDynamic(this, &UWUSettingRowSlider::HandleValueChanged);
	Slider->OnMouseCaptureBegin.AddDynamic(this, &UWUSettingRowSlider::HandleCaptureBegin);
	Slider->OnMouseCaptureEnd.AddDynamic(this, &UWUSettingRowSlider::HandleCaptureEnd);
	Slider->OnControllerCaptureBegin.AddDynamic(this, &UWUSettingRowSlider::HandleCaptureBegin);
	Slider->OnControllerCaptureEnd.AddDynamic(this, &UWUSettingRowSlider::HandleCaptureEnd);
}

bool UWUSettingRowSlider::SupportsType(EWUSettingType Type) const
{
	return Type == EWUSettingType::Float || Type == EWUSettingType::Int;
}

void UWUSettingRowSlider::InitControl()
{
	Slider->SetMinValue(Descriptor.MinValue);
	Slider->SetMaxValue(Descriptor.MaxValue);
	Slider->SetStepSize(GetEffectiveStep());
}

void UWUSettingRowSlider::RefreshFromValue()
{
	FWUSettingValue Value;
	if (!GetCurrentValue(Value))
	{
		return;
	}

	const float Current = Value.Type == EWUSettingType::Int ? static_cast<float>(Value.IntValue) : Value.FloatValue;
	Slider->SetValue(Current);
	UpdateValueText(Current);
}

void UWUSettingRowSlider::HandleValueChanged(float NewValue)
{
	UpdateValueText(SnapToStep(NewValue));

	// Keyboard/gamepad steps have no capture begin/end: commit each step
	if (!bDragging)
	{
		CommitSliderValue(NewValue);
	}
}

void UWUSettingRowSlider::HandleCaptureBegin()
{
	bDragging = true;
}

void UWUSettingRowSlider::HandleCaptureEnd()
{
	bDragging = false;
	CommitSliderValue(Slider->GetValue());
}

float UWUSettingRowSlider::GetEffectiveStep() const
{
	// Int settings never step by fractions
	return Descriptor.Type == EWUSettingType::Int ? FMath::Max(1.f, FMath::RoundToFloat(Descriptor.Step)) : Descriptor.Step;
}

float UWUSettingRowSlider::SnapToStep(float RawValue) const
{
	const float Step = GetEffectiveStep();
	if (Step <= 0.f)
	{
		return RawValue;
	}

	const float Snapped = Descriptor.MinValue + FMath::RoundToFloat((RawValue - Descriptor.MinValue) / Step) * Step;
	return FMath::Clamp(Snapped, Descriptor.MinValue, Descriptor.MaxValue);
}

void UWUSettingRowSlider::UpdateValueText(float Value) const
{
	if (!ValueText)
	{
		return;
	}

	if (Descriptor.Type == EWUSettingType::Int || GetEffectiveStep() >= 1.f)
	{
		ValueText->SetText(FText::AsNumber(FMath::RoundToInt(Value)));
		return;
	}

	FNumberFormattingOptions Format;
	Format.MinimumFractionalDigits = 2;
	Format.MaximumFractionalDigits = 2;
	ValueText->SetText(FText::AsNumber(Value, &Format));
}

void UWUSettingRowSlider::CommitSliderValue(float RawValue)
{
	const float Snapped = SnapToStep(RawValue);
	CommitValue(Descriptor.Type == EWUSettingType::Int
		? FWUSettingValue::MakeInt(FMath::RoundToInt(Snapped))
		: FWUSettingValue::MakeFloat(Snapped));
}
