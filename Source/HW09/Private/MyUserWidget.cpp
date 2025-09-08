#include "MyUserWidget.h"
#include "MyPlayerState.h"
#include "MyGameModeBase.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "GameFramework/PlayerState.h"

void UMyUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Warning, TEXT("NativeConstruct called"));

	// 게임 위젯 바인딩
	Card1Input = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("Card1Input")));
	Card2Input = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("Card2Input")));
	Card3Input = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("Card3Input")));
	PlayerCountText = Cast<UTextBlock>(GetWidgetFromName(TEXT("PlayerCountText")));
	GameStatusText = Cast<UTextBlock>(GetWidgetFromName(TEXT("GameStatusText")));
	SubmitCardsButton = Cast<UButton>(GetWidgetFromName(TEXT("SubmitCardsButton")));

	// 채팅 위젯 바인딩
	ChatScrollBox = Cast<UScrollBox>(GetWidgetFromName(TEXT("ChatScrollBox")));
	ChatInputBox = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("ChatInputBox")));
	SendChatButton = Cast<UButton>(GetWidgetFromName(TEXT("SendChatButton")));

	UE_LOG(LogTemp, Warning, TEXT("Card1Input: %s"), Card1Input ? TEXT("Found") : TEXT("NULL"));
	UE_LOG(LogTemp, Warning, TEXT("Card2Input: %s"), Card2Input ? TEXT("Found") : TEXT("NULL"));
	UE_LOG(LogTemp, Warning, TEXT("Card3Input: %s"), Card3Input ? TEXT("Found") : TEXT("NULL"));

	if (SubmitCardsButton)
	{
		SubmitCardsButton->OnClicked.AddDynamic(this, &UMyUserWidget::OnSubmitButtonClicked);
		UE_LOG(LogTemp, Warning, TEXT("Button binding successful"));
	}

	if (SendChatButton)
	{
		SendChatButton->OnClicked.AddDynamic(this, &UMyUserWidget::OnSendChatClicked);
	}

	if (ChatInputBox)
	{
		ChatInputBox->OnTextCommitted.AddDynamic(this, &UMyUserWidget::OnChatInputTextCommitted);
	}

	// 1초마다 플레이어 상태 업데이트
	GetWorld()->GetTimerManager().SetTimer(
		StatusUpdateTimer,
		this,
		&UMyUserWidget::UpdatePlayerStatus,
		1.0f,
		true
	);

	UE_LOG(LogTemp, Warning, TEXT("Chat widgets bound: ScrollBox=%s, InputBox=%s, Button=%s"),
		ChatScrollBox ? TEXT("OK") : TEXT("NULL"),
		ChatInputBox ? TEXT("OK") : TEXT("NULL"),
		SendChatButton ? TEXT("OK") : TEXT("NULL"));
}

void UMyUserWidget::OnSubmitButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Button Clicked!"));
	OnSubmitCards();
}

