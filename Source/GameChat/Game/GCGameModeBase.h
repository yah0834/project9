#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GCGameModeBase.generated.h"

class AGCPlayerController;
/**
 * 
 */
UCLASS()
class GAMECHAT_API AGCGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void OnPostLogin(AController* NewPlayer) override;

	FString GenerateSecretNumber();

	bool IsGuessNumberString(const FString& InNumberString);

	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);

	virtual void BeginPlay() override;

	void PrintChatMessageString(AGCPlayerController* InChattingPlayerController, const FString& InChatMessageString);

	void IncreaseGuessCount(AGCPlayerController* InChattingPlayerController);

	void ResetGame();

	void JudgeGame(AGCPlayerController* InChattingPlayerController, int InStrikeCount);

protected:
	FString SecretNumberString;

	TArray<TObjectPtr<AGCPlayerController>> AllPlayerControllers;
};
