#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BlueprintsRegex.generated.h"


UCLASS()
class COMODO_VIRTUAL_LAB_API UBlueprintsRegex : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="COMODO|RegEx")
	static bool IsAValidIPAddress(const FText& Address);

	UFUNCTION(BlueprintCallable, Category="COMODO|RegEx")
	static bool IsAValidPortNumber(const FText& PortNumber);
};
