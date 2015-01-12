#include "SQLite3UE4PluginPrivatePCH.h"
#include "SQLiteBlueprintFunctionLibrary.h"
#include "Engine.h"

static USQLiteDatabase* DatabasePtr = NewObject<USQLiteDatabase>();

USQLiteBlueprintFunctionLibrary::USQLiteBlueprintFunctionLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UObject* USQLiteBlueprintFunctionLibrary::NewObjectFromBlueprint(UObject* WorldContextObject, TSubclassOf<UObject> UC)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject);
	UObject* tempObject = StaticConstructObject(UC);

	return tempObject;
}