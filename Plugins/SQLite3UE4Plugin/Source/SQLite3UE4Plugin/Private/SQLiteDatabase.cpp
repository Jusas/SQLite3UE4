#include "SQLite3UE4PluginPrivatePCH.h"
#include "../Classes/SQLiteDatabase.h"

#define LOGSQLITE(verbosity, text) UE_LOG(LogDatabase, verbosity, TEXT("SQLite: %s"), text)

TMap<FString, FString> USQLiteDatabase::Databases;

//--------------------------------------------------------------------------------------------------------------

USQLiteDatabase::USQLiteDatabase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

//--------------------------------------------------------------------------------------------------------------

bool USQLiteDatabase::RegisterDatabase(FString Name, FString Filename, bool RelativeToGameContentDirectory)
{
	FString actualFilename = Filename;

	//UE_LOG(LogDatabase, Warning, TEXT("SQLLite Plugin: RegisterDatabase() !!!"));

	if (RelativeToGameContentDirectory)
	{
		actualFilename = FPaths::GameDir() + Filename;
		//FString message = "!!!!!actualFilename '" + actualFilename ;
		//LOGSQLITE(Warning, *message);
	}
	
	if (IsDatabaseRegistered(Name))
	{
		FString message = "Database '" + actualFilename + "' is already registered, skipping.";
		LOGSQLITE(Warning, *message);
		return true;
	}

	if (!IsValidDatabase(actualFilename, true))
	{
		FString message = "Unable to add database '" + actualFilename + "', it is not valid (problems opening it +++)!";
		LOGSQLITE(Error, *message);
		return false;
	}
	
	Databases.Add(Name, actualFilename);

	FString successMessage = "Registered SQLite database '" + actualFilename + "' successfully.";
	LOGSQLITE(Verbose, *successMessage);
	return true;
	
}

//--------------------------------------------------------------------------------------------------------------

