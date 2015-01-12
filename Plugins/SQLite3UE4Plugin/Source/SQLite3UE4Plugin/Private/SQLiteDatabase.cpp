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

	if (RelativeToGameContentDirectory)
	{
		actualFilename = FPaths::GameContentDir() + Filename;
	}
	
	if (IsDatabaseRegistered(Name))
	{
		FString message = "Database '" + actualFilename + "' is already registered, skipping.";
		LOGSQLITE(Warning, *message);
		return true;
	}

	if (!IsValidDatabase(actualFilename, true))
	{
		FString message = "Unable to add database '" + actualFilename + "', it is not valid (problems opening it)!";
		LOGSQLITE(Error, *message);
		return false;
	}
	
	Databases.Add(Name, actualFilename);

	FString successMessage = "Registered SQLite database '" + actualFilename + "' successfully.";
	LOGSQLITE(Verbose, *successMessage);
	return true;
	
}

//--------------------------------------------------------------------------------------------------------------

bool USQLiteDatabase::GetDataToObject(FString DatabaseName, FString Table, TArray<FString> Fields, FString Criteria, UObject* DataToFill)
{
	//////////////////////////////////////////////////////////////////////////
	// Check input validness.
	//////////////////////////////////////////////////////////////////////////

	if (Criteria.Len() == 0)
	{
		LOGSQLITE(Error, TEXT("This database query must have criteria!"));
		return false;
	}

	if (DataToFill == NULL)
	{
		LOGSQLITE(Error, TEXT("FilledData needs to be set to get any results!"));
		return false;
	}

	if (Table.Len() == 0)
	{
		LOGSQLITE(Error, TEXT("The query needs the table name!"));
		return false;
	}

	if (Fields.Num() == 0)
	{
		LOGSQLITE(Error, TEXT("The query needs fields! You may use * to get all fields."));
		return false;
	}

	if (!IsDatabaseRegistered(DatabaseName) || !IsValidDatabase(Databases[DatabaseName], false))
	{
		LOGSQLITE(Error, TEXT("Unable to get data to object, invalid database!"));
		return false;
	}


	//////////////////////////////////////////////////////////////////////////
	// Create a query and open the database
	//////////////////////////////////////////////////////////////////////////

	// Note: Sqlite wants UTF-8 strings.

	sqlite3* db;
	ANSICHAR* dbNameAsUtf8 = TCHAR_TO_UTF8(*Databases[DatabaseName]);

	int32 sqlReturnCode = sqlite3_open(dbNameAsUtf8, &db);
	if (sqlReturnCode != SQLITE_OK)
	{
		LOGSQLITE(Error, TEXT("Unable to get data to object, could not open the database!"));
		return false;
	}

	FString FieldString;
	for (int32 i = 0; i < Fields.Num(); i++)
	{
		FieldString.Append(Fields[i] + (i < Fields.Num() - 1 ? "," : ""));
	}

	FString query = "SELECT " + FieldString + " FROM " + Table;
	if (Criteria.Len() > 0)
	{
		query += " WHERE " + Criteria + " LIMIT 1";
	}

	sqlite3_stmt* preparedStatement;
	ANSICHAR* queryAsUtf8 = TCHAR_TO_UTF8(*query);

	sqlReturnCode = sqlite3_prepare_v2(db, queryAsUtf8, -1, &preparedStatement, NULL);
	if (sqlReturnCode != SQLITE_OK)
	{
		const char* errorMessage = sqlite3_errmsg(db);
		FString error = "SQL error: " + FString(UTF8_TO_TCHAR(errorMessage));
		LOGSQLITE(Error, *error);
		LOGSQLITE(Error, *FString::Printf(TEXT("The attempted query was: %s"), *query));
		sqlite3_finalize(preparedStatement);
		sqlite3_close(db);
		return false;
	}
	
	//////////////////////////////////////////////////////////////////////////
	// Get and assign the data
	//////////////////////////////////////////////////////////////////////////

	sqlReturnCode = sqlite3_step(preparedStatement);
	if (sqlReturnCode == SQLITE_DONE)
	{
		LOGSQLITE(Warning, TEXT("Zero rows returned from SQLite query."));
		sqlite3_finalize(preparedStatement);
		sqlite3_close(db);
	}
	else if (sqlReturnCode == SQLITE_ROW)
	{
		LOGSQLITE(Verbose, TEXT("Query returned a result."));
		auto propertyMap = CollectProperties(DataToFill);

		int32 resultColumnCount = sqlite3_column_count(preparedStatement);
		for (int32 c = 0; c < resultColumnCount; c++)
		{
			int32 columnType = sqlite3_column_type(preparedStatement, c);
			const char* columnName = sqlite3_column_name(preparedStatement, c);
			FString columnNameStr = UTF8_TO_TCHAR(columnName);

			if (propertyMap.Contains(columnNameStr))
			{
							
				if (columnType == SQLITE_INTEGER)
				{
					int64 columnValue = sqlite3_column_int64(preparedStatement, c);
					UInt64Property* int64prop = NULL;
					UIntProperty* int32prop = NULL;
					UInt16Property* int16prop = NULL;
					UInt8Property* int8prop = NULL;
					UBoolProperty* boolProp = NULL;
					
					if ((int64prop = Cast<UInt64Property>(propertyMap[columnNameStr])) != NULL)
					{
						int64prop->SetPropertyValue_InContainer(DataToFill, columnValue);						
						LOGSQLITE(Verbose, *FString::Printf(TEXT("Property '%s' was set to '%d'"), *columnNameStr, columnValue));
					}					
					else if ((int32prop = Cast<UIntProperty>(propertyMap[columnNameStr])) != NULL)
					{
						int32prop->SetPropertyValue_InContainer(DataToFill, (int32)columnValue);
						LOGSQLITE(Verbose, *FString::Printf(TEXT("Property '%s' was set to '%d'"), *columnNameStr, columnValue));
					}
					else if ((int16prop = Cast<UInt16Property>(propertyMap[columnNameStr])) != NULL)
					{
						int16prop->SetPropertyValue_InContainer(DataToFill, (int16)columnValue);
						LOGSQLITE(Verbose, *FString::Printf(TEXT("Property '%s' was set to '%d'"), *columnNameStr, columnValue));
					}
					else if ((int8prop = Cast<UInt8Property>(propertyMap[columnNameStr])) != NULL)
					{
						int8prop->SetPropertyValue_InContainer(DataToFill, (int8)columnValue);
						LOGSQLITE(Verbose, *FString::Printf(TEXT("Property '%s' was set to '%d'"), *columnNameStr, columnValue));
					}					
					else if ((boolProp = Cast<UBoolProperty>(propertyMap[columnNameStr])) != NULL)
					{
						boolProp->SetPropertyValue_InContainer(DataToFill, columnValue > 0);
						LOGSQLITE(Verbose, *FString::Printf(TEXT("Property '%s' was set to '%d'"), *columnNameStr, columnValue));
					}
				}

				else if (columnType == SQLITE_FLOAT)
				{
					float columnValue = sqlite3_column_double(preparedStatement, c);
					UDoubleProperty* doubleProp = NULL;
					UFloatProperty* floatProp = NULL;					
					if ((doubleProp = Cast<UDoubleProperty>(propertyMap[columnNameStr])) != NULL)
					{
						doubleProp->SetPropertyValue_InContainer(DataToFill, columnValue);
						LOGSQLITE(Verbose, *FString::Printf(TEXT("Property '%s' was set to '%f'"), *columnNameStr, columnValue));
					}
					else if ((floatProp = Cast<UFloatProperty>(propertyMap[columnNameStr])) != NULL)
					{
						floatProp->SetPropertyValue_InContainer(DataToFill, columnValue);
						LOGSQLITE(Verbose, *FString::Printf(TEXT("Property '%s' was set to '%f'"), *columnNameStr, columnValue));
					}
				}

				else if (columnType == SQLITE_TEXT)
				{
					const unsigned char* columnValue = sqlite3_column_text(preparedStatement, c);
					UStrProperty* strProp = NULL;
					if ((strProp = Cast<UStrProperty>(propertyMap[columnNameStr])) != NULL)
					{
						strProp->SetPropertyValue_InContainer(DataToFill, UTF8_TO_TCHAR(columnValue));
						LOGSQLITE(Verbose, *FString::Printf(TEXT("Property '%s' was set to '%s'"), *columnNameStr, UTF8_TO_TCHAR(columnValue)));
					}
				}

				else if (columnType == SQLITE_NULL)
				{
					LOGSQLITE(Verbose, *FString::Printf(TEXT("Property '%s' was returned from query as NULL, not assigning."), *columnNameStr));
				}
				
				// No support for SQLITE_BLOB for now.
			}
			
		}

		//////////////////////////////////////////////////////////////////////////
		// Release the statement and close the connection
		//////////////////////////////////////////////////////////////////////////

		sqlite3_finalize(preparedStatement);
		sqlite3_close(db);
	}

	return true;
}

