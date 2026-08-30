// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitCharacter.h"

#include "AIController.h"
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
	//mesh相对位置
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -88));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90, 0));
	
	// 设置 Quinn 网格体
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(
		TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny.SKM_Manny")
	);
	if (MeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);
	}
	//绑定动画
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBPClass(
		TEXT("/Game/Characters/Mannequins/Animations/ABP_Manny")
	);
	if (AnimBPClass.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(AnimBPClass.Class);
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
	//添加AI控制器
	AIControllerClass = AAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	// 配置角色移动
	GetCharacterMovement()->bOrientRotationToMovement = true; // 将角色转向移动方向
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;//约束到地面
	GetCharacterMovement()->bSnapToPlaneAtStart = true;//当角色开始移动时，把角色的位置自动调整到约束的移动平面上。
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
	//绑定动画蓝图
	// 绑定动画蓝图
	// UClass* AnimBPClass = LoadClass<UAnimInstance>(
	// 	nullptr,
	// 	TEXT("/Game/Characters/Mannequins/Animations/ABP_Manny.ABP_Manny_C")
	// );
	// if (AnimBPClass)
	// {
	// 	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	// 	GetMesh()->SetAnimInstanceClass(AnimBPClass);
	// }
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

