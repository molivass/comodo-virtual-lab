#include "Kinematics.h"

FMatrix UKinematics::GetTransformMatrix(double theta, double alfa, double d, double a) {
	double Ct = FMath::Cos(FMath::DegreesToRadians(theta));
	double St = FMath::Sin(FMath::DegreesToRadians(theta));
	double Ca = FMath::Cos(FMath::DegreesToRadians(alfa));
	double Sa = FMath::Sin(FMath::DegreesToRadians(alfa));
	return FMatrix(
		FPlane(Ct, -Ca*St, Sa*St, a*Ct),
		FPlane(St, Ca*Ct, -Sa*Ct, a*St),
		FPlane(0, Sa, Ca, d),
		FPlane(0, 0, 0, 1)
		);
}

FMatrix UKinematics::GetRotationMatrix(const FRotator& Rotation) {
	FVector XAxis, YAxis, ZAxis;
	FRotationMatrix(Rotation).GetScaledAxes(XAxis, YAxis, ZAxis);
	
	return FMatrix(
		FPlane(ZAxis.X, -YAxis.X, -XAxis.X, 0),
		FPlane(-ZAxis.Y, YAxis.Y, XAxis.Y, 0),
		FPlane(ZAxis.Z, -YAxis.Z, -XAxis.Z, 0),
		FPlane(0,0,0,1)
		);
}


FMatrix UKinematics::GetTransformMatrixAtPosition(F6AxesRobotParams RobotParams, TArray<double> JointAngles) {
	FMatrix result = GetTransformMatrix(JointAngles[0], RobotParams.alpha[0], RobotParams.d[0], RobotParams.a[0]);
	for (int i= 1; i < 6; i++) {
		result = result * GetTransformMatrix(JointAngles[i], RobotParams.alpha[i], RobotParams.d[i], RobotParams.a[i]);
	}
	
	return result;
}

// void UKinematics::GetCurrentPosition(FRobotParams RobotParams, double& OutX, double& OutY, double& OutZ) {
// 	FMatrix TransformMatrix = GetTransformMatrixAtPosition(RobotParams, RobotParams.theta);
// 	OutX = TransformMatrix.M[0][3];
// 	OutY = TransformMatrix.M[1][3];
// 	OutZ = TransformMatrix.M[2][3];
// }

FMatrix UKinematics::ChangeLocationInTransformMatrix(FMatrix TransformMatrix, const FVector& Location) {
	FMatrix ReturnMatrix = TransformMatrix;
	ReturnMatrix.M[0][3] = Location.X;
	ReturnMatrix.M[1][3] = Location.Y;
	ReturnMatrix.M[2][3] = Location.Z;
	return ReturnMatrix;
}

FVector UKinematics::GetLocationFromTransformMatrix(const FMatrix& TransformMatrix) {
	return FVector(TransformMatrix.M[0][3], TransformMatrix.M[1][3], TransformMatrix.M[2][3]);
}