//--------------------------------------------------------------------------------------------------------------

TMap<FString, UProperty*> USQLiteDatabase::CollectProperties(UObject* SourceObject)
{
	
	UClass* SourceObjectClass = SourceObject->GetClass();
	TMap<FString, UProperty*> Props;
	for (TFieldIterator<UProperty> PropIt(SourceObjectClass, EFieldIteratorFlags::SuperClassFlags::IncludeSuper); PropIt; ++PropIt)
	{
		Props.Add(*PropIt->GetNameCPP(), *PropIt);
		// todo vertaa propin classia (GetClass) vaan UBoolProperty::StaticClass jne määrityksiin. 
		// Palautetaan propertyistä lista nimi + property, sitten käydään sql fieldit läpi, jos löytyy saman niminen fieldi niin sitten katotaa prop->GetClass() 
		// ja verrataan UBooleanProperty::StaticClassiin, UFloatProperty jne... ja sitten castataan se prop siihen classiin ja kutsutaan sitä SetPropertyValue_InContaineria
	}

	return Props;
}

//--------------------------------------------------------------------------------------------------------------

bool USQLiteDatabase::IsDatabaseRegistered(FString DatabaseName)
{
	return Databases.Contains(DatabaseName);
}

//--------------------------------------------------------------------------------------------------------------

bool USQLiteDatabase::IsValidDatabase(FString DatabaseFilename, bool TestByOpening)
{
	if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*DatabaseFilename))
	{
		if (TestByOpening)
		{
			sqlite3* db;
			if (sqlite3_open(TCHAR_TO_ANSI(*DatabaseFilename), &db) == SQLITE_OK)
			{
				sqlite3_close(db);
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return true;
		}
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------
