// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MyActor.generated.h"

/**
 * 
 */
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
