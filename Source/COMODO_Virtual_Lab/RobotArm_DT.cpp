// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotArm_DT.h"

// Sets default values
ARobotArm_DT::ARobotArm_DT() {
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent->SetMobility(EComponentMobility::Movable);
	SetRootComponent(RootComponent);

	JointTransforms.SetNum(6);
	LinkMeshes.SetNum(6);


	
	ToolCenterPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ToolCenterPoint"));
	ToolCenterPoint->SetupAttachment(RootComponent);
    
	ToolMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ToolMesh"));
	ToolMesh->SetupAttachment(ToolCenterPoint);
}

// Called when the game starts or when spawned
void ARobotArm_DT::BeginPlay() {
	Super::BeginPlay();

	InitializeRobotStructure();
	
}

// Called every frame
void ARobotArm_DT::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

void ARobotArm_DT::InitializeRobotStructure() {
	for (int32 i = 0; i < 6; i++) {
		FString JointName = FString::Printf(TEXT("Joint_%d"), i + 1);
		FString LinkName = FString::Printf(TEXT("Link_%d"), i + 1);
        
		// Create joint transform
		JointTransforms[i] = CreateDefaultSubobject<USceneComponent>(*JointName);
        
		if (i == 0) {
			JointTransforms[i]->SetupAttachment(RootComponent);
		} else {
			JointTransforms[i]->SetupAttachment(JointTransforms[i - 1]);
		}
        
		// Create visual mesh for link
		LinkMeshes[i] = CreateDefaultSubobject<UStaticMeshComponent>(*LinkName);
		LinkMeshes[i]->SetupAttachment(JointTransforms[i]);
        
		// Set initial transform
		JointTransforms[i]->SetRelativeLocation(FVector::ZeroVector);
		JointTransforms[i]->SetRelativeRotation(FRotator::ZeroRotator);
	}
    
	// Attach TCP to last joint
	ToolCenterPoint->SetupAttachment(JointTransforms[5]);
}

