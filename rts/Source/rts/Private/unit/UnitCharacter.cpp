// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitCharacter.h"

#include "MyAttributeSet.h"
#include "NiagaraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NiagaraSystem.h"
#include "Components/CapsuleComponent.h"

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
	//创建选中特效
	SelectionEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SelectionEffect"));
	SelectionEffectComponent->SetupAttachment(RootComponent);
	SelectionEffect = LoadObject<UNiagaraSystem>(
		nullptr,
		TEXT("/Game/UI/SelectBox.SelectBox")
	);
	SelectionEffectComponent->SetAsset(SelectionEffect);
	// SelectionEffectComponent->SetAutoActivate(true);
	
}

void AUnitCharacter::SetSelected(bool bIsSelected){
	UE_LOG(LogTemp, Log, TEXT("SetSelected移除或开启特效"));
	if (bIsSelected)
	{
		SelectionEffectComponent->SetVisibility(true);bSelected = true;
		UE_LOG(LogTemp, Log, TEXT("被选中，开启特效"));
	}
	else
	{
		SelectionEffectComponent->SetVisibility(false);bSelected = false;
		UE_LOG(LogTemp, Log, TEXT("移除选中特效"));
	}
		
};

void AUnitCharacter::BeginPlay()
{
	Super::BeginPlay();
	//设置ASC的owner
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	
	SelectionEffectComponent->SetVisibility(false);
	// 稍微向前移动
	// AddMovementInput(GetActorForwardVector(), 100);
	//创建特效，选中框
	// SelectionEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SelectionEffect"));
	// SelectionEffectComponent->SetupAttachment(RootComponent);
	// SelectionEffectComponent->SetAutoActivate(false);
	
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

