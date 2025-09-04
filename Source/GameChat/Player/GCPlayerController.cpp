#include "Player/GCPlayerController.h"
#include "UI/GCChatInput.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameChat.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Game/GCGameModeBase.h"
#include "GCPlayerState.h"
#include "Net/UnrealNetwork.h"

AGCPlayerController::AGCPlayerController()
{
	bReplicates = true;
}

void AGCPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}

	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);

	if (IsValid(ChatInputWidgetClass) == true)
	{
		ChatInputWidgetInstance = CreateWidget<UGCChatInput>(this, ChatInputWidgetClass);
		if (IsValid(ChatInputWidgetInstance) == true)
		{
			ChatInputWidgetInstance->AddToViewport();
		}
	}

	if (IsValid(NotificationTextWidgetClass) == true)
	{
		NotificationTextWidgetInstance = CreateWidget<UUserWidget>(this, NotificationTextWidgetClass);
		if (IsValid(NotificationTextWidgetInstance) == true)
		{
			NotificationTextWidgetInstance->AddToViewport();
		}
	}
}

void AGCPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
	ChatMessageString = InChatMessageString;

	//PrintChatMessageString(ChatMessageString);
	if (IsLocalController() == true)
	{
		//ServerRPCPrintChatMessageString(InChatMessageString);

		AGCPlayerState* GCPS = GetPlayerState<AGCPlayerState>();
		if (IsValid(GCPS) == true)
		{
			//FString CombinedMessageString = GCPS->PlayerNameString + TEXT(": ") + InChatMessageString;
			FString CombinedMessageString = GCPS->GetPlayerInfoString() + TEXT(": ") + InChatMessageString;
			ServerRPCPrintChatMessageString(CombinedMessageString);
		}
	}
}

void AGCPlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
	//UKismetSystemLibrary::PrintString(this, ChatMessageString, true, true, FLinearColor::Red, 5.0f);

	GameChatFunctionLibrary::MyPrintString(this, InChatMessageString, 10.f);
}

void AGCPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	PrintChatMessageString(InChatMessageString);
}

void AGCPlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	/*for (TActorIterator<AGCPlayerController> It(GetWorld()); It; ++It)
	{
		AGCPlayerController* CXPlayerController = *It;
		if (IsValid(CXPlayerController) == true)
		{
			CXPlayerController->ClientRPCPrintChatMessageString(InChatMessageString);
		}
	}*/

	AGameModeBase* GM = UGameplayStatics::GetGameMode(this);
	if (IsValid(GM) == true)
	{
		AGCGameModeBase* GCGM = Cast<AGCGameModeBase>(GM);
		if (IsValid(GCGM) == true)
		{
			GCGM->PrintChatMessageString(this, InChatMessageString);
		}
	}
}

void AGCPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, NotificationText);
}
