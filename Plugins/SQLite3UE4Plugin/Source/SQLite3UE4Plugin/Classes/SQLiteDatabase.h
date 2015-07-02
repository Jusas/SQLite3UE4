#pragma once
#include "sqlite3.h"
#include "SQLiteBlueprintNodes.h"
#include "SQLiteDatabase.generated.h"

USTRUCT(BlueprintType)
struct SQLITE3UE4PLUGIN_API FSQLiteDatabaseReference
{
	GENERATED_USTRUCT_BODY()

		/** The database name (not the filename) */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SQLite Database Reference")
		FString DatabaseName;

	/** The database tables we want to get data from */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SQLite Database Reference")
		TArray<FString> Tables;
};

USTRUCT(BlueprintType)
struct SQLITE3UE4PLUGIN_API FSQLiteKeyValuePair
{
	GENERATED_USTRUCT_BODY()

		/** The database table field name */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SQLite Key Value Pair")
		FString Key;

	/** The value of the field */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SQLite Key Value Pair")
		FString Value;
};

USTRUCT(BlueprintType)
struct SQLITE3UE4PLUGIN_API FSQLiteQueryResultRow
{
	GENERATED_USTRUCT_BODY()

		/** A list of field name, field value pairs */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SQLite Query Result")
		TArray<FSQLiteKeyValuePair> Fields;
};

USTRUCT(BlueprintType)
struct SQLITE3UE4PLUGIN_API FSQLiteQueryResult
{
	GENERATED_USTRUCT_BODY()

		/** The resulting rows from the query */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SQLite Query Result")
		TArray<FSQLiteQueryResultRow> ResultRows;

	/** Was the query successful or not */
	UPROPERTY(BlueprintReadOnly, Category = "SQLite Query Result")
		bool Success;

	/** If the query was unsuccessful a human readable error message will be populated here */
	UPROPERTY(BlueprintReadOnly, Category = "SQLite Query Result")
		FString ErrorMessage;

};



// A few things for internal use here.
namespace SQLiteResultValueTypes
{
	enum SQLiteResultValType
	{
		Integer,
		Float,
		Text,
		UnsupportedValueType
	};
}

// Result field, used as an intermediary when collecting results from
// an SQLITE3 query.
struct SQLiteResultField
{
	FString StringValue;
	double DoubleValue;
	int64 IntValue;

	FString Name;
	SQLiteResultValueTypes::SQLiteResultValType Type;

	FString ToString()
	{
		if (Type == SQLiteResultValueTypes::Text)
			return StringValue;
		else if (Type == SQLiteResultValueTypes::Integer)
			return FString::Printf(TEXT("%i"), IntValue);
		else if (Type == SQLiteResultValueTypes::Float)
			return FString::Printf(TEXT("%f"), DoubleValue);

		return StringValue;
	}
};

// Represents a single row in the result.
struct SQLiteResultValue
{
	TArray<SQLiteResultField> Fields;
};

// The internal result object.
struct SQLiteQueryResult
{
	bool Success;
	FString ErrorMessage;
	TArray<SQLiteResultValue> Results;
};



/**
* SQLite main database class.
*/
UCLASS()
class SQLITE3UE4PLUGIN_API USQLiteDatabase : public UObject
{
	GENERATED_UCLASS_BODY()

public:

	/** Add a database to the list of databases. It will be checked that it's valid (will try to open it) */
	UFUNCTION(BlueprintCallable, Category = "SQLite")
		static bool RegisterDatabase(FString Name, FString Filename, bool RelativeToGameContentDirectory);

	/** Checks if the database is registered, ie. that it can be found in Databases. */
	UFUNCTION(BlueprintCallable, Category = "SQLite")
		static bool IsDatabaseRegistered(FString DatabaseName);

	/** Get data from the database using a select statement straight into an UObject, ie. populates its properties. */
	UFUNCTION(BlueprintCallable, Category = "SQLite", meta = (DisplayName = "Get Data Into Object (manual query)"))
		static bool GetDataIntoObject(const FString& DatabaseName, const FString& Query, UObject* ObjectToPopulate);

