// Fill out your copyright notice in the Description page of Project Settings.

#include "SQLite.h"
#include "MyActor.h"
#include "SQLiteDatabase.h"




bool AMyActor::GetMyStats()
{
	FString dbName = TEXT("TestDatabase");
	if (!USQLiteDatabase::IsDatabaseRegistered(dbName))
	{
		USQLiteDatabase::RegisterDatabase(dbName, "Databases/TestDatabase.db", true);
	}

	bool didPopulate = USQLiteDatabase::GetDataIntoObject(dbName, FString::Printf(TEXT("SELECT * FROM Actors WHERE Name = \"%s\""), *Name), this);

	return didPopulate;
}
