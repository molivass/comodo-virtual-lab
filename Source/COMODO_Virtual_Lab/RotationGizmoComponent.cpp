#include "RotationGizmoComponent.h"
#include "VirtualLab_GameModeBase.h"
#include "Kismet/KismetMathLibrary.h"

void URotationGizmoComponent::Dragged(const FVector2D MouseDelta) {
	// Super::Dragged(MouseDelta);
	const AVirtualLab_GameModeBase* GameMode = Cast<AVirtualLab_GameModeBase>(GetWorld()->GetAuthGameMode());
	if (!GameMode) return;
	
	const APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if(!PlayerController) return;
	
	USceneComponent* Parent = GetAttachParentActor()->GetParentComponent()->GetAttachParent();
	const FTransform ActorTransform = GameMode->Robot->GetActorTransform();
	const FRotator CameraRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
	const FVector CameraForward = UKismetMathLibrary::GetForwardVector(CameraRotation);
	// const FVector CameraUp = UKismetMathLibrary::GetUpVector(CameraRotation);
	// const FVector CameraRight = UKismetMathLibrary::GetRightVector(CameraRotation);
	
	const FVector RotationAxis = GetForwardVector();
	
	
	FVector2D MousePosition;
	PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y);
	/////
	// FVector2D ScreenAxisDirection;
	// ScreenAxisDirection.X = FVector::DotProduct(AxisDirection,CameraRight);
	// ScreenAxisDirection.Y = FVector::DotProduct(AxisDirection,CameraUp);
	// ScreenAxisDirection.Normalize();
	
	MousePositions.EmplaceAt(0, MouseDelta);
	
	while (MousePositions.Num() > 10) {
		MousePositions.Pop();
	}
	float Direction = 0;
	if (MousePositions.Num() > 3 ) {
		float Area = 0;
		for (int i = 0; i < MousePositions.Num() - 1; i++) {
			Area += FVector2D::CrossProduct(MousePositions[i], MousePositions[i + 1]);
		}
		//UE_LOG(LogTemp, Warning, TEXT("Area: %f"), Area);
		const float ViewAlignment = FMath::Sign(FVector::DotProduct(CameraForward, RotationAxis));
		Direction = FMath::Sign(Area) * ViewAlignment * -1.f;
	}
	//float Direction = FMath::Sign(FVector2D::CrossProduct(ScreenAxisDirection, RelativeMousePos));
	
	// if (LastMouseDelta != MouseDelta) {
	// 	float ViewAlignment = FMath::Sign(FVector::DotProduct(CameraForward, AxisDirection));
	// 	Direction = FMath::Sign(FVector2D::CrossProduct(LastMouseDelta, MouseDelta)) * ViewAlignment;
	// 	LastMouseDelta = MouseDelta;
	// }
	
	FVector2D ScreenPosition;
	PlayerController->ProjectWorldLocationToScreen(GetComponentLocation(),ScreenPosition);
	const FVector2D RelativeMousePos = MousePosition - ScreenPosition;
	
	const float RotationAmount = (RelativeMousePos - (RelativeMousePos - MouseDelta)).Length() * Direction;
	
	const FQuat LocalRotation = ActorTransform.InverseTransformRotation(Parent->GetComponentQuat());
	const FVector LocalRotationAxis = ActorTransform.InverseTransformVector(RotationAxis);
	
	FRotator RotDelta = Parent->GetComponentRotation().Vector().RotateAngleAxisRad(RotationAmount, RotationAxis).Rotation();
	UE_LOG(LogTemp, Display, TEXT("%s"), *RotDelta.ToString())
	// FRotator NewRot = ActorTransform.TransformRotation();
	
	const FQuat RotationDelta(LocalRotationAxis, FMath::DegreesToRadians(RotationAmount));
	const FQuat NewLocalRotation = RotationDelta * LocalRotation;
	
	UE_LOG(LogTemp, Display, TEXT("NewLocalRot: %f %f %f"), NewLocalRotation.Rotator().Roll, NewLocalRotation.Rotator().Pitch, NewLocalRotation.Rotator().Yaw);
	
	const FQuat NewWorldRotation = ActorTransform.TransformRotation(NewLocalRotation);
	UE_LOG(LogTemp, Display, TEXT("NewWorldRot: %f %f %f"), NewWorldRotation.Rotator().Roll, NewWorldRotation.Rotator().Pitch, NewWorldRotation.Rotator().Yaw);
	
	Parent->SetWorldRotation(NewWorldRotation);
}

void URotationGizmoComponent::Released() {
	MousePositions.Empty(10);
}
