// Fill out your copyright notice in the Description page of Project Settings.


#include "HttpHelper.h"
#include "Misc/FileHelper.h"
#include "Misc/Base64.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Interfaces/IHttpRequest.h"
#include "Json.h"

TArray<uint8> UHttpHelper::ReadFile(const FString& FilePath)
{
    TArray<uint8> FileData;
    if (FFileHelper::LoadFileToArray(FileData, *FilePath))
    {
        return FileData;
    }
    return TArray<uint8>();
}

FString UHttpHelper::ConvertToBase64(const TArray<uint8>& Data)
{
    return FBase64::Encode(Data);
}

bool UHttpHelper::SendFileToServer(const FString& FilePath1, const FString& FilePath2, const FString& Url)
{
    // Odczyt pierwszego pliku
    TArray<uint8> FileData1 = ReadFile(FilePath1);
    // Odczyt drugiego pliku
    TArray<uint8> FileData2 = ReadFile(FilePath2);

    if (FileData1.Num() > 0 && FileData2.Num() > 0)
    {

        FString Base64Data1 = ConvertToBase64(FileData1);
        FString Base64Data2 = ConvertToBase64(FileData2);


        TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

        JsonObject->SetStringField(TEXT("wynik"), Base64Data1); // Pierwszy plik
        JsonObject->SetStringField(TEXT("photo"), Base64Data2); // Drugi plik

        FString OutputString;
        TSharedRef<TJsonWriter<TCHAR>> Writer = TJsonWriterFactory<TCHAR>::Create(&OutputString);
        FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);


        TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
        HttpRequest->SetURL(Url);
        HttpRequest->SetVerb(TEXT("POST"));
        HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
        HttpRequest->SetContentAsString(OutputString);


        bool bWasSuccessfulRequest = false;



        HttpRequest->OnProcessRequestComplete().BindLambda([&bWasSuccessfulRequest](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
            {
                if (bWasSuccessful)
                {
                    UE_LOG(LogTemp, Log, TEXT("Plik zostal wyslany pomyslnie!"));
                    bWasSuccessfulRequest = true;
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("Blad wysylania pliku."));
                    bWasSuccessfulRequest = false;
                }
            });

        HttpRequest->ProcessRequest();


        return bWasSuccessfulRequest;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Nie udalo sie odczytac pliku."));
        return false;
    }
}

bool UHttpHelper::GetImageFromServer(const FString& Url, const FString& SaveFilePath)
{
    TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->SetURL(Url);
    HttpRequest->SetVerb(TEXT("GET"));

    bool bWasSuccessfulRequest = false;

    HttpRequest->OnProcessRequestComplete().BindLambda([&bWasSuccessfulRequest, SaveFilePath](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
        {
            if (bWasSuccessful && Response.IsValid())
            {
                // Odczytanie odpowiedzi JSON
                TSharedPtr<FJsonObject> JsonResponse;
                TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

                if (FJsonSerializer::Deserialize(Reader, JsonResponse) && JsonResponse.IsValid())
                {
                    FString EncodedImage;
                    if (JsonResponse->TryGetStringField("image", EncodedImage))
                    {
                        TArray<uint8> ImageData;
                        if (FBase64::Decode(EncodedImage, ImageData))
                        {
                            // Zapisz dane obrazu do pliku (np. jako .jpg)
                            if (FFileHelper::SaveArrayToFile(ImageData, *SaveFilePath))
                            {
                                UE_LOG(LogTemp, Log, TEXT("Obraz zostal pomyslnie zapisany do: %s"), *SaveFilePath);
                                bWasSuccessfulRequest = true;
                            }
                            else
                            {
                                UE_LOG(LogTemp, Error, TEXT("Nie udalo sie zapisac obrazu do pliku: %s"), *SaveFilePath);
                                bWasSuccessfulRequest = false;
                            }
                        }
                        else
                        {
                            UE_LOG(LogTemp, Error, TEXT("Nie udalo sie zdekodowac obrazu z formatu Base64."));
                            bWasSuccessfulRequest = false;
                        }
                    }
                    else
                    {
                        UE_LOG(LogTemp, Error, TEXT("Nie udalo sie odczytac pola 'image' z odpowiedzi JSON."));
                        bWasSuccessfulRequest = false;
                    }
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("Nie udalo sie przetworzyc odpowiedzi JSON."));
                    bWasSuccessfulRequest = false;
                }
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Blad podczas pobierania obrazu z serwera."));
                bWasSuccessfulRequest = false;
            }
        });

    HttpRequest->ProcessRequest();

    return bWasSuccessfulRequest;
}
