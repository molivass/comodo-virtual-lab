#pragma once

#include "CoreMinimal.h"
#include "GizmoComponent.h"
#include "RotationGizmoComponent.generated.h"

/**
 * 
 */
UCLASS()
class COMODO_VIRTUAL_LAB_API URotationGizmoComponent : public UGizmoComponent {
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	virtual void Dragged(const FVector2D MouseDelta) override;
	
	UFUNCTION(BlueprintCallable)
	void Released();
	
private:
	FVector2D LastMouseDelta;
	TArray<FVector2D> MousePositions;
};
