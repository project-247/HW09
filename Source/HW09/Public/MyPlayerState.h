#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "MyPlayerState.generated.h"

UCLASS()
class HW09_API AMyPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AMyPlayerState();

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game State")
	TArray<int32> PlayerCards;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game State")
	int32 CurrentHitCount = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game State")
	bool bIsWinner = false;

	UFUNCTION(BlueprintCallable, Category = "Player Actions")
	void SubmitCards(const TArray<int32>& Cards);

	// Getter 함수들
	UFUNCTION(BlueprintCallable, Category = "Player Info")
	int32 GetCurrentHitCount() const { return CurrentHitCount; }

	UFUNCTION(BlueprintCallable, Category = "Player Info")
	int32 GetMaxHitCount() const { return 3; }

	UFUNCTION(BlueprintCallable, Category = "Player Info")
	bool GetIsWinner() const { return bIsWinner; }

	UFUNCTION(BlueprintCallable, Category = "Player Info")
	FString GetPlayerStatus() const;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};