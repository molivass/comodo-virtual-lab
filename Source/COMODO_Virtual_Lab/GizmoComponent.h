#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "GizmoComponent.generated.h"


UENUM(BlueprintType)
enum class EGizmoAxis : uint8 {
	X,
	Y,
	Z,
	All
};


UCLASS()
class COMODO_VIRTUAL_LAB_API UGizmoComponent : public UStaticMeshComponent {
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "COMODO|Gizmo")
	EGizmoAxis GizmoAxis;
	
	virtual void Dragged(const FVector2D MouseDelta) {}

	UFUNCTION(BlueprintCallable,Category = "COMODO|Gizmo")
	void HideComponent();
	
	UFUNCTION(BlueprintCallable,Category = "COMODO|Gizmo")
	void ShowComponent();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "COMODO|Gizmo")
	void UpdateUI(const double NewAngle, const int JointIndex);
};
