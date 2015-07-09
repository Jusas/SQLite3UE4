#pragma once
#include "SQLiteDatabaseStructs.generated.h"

USTRUCT(BlueprintType)
struct SQLITE3UE4PLUGIN_API FSQLiteIndex
{
	GENERATED_USTRUCT_BODY()

		/** String with piece if SQL script*/
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SQLite Index")
		FString ResultStr = "";

	/** Index name*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SQLite Index")
		FString IndexName = "";

};

USTRUCT(BlueprintType)
struct SQLITE3UE4PLUGIN_API FSQLitePrimaryKey
{
	GENERATED_USTRUCT_BODY()

		/** String with piece if SQL script*/
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SQLite Primary Key")
		FString ResultStr = "";
};

USTRUCT(BlueprintType)
struct SQLITE3UE4PLUGIN_API FSQLiteTableField
{
	GENERATED_USTRUCT_BODY()

		/** String with piece if SQL script*/
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SQLite Table Field")
		FString ResultStr = "";

	/** Field name*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SQLite Table Field")
		FString FieldName = "";

	/** Field type*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SQLite Table Field")
		FString FieldType = "";

	/** Field value*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SQLite Table Field")
		FString FieldValue = "";

}; 

USTRUCT(BlueprintType)
struct SQLITE3UE4PLUGIN_API FSQLiteTableRowSimulator
{
	GENERATED_USTRUCT_BODY()

		/** Index name*/
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SQLite Index")
		TArray<FSQLiteTableField> rowsOfFields;

};

USTRUCT(BlueprintType)
struct SQLITE3UE4PLUGIN_API FSQLiteTable
{
	GENERATED_USTRUCT_BODY()

		/** Database name*/
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SQLite Table")
		FString DatabaseName = "";

	/** Table name*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SQLite Table")
		FString TableName = "";

	/** Array with Fields*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SQLite Table")
		TArray<FSQLiteTableField> Fields;

	/** Primary Key */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SQLite Table")
		FSQLitePrimaryKey PK;

	/** Created Key */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SQLite Table")
		bool Created = false;

};
