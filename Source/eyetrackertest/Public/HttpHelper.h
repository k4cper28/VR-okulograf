// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HttpHelper.generated.h"

/**
 * 
 */
UCLASS()
class EYETRACKERTEST_API UHttpHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	// Funkcja do odczytu pliku binarnego
	UFUNCTION(BlueprintCallable, Category = "File HTTP")
	static TArray<uint8> ReadFile(const FString& FilePath);

	// Funkcja do konwersji na Base64
	UFUNCTION(BlueprintCallable, Category = "File HTTP")
	static FString ConvertToBase64(const TArray<uint8>& Data);

	UFUNCTION(BlueprintCallable, Category = "File HTTP")
	static bool SendFileToServer(const FString& FilePath1, const FString& FilePath2, const FString& Url);

	UFUNCTION(BlueprintCallable, Category = "File HTTP")
	static bool GetImageFromServer(const FString& Url, const FString& SaveFilePath);

};
