// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GCGameStateBase.h"

#include "Kismet/GameplayStatics.h"
#include "Player/GCPlayerController.h"

void AGCGameStateBase::MulticastRPCBroadcastLoginMessage_Implementation(const FString& InNameString)
{
	if (HasAuthority() == false)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsValid(PC) == true)
		{
			AGCPlayerController* GCPC = Cast<AGCPlayerController>(PC);
			if (IsValid(GCPC) == true)
			{
				FString NotificationString = InNameString + TEXT(" has joined the game.");
				GCPC->PrintChatMessageString(NotificationString);
			}
		}
	}
}
