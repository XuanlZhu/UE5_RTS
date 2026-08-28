// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayEffectTypes.h"
#include "HUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class UHUDWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateHealth(float NewHealth);
	void OnHealthChanged(const FOnAttributeChangeData& Data){UpdateHealth(Data.NewValue);};
};
