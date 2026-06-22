#include "Kinematics.h"

/**
 * Calcula la matriz de transformación homogénea según los parámetros DH del robot
 * 
 * @param theta Ángulo de la articulación (en grados)
 * @param alpha Ángulo de torsión del eslabón (en grados)
 * @param d Desplazamiento del eslabón a lo largo del eje Z
 * @param a Longitud del eslabón a lo largo del eje X
 * @return Matriz de transformación 4x4 que representa la transformación DH
 */
FMatrix UKinematics::GetTransformMatrix(double theta, double alpha, double d, double a) {
	double St, Sa;
	double Ct, Ca;
	FMath::SinCos(&St, &Ct, FMath::DegreesToRadians(theta));
	FMath::SinCos(&Sa, &Ca, FMath::DegreesToRadians(alpha));
	return FMatrix(
		FPlane(Ct, -Ca*St, Sa*St, a*Ct),
		FPlane(St, Ca*Ct, -Sa*Ct, a*St),
		FPlane(0, Sa, Ca, d),
		FPlane(0, 0, 0, 1)
		);
}

/**
 * Convierte un rotator de Unreal a una matriz de rotación en coordenadas del robot
 * 
 * @param Rotation Rotator de Unreal Engine a convertir
 * @return Matriz de rotación 4x4 en el sistema de coordenadas del robot
 */
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

/**
 * Calcula la matriz de transformación final para un robot de 6 ejes
 * 
 * @param RobotParams Parámetros DH del robot
 * @param JointAngles Array con los 6 ángulos de las articulaciones
 * @return Matriz de transformación 4x4 desde la base hasta el TCP
 */
FMatrix UKinematics::GetTransformMatrixAtPosition(F6AxesRobotParams RobotParams, TArray<double> JointAngles) {
	FMatrix Result = GetTransformMatrix(JointAngles[0], RobotParams.alpha[0], RobotParams.d[0], RobotParams.a[0]);
	for (int i= 1; i < 6; i++) {
		Result = Result * GetTransformMatrix(JointAngles[i], RobotParams.alpha[i], RobotParams.d[i], RobotParams.a[i]);
	}
	
	return Result;
}

/**
 * Modifica la posición de una matriz de transformación
 * 
 * @param TransformMatrix Matriz de transformación original
 * @param Location Nueva posición (X, Y, Z) a establecer
 * @return Nueva matriz de transformación con la ubicación actualizada
 */
FMatrix UKinematics::ChangeLocationInTransformMatrix(FMatrix TransformMatrix, const FVector& Location) {
	FMatrix ReturnMatrix = TransformMatrix;
	ReturnMatrix.M[0][3] = Location.X;
	ReturnMatrix.M[1][3] = Location.Y;
	ReturnMatrix.M[2][3] = Location.Z;
	return ReturnMatrix;
}

/**
 * Extrae la posición de una matriz de transformación
 * 
 * @param TransformMatrix Matriz de transformación 4x4
 * @return Vector con las coordenadas X, Y, Z de la posición
 */
FVector UKinematics::GetLocationFromTransformMatrix(const FMatrix& TransformMatrix) {
	return FVector(TransformMatrix.M[0][3], TransformMatrix.M[1][3], TransformMatrix.M[2][3]);
}


/**
 * Calcula la cinemática inversa para un robot de 6 ejes, encontrando los ángulos
 * más cercanos a la posición actual del robot
 * 
 * @param RobotParams Parámetros DH del robot
 * @param TargetPosTransformMatrix Matriz de transformación objetivo del TCP
 * @param CurrentAxes Ángulos actuales de las articulaciones, para seleccionar la solución más cercana
 * @param OutAxes Resultado de los cálculos. Retorna un array de 6 valores con los ángulos en grados.
 * @return True si se encontró una solución válida, False en caso contrario
 */
