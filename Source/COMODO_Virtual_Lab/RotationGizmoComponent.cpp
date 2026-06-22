#include "RotationGizmoComponent.h"
#include "VirtualLab_GameModeBase.h"
#include "Kismet/KismetMathLibrary.h"

void URotationGizmoComponent::Dragged(const FVector2D MouseDelta) {

	const AVirtualLab_GameModeBase* GameMode = Cast<AVirtualLab_GameModeBase>(GetWorld()->GetAuthGameMode());
	if (!GameMode) return;
	
	const APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if(!PlayerController) return;
	
	USceneComponent* Parent = GetAttachParentActor()->GetParentComponent()->GetAttachParent();
	
	const FRotator CameraRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
	const FVector CameraForward = UKismetMathLibrary::GetForwardVector(CameraRotation);
	const FVector RotationAxis = GetForwardVector();
	
	MouseAccumulatedDeltas.EmplaceAt(0, MouseDelta);
	
	while (MouseAccumulatedDeltas.Num() > 10) {
		MouseAccumulatedDeltas.Pop();
	}
	
	float Direction = 0;
	if (MouseAccumulatedDeltas.Num() > 3 ) {
		float Area = 0;
		for (int i = 0; i < MouseAccumulatedDeltas.Num() - 1; i++) {
			Area += FVector2D::CrossProduct(MouseAccumulatedDeltas[i], MouseAccumulatedDeltas[i + 1]);
		}

		const float ViewAlignment = FMath::Sign(FVector::DotProduct(CameraForward, RotationAxis));
		Direction = FMath::Sign(Area) * ViewAlignment * -1.f;
	}

	FVector2D MousePosition;
	PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y);
	FVector2D ScreenPosition;
	PlayerController->ProjectWorldLocationToScreen(GetComponentLocation(),ScreenPosition);
	const FVector2D RelativeMousePos = MousePosition - ScreenPosition;
	
	const float RotationAmount = (RelativeMousePos - (RelativeMousePos - MouseDelta)).Length() * Direction;
	
	const int JointIndex = FCString::Atoi(*Parent->ComponentTags[0].ToString());
	const double AxisMin = GameMode->Robot->RobotDhParams.AxisMin[JointIndex];
	const double AxisMax = GameMode->Robot->RobotDhParams.AxisMax[JointIndex];
	double NewAngle = GameMode->Robot->DesiredTheta[JointIndex];
	
	switch (GizmoAxis) {
		case EGizmoAxis::X:
			//NewAngle = GameMode->Robot->DesiredTheta[JointIndex] - RotationAmount;
			if(NewAngle - RotationAmount <= AxisMax && NewAngle - RotationAmount >= AxisMin) {
				NewAngle -= RotationAmount;
				GameMode->Robot->DesiredTheta[JointIndex] = NewAngle;
				Parent->SetRelativeRotation(FRotator(0, 0, NewAngle));
			}
			break;
		case EGizmoAxis::Y:
			//NewAngle = GameMode->Robot->DesiredTheta[JointIndex] - RotationAmount;
			if(NewAngle - RotationAmount <= AxisMax && NewAngle - RotationAmount >= AxisMin) {
				NewAngle -= RotationAmount;
				GameMode->Robot->DesiredTheta[JointIndex] = NewAngle;
				Parent->SetRelativeRotation(FRotator(NewAngle,0,0));
			}
			break;
		case EGizmoAxis::Z:
			//NewAngle = GameMode->Robot->DesiredTheta[JointIndex] + RotationAmount;
			if(NewAngle + RotationAmount <= AxisMax && NewAngle + RotationAmount >= AxisMin) {
				NewAngle += RotationAmount;
				GameMode->Robot->DesiredTheta[JointIndex] = NewAngle;
				Parent->SetRelativeRotation(FRotator(0, NewAngle, 0));
			}
			break;
		case EGizmoAxis::All:
			break;
	}
	
	UpdateUI(NewAngle, JointIndex);
}

void URotationGizmoComponent::Released() {
	MouseAccumulatedDeltas.Empty(10);
}
