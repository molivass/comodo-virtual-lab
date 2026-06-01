// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueprintsRegex.h"

bool UBlueprintsRegex::IsAValidIPAddress(const FText& Address) {
	FRegexPattern Pattern(TEXT("^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"));
	FRegexMatcher Matcher(Pattern, Address.ToString());

	return Matcher.FindNext();
}

bool UBlueprintsRegex::IsAValidPortNumber(const FText& PortNumber) {
	FRegexPattern Pattern(TEXT("^((6553[0-5])|(655[0-2][0-9])|(65[0-4][0-9]{2})|(6[0-4][0-9]{3})|([1-5][0-9]{4})|([0-5]{0,5})|([0-9]{1,4}))$"));
	FRegexMatcher Matcher(Pattern, PortNumber.ToString());

	return Matcher.FindNext();
}