bool UKinematics::GetClosestIKAnglesToLocation(F6AxesRobotParams RobotParams, FMatrix TargetPosTransformMatrix,  const TArray<double>& CurrentAxes,
	TArray<double>& OutAxes) {
	TArray<TArray<double>> Solutions;

	// Cálculo del WCP. Posición de la muñeca
	const double Wx = TargetPosTransformMatrix.M[0][3] - RobotParams.d[5] * TargetPosTransformMatrix.M[0][2];
	const double Wy = TargetPosTransformMatrix.M[1][3] - RobotParams.d[5] * TargetPosTransformMatrix.M[1][2];
	const double Wz = TargetPosTransformMatrix.M[2][3] - RobotParams.d[5] * TargetPosTransformMatrix.M[2][2];

	// Parámetros para la solución de cinemática inversa
	const double K1 = 2 * RobotParams.a[1] * RobotParams.a[2];
	const double K2 = 2 * RobotParams.a[1] * RobotParams.d[3];
	const double K3_Base = FMath::Square(Wx) + FMath::Square(Wy) + FMath::Square(Wz - RobotParams.d[0]) - 
		FMath::Square(RobotParams.d[3]) - FMath::Square(RobotParams.a[2]) - FMath::Square(RobotParams.a[1]);

	// Verificación de discriminante
	const double Disc = FMath::Square(K1) + FMath::Square(K2) - FMath::Square(K3_Base);
	if (Disc < 0){
		return false;
	}

	// Opciones para Theta1 y 3 (dos posibles soluciones cada uno, por la arcotangente)
	double Theta1_Options[2] = { atan2(Wy, Wx), atan2(Wy, Wx) + PI };
	double Theta3_Options[2] = {
		atan2(K2, K1) - atan2(sqrt(Disc), K3_Base),
		atan2(K2, K1) + atan2(sqrt(Disc), K3_Base)
	};

	// Iteración sobre todas las combinaciones de soluciones
	for (double Theta1 : Theta1_Options) {
		if (!IsAngleInValidRange(FMath::RadiansToDegrees(Theta1), 
			RobotParams.AxisMin[0], RobotParams.AxisMax[0])) {
			continue; //Si el eje se sale de los rangos mecánicos, deja de calcular esa opción y pasa a la siguiente.
		}

		// Cálculo de la distancia radial desde el eje Z
		const double Wr = FMath::Abs(cos(Theta1)) > FMath::Abs(sin(Theta1)) ? Wx / cos(Theta1) : Wy / sin(Theta1);
		
		for (double Theta3 : Theta3_Options) {
			if (!IsAngleInValidRange(-FMath::RadiansToDegrees(Theta3),
				RobotParams.AxisMin[2], RobotParams.AxisMax[2])) {
				continue; //Misma situación que en Theta1, Theta2 fuera de rango
			}

			// Cálculo de Theta2
			const double K1_Theta2 = RobotParams.d[3] * sin(Theta3) + RobotParams.a[2] * cos(Theta3) + RobotParams.a[1];
			const double K2_Theta2 = RobotParams.a[2] * sin(Theta3) - RobotParams.d[3] * cos(Theta3);
			double Theta2 = CalcTheta2(K1_Theta2, K2_Theta2, Wr, Wz - RobotParams.d[0]);
			
			if (!IsAngleInValidRange(90.f - FMath::RadiansToDegrees(Theta2),
				RobotParams.AxisMin[1], RobotParams.AxisMax[1])) {
				continue; // Theta2 fuera de rango, pasa a la siguiente iteración
			}

			// Cálculo de la matriz de rotación R30 y R63
			FMatrix R30 = CalcR30(Theta1, Theta2, Theta3);
			FMatrix R63 = R30.GetTransposed() * TargetPosTransformMatrix;

			// Cálculo de Theta4, Theta5, Theta6, articulaciones de la muñeca
			const double Theta4_Base = CalcTheta4(R63);
			double Theta4_Options[2] = { Theta4_Base, Theta4_Base + PI };
			for (double Theta4 : Theta4_Options) {
				if (!IsAngleInValidRange(FMath::RadiansToDegrees(Theta4), RobotParams.AxisMin[3], RobotParams.AxisMax[3])) {
					continue; // Theta4 fuera de rango
				}
				
				double Theta5 = CalcTheta5(R63, Theta4);
				if (!IsAngleInValidRange(-FMath::RadiansToDegrees(Theta5), RobotParams.AxisMin[4], RobotParams.AxisMax[4])) {
					continue; // Theta5 fuera de rango
				}
				
				double Theta6 = CalcTheta6(R63, Theta4);
				if (!IsAngleInValidRange(FMath::RadiansToDegrees(Theta6), RobotParams.AxisMin[5], RobotParams.AxisMax[5])) {
					continue; // Theta6 fuera de rango
				}

				// Almacenamiento de la solución
				TArray<double> Solution;
				Solution.Empty(6);
				Solution.Add(FMath::RadiansToDegrees(Theta1));
				Solution.Add(FMath::RadiansToDegrees(Theta2));
				Solution.Add(FMath::RadiansToDegrees(Theta3));
				Solution.Add(FMath::RadiansToDegrees(Theta4));
				Solution.Add(FMath::RadiansToDegrees(Theta5));
				Solution.Add(FMath::RadiansToDegrees(Theta6));
				//UE_LOG(LogTemp, Warning, TEXT("%f %f %f %f %f %f"), 
				//Theta1*180/PI, 90-Theta2*180/PI, -Theta3*180/PI, Theta4*180/PI, -Theta5*180/PI, Theta6*180/PI);	
				Solutions.Add(Solution);
			}
			
		}
	}
	
	if (Solutions.Num() == 0) {
		return false; // No se encontraron soluciones
	}

	// Selección de la solución más cercana a la configuración actual
	TArray<double> BestSolution = SelectOptimalSolution(Solutions, CurrentAxes);
	
	// Ajuste de ángulos para el sistema de coordenadas del robot
	BestSolution[1] = 90.f - BestSolution[1];
	BestSolution[2] = -BestSolution[2];
	BestSolution[4] = -BestSolution[4];
	
	OutAxes = BestSolution;
	return true;
}

