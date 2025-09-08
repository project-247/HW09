#include "MyPlayerState.h"
#include "Net/UnrealNetwork.h"

AMyPlayerState::AMyPlayerState()
{
	CurrentHitCount = 0;
	bIsWinner = false;
	PlayerCards.Reserve(3);
}

void AMyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyPlayerState, PlayerCards);
	DOREPLIFETIME(AMyPlayerState, CurrentHitCount);
	DOREPLIFETIME(AMyPlayerState, bIsWinner);
}

void AMyPlayerState::SubmitCards(const TArray<int32>& Cards)
{
	if (Cards.Num() == 3)
	{
		PlayerCards = Cards;
	}
}

FString AMyPlayerState::GetPlayerStatus() const
{
	if (bIsWinner)
		return TEXT("WINNER");
	else if (CurrentHitCount >= 3)
		return TEXT("OUT");
	else
		return FString::Printf(TEXT("시도: %d/3"), CurrentHitCount);
}