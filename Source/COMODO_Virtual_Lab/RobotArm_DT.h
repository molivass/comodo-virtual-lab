#pragma once

#include "CoreMinimal.h"
#include "Kinematics.h"
#include "GameFramework/Actor.h"
#include "RobotArm_DT.generated.h"

//const TArray<double> a = {0.f, 28.f, 1.f, 0.f, 0.f, 0.f};
//const TArray<double> d = {32.7f, 0.f, 0.f, 30.f, 0.f, 6.4f};
//*UE_PI/180.f
// const TArray<double> Alpha = {90.f, 0.f, 90.f, -90.f, 90.f, 0.f};
// const TArray<double> AxisMin = {-230.f, -115.f, -205.f, -230.f, -125.f, -400.f};
// const TArray<double> AxisMax = {230.f, 113.f, 55.f, 230.f, 120.f, 400.f};
//const TArray<double> Theta = {};


UCLASS()
class COMODO_VIRTUAL_LAB_API ARobotArm_DT : public AActor {
	GENERATED_BODY()
	
public:	
	ARobotArm_DT();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "COMODO|Components")
	USceneComponent* Root;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "COMODO|Components")
	UStaticMeshComponent* Base;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "COMODO|Components")
	UStaticMeshComponent* Link1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "COMODO|Components")
	UStaticMeshComponent* Link2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "COMODO|Components")
	UStaticMeshComponent* Link3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "COMODO|Components")
	UStaticMeshComponent* Link4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "COMODO|Components")
	UStaticMeshComponent* Link5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "COMODO|Components")
	UStaticMeshComponent* Link6;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "COMODO|Components")
	UStaticMeshComponent* GhostBase;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "COMODO|Components")
	UStaticMeshComponent* GhostLink1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "COMODO|Components")
	UStaticMeshComponent* GhostLink2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "COMODO|Components")
	UStaticMeshComponent* GhostLink3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "COMODO|Components")
	UStaticMeshComponent* GhostLink4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "COMODO|Components")
	UStaticMeshComponent* GhostLink5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "COMODO|Components")
	UStaticMeshComponent* GhostLink6;
	
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "COMODO|Kinematics")
	// TArray<double> theta = {0.f,90.f,-0.f,0.f,-0.f,0.f};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "COMODO|Kinematics")
	F6AxesRobotParams RobotDhParams;

	UFUNCTION(BlueprintPure, Category = "COMODO|Kinematics")
	TArray<double> GetCurrentThetas() {return RobotDhParams.theta;}
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "COMODO|Robot")
	void MoveJoints(const TArray<double>& JointAngles);
	
private:
	// virtual void MoveJoints_Implementation(const TArray<double>& JointAngles);
};
