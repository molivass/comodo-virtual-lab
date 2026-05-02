// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RobotArm_DT.h"
#include "GameFramework/GameModeBase.h"
#include "VirtualLab_GameModeBase.generated.h"
/**
 * 
 */
UCLASS()
class COMODO_VIRTUAL_LAB_API AVirtualLab_GameModeBase : public AGameModeBase {
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ARobotArm_DT* Robot;
};
