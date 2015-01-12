
#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SQLiteBlueprintFunctionLibrary.generated.h"

/**
*	Blueprint function library, convenience stuff.
*/
UCLASS()
class USQLiteBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	public:

		UFUNCTION(BlueprintPure, meta = (DefaultToSelf = "WorldContextObject", HidePin = "WorldContextObject"), Category = "SQLite")
		static UObject* NewObjectFromBlueprint(UObject* WorldContextObject, TSubclassOf<UObject> UC);

};
