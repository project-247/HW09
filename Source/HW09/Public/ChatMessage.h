#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ChatMessage.generated.h"

USTRUCT(BlueprintType)
struct HW09_API FChatMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString PlayerName;

	UPROPERTY(BlueprintReadOnly)
	FString Message;

	UPROPERTY(BlueprintReadOnly)
	FDateTime Timestamp;

	FChatMessage()
	{
		PlayerName = TEXT("");
		Message = TEXT("");
		Timestamp = FDateTime::Now();
	}

	FChatMessage(const FString& InPlayerName, const FString& InMessage)
		: PlayerName(InPlayerName), Message(InMessage), Timestamp(FDateTime::Now())
	{
	}
};