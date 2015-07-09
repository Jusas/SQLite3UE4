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

	//UFUNCTION(BlueprintCallable, Category = "SQLite|Value Conversion")
	//static int64 CastToInt64(FString SQLiteResultValue);

	UFUNCTION(BlueprintCallable, Category = "SQLite|Value Conversion")
		static bool CastToBoolean(FString SQLiteResultValue);

	UFUNCTION(BlueprintCallable, Category = "SQLite|Value Conversion")
		static float CastToFloat(FString SQLiteResultValue);

	//UFUNCTION(BlueprintCallable, Category = "SQLite|Value Conversion")
	//static double CastToDouble(FString SQLiteResultValue);

	/**
	* Blueprint nodes for building queries.
	*/

	/** Start a new query block. */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Query Start", CompactNodeTitle = "("), Category = "SQLite|Query")
		static FSQLiteQueryTermExpectedNode QueryStart(FSQLiteQueryTermExpectedNode LogicOperationOrNone);

	/** End a query block. NOTE: To link this query to a Get Data node's query pin, link a Finalize Query node to the end of the query chain and link that to the pin. */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Query End", CompactNodeTitle = ")"), Category = "SQLite|Query")
		static FSQLiteQueryLogicExpectedNode QueryEnd(const FSQLiteQueryLogicExpectedNode& LastQueryTerm);

	/** A query term. Basically creating a "Key <operator> Value" comparison. */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Query Term"), Category = "SQLite|Query")
		static FSQLiteQueryLogicExpectedNode QueryTerm(const FSQLiteQueryTermExpectedNode& LogicOperation, FString Field, FString Operator, FString Value);

	/** AND logic node, combining two terms. */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "AND", CompactNodeTitle = "AND"), Category = "SQLite|Query|Logic")
		static FSQLiteQueryTermExpectedNode QueryLogicAnd(const FSQLiteQueryLogicExpectedNode& LHS);

	/** OR logic node, combining two terms. */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "OR", CompactNodeTitle = "OR"), Category = "SQLite|Query|Logic")
		static FSQLiteQueryTermExpectedNode QueryLogicOr(const FSQLiteQueryLogicExpectedNode& LHS);

	/** Finalizes the query. This node's output can be linked to a Get Data node's query source pin. */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Finalize Query"), Category = "SQLite|Query")
		static FSQLiteQueryFinalizedQuery QueryFinal(const FSQLiteQueryLogicExpectedNode& QueryEndNode);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "INTEGER (SQLite)"), Category = "SQLite|Query|DataTypes")
		static FSQLiteTableField SQLiteINTEGER(FString FieldName, const bool PK, const bool AI, const bool Unique);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "TEXT (SQLite)"), Category = "SQLite|Query|DataTypes")
		static FSQLiteTableField SQLiteTEXT(const FString FieldName, const bool PK, const bool Unique);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "REAL (SQLite)"), Category = "SQLite|Query|DataTypes")
		static FSQLiteTableField SQLiteREAL(const FString FieldName, const bool PK, const bool Unique);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "NUMERIC (SQLite)"), Category = "SQLite|Query|DataTypes")
		static FSQLiteTableField SQLiteNUMERIC(const FString FieldName, const bool PK, const bool Unique);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Primary Key (SQLite)"), Category = "SQLite|Query|DataTypes")
		static FSQLitePrimaryKey SQLitePrimaryKey(const TArray<FSQLiteTableField> Fields);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Index (SQLite)"), Category = "SQLite|Query|DataTypes")
		static FSQLiteIndex SQLiteIndexFunction(const TArray<FSQLiteTableField> Fields, FString idxName, bool Unique);



};