bool UKinematics::GetClosestIKAnglesToLocation(F6AxesRobotParams RobotParams, FMatrix PosTransformMatrix,  const TArray<double>& CurrentAxis,
	TArray<double>& OutAxis) {
	TArray<TArray<double>> Solutions;
	
	double Wx = PosTransformMatrix.M[0][3] - RobotParams.d[5] * PosTransformMatrix.M[0][2];
	double Wy = PosTransformMatrix.M[1][3] - RobotParams.d[5] * PosTransformMatrix.M[1][2];
	double Wz = PosTransformMatrix.M[2][3] - RobotParams.d[5] * PosTransformMatrix.M[2][2];
	
	double K1 = 2 * RobotParams.a[1] * RobotParams.a[2];
	double K2 = 2 * RobotParams.a[1] * RobotParams.d[3];
	double K3_Base = FMath::Square(Wx) + FMath::Square(Wy) + FMath::Square(Wz - RobotParams.d[0]) - 
		FMath::Square(RobotParams.d[3]) - FMath::Square(RobotParams.a[2]) - FMath::Square(RobotParams.a[1]);
	
	double Disc = FMath::Square(K1) + FMath::Square(K2) - FMath::Square(K3_Base);
	if (Disc < 0){
		return false;
	}
	
	double Theta1_Options[2] = { atan2(Wy, Wx), atan2(Wy, Wx) + PI };
	double Theta3_Options[2] = {
		atan2(K2, K1) - atan2(sqrt(Disc), K3_Base),
		atan2(K2, K1) + atan2(sqrt(Disc), K3_Base)
	};
	
	for (double Theta1 : Theta1_Options) {
		if (!IsAngleInValidRange(FMath::RadiansToDegrees(Theta1), 
			RobotParams.AxisMin[0], RobotParams.AxisMax[0])) {
			continue;
		}
		
		//VALIDATE
		double Wr = FMath::Abs(cos(Theta1)) > FMath::Abs(sin(Theta1)) ? Wx / cos(Theta1) : Wy / sin(Theta1);
		
		for (double Theta3 : Theta3_Options) {
			if (!IsAngleInValidRange(-FMath::RadiansToDegrees(Theta3),
				RobotParams.AxisMin[2], RobotParams.AxisMax[2])) {
				continue;
			}
			
			double K1_Theta2 = RobotParams.d[3] * sin(Theta3) + RobotParams.a[2] * cos(Theta3) + RobotParams.a[1];
			double K2_Theta2 = RobotParams.a[2] * sin(Theta3) - RobotParams.d[3] * cos(Theta3);
			double Theta2 = CalcTheta2(K1_Theta2, K2_Theta2, Wr, Wz - RobotParams.d[0]);
			
			if (!IsAngleInValidRange(90.f - FMath::RadiansToDegrees(Theta2),
				RobotParams.AxisMin[1], RobotParams.AxisMax[1])) {
				continue;
			}
			
			FMatrix R30 = CalcR30(Theta1, Theta2, Theta3);
			FMatrix R63 = R30.GetTransposed() * PosTransformMatrix;
			
			double Theta4_base = CalcTheta4(R63);
			double Theta4_options[2] = { Theta4_base, Theta4_base + PI };
			for (double Theta4 : Theta4_options) {
				if (!IsAngleInValidRange(FMath::RadiansToDegrees(Theta4), RobotParams.AxisMin[3], RobotParams.AxisMax[3])) {
					continue;
				}
				
				double Theta5 = CalcTheta5(R63, Theta4);
				if (!IsAngleInValidRange(-FMath::RadiansToDegrees(Theta5), RobotParams.AxisMin[4], RobotParams.AxisMax[4])) {
					continue;
				}
				
				double Theta6 = CalcTheta6(R63, Theta4);
				if (!IsAngleInValidRange(FMath::RadiansToDegrees(Theta6), RobotParams.AxisMin[5], RobotParams.AxisMax[5])) {
					continue;
				}
				
				TArray<double> Solution;
				Solution.Empty(6);
				Solution.Add(FMath::RadiansToDegrees(Theta1));
				Solution.Add(90 - FMath::RadiansToDegrees(Theta2));
				Solution.Add(-FMath::RadiansToDegrees(Theta3));
				Solution.Add(FMath::RadiansToDegrees(Theta4));
				Solution.Add(-FMath::RadiansToDegrees(Theta5));
				Solution.Add(FMath::RadiansToDegrees(Theta6));
		// 		UE_LOG(LogTemp, Warning, TEXT("%f %f %f %f %f %f"), 
		// Theta1*180/PI, 90-Theta2*180/PI, -Theta3*180/PI, Theta4*180/PI, -Theta5*180/PI, Theta6*180/PI);	
				Solutions.Add(Solution);
			}
			
		}
	}
	// UE_LOG(LogTemp, Display, TEXT("Valid Solutions: %d"), Solutions.Num());
	if (Solutions.Num() == 0) {
		return false;
	}
	
	TArray<double> BestSolution = SelectOptimalSolution(Solutions, CurrentAxis);
	//BestSolution[1] = 90.f - BestSolution[1];
	//BestSolution[2] = -BestSolution[2];
	//BestSolution[4] = -BestSolution[4];
	
	OutAxis = BestSolution;
	return true;
}


// double UKinematics::CalcTheta31(const double K1, const double K2, const double K3) {
// 	return atan2(K2,K1) - atan2(sqrt(K1*K1 + K2*K2 - K3*K3),K3);
// }

// double UKinematics::CalcTheta32(const double K1, const double K2, const double K3) {
// 	return atan2(K2,K1) + atan2(sqrt(K1*K1 + K2*K2 - K3*K3),K3);
// }

double UKinematics::CalcTheta2(const double K1, const double K2, const double K3, const double K4) {
	return atan2(K1*K4-K2*K3, K1*K3+K2*K4);
}

FMatrix UKinematics::CalcR30(const double Theta1, const double Theta2, const double Theta3) {
	return FMatrix(
		FPlane(cos(Theta1)*cos(Theta2+Theta3), sin(Theta1), cos(Theta1)*sin(Theta2+Theta3),0),
		FPlane(sin(Theta1)*cos(Theta2+Theta3), -cos(Theta1), sin(Theta1)*sin(Theta2+Theta3), 0),
		FPlane(sin(Theta2+Theta3), 0, -cos(Theta2+Theta3), 0),
		FPlane(0,0,0,0)
		);
}

double UKinematics::CalcTheta4(const FMatrix& R) {
	if (FMath::Abs(R.M[0][2]) < 1e-8 && FMath::Abs(R.M[1][2]) < 1e-8) {
		return 0.f;
	}
	return atan(R.M[1][2]/R.M[0][2]);
}

double UKinematics::CalcTheta5(const FMatrix& R, const double Theta4) {
	return atan2(cos(Theta4)*R.M[0][2]+sin(Theta4)*R.M[1][2],R.M[2][2]);
}

double UKinematics::CalcTheta6(const FMatrix& R, const double Theta4) {
	return atan2(-sin(Theta4)*R.M[0][0]+cos(Theta4)*R.M[1][0],
		-sin(Theta4)*R.M[0][1]+cos(Theta4)*R.M[1][1]);
}

bool UKinematics::IsAngleInValidRange(double Angle, double MinAngle, double MaxAngle) {
	return Angle >= MinAngle && Angle <= MaxAngle;
}

TArray<double> UKinematics::SelectOptimalSolution(TArray<TArray<double>> Solutions, TArray<double> CurrentThetas) {
	int BestSolutionIndex = -1;
	
	double MinDistance = DBL_MAX;
	for (int i = 0; i < Solutions.Num(); i++) {
		double SolutionDistance = 0.f;
		for (int j = 0; j < 6; j++) {
			double Delta = FMath::Abs(Solutions[i][j] - CurrentThetas[j]);
			SolutionDistance += Delta;
		}
		
		if (SolutionDistance < MinDistance) {
			MinDistance = SolutionDistance;
			BestSolutionIndex = i;
		}
	}
	
	return Solutions[BestSolutionIndex];
}