void UMyUserWidget::OnSubmitCards()
{
	UE_LOG(LogTemp, Warning, TEXT("OnSubmitCards called"));

	if (!ValidateCardInput())
	{
		UE_LOG(LogTemp, Warning, TEXT("Validation failed"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Validation passed"));

	TArray<int32> Cards = GetCardInputs();
	UE_LOG(LogTemp, Warning, TEXT("Cards: %d, %d, %d"), Cards[0], Cards[1], Cards[2]);

	if (APlayerState* PS = GetOwningPlayerState())
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerState found"));
		if (AMyPlayerState* MyPS = Cast<AMyPlayerState>(PS))
		{
			UE_LOG(LogTemp, Warning, TEXT("MyPlayerState cast successful"));
			MyPS->SubmitCards(Cards);

			if (AMyGameModeBase* GameMode = Cast<AMyGameModeBase>(GetWorld()->GetAuthGameMode()))
			{
				UE_LOG(LogTemp, Warning, TEXT("GameMode found, calling ServerCompareCards"));
				GameMode->ServerCompareCards();
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("GameMode NOT found"));
			}
		}
	}

	// 입력 필드 초기화
	if (Card1Input) Card1Input->SetText(FText::GetEmpty());
	if (Card2Input) Card2Input->SetText(FText::GetEmpty());
	if (Card3Input) Card3Input->SetText(FText::GetEmpty());
}

bool UMyUserWidget::ValidateCardInput()
{
	TArray<int32> Cards = GetCardInputs();

	UE_LOG(LogTemp, Warning, TEXT("Card count: %d"), Cards.Num());

	if (Cards.Num() != 3)
	{
		UE_LOG(LogTemp, Warning, TEXT("Card count is not 3"));
		return false;
	}

	for (int32 i = 0; i < Cards.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("Card[%d] = %d"), i, Cards[i]);
		if (Cards[i] < 1 || Cards[i] > 9)
		{
			UE_LOG(LogTemp, Warning, TEXT("Card %d is out of range (1-9)"), Cards[i]);
			return false;
		}
	}

	for (int32 i = 0; i < Cards.Num(); i++)
	{
		for (int32 j = i + 1; j < Cards.Num(); j++)
		{
			if (Cards[i] == Cards[j])
			{
				UE_LOG(LogTemp, Warning, TEXT("Duplicate cards found: %d"), Cards[i]);
				return false;
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Validation successful"));
	return true;
}

TArray<int32> UMyUserWidget::GetCardInputs()
{
	TArray<int32> Cards;

	if (Card1Input)
	{
		FString Card1Str = Card1Input->GetText().ToString();
		UE_LOG(LogTemp, Warning, TEXT("Card1 input: '%s'"), *Card1Str);
		if (!Card1Str.IsEmpty())
		{
			int32 CardValue = FCString::Atoi(*Card1Str);
			UE_LOG(LogTemp, Warning, TEXT("Card1 converted to: %d"), CardValue);
			Cards.Add(CardValue);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Card1 input is empty"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Card1Input is null"));
	}

	if (Card2Input)
	{
		FString Card2Str = Card2Input->GetText().ToString();
		UE_LOG(LogTemp, Warning, TEXT("Card2 input: '%s'"), *Card2Str);
		if (!Card2Str.IsEmpty())
		{
			int32 CardValue = FCString::Atoi(*Card2Str);
			UE_LOG(LogTemp, Warning, TEXT("Card2 converted to: %d"), CardValue);
			Cards.Add(CardValue);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Card2Input is null"));
	}

	if (Card3Input)
	{
		FString Card3Str = Card3Input->GetText().ToString();
		UE_LOG(LogTemp, Warning, TEXT("Card3 input: '%s'"), *Card3Str);
		if (!Card3Str.IsEmpty())
		{
			int32 CardValue = FCString::Atoi(*Card3Str);
			UE_LOG(LogTemp, Warning, TEXT("Card3 converted to: %d"), CardValue);
			Cards.Add(CardValue);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Card3Input is null"));
	}

	UE_LOG(LogTemp, Warning, TEXT("Total cards collected: %d"), Cards.Num());
	return Cards;
}

void UMyUserWidget::UpdateGameStatus()
{
	// 게임 상태 업데이트 로직
}

void UMyUserWidget::UpdatePlayerCount(int32 Count)
{
	if (PlayerCountText)
	{
		PlayerCountText->SetText(FText::FromString(FString::Printf(TEXT("Players: %d"), Count)));
	}
}

void UMyUserWidget::DisplayGameResult(int32 Strikes, int32 Balls, const FString& ResultType)
{
	if (GameStatusText)
	{
		FString ResultText;
		if (ResultType == TEXT("WIN"))
		{
			ResultText = FString::Printf(TEXT("3S - 승리!"));
		}
		else if (ResultType == TEXT("OUT"))
		{
			ResultText = TEXT("OUT - 탈락!");
		}
		else
		{
			ResultText = FString::Printf(TEXT("%dS %dB"), Strikes, Balls);
		}

		GameStatusText->SetText(FText::FromString(ResultText));
		UE_LOG(LogTemp, Warning, TEXT("UI Updated: %s"), *ResultText);
	}
}

void UMyUserWidget::UpdatePlayerStatus()
{
	if (APlayerState* PS = GetOwningPlayerState())
	{
		if (AMyPlayerState* MyPS = Cast<AMyPlayerState>(PS))
		{
			FString StatusText = FString::Printf(TEXT("플레이어: %s | %s"),
				*MyPS->GetPlayerName(),
				*MyPS->GetPlayerStatus());

			if (PlayerCountText)
			{
				PlayerCountText->SetText(FText::FromString(StatusText));
			}
		}
	}
}

void UMyUserWidget::ShowGameEndMessage(const FString& Message)
{
	if (GameStatusText)
	{
		GameStatusText->SetText(FText::FromString(Message));

		// 5초 후 상태 초기화
		GetWorld()->GetTimerManager().SetTimer(
			StatusResetTimer,
			this,
			&UMyUserWidget::ResetStatusDisplay,
			5.0f,
			false
		);
	}
}

void UMyUserWidget::ResetStatusDisplay()
{
	if (GameStatusText)
	{
		GameStatusText->SetText(FText::FromString(TEXT("게임 진행 중...")));
	}
}

// 채팅 관련 함수들
void UMyUserWidget::OnSendChatClicked()
{
	SendChatMessage();
}

void UMyUserWidget::OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		SendChatMessage();
	}
}

void UMyUserWidget::SendChatMessage()
{
	if (!ChatInputBox)
		return;

	FString Message = ChatInputBox->GetText().ToString().TrimStartAndEnd();
	if (Message.IsEmpty())
		return;

	// 플레이어 이름 가져오기
	FString PlayerName = TEXT("Player");
	if (APlayerState* PS = GetOwningPlayerState())
	{
		PlayerName = PS->GetPlayerName();
		if (PlayerName.IsEmpty())
			PlayerName = TEXT("Anonymous");
	}

	// 시도 횟수 정보 추가
	if (AMyPlayerState* MyPS = Cast<AMyPlayerState>(GetOwningPlayerState()))
	{
		FString StatusInfo = FString::Printf(TEXT(" (%s)"), *MyPS->GetPlayerStatus());
		PlayerName += StatusInfo;
	}

	// GameMode를 통해 채팅 전송
	if (AMyGameModeBase* GameMode = Cast<AMyGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->ServerSendChatMessage(PlayerName, Message);
	}

	// 입력 필드 초기화
	ChatInputBox->SetText(FText::GetEmpty());

	UE_LOG(LogTemp, Warning, TEXT("Sending chat: [%s] %s"), *PlayerName, *Message);
}

void UMyUserWidget::AddChatMessage(const FString& PlayerName, const FString& Message)
{
	if (!ChatScrollBox)
		return;

	// 새 텍스트 블록 생성
	UTextBlock* ChatLine = NewObject<UTextBlock>(this);
	if (ChatLine)
	{
		FString FullMessage = FString::Printf(TEXT("[%s] %s"), *PlayerName, *Message);
		ChatLine->SetText(FText::FromString(FullMessage));

		// 스타일 설정
		FSlateFontInfo FontInfo = ChatLine->GetFont();
		FontInfo.Size = 12;
		ChatLine->SetFont(FontInfo);

		// 스크롤박스에 추가
		ChatScrollBox->AddChild(ChatLine);

		// 스크롤을 맨 아래로
		ChatScrollBox->ScrollToEnd();
	}
}