	/** Blueprint: Gets data from the database using a select statement straight into an UObject, ie. populates its properties.
	*   Note: Does not create a new object. ObjectToPopulate is the reference to the object you want to populate. */
	UFUNCTION(BlueprintCallable, Category = "SQLite", meta = (DisplayName = "Get Data Into Object"))
		static bool GetDataIntoObjectBP(const FSQLiteDatabaseReference& DataSource, TArray<FString> Fields, FSQLiteQueryFinalizedQuery Query, UObject* ObjectToPopulate);

	/** Get data from the database using a select statement and return the rows. */
	UFUNCTION(BlueprintCallable, Category = "SQLite", meta = (DisplayName = "Get Data From Table(s) (manual query)"))
		static FSQLiteQueryResult GetData(const FString& DatabaseName, const FString& Query);

	/** Blueprint: Get data from the database. Returns the resulting rows. */
	UFUNCTION(BlueprintCallable, Category = "SQLite", meta = (DisplayName = "Get Data From Table(s)"))
		static FSQLiteQueryResult GetDataBP(const FSQLiteDatabaseReference& DataSource, TArray<FString> Fields, FSQLiteQueryFinalizedQuery Query, int32 MaxResults = -1, int32 ResultOffset = 0);

	/** Create table in the database. */
	UFUNCTION(BlueprintCallable, Category = "SQLite|Query", meta = (DisplayName = "Create Table"))
		static bool CreateTable(const FString DatabaseName, const FString TableName,
		const TArray<FString> Fields, const FString PK, FString &TableNameOutput);

	/** Create indexes for table */
	UFUNCTION(BlueprintCallable, Category = "SQLite|Query", meta = (DisplayName = "Create Indexes"))
		static bool CreateIndexes(const FString DatabaseName, const FString TableName, const TArray<FString> Indexes);

	/** Create index for table */
	UFUNCTION(BlueprintCallable, Category = "SQLite|Query", meta = (DisplayName = "Create Index"))
		static bool CreateIndex(const FString DatabaseName, const FString TableName, const FString Index);

	/** Drop index*/
	UFUNCTION(BlueprintCallable, Category = "SQLite|Query", meta = (DisplayName = "Drop Index"))
		static bool DropIndex(const FString DatabaseName, const FString IndexName);

	/** Drop Table*/
	UFUNCTION(BlueprintCallable, Category = "SQLite|Query", meta = (DisplayName = "Drop Table"))
		static bool DropTable(const FString DatabaseName, const FString TableName);

	/** Truncate Table*/
	UFUNCTION(BlueprintCallable, Category = "SQLite|Query", meta = (DisplayName = "Truncate Table"))
		static bool TruncateTable(const FString DatabaseName, const FString TableName);

	/** Is table exists?*/
	UFUNCTION(BlueprintCallable, Category = "SQLite|Query", meta = (DisplayName = "Is table exists?"))
		static bool IsTableExists(const FString DatabaseName, const FString TableName);

private:
	/** Checks database validity (if the file exists and/or if it can be opened). */
	static bool IsValidDatabase(FString DatabaseFilename, bool TestByOpening);
	/** Tries to open a database. */
	static bool CanOpenDatabase(FString DatabaseFilename);
	/** Collects all properties from an UObject and maps them by the property name. */
	static TMap<FString, UProperty*> CollectProperties(UObject* SourceObject);
	/** Constructs an SQL query from the blueprint fed data. */
	static FString ConstructQuery(TArray<FString> Tables, TArray<FString> Fields, FSQLiteQueryFinalizedQuery QueryObject, int32 MaxResults = -1, int32 ResultOffset = 0);
	/** Runs a query and returns fetched rows. */
	static SQLiteQueryResult RunQueryAndGetResults(FString DatabaseName, FString Query);
	/** Assigns a result row's fields' values to an UObject, ie. assigns them to the properties that have the same name. */
	static void AssignResultsToObjectProperties(const SQLiteResultValue& ResultValue, UObject* ObjectToPopulate);
	static void PrepareStatement(const FString* DatabaseName, const FString* Query, sqlite3** Db, int32** SqlReturnCode,
		sqlite3_stmt** PreparedStatement);
	static bool ExecSql(const FString DatabaseName, const FString Query);

private:
	/** A list of the databases for convenience, easier to refer to them by name rather than a long filename. */
	static TMap<FString, FString> Databases;

};
