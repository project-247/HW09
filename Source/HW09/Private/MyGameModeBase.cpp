#include "MyGameModeBase.h"
#include "MyPlayerState.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "ChatMessage.h"

AMyGameModeBase::AMyGameModeBase()
{
	PrimaryActorTick.bCanEverTick = false;
	PlayerStateClass = AMyPlayerState::StaticClass();
}

void AMyGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	InitializeGame();

	// 마우스 커서 표시 및 UI 전용 모드 설정
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeUIOnly());
	}

	// 플레이어 수 UI 업데이트
	GetWorldTimerManager().SetTimer(
		PlayerCountUpdateTimer,
		this,
		&AMyGameModeBase::UpdatePlayerCountDisplay,
		1.0f,
		true
	);
}

void AMyGameModeBase::UpdatePlayerCountDisplay()
{
	if (AGameStateBase* CurrentGameState = GetGameState<AGameStateBase>())
	{
		int32 PlayerCount = CurrentGameState->PlayerArray.Num();
		UE_LOG(LogTemp, Warning, TEXT("Current Player Count: %d"), PlayerCount);
	}
}

void AMyGameModeBase::InitializeGame()
{
	// 서버 카드 초기화 (1-9 중 3개 랜덤)
	CardDeck.Empty();
	for (int32 i = 1; i <= 9; i++)
	{
		CardDeck.Add(i);
	}

	// 서버 카드 3개 선택
	ServerCards.Empty();
	for (int32 i = 0; i < 3; i++)
	{
		int32 RandomIndex = FMath::RandRange(0, CardDeck.Num() - 1);
		ServerCards.Add(CardDeck[RandomIndex]);
		CardDeck.RemoveAt(RandomIndex);
	}

	// 서버 카드 로그 출력 추가
	UE_LOG(LogTemp, Warning, TEXT("Server Cards Generated: %d, %d, %d"),
		ServerCards[0], ServerCards[1], ServerCards[2]);

	OnGameStart();
}

void AMyGameModeBase::ComparePlayerCards(AMyPlayerState* Player)
{
	if (!Player || Player->PlayerCards.Num() != 3)
		return;

	// 스트라이크(S), 볼(B), 아웃(OUT) 계산
	int32 Strikes = 0;
	int32 Balls = 0;

	// 자리수와 값 비교 로직
	for (int32 i = 0; i < 3; i++)
	{
		if (Player->PlayerCards[i] == ServerCards[i])
		{
			Strikes++;
		}
		else if (ServerCards.Contains(Player->PlayerCards[i]))
		{
			Balls++;
		}
	}

	// UI에 결과 표시
	if (Strikes == 3)
	{
		BroadcastGameResult(Strikes, Balls, TEXT("WIN"));
		Player->bIsWinner = true;
		OnPlayerWin(Player);
		EndGame();
	}
	else if (Strikes == 0 && Balls == 0)
	{
		BroadcastGameResult(Strikes, Balls, TEXT("OUT"));
		Player->CurrentHitCount++;
		if (Player->CurrentHitCount >= 3)
		{
			OnPlayerOut(Player);
			RemovePlayer(Player);
		}
	}
	else
	{
		BroadcastGameResult(Strikes, Balls, TEXT("CONTINUE"));
	}
}

void AMyGameModeBase::BroadcastGameResult(int32 Strikes, int32 Balls, const FString& ResultType)
{
	UE_LOG(LogTemp, Warning, TEXT("Game Result: %dS %dB - %s"), Strikes, Balls, *ResultType);

	// 모든 플레이어 컨트롤러의 UI 업데이트
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (APlayerController* PC = Iterator->Get())
		{
			// UI 위젯 업데이트 로직 (추후 구현)
		}
	}
}

void AMyGameModeBase::BroadcastEndGameMessage(const FString& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("Broadcasting End Game Message: %s"), *Message);

	// 모든 플레이어에게 종료 메시지 전달 (추후 구현)
}

void AMyGameModeBase::DealCards()
{
	// 플레이어들에게 카드 배분 로직
}

void AMyGameModeBase::ServerCompareCards_Implementation()
{
	// 모든 플레이어의 카드 비교
	for (APlayerState* PlayerState : GetGameState<AGameStateBase>()->PlayerArray)
	{
		AMyPlayerState* CardPlayerState = Cast<AMyPlayerState>(PlayerState);
		if (CardPlayerState && !CardPlayerState->bIsWinner)
		{
			ComparePlayerCards(CardPlayerState);
		}
	}
}

