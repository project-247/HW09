#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "MyUserWidget.generated.h"

UCLASS()
class HW09_API UMyUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 게임 관련 UI 요소들
	class UTextBlock* PlayerCountText;
	class UButton* SubmitCardsButton;
	class UEditableTextBox* Card1Input;
	class UEditableTextBox* Card2Input;
	class UEditableTextBox* Card3Input;
	class UTextBlock* GameStatusText;

	// 채팅 관련 UI 요소들
	class UScrollBox* ChatScrollBox;
	class UEditableTextBox* ChatInputBox;
	class UButton* SendChatButton;

	UFUNCTION(BlueprintCallable, Category = "UI Actions")
	void OnSubmitCards();

	UFUNCTION(BlueprintCallable, Category = "UI Updates")
	void UpdateGameStatus();

	UFUNCTION(BlueprintCallable, Category = "UI Updates")
	void UpdatePlayerCount(int32 Count);

	UFUNCTION(BlueprintCallable, Category = "UI Updates")
	void DisplayGameResult(int32 Strikes, int32 Balls, const FString& ResultType);

	UFUNCTION(BlueprintCallable, Category = "UI Updates")
	void UpdatePlayerStatus();

	UFUNCTION(BlueprintCallable, Category = "UI Updates")
	void ShowGameEndMessage(const FString& Message);

	// 채팅 관련 함수들
	UFUNCTION(BlueprintCallable)
	void OnSendChatClicked();

	UFUNCTION(BlueprintCallable)
	void AddChatMessage(const FString& PlayerName, const FString& Message);

	UFUNCTION(BlueprintCallable)
	void SendChatMessage();

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnSubmitButtonClicked();

	UFUNCTION()
	void OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	bool ValidateCardInput();
	TArray<int32> GetCardInputs();

	FTimerHandle StatusResetTimer;
	FTimerHandle StatusUpdateTimer;

	UFUNCTION()
	void ResetStatusDisplay();
};