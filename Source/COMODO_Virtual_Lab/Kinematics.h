#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kinematics.generated.h"

USTRUCT(BlueprintType)
struct F6AxesRobotParams {
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="COMODO|Kinematics")
	TArray<double> theta;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="COMODO|Kinematics")
	TArray<double> alpha;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="COMODO|Kinematics")
	TArray<double> d;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="COMODO|Kinematics")
	TArray<double> a;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="COMODO|Kinematics")
	TArray<double> AxisMin;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="COMODO|Kinematics")
	TArray<double> AxisMax;
	
	F6AxesRobotParams() {
		theta = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f};
		alpha = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f};
		d = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f};
		a = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f};
		AxisMin = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f};
		AxisMax = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f};
	}
	
	F6AxesRobotParams( const TArray<double>& InTheta, const TArray<double>& InAlpha, const TArray<double>& InD,
		const TArray<double>& InA, const TArray<double>& InMin, const TArray<double>& InMax){
		theta = InTheta;
		theta.SetNumZeroed(6);
		alpha = InAlpha;
		alpha.SetNumZeroed(6);
		d = InD;
		d.SetNumZeroed(6);
		a = InA;
		a.SetNumZeroed(6);
		AxisMin = InMin;
		AxisMax.SetNumZeroed(6);
		AxisMax = InMax;
		AxisMax.SetNumZeroed(6);
	}
};

UCLASS()
class COMODO_VIRTUAL_LAB_API UKinematics : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "COMODO|Kinematics")
	static FMatrix GetTransformMatrix(double theta, double alfa, double d, double a);
	
	UFUNCTION(BlueprintCallable, Category = "COMODO|Kinematics")
	static FMatrix GetTransformMatrixAtPosition(F6AxesRobotParams RobotParams, TArray<double> JointAngles);
	
	// UFUNCTION(BlueprintCallable, Category = "COMODO|Kinematics")
	// static void GetCurrentPosition(FRobotParams RobotParams, double& OutX , double& OutY, double& OutZ)
	
	UFUNCTION(BlueprintCallable, Category = "COMODO|Kinematics")
	static FMatrix GetRotationMatrix(const FRotator& Rotation);
	
	UFUNCTION(BlueprintCallable, Category = "COMODO|Kinematics")
	static FMatrix ChangeLocationInTransformMatrix(FMatrix TransformMatrix,const FVector& Location);
	
	UFUNCTION(BlueprintCallable, Category = "COMODO|Kinematics")
	static FVector GetLocationFromTransformMatrix(const FMatrix& TransformMatrix);
	
	UFUNCTION(BlueprintCallable, Category = "COMODO|Kinematics")
	static bool GetClosestIKAnglesToLocation(F6AxesRobotParams RobotParams, FMatrix TargetPosTransformMatrix, const TArray<double>& CurrentAxis,
		TArray<double>& OutAxis);
	
private:
	// static double CalcTheta31(double K1, double K2, double K3);
	// static double CalcTheta32(double K1, double K2, double K3);
	static double CalcTheta2(double K1, double K2, double K3, double K4);
	static FMatrix CalcR30(double Theta1, double Theta2, double Theta3);
	static double CalcTheta4(const FMatrix& R);
	static double CalcTheta5(const FMatrix& R, double Theta4);
	static double CalcTheta6(const FMatrix& R, double Theta4);
	static bool IsAngleInValidRange(double Angle, double MinAngle, double MaxAngle);
	static TArray<double> SelectOptimalSolution(TArray<TArray<double>> Solutions, TArray<double> CurrentThetas);
	
};
