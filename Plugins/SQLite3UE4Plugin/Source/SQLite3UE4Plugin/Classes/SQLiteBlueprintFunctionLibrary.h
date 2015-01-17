
#pragma once
#include "SQLiteDatabase.h"
#include "SQLiteBlueprintNodes.h"
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

		/** 
		* Simple casts for blueprint use.
		*/
		UFUNCTION(BlueprintCallable, Category = "SQLite|Value Conversion")
		static int32 CastToInt(FString SQLiteResultValue);

		UFUNCTION(BlueprintCallable, Category = "SQLite|Value Conversion")
		static int64 CastToInt64(FString SQLiteResultValue);

		UFUNCTION(BlueprintCallable, Category = "SQLite|Value Conversion")
		static bool CastToBoolean(FString SQLiteResultValue);

		UFUNCTION(BlueprintCallable, Category = "SQLite|Value Conversion")
		static float CastToFloat(FString SQLiteResultValue);

		UFUNCTION(BlueprintCallable, Category = "SQLite|Value Conversion")
		static double CastToDouble(FString SQLiteResultValue);

		/** 
		*
		*/

		
		//UFUNCTION(BlueprintPure, meta = (DefaultToSelf = "LHS", FriendlyName = "Query Start", CompactNodeTitle = "("), Category = "SQLite|Query")
		//static FSQLiteQueryTermExpectedNode QueryStart(const FSQLiteQueryTermExpectedNode& LogicOperationOrNone);


		UFUNCTION(BlueprintPure, meta = (FriendlyName = "Query Start", CompactNodeTitle = "("), Category = "SQLite|Query")
		static FSQLiteQueryTermExpectedNode QueryStart(FSQLiteQueryTermExpectedNode LogicOperationOrNone);

		UFUNCTION(BlueprintPure, meta = (FriendlyName = "Query End", CompactNodeTitle = ")"), Category = "SQLite|Query")
		static FSQLiteQueryLogicExpectedNode QueryEnd(const FSQLiteQueryLogicExpectedNode& LastQueryTerm);

		UFUNCTION(BlueprintPure, meta = (FriendlyName = "Query Term"), Category = "SQLite|Query")
		static FSQLiteQueryLogicExpectedNode QueryTerm(const FSQLiteQueryTermExpectedNode& LogicOperation, FString Field, FString Operator, FString Value);

		// Jaa And, Or, jne. nodeiksi
		UFUNCTION(BlueprintPure, meta = (FriendlyName = "AND", CompactNodeTitle = "AND"), Category = "SQLite|Query|Logic")
		static FSQLiteQueryTermExpectedNode QueryLogicAnd(const FSQLiteQueryLogicExpectedNode& LHS);

		UFUNCTION(BlueprintPure, meta = (FriendlyName = "OR", CompactNodeTitle = "OR"), Category = "SQLite|Query|Logic")
		static FSQLiteQueryTermExpectedNode QueryLogicOr(const FSQLiteQueryLogicExpectedNode& LHS);

		UFUNCTION(BlueprintPure, meta = (FriendlyName = "Finalize Query"), Category = "SQLite|Query")
		static FSQLiteQueryFinalizedQuery QueryFinal(const FSQLiteQueryLogicExpectedNode& QueryEndNode);


};