/**
 * Calcula Theta2 usando la fórmula de atan2 con dos parámetros
 * 
 * @param K1 Parámetro K1 (constante)
 * @param K2 Parámetro K2 (constante)
 * @param K3 Parámetro K3 (distancia radial)
 * @param K4 Parámetro K4 (distancia vertical)
 * @return Ángulo Theta2 en radianes
 */
double UKinematics::CalcTheta2(const double K1, const double K2, const double K3, const double K4) {
	return atan2(K1*K4-K2*K3, K1*K3+K2*K4);
}

/**
 * Calcula la matriz de rotación R30 (desde la base hasta la articulación 3)
 * 
 * @param Theta1 Ángulo de la articulación 1 en radianes
 * @param Theta2 Ángulo de la articulación 2 en radianes
 * @param Theta3 Ángulo de la articulación 3 en radianes
 * @return Matriz de rotación 4x4 de la base a la articulación 3
 */
FMatrix UKinematics::CalcR30(const double Theta1, const double Theta2, const double Theta3) {
	return FMatrix(
		FPlane(cos(Theta1)*cos(Theta2+Theta3), sin(Theta1), cos(Theta1)*sin(Theta2+Theta3),0),
		FPlane(sin(Theta1)*cos(Theta2+Theta3), -cos(Theta1), sin(Theta1)*sin(Theta2+Theta3), 0),
		FPlane(sin(Theta2+Theta3), 0, -cos(Theta2+Theta3), 0),
		FPlane(0,0,0,0)
		);
}

/**
 * Calcula el ángulo Theta4 a partir de la matriz de rotación
 * 
 * @param R Matriz de rotación R63
 * @return Ángulo Theta4 en radianes (o 0 si hay singularidad)
 */
double UKinematics::CalcTheta4(const FMatrix& R) {
	if (FMath::Abs(R.M[0][2]) < 1e-8 && FMath::Abs(R.M[1][2]) < 1e-8) {
		return 0.f;
	}
	return atan(R.M[1][2]/R.M[0][2]);
}

/**
 * Calcula el ángulo Theta5 a partir de la matriz de rotación y Theta4
 * 
 * @param R Matriz de rotación R63
 * @param Theta4 Ángulo de la articulación 4 en radianes
 * @return Ángulo Theta5 en radianes
 */
double UKinematics::CalcTheta5(const FMatrix& R, const double Theta4) {
	return atan2(cos(Theta4)*R.M[0][2]+sin(Theta4)*R.M[1][2],R.M[2][2]);
}

/**
 * Calcula el ángulo Theta6 a partir de la matriz de rotación y Theta4
 * 
 * @param R Matriz de rotación R63
 * @param Theta4 Ángulo de la articulación 4 en radianes
 * @return Ángulo Theta6 en radianes
 */
double UKinematics::CalcTheta6(const FMatrix& R, const double Theta4) {
	return atan2(-sin(Theta4)*R.M[0][0]+cos(Theta4)*R.M[1][0],
		-sin(Theta4)*R.M[0][1]+cos(Theta4)*R.M[1][1]);
}

/**
 * Verifica si un ángulo está dentro del rango válido de una articulación
 * 
 * @param Angle Ángulo a verificar en grados
 * @param MinAngle Límite mínimo del ángulo en grados
 * @param MaxAngle Límite máximo del ángulo en grados
 * @return True si el ángulo está dentro del rango, False en caso contrario
 */
bool UKinematics::IsAngleInValidRange(double Angle, double MinAngle, double MaxAngle) {
	return Angle >= MinAngle && Angle <= MaxAngle;
}

/**
 * Selecciona la solución óptima entre las soluciones de cinemática inversa
 * Se basa en la distancia mínima de las articulaciones a la configuración actual
 * 
 * @param Solutions Array de soluciones (cada una con 6 ángulos en grados)
 * @param CurrentThetas Ángulos actuales de las articulaciones (en grados)
 * @return La solución con la distancia mínima a la configuración actual
 */
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


