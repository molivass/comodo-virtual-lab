// Fill out your copyright notice in the Description page of Project Settings.


#include "TranslationGizmoComponent.h"
#include "VirtualLab_GameModeBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kinematics.h"

void UTranslationGizmoComponent::Dragged(const FVector2D MouseDelta) {
	// Super::Dragged(MouseXDelta, MouseYDelta);
	AVirtualLab_GameModeBase* GameMode = Cast<AVirtualLab_GameModeBase>(GetWorld()->GetAuthGameMode());
	if (!GameMode) return;
	
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if(!PlayerController) return;
	
	// const FVector CameraLocation = PlayerController->PlayerCameraManager->GetCameraLocation();
	const FRotator CameraRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
	const FVector CameraUp = UKismetMathLibrary::GetUpVector(CameraRotation);
	const FVector CameraRight = UKismetMathLibrary::GetRightVector(CameraRotation);
	
	const FVector AxisDirection = GetForwardVector();
	FVector2D ScreenAxisDirection;
    ScreenAxisDirection.X = FVector::DotProduct(AxisDirection,CameraRight);
    ScreenAxisDirection.Y = FVector::DotProduct(AxisDirection,CameraUp);
    ScreenAxisDirection.Normalize();
	
	const float MoveAmount = FVector2D::DotProduct(MouseDelta, ScreenAxisDirection);

	const FTransform ActorTransform = GameMode->Robot->GetActorTransform();

	FRotator LocalRot = ActorTransform.InverseTransformRotation(GameMode->Robot->Link6->GetComponentQuat()).Rotator();
	LocalRot = FRotator(LocalRot.Pitch, LocalRot.Yaw-180, LocalRot.Roll);

	const FVector WorldLocation = GameMode->Robot->Link6->GetComponentLocation();
	
	// FVector CameraToObject = (WorldLocation - CameraLocation).GetSafeNormal();
	// FVector CameraDirection = (CameraLocation - ActorTransform.GetLocation()).GetSafeNormal();
	
	// if (FVector::DotProduct(AxisDirection,CameraToObject) > 0.0f) {
	// 	MoveAmount *= -1;
	// }
	//FVector NewLocation = LocalPos + AxisDirection * MoveAmount;
	FVector NewWorldPos = WorldLocation + (AxisDirection * MoveAmount);
	FVector NewLocalPos = ActorTransform.InverseTransformPosition(NewWorldPos);
	NewLocalPos = FVector(-NewLocalPos.X, NewLocalPos.Y, NewLocalPos.Z);
	
	FMatrix RotationMatrix = UKinematics::GetRotationMatrix(LocalRot);
	FMatrix TransformMatrix = UKinematics::ChangeLocationInTransformMatrix(RotationMatrix, NewLocalPos);
	
	// TArray<double> CurrentJointAngles = {0.f, 90.f, -0.f, 0.f, -0.f, 0.f};
	TArray<double> NewJointAngles;
	UKinematics::GetClosestIKAnglesToLocation(GameMode->Robot->RobotDhParams, TransformMatrix, GameMode->Robot->GetCurrentThetas(), NewJointAngles );
	if(NewJointAngles.Num() != 0) {
		GameMode->Robot->MoveJoints(NewJointAngles);
	}
}
