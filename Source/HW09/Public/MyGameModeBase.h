#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyPlayerState.h"
#include "ChatMessage.h"
#include "MyGameModeBase.generated.h"

UCLASS()
class HW09_API AMyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMyGameModeBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Settings")
	int32 MaxPlayers = 9;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cards")
	TArray<int32> CardDeck;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cards")
	TArray<int32> ServerCards;

	UFUNCTION(BlueprintCallable, Category = "Game Logic")
	void InitializeGame();

	void DealCards();
	void CheckWinCondition();

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Game Logic")
	void ServerCompareCards();

	// 블루프린트 이벤트들
	UFUNCTION(BlueprintImplementableEvent, Category = "Game Events")
	void OnGameStart();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Events")
	void OnPlayerWin(AMyPlayerState* Winner);

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Events")
	void OnPlayerOut(AMyPlayerState* OutPlayer);

	UFUNCTION(BlueprintCallable)
	void BroadcastGameResult(int32 Strikes, int32 Balls, const FString& ResultType);

	UFUNCTION(BlueprintCallable)
	void BroadcastEndGameMessage(const FString& Message);

	// 채팅 관련 함수들
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerSendChatMessage(const FString& PlayerName, const FString& Message);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void MulticastReceiveChatMessage(const FString& PlayerName, const FString& Message);

protected:
	virtual void BeginPlay() override;

	void ComparePlayerCards(AMyPlayerState* Player);
	void RemovePlayer(AMyPlayerState* Player);
	void EndGame();

private:
	void ServerCompareCards_Implementation();

	FTimerHandle RestartGameTimerHandle;
	UFUNCTION()
	void RestartGame();

	FTimerHandle PlayerCountUpdateTimer;
	UFUNCTION()
	void UpdatePlayerCountDisplay();

	// 채팅 메시지 저장
	UPROPERTY()
	TArray<FChatMessage> ChatHistory;

	void ServerSendChatMessage_Implementation(const FString& PlayerName, const FString& Message);
	void MulticastReceiveChatMessage_Implementation(const FString& PlayerName, const FString& Message);
};