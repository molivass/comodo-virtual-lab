// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RobotArm_DT.generated.h"

UCLASS()
class COMODO_VIRTUAL_LAB_API ARobotArm_DT : public AActor {
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARobotArm_DT();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
