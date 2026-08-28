// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"

#include "UnitCharacter.generated.h"

class UMyAttributeSet;

UCLASS()
class AUnitCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AUnitCharacter();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override{return AbilitySystemComponent;};
	
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
private:
	UPROPERTY()
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	UMyAttributeSet* AttributeSet;
};
