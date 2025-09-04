// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GCPawn.h"
#include "GameChat.h"

void AGCPawn::BeginPlay()
{
	Super::BeginPlay();

	FString NetRoleString = GameChatFunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("GCPawn::BeginPlay() %s [%s]"), *GameChatFunctionLibrary::GetNetModeString(this), *NetRoleString);
	GameChatFunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}

void AGCPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	FString NetRoleString = GameChatFunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("GCPawn::PossessedBy() %s [%s]"), *GameChatFunctionLibrary::GetNetModeString(this), *NetRoleString);
	GameChatFunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}