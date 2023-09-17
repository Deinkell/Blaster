// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterHUD.h"
#include "CharacterOverlay.h"
#include "GameFramework/PlayerController.h"
#include "Announcement.h"
#include "ElimAnnouncement.h"
#include "Components/HorizontalBox.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"

void ABlasterHUD::BeginPlay()
{
    Super::BeginPlay();
}

void ABlasterHUD::AddCharacterOverlay()
{
    APlayerController* PlayerController =  GetOwningPlayerController();
    if(PlayerController && CharacterOverlayClass)
    {
        CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);
        CharacterOverlay->AddToViewport();
    }
}

void ABlasterHUD::AddAnnouncement()
{   
    APlayerController* PlayerController =  GetOwningPlayerController();
    if(PlayerController && AnnouncementClass)
    {
        Announcement = CreateWidget<UAnnouncement>(PlayerController, AnnouncementClass);
        Announcement->AddToViewport();
    }
}

void ABlasterHUD::AddElimAnnouncement(FString Attacker, FString Victim)
{
    OwingPlayer = OwingPlayer == nullptr ? GetOwningPlayerController() : OwingPlayer;
    if(OwingPlayer && ElimAnnouncementClass)
    {
        UElimAnnouncement* ElimAnnouncementWidget = CreateWidget<UElimAnnouncement>(OwingPlayer, ElimAnnouncementClass);
        if(ElimAnnouncementWidget)
        {
            ElimAnnouncementWidget->SetElimAnnouncementText(Attacker, Victim);
            ElimAnnouncementWidget->AddToViewport();
            for(auto Msg : ElimMessages)
            {
                if(Msg && Msg->AnnouncementBox)
                {
                    UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Msg->AnnouncementBox);
                    if(CanvasSlot)
                    {
                        FVector2D Position = CanvasSlot->GetPosition();
                        FVector2D NewPosition(CanvasSlot->GetPosition().X, Position.Y - CanvasSlot->GetSize().Y);
                        CanvasSlot->SetPosition(NewPosition);
                    }
                }
            }
            
            ElimMessages.Add(ElimAnnouncementWidget);

            FTimerHandle ElimMsgTimer;
            FTimerDelegate ElimMsgDelegate;
            ElimMsgDelegate.BindUFunction(this, FName("ElimAnnouncementTimerFinished"), ElimAnnouncementWidget);
            GetWorldTimerManager().SetTimer(ElimMsgTimer, ElimMsgDelegate, ElimAnnouncementTime, false);
        }
    }
}

void ABlasterHUD::ElimAnnouncementTimerFinished(UElimAnnouncement* MsgToRemove)
{
    if(MsgToRemove)   
    {
        MsgToRemove->RemoveFromParent();
    }
}

void ABlasterHUD::DrawHUD()
{
    Super::DrawHUD();    

    FVector2D ViewportSize;
    if(GEngine)
    {       
        GEngine->GameViewport->GetViewportSize(ViewportSize);
        const FVector2D ViewprtCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

        float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrosshairsSpread;

        if(HUDPackage.CrosshairsCenter)
        {           
            FVector2D Spread(0.f, 0.f);
            DrawCorsshair(HUDPackage.CrosshairsCenter, ViewprtCenter, Spread, HUDPackage.CrosshairsColor);
        }
        if(HUDPackage.CrosshairsLeft)
        {
            FVector2D Spread(-SpreadScaled, 0.f);
            DrawCorsshair(HUDPackage.CrosshairsLeft, ViewprtCenter, Spread, HUDPackage.CrosshairsColor);
        }
        if(HUDPackage.CrosshairsRight)
        {
            FVector2D Spread(SpreadScaled, 0.f);
            DrawCorsshair(HUDPackage.CrosshairsRight, ViewprtCenter, Spread, HUDPackage.CrosshairsColor);
        }
        if(HUDPackage.CrosshairsTop)
        {
            FVector2D Spread(0.f, -SpreadScaled);
            DrawCorsshair(HUDPackage.CrosshairsTop, ViewprtCenter, Spread, HUDPackage.CrosshairsColor);
        }
        if(HUDPackage.CrosshairsBottom)
        {
            FVector2D Spread(0.f, SpreadScaled);
            DrawCorsshair(HUDPackage.CrosshairsBottom, ViewprtCenter, Spread, HUDPackage.CrosshairsColor);
        }
    }
}

void ABlasterHUD::DrawCorsshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairColor)
{
    const float TextureWidth = Texture->GetSizeX();
    const float TextureHeight = Texture->GetSizeY();
    const FVector2D TextureDrawPoint(ViewportCenter.X - (TextureWidth / 2.f) + Spread.X,
                       ViewportCenter.Y - (TextureHeight / 2.f) + Spread.Y);

    DrawTexture(Texture, TextureDrawPoint.X, TextureDrawPoint.Y, TextureWidth, TextureHeight, 
                    0.f, 0.f, 1.f, 1.f, CrosshairColor);   
}