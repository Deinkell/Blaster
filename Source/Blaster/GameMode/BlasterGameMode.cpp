// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Blaster/GameState/BlasterGameState.h"

namespace MatchState
{
    const FName Cooldown = FName("Cooldown");
}

ABlasterGameMode::ABlasterGameMode()
{
    bDelayedStart = true;
}

void ABlasterGameMode::BeginPlay()
{
    Super::BeginPlay();

    LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void ABlasterGameMode::OnMatchStateSet() 
{
    Super::OnMatchStateSet();

    for(FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        ABlasterPlayerController* BlasterPlayer = Cast<ABlasterPlayerController>(*It);
        if(BlasterPlayer)
        {
            BlasterPlayer->OnMatchStateSet(MatchState, bTeamsMatch);
        }
    }
}

void ABlasterGameMode::Tick(float DeltaTime) 
{
    Super::Tick(DeltaTime);

    if(MatchState == MatchState::WaitingToStart)
    {
        CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
        if(CountdownTime <= 0.f)
        {
            StartMatch();
        }
    }
    else if(MatchState == MatchState::InProgress)
    {
        CountdownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime; 
        if(CountdownTime <= 0.f)
        {
            SetMatchState(MatchState::Cooldown);
        }
    }
    else if(MatchState == MatchState::Cooldown)
    {
        CountdownTime = CooldownTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
        if(CountdownTime <= 0.f)
        {
            RestartGame();
        }
    }
}

float ABlasterGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
    return BaseDamage;
}

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* ElimmedCharacter, ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController)
{
    if (AttackerController == nullptr || AttackerController->PlayerState == nullptr) return;
	if (VictimController == nullptr || VictimController->PlayerState == nullptr) return;

    ABlasterPlayerState* AttackerPlayerState = AttackerController ? Cast<ABlasterPlayerState>(AttackerController->PlayerState) : nullptr;
    ABlasterPlayerState* VictimPlayerState = VictimController ? Cast<ABlasterPlayerState>(VictimController->PlayerState) : nullptr;

    ABlasterGameState* BlasterGamestate = GetGameState<ABlasterGameState>();

    if(AttackerPlayerState && AttackerPlayerState != VictimPlayerState && BlasterGamestate)
    {
        TArray<ABlasterPlayerState*> PlayerCurrentlyInTheLead;
        for(auto LeadPlayer : BlasterGamestate->TopScoringPlayers)
        {
            PlayerCurrentlyInTheLead.Add(LeadPlayer);
        }

        AttackerPlayerState->AddToScore(1.f);
        BlasterGamestate->UpdateTopScore(AttackerPlayerState);
        if(BlasterGamestate->TopScoringPlayers.Contains(AttackerPlayerState))
        {
            ABlasterCharacter* Leader = Cast<ABlasterCharacter>(AttackerPlayerState->GetPawn());
            if(Leader)
            {
                Leader->MulticastGainedTheLead();
            }
        }

        for(int32 i = 0; i < PlayerCurrentlyInTheLead.Num(); i++)
        {
            if(!BlasterGamestate->TopScoringPlayers.Contains(PlayerCurrentlyInTheLead[i]))
            {
                ABlasterCharacter* Loser = Cast<ABlasterCharacter>(PlayerCurrentlyInTheLead[i]->GetPawn());
                if(Loser)
                {
                    Loser->MulticastLostTheLead();
                }
            }
        }
    }

    if(VictimPlayerState)
    {
        VictimPlayerState->AddToDefeats(1);
    }

    if(ElimmedCharacter)
    {
        ElimmedCharacter->Elim(false);
    }

    for(FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        ABlasterPlayerController* BlasterPlayer = Cast<ABlasterPlayerController>(*It);
        if(BlasterPlayer && AttackerPlayerState && VictimPlayerState)
        {
            BlasterPlayer->BroadcastElim(AttackerPlayerState, VictimPlayerState);
        }
    }
}

void ABlasterGameMode::RequestRespawn(ACharacter* ElimCharacter, AController* ElimedController)
{
    if(ElimCharacter)
    {
        ElimCharacter->Reset();
        ElimCharacter->Destroy();
    }

    if(ElimedController)
    {
        TArray<AActor*> PlayerStarts;
        UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
        int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
        RestartPlayerAtPlayerStart(ElimedController, PlayerStarts[Selection]);
    }
}

void ABlasterGameMode::PlayerLeftGame(ABlasterPlayerState* PlayerLeaving)
{
    if(PlayerLeaving == nullptr) return;

    ABlasterGameState* BlasterGamestate = GetGameState<ABlasterGameState>();
    if(BlasterGamestate && BlasterGamestate->TopScoringPlayers.Contains(PlayerLeaving))
    {
        BlasterGamestate->TopScoringPlayers.Remove(PlayerLeaving);
    }

    ABlasterCharacter* CharacterLeaving = Cast<ABlasterCharacter>(PlayerLeaving->GetPawn());
    if(CharacterLeaving)
    {
        CharacterLeaving->Elim(true);
    }
}