#pragma once

#include "SQLiteDatabase.generated.h"

USTRUCT()
struct FSQLiteFile
{
	GENERATED_USTRUCT_BODY()

	FString Name;
	FString Filename;
};

namespace SQLiteFillableTypes
{
	enum EFillableTypes
	{
		Boolean,
		Integer,
		String,
		Float,
		Byte,

	};
}

/**
* Example of declaring a UObject in a plugin module
*/
UCLASS()
class USQLiteDatabase : public UObject
{
	GENERATED_UCLASS_BODY()

	public:

		/** Add a database to the list of databases. It will be checked that it's valid (will try to open it)*/
		UFUNCTION(BlueprintCallable, Category = "SQLite")
		static bool RegisterDatabase(FString Name, FString Filename, bool RelativeToGameContentDirectory);

		// https://www.unrealengine.com/blog/unreal-property-system-reflection
		/** Gets data from the database using a select statement */
		UFUNCTION(BlueprintCallable, Category = "SQLite")
		static bool GetDataToObject(FString DatabaseName, FString Table, TArray<FString> Fields, FString Criteria, UObject* FilledData);



	private:
		static bool IsValidDatabase(FString DatabaseFilename, bool TestByOpening);
		static bool IsDatabaseRegistered(FString DatabaseName);
		static TMap<FString, UProperty*> CollectProperties(UObject* SourceObject);

	private:
		static TMap<FString, FString> Databases;

};
