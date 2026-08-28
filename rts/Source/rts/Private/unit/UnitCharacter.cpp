// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitCharacter.h"

#include "MyAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AUnitCharacter::AUnitCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	//添加ASC和属性系统
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet = CreateDefaultSubobject<UMyAttributeSet>(TEXT("AttributeSet"));
	
	// 设置 Quinn 网格体
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(
		TEXT("/Game/Characters/Mannequins/Meshes/SKM_Quinn.SKM_Quinn")
	);

	if (MeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);
	}
	
}

void AUnitCharacter::BeginPlay()
{
	Super::BeginPlay();
	//设置ASC的owner
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// 稍微向前移动
	// AddMovementInput(GetActorForwardVector(), 100);
}

void AUnitCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}
//设置输入组件
void AUnitCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