void AMyGameModeBase::CheckWinCondition()
{
	// 승리 조건 체크 로직
	int32 ActivePlayers = 0;
	AMyPlayerState* WinnerCandidate = nullptr;

	if (AGameStateBase* CurrentGameState = GetGameState<AGameStateBase>())
	{
		for (APlayerState* PS : CurrentGameState->PlayerArray)
		{
			if (AMyPlayerState* CardPS = Cast<AMyPlayerState>(PS))
			{
				if (CardPS->bIsWinner)
				{
					EndGame();
					return;
				}

				if (CardPS->CurrentHitCount < 3)
				{
					ActivePlayers++;
					WinnerCandidate = CardPS;
				}
			}
		}
	}

	// 마지막 1명만 남으면 승리
	if (ActivePlayers == 1 && WinnerCandidate)
	{
		WinnerCandidate->bIsWinner = true;
		OnPlayerWin(WinnerCandidate);
		EndGame();
	}
	// 모든 플레이어가 탈락하면 무승부
	else if (ActivePlayers == 0)
	{
		EndGame();
	}
}

void AMyGameModeBase::RemovePlayer(AMyPlayerState* Player)
{
	// 플레이어 제거 로직
	if (Player)
	{
		// 플레이어를 비활성화 상태로 설정
		Player->CurrentHitCount = 3; // 아웃 상태로 마킹

		// 플레이어의 Pawn 제거 (선택사항)
		if (APlayerController* PC = Cast<APlayerController>(Player->GetOwner()))
		{
			if (APawn* PlayerPawn = PC->GetPawn())
			{
				PlayerPawn->Destroy();
			}
		}

		// 승리 조건 재확인
		CheckWinCondition();

		UE_LOG(LogTemp, Warning, TEXT("Player %s has been removed from the game"),
			*Player->GetPlayerName());
	}
}

void AMyGameModeBase::EndGame()
{
	// 게임 종료 로직
	UE_LOG(LogTemp, Warning, TEXT("Game Ended!"));

	FString EndMessage;
	bool bHasWinner = false;

	if (AGameStateBase* CurrentGameState = GetGameState<AGameStateBase>())
	{
		for (APlayerState* PS : CurrentGameState->PlayerArray)
		{
			if (AMyPlayerState* CardPS = Cast<AMyPlayerState>(PS))
			{
				if (CardPS->bIsWinner)
				{
					EndMessage = FString::Printf(TEXT("%s 승리!"), *CardPS->GetPlayerName());
					bHasWinner = true;
					UE_LOG(LogTemp, Warning, TEXT("Winner: %s"), *CardPS->GetPlayerName());
					break;
				}
			}
		}
	}

	if (!bHasWinner)
	{
		EndMessage = TEXT("무승부! 모든 플레이어가 탈락했습니다.");
	}

	// UI에 종료 메시지 표시
	BroadcastEndGameMessage(EndMessage);

	// 게임 재시작 타이머 설정 (10초 후)
	GetWorldTimerManager().SetTimer(
		RestartGameTimerHandle,
		this,
		&AMyGameModeBase::RestartGame,
		10.0f,
		false
	);
}

void AMyGameModeBase::RestartGame()
{
	// 모든 플레이어 상태 초기화
	if (AGameStateBase* CurrentGameState = GetGameState<AGameStateBase>())
	{
		for (APlayerState* PS : CurrentGameState->PlayerArray)
		{
			if (AMyPlayerState* CardPS = Cast<AMyPlayerState>(PS))
			{
				CardPS->CurrentHitCount = 0;
				CardPS->bIsWinner = false;
				CardPS->PlayerCards.Empty();
			}
		}
	}

	// 채팅 히스토리 초기화
	ChatHistory.Empty();

	// 타이머 핸들 클리어
	GetWorldTimerManager().ClearTimer(RestartGameTimerHandle);

	// 새 게임 시작
	InitializeGame();

	UE_LOG(LogTemp, Warning, TEXT("Game Restarted!"));
}

// 채팅 시스템 구현
void AMyGameModeBase::ServerSendChatMessage_Implementation(const FString& PlayerName, const FString& Message)
{
	// 채팅 히스토리에 저장
	ChatHistory.Add(FChatMessage(PlayerName, Message));

	// 모든 클라이언트에게 전송
	MulticastReceiveChatMessage(PlayerName, Message);

	UE_LOG(LogTemp, Warning, TEXT("Chat: [%s] %s"), *PlayerName, *Message);
}

void AMyGameModeBase::MulticastReceiveChatMessage_Implementation(const FString& PlayerName, const FString& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("Received Chat: [%s] %s"), *PlayerName, *Message);

	// 모든 플레이어 컨트롤러의 UI 업데이트
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (APlayerController* PC = Iterator->Get())
		{
			// UI 위젯 찾아서 채팅 메시지 추가 (추후 구현)
		}
	}
}