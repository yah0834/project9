#include "GCGameModeBase.h"
#include "GCGameStateBase.h"
#include "Player/GCPlayerController.h"
#include "EngineUtils.h"
#include "Player/GCPlayerState.h"

void AGCGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
	/*AGCGameStateBase* GCGameStateBase = GetGameState<AGCGameStateBase>();
	if (IsValid(GCGameStateBase) == true)
	{
		GCGameStateBase->MulticastRPCBroadcastLoginMessage(TEXT("XXXXXXX"));
	}

	AGCPlayerController* GCPlayerController = Cast<AGCPlayerController>(NewPlayer);
	if (IsValid(GCPlayerController) == true)
	{
		AllPlayerControllers.Add(GCPlayerController);
	}*/

	AGCPlayerController* GCPlayerController = Cast<AGCPlayerController>(NewPlayer);
	if (IsValid(GCPlayerController) == true)
	{

		GCPlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server."));

		AllPlayerControllers.Add(GCPlayerController);

		AGCPlayerState* GCPS = GCPlayerController->GetPlayerState<AGCPlayerState>();
		if (IsValid(GCPS) == true)
		{
			GCPS->PlayerNameString = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());
		}

		AGCGameStateBase* GCGameStateBase = GetGameState<AGCGameStateBase>();
		if (IsValid(GCGameStateBase) == true)
		{
			GCGameStateBase->MulticastRPCBroadcastLoginMessage(GCPS->PlayerNameString);
		}
	}
}

// 비밀 숫자를 생성하는 함수 (예: "123" 처럼 1~9 중 중복 없는 3자리 숫자)
FString AGCGameModeBase::GenerateSecretNumber()
{
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; ++i)
	{
		Numbers.Add(i);
	}

	// 랜덤 시드 초기화 (현재 시간 기반)
	FMath::RandInit(FDateTime::Now().GetTicks());
	// 혹시 모를 0 이하 숫자 제거 (안전장치)
	Numbers = Numbers.FilterByPredicate([](int32 Num) { return Num > 0; });

	FString Result;
	// 3번 반복 → 3자리 숫자 생성
	for (int32 i = 0; i < 3; ++i)
	{
		// 랜덤 인덱스 뽑기
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		// 뽑은 숫자를 문자열에 추가
		Result.Append(FString::FromInt(Numbers[Index]));
		// 중복 방지를 위해 사용한 숫자 제거
		Numbers.RemoveAt(Index);
	}

	return Result;
}

// 입력된 문자열이 올바른 추측(Guess) 숫자인지 검사하는 함수
bool AGCGameModeBase::IsGuessNumberString(const FString& InNumberString)
{
	bool bCanPlay = false;

	do {
		// 반드시 길이가 3이어야 함
		if (InNumberString.Len() != 3)
		{
			break;
		}

		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits;// 고유한 숫자 저장용 Set
		for (TCHAR C : InNumberString)
		{
			// 숫자가 아니거나 '0'이면 잘못된 입력
			if (FChar::IsDigit(C) == false || C == '0')
			{
				bIsUnique = false;
				break;
			}
			// 고유성 체크: 같은 숫자가 2번 나오면 안 됨
			UniqueDigits.Add(C);
		}
		// 중복 또는 조건 위반이면 종료
		if (bIsUnique == false)
		{
			break;
		}
		// 여기까지 통과했으면 정상 입력
		bCanPlay = true;

	} while (false);

	return bCanPlay;
}

// 비밀 숫자와 플레이어가 추측한 숫자를 비교하여 결과 반환
// 예: Secret="123", Guess="135" → "1S1B"
FString AGCGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
	int32 StrikeCount = 0, BallCount = 0;

	for (int32 i = 0; i < 3; ++i)
	{
		// Strike: 같은 위치에 같은 숫자일 때
		if (InSecretNumberString[i] == InGuessNumberString[i])
		{
			StrikeCount++;
		}
		else
		{
			// Ball: 다른 위치에 있지만 숫자가 포함되어 있을 때
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
			if (InSecretNumberString.Contains(PlayerGuessChar))
			{
				BallCount++;
			}
		}
	}

	// Strike와 Ball이 모두 없으면 OUT
	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT");
	}

	// 예: 2 Strike, 1 Ball → "2S1B"
	return FString::Printf(TEXT("%dS%dB"), StrikeCount, BallCount);
}

void AGCGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	SecretNumberString = GenerateSecretNumber();
}

void AGCGameModeBase::PrintChatMessageString(AGCPlayerController* InChattingPlayerController, const FString& InChatMessageString)
{
	int Index = InChatMessageString.Len() - 3;
	FString GuessNumberString = InChatMessageString.RightChop(Index);
	if (IsGuessNumberString(GuessNumberString) == true)
	{
		FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);
		IncreaseGuessCount(InChattingPlayerController);

		for (TActorIterator<AGCPlayerController> It(GetWorld()); It; ++It)
		{
			AGCPlayerController* GCPlayerController = *It;
			if (IsValid(GCPlayerController) == true)
			{
				FString CombinedMessageString = InChatMessageString + TEXT(" -> ") + JudgeResultString;
				GCPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);

				int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
				JudgeGame(InChattingPlayerController, StrikeCount);
			}
		}
	}
	else
	{
		for (TActorIterator<AGCPlayerController> It(GetWorld()); It; ++It)
		{
			AGCPlayerController* GCPlayerController = *It;
			if (IsValid(GCPlayerController) == true)
			{
				GCPlayerController->ClientRPCPrintChatMessageString(InChatMessageString);
			}
		}
	}
}

void AGCGameModeBase::IncreaseGuessCount(AGCPlayerController* InChattingPlayerController)
{
	AGCPlayerState* GCPS = InChattingPlayerController->GetPlayerState<AGCPlayerState>();
	if (IsValid(GCPS) == true)
	{
		GCPS->CurrentGuessCount++;
	}
}

void AGCGameModeBase::ResetGame()
{
	SecretNumberString = GenerateSecretNumber();

	for (const auto& GCPlayerController : AllPlayerControllers)
	{
		AGCPlayerState* GCPS = GCPlayerController->GetPlayerState<AGCPlayerState>();
		if (IsValid(GCPS) == true)
		{
			GCPS->CurrentGuessCount = 0;
		}
	}
}

void AGCGameModeBase::JudgeGame(AGCPlayerController* InChattingPlayerController, int InStrikeCount)
{
	if (3 == InStrikeCount)
	{
		AGCPlayerState* GCPS = InChattingPlayerController->GetPlayerState<AGCPlayerState>();
		for (const auto& GCPlayerController : AllPlayerControllers)
		{
			if (IsValid(GCPS) == true)
			{
				FString CombinedMessageString = GCPS->PlayerNameString + TEXT(" has won the game.");
				GCPlayerController->NotificationText = FText::FromString(CombinedMessageString);

				ResetGame();
			}
		}
	}
	else
	{
		bool bIsDraw = true;
		for (const auto& GCPlayerController : AllPlayerControllers)
		{
			AGCPlayerState* GCPS = GCPlayerController->GetPlayerState<AGCPlayerState>();
			if (IsValid(GCPS) == true)
			{
				if (GCPS->CurrentGuessCount < GCPS->MaxGuessCount)
				{
					bIsDraw = false;
					break;
				}
			}
		}

		if (true == bIsDraw)
		{
			for (const auto& GCPlayerController : AllPlayerControllers)
			{
				GCPlayerController->NotificationText = FText::FromString(TEXT("Draw..."));

				ResetGame();
			}
		}
	}
}