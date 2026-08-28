// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitCharacter.h"

#include "MyAttributeSet.h"

// Sets default values
AUnitCharacter::AUnitCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet = CreateDefaultSubobject<UMyAttributeSet>(TEXT("AttributeSet"));
	
}

void AUnitCharacter::BeginPlay()
{
	Super::BeginPlay();
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void AUnitCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void AUnitCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

