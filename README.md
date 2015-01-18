# SQLite3UE4
SQLite3 Database Plugin for Unreal Engine 4


## What is it?
A plugin for UE4 that provides code and blueprint functionality that enables you to use SQLite3 databases in your projects.

Currently only supports reading data from databases. Updates and inserts will come next.
By using reflection it is possible to get data directly into member properties of C++ classes and blueprints, provided that the database field names match the UObject property names and that they have compatible data types. By using a normal query you can get data from tables into an array of string key -> value pairs.
Both C++ and blueprints are supported. For convenience, queries can be constructed from nodes in blueprints (easier to plug in variables into queries) or entered directly as a string. From C++ you're expected to use the string version of the methods.


## Currently supported platforms

The current version was built with Unreal Engine version 4.6.1.

Win32 and Win64 platforms are currently supported and tested. Mac and Linux should work as well but requires compiling the sqlite3 binaries and editing the build script so that the compiler can find the correct libraries to link to. Contributions on this would be much appreciated.


## How to install

For the convenience of testing and providing a sample of its usage, the plugin is inside a sample project in the repository (/Plugins/SQLite3UE4). Download the repository and just copy that folder into your own project's Plugins folder and you should be set.


## How does it work?

The plugin is pretty straightforward.
The steps to get your data are:

* Register a database to the plugin
* Construct a query from blueprint nodes or by supplying the SQL query string
* Plug in the query to a Get Data method and you're done

It's really simple.


## Sample usage

Here's a screenshot of a query setup with blueprints.

![alt text](http://i.imgur.com/5BtGuzH.png "Blueprint query sample")

And here's a simple sample in C++:

Header:
```c++
UCLASS()
class SQLITE_API AMyActor : public AActor
{
	GENERATED_BODY()


public:

	UFUNCTION(BlueprintCallable, Category = "My Actor")
	bool GetMyStats();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Actor")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Actor")
	int32 Age;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Actor")
	FString Gender;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Actor")
	float Height;
	
	
};

```

CPP:

```c++
#include "MyActor.h"
#include "SQLiteDatabase.h"

bool AMyActor::GetMyStats()
{
	FString dbName = TEXT("TestDatabase");
	FString actorName = TEXT("Bruce Willis");

	if (!USQLiteDatabase::IsDatabaseRegistered(dbName))
	{
		USQLiteDatabase::RegisterDatabase(dbName, "Databases/TestDatabase.db", true);
	}

	bool didPopulate = USQLiteDatabase::GetDataIntoObject(dbName, FString::Printf(TEXT("SELECT Name, Age, Gender, Height FROM Actors WHERE Name = \"%s\""), *actorName), this);

	return didPopulate;
}
```

