// Copyright Epic Games, Inc. All Rights Reserved.

#include "rtsGameMode.h"
#include "rtsPlayerController.h"
#include "rtsCharacter.h"
#include "UObject/ConstructorHelpers.h"

ArtsGameMode::ArtsGameMode()
{
	// 使用我们自定义的 PlayerController 类
	PlayerControllerClass = ArtsPlayerController::StaticClass();

	// 将默认 Pawn 类设置为我们的蓝图角色
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// 将默认控制器设置为我们基于蓝图（Blueprint）创建的控制器。
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}

void ArtsGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	FActorSpawnParameters SpawnParams;//刷新参数
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	GetWorld()->SpawnActor<AUnitCharacter>(
		AUnitCharacter::StaticClass(),
		FVector(1000, 1800, 0),
		FRotator::ZeroRotator,
		SpawnParams
	);
	UE_LOG(LogTemp, Log, TEXT("生成一个敌人"));
}