bool USQLiteDatabase::GetDataIntoObject(const FString& DatabaseName, const FString& Query, UObject* ObjectToPopulate)
{
	//////////////////////////////////////////////////////////////////////////
	// Check input validness.
	//////////////////////////////////////////////////////////////////////////

	if (ObjectToPopulate == NULL)
	{
		LOGSQLITE(Error, TEXT("ObjectToPopulate needs to be set to get any results!"));
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// Validate the database
	//////////////////////////////////////////////////////////////////////////

	if (!IsDatabaseRegistered(DatabaseName) ||
		!IsValidDatabase(Databases[DatabaseName], false))
	{
		LOGSQLITE(Error, *FString::Printf(TEXT("Unable to get data into object, invalid database '%s'"), *DatabaseName));
		return false;
	}

	if (!CanOpenDatabase(Databases[DatabaseName]))
	{
		LOGSQLITE(Error, *FString::Printf(TEXT("Unable to open database '%s'"), *DatabaseName));
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// Get the results
	//////////////////////////////////////////////////////////////////////////

	SQLiteQueryResult queryResult = RunQueryAndGetResults(DatabaseName, Query);

	if (queryResult.Success && queryResult.Results.Num() > 0)
	{
		AssignResultsToObjectProperties(queryResult.Results[0], ObjectToPopulate);
		return true;
	}
	else if (!queryResult.Success)
	{
		LOGSQLITE(Error, *FString::Printf(TEXT("Query resulted in an error: '%s'"), *queryResult.ErrorMessage));
		return false;
	}
	else if (queryResult.Results.Num() == 0)
	{
		LOGSQLITE(Error, TEXT("Query returned zero rows, no data to assign to object properties."));
		return false;
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------

bool USQLiteDatabase::GetDataIntoObjectBP(const FSQLiteDatabaseReference& DataSource, TArray<FString> Fields, 
	FSQLiteQueryFinalizedQuery Query, UObject* ObjectToPopulate)
{
	//////////////////////////////////////////////////////////////////////////
	// Check input validness.
	//////////////////////////////////////////////////////////////////////////

	if (ObjectToPopulate == NULL)
	{
		LOGSQLITE(Error, TEXT("ObjectToPopulate needs to be set to get any results!"));
		return false;
	}

	if (DataSource.Tables.Num() == 0)
	{
		LOGSQLITE(Error, TEXT("The query needs the table name!"));
		return false;
	}

	if (Fields.Num() == 0)
	{
		LOGSQLITE(Error, TEXT("The query needs fields! You may use * to get all fields."));
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// Validate the database
	//////////////////////////////////////////////////////////////////////////

	if (!IsDatabaseRegistered(DataSource.DatabaseName) ||
		!IsValidDatabase(Databases[DataSource.DatabaseName], true))
	{
		LOGSQLITE(Error, TEXT("Unable to get data to object, database validation failed!"));
		return false;
	}


	//////////////////////////////////////////////////////////////////////////
	// Get the results
	//////////////////////////////////////////////////////////////////////////

	FString constructedQuery = ConstructQuery(DataSource.Tables, Fields, Query, 1, 0);

	SQLiteQueryResult queryResult = RunQueryAndGetResults(DataSource.DatabaseName, constructedQuery);

	if (queryResult.Success && queryResult.Results.Num() > 0)
	{
		AssignResultsToObjectProperties(queryResult.Results[0], ObjectToPopulate);
		return true;
	}
	else if (!queryResult.Success)
	{
		LOGSQLITE(Error, *FString::Printf(TEXT("Query resulted in an error: '%s'"), *queryResult.ErrorMessage));
		return false;
	}
	else if (queryResult.Results.Num() == 0)
	{
		LOGSQLITE(Error, TEXT("Query returned zero rows, no data to assign to object properties."));
		return false;
	}

	return false;

}

//--------------------------------------------------------------------------------------------------------------

TMap<FString, UProperty*> USQLiteDatabase::CollectProperties(UObject* SourceObject)
{
	
	UClass* SourceObjectClass = SourceObject->GetClass();
	TMap<FString, UProperty*> Props;
	for (TFieldIterator<UProperty> PropIt(SourceObjectClass, EFieldIteratorFlags::SuperClassFlags::IncludeSuper); 
		PropIt; ++PropIt)
	{
		Props.Add(*PropIt->GetNameCPP(), *PropIt);
	}

	return Props;
}

//--------------------------------------------------------------------------------------------------------------

bool USQLiteDatabase::IsDatabaseRegistered(FString DatabaseName)
{
	return Databases.Contains(DatabaseName);
}

//--------------------------------------------------------------------------------------------------------------

bool USQLiteDatabase::CanOpenDatabase(FString DatabaseFilename)
{
	sqlite3* db;
	if (sqlite3_open(TCHAR_TO_ANSI(*DatabaseFilename), &db) == SQLITE_OK)
	{
		sqlite3_close(db);
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------

bool USQLiteDatabase::IsValidDatabase(FString DatabaseFilename, bool TestByOpening)
{
	if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*DatabaseFilename))
	{
		if (TestByOpening)
		{
			return CanOpenDatabase(DatabaseFilename);
		}
		else
		{
			return true;
		}
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------

FSQLiteQueryResult USQLiteDatabase::GetData(const FString& DatabaseName, const FString& Query)
{
	FSQLiteQueryResult result;

	//////////////////////////////////////////////////////////////////////////
	// Validate the database
	//////////////////////////////////////////////////////////////////////////

	if (!IsDatabaseRegistered(DatabaseName) ||
		!IsValidDatabase(Databases[DatabaseName], true))
	{
		LOGSQLITE(Error, TEXT("Unable to get data to object, database validation failed!"));
		result.Success = false;
		result.ErrorMessage = TEXT("Database validation failed");
		return result;
	}

	//////////////////////////////////////////////////////////////////////////
	// Get the results
	//////////////////////////////////////////////////////////////////////////

	SQLiteQueryResult queryResult = RunQueryAndGetResults(DatabaseName, Query);
	result.Success = queryResult.Success;
	result.ErrorMessage = queryResult.ErrorMessage;

	for (auto row : queryResult.Results)
	{
		FSQLiteQueryResultRow outRow;
		for (auto field : row.Fields)
		{
			FSQLiteKeyValuePair outField;
			outField.Key = field.Name;
			outField.Value = field.ToString();

			outRow.Fields.Add(outField);
		}
		result.ResultRows.Add(outRow);
	}

	return result;

}

//--------------------------------------------------------------------------------------------------------------

FSQLiteQueryResult USQLiteDatabase::GetDataBP(const FSQLiteDatabaseReference& DataSource, 
	TArray<FString> Fields, FSQLiteQueryFinalizedQuery Query, int32 MaxResults, int32 ResultOffset)
{

	FSQLiteQueryResult result;

	//////////////////////////////////////////////////////////////////////////
	// Check input validness.
	//////////////////////////////////////////////////////////////////////////

	if (DataSource.Tables.Num() == 0)
	{
		LOGSQLITE(Error, TEXT("The query needs at least one table name!"));
		result.Success = false;
		result.ErrorMessage = TEXT("No table given");
		return result;
	}

	if (Fields.Num() == 0)
	{
		LOGSQLITE(Error, TEXT("The query needs fields! You can use * to get all fields."));
		result.Success = false;
		result.ErrorMessage = TEXT("No fields given");
		return result;
	}

	FString constructedQuery = ConstructQuery(DataSource.Tables, Fields, Query, MaxResults, ResultOffset);

	return GetData(DataSource.DatabaseName, constructedQuery);
	
}

//--------------------------------------------------------------------------------------------------------------

FString USQLiteDatabase::ConstructQuery(TArray<FString> Tables, TArray<FString> Fields, 
	FSQLiteQueryFinalizedQuery QueryObject, int32 MaxResults, int32 ResultOffset)
{
	FString fieldString;
	for (int32 i = 0; i < Fields.Num(); i++)
	{
		fieldString.Append(Fields[i] + (i < Fields.Num() - 1 ? "," : ""));
	}

	FString tableList = FString::Join(Tables, TEXT(","));
	TArray<FString> allQueryParams;

	allQueryParams.Add(FString::Printf(TEXT("SELECT %s FROM %s"), *fieldString, *tableList));

	if (QueryObject.Query.Len() > 0)
	{
		allQueryParams.Add(FString::Printf(TEXT("WHERE %s"), *QueryObject.Query));
	}

	if (MaxResults >= 0)
	{
		allQueryParams.Add(FString::Printf(TEXT("LIMIT %i"), MaxResults));
	}

	if (ResultOffset > 0)
	{
		allQueryParams.Add(FString::Printf(TEXT("OFFSET %i"), ResultOffset));
	}

	FString finalQuery = FString::Join(allQueryParams, TEXT(" "));
	return finalQuery;

}

//--------------------------------------------------------------------------------------------------------------

SQLiteQueryResult USQLiteDatabase::RunQueryAndGetResults(FString DatabaseName, FString Query)
{
	sqlite3* db;
	ANSICHAR* dbNameAsUtf8 = TCHAR_TO_UTF8(*Databases[DatabaseName]);
	SQLiteQueryResult result;

	int32 sqlReturnCode = sqlite3_open(dbNameAsUtf8, &db);

	sqlite3_stmt* preparedStatement;
	ANSICHAR* queryAsUtf8 = TCHAR_TO_UTF8(*Query);

	sqlReturnCode = sqlite3_prepare_v2(db, queryAsUtf8, -1, &preparedStatement, NULL);
	if (sqlReturnCode != SQLITE_OK)
	{
		const char* errorMessage = sqlite3_errmsg(db);
		FString error = "SQL error: " + FString(UTF8_TO_TCHAR(errorMessage));
		LOGSQLITE(Error, *error);
		LOGSQLITE(Error, *FString::Printf(TEXT("The attempted query was: %s"), *Query));
		result.ErrorMessage = error;
		result.Success = false;
		sqlite3_finalize(preparedStatement);
		sqlite3_close(db);
		return result;
	}

	//////////////////////////////////////////////////////////////////////////
	// Get and assign the data
	//////////////////////////////////////////////////////////////////////////

	TArray<SQLiteResultValue> resultRows;

	for (sqlReturnCode = sqlite3_step(preparedStatement); 
		sqlReturnCode != SQLITE_DONE && sqlReturnCode == SQLITE_ROW; 
		sqlReturnCode = sqlite3_step(preparedStatement))
	{
		SQLiteResultValue row;

		LOGSQLITE(Verbose, TEXT("Query returned a result row."));
		int32 resultColumnCount = sqlite3_column_count(preparedStatement);
		for (int32 c = 0; c < resultColumnCount; c++)
		{
			int32 columnType = sqlite3_column_type(preparedStatement, c);
			const char* columnName = sqlite3_column_name(preparedStatement, c);
			FString columnNameStr = UTF8_TO_TCHAR(columnName);
			SQLiteResultField val;
			val.Name = columnNameStr;
			switch (columnType)
			{
			case SQLITE_INTEGER:
				val.Type = SQLiteResultValueTypes::Integer;
				val.IntValue = sqlite3_column_int64(preparedStatement, c);
				break;
			case SQLITE_TEXT:
				val.Type = SQLiteResultValueTypes::Text;
				val.StringValue = UTF8_TO_TCHAR(sqlite3_column_text(preparedStatement, c));
				break;
			case SQLITE_FLOAT:
				val.Type = SQLiteResultValueTypes::Float;
				val.DoubleValue = sqlite3_column_double(preparedStatement, c);
				break;
			case SQLITE_NULL:
			default:
				val.Type = SQLiteResultValueTypes::UnsupportedValueType;
			}

			if (val.Type != SQLiteResultValueTypes::UnsupportedValueType)
			{
				row.Fields.Add(val);
			}
		}

		resultRows.Add(row);
	}

	//////////////////////////////////////////////////////////////////////////
	// Release the statement and close the connection
	//////////////////////////////////////////////////////////////////////////

	sqlite3_finalize(preparedStatement);
	sqlite3_close(db);

	result.Results = resultRows;
	result.Success = true;
	return result;

}

//--------------------------------------------------------------------------------------------------------------

void USQLiteDatabase::AssignResultsToObjectProperties(const SQLiteResultValue& ResultValue, UObject* ObjectToPopulate)
{
	auto propertyMap = CollectProperties(ObjectToPopulate);
	for (SQLiteResultField field : ResultValue.Fields)
	{
		if (propertyMap.Contains(field.Name))
		{
			UProperty* targetProperty = propertyMap[field.Name];

			if (field.Type == SQLiteResultValueTypes::Integer)
			{
				UInt64Property* int64prop = NULL;
				UIntProperty* int32prop = NULL;
				UInt16Property* int16prop = NULL;
				UInt8Property* int8prop = NULL;
				UBoolProperty* boolProp = NULL;

				if ((int64prop = Cast<UInt64Property>(targetProperty)) != NULL)
				{
					int64prop->SetPropertyValue_InContainer(ObjectToPopulate, field.IntValue);
					LOGSQLITE(Verbose, *FString::Printf(TEXT("Property '%s' was set to '%d'"), *field.Name, field.IntValue));
				}
				else if ((int32prop = Cast<UIntProperty>(targetProperty)) != NULL)
				{
					int32prop->SetPropertyValue_InContainer(ObjectToPopulate, (int32)field.IntValue);
					LOGSQLITE(Verbose, *FString::Printf(TEXT("Property '%s' was set to '%d'"), *field.Name, field.IntValue));
				}
				else if ((int16prop = Cast<UInt16Property>(targetProperty)) != NULL)
				{
					int16prop->SetPropertyValue_InContainer(ObjectToPopulate, (int16)field.IntValue);
					LOGSQLITE(Verbose, *FString::Printf(TEXT("Property '%s' was set to '%d'"), *field.Name, field.IntValue));
				}
				else if ((int8prop = Cast<UInt8Property>(targetProperty)) != NULL)
				{
					int8prop->SetPropertyValue_InContainer(ObjectToPopulate, (int8)field.IntValue);
					LOGSQLITE(Verbose, *FString::Printf(TEXT("Property '%s' was set to '%d'"), *field.Name, field.IntValue));
				}
				else if ((boolProp = Cast<UBoolProperty>(targetProperty)) != NULL)
				{
					boolProp->SetPropertyValue_InContainer(ObjectToPopulate, field.IntValue > 0);
					LOGSQLITE(Verbose, *FString::Printf(TEXT("Property '%s' was set to '%d'"), *field.Name, field.IntValue));
				}
			}

			else if (field.Type == SQLiteResultValueTypes::Float)
			{
				UDoubleProperty* doubleProp = NULL;
				UFloatProperty* floatProp = NULL;
				if ((doubleProp = Cast<UDoubleProperty>(targetProperty)) != NULL)
				{
					doubleProp->SetPropertyValue_InContainer(ObjectToPopulate, field.DoubleValue);
					LOGSQLITE(Verbose, *FString::Printf(TEXT("Property '%s' was set to '%f'"), *field.Name, field.DoubleValue));
				}
				else if ((floatProp = Cast<UFloatProperty>(targetProperty)) != NULL)
				{
					floatProp->SetPropertyValue_InContainer(ObjectToPopulate, (float)field.DoubleValue);
					LOGSQLITE(Verbose, *FString::Printf(TEXT("Property '%s' was set to '%f'"), *field.Name, field.DoubleValue));
				}
			}

			else if (field.Type == SQLiteResultValueTypes::Text)
			{
				UStrProperty* strProp = NULL;
				if ((strProp = Cast<UStrProperty>(targetProperty)) != NULL)
				{
					strProp->SetPropertyValue_InContainer(ObjectToPopulate, field.StringValue);
					LOGSQLITE(Verbose, *FString::Printf(TEXT("Property '%s' was set to '%s'"), *field.Name, *field.StringValue.Mid(0, 64)));
				}
			}

		}
	}
}

//--------------------------------------------------------------------------------------------------------------
