// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#define TRACE_LENGTH 80000.f

#define CUSTOM_DEPTH_PURPLE 250
#define CUSTOM_DEPTH_BLUE 251
#define CUSTOM_DEPTH_TAN 252

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_AssaultRifle UMETA(DisplayName="Assault Rifle")	,
	EWT_RocketLauncher UMETA(DisplayName="Rocket Launcher"),
	EWT_Pistol UMETA(DisplayName="Pistol"),
	EWT_SubmachineGun UMETA(DisplayName="SubmachinGun"),
	EWT_Shotgun UMETA(DisplayName="Shotgun"),
	EWT_SniperRifle UMETA(DisplayName="Sniper Rifle"),
	EWT_GrenadeLauncher UMETA(DisplayName="GrenadeLauncher"),
	EWT_Flag UMETA(DisplayName="Flag"),
	
	EWT_MAX UMETA(DisplayName="DefaultMAX")	

};