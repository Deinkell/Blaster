// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

UENUM(BlueprintType)
enum class ETeam : uint8
{
	ET_RedTeam UMETA(DisplayName = "RedTeam"),
	ET_BlueTeam UMETA(DisplayName = "BlueTeam"),
	ET_NoTeam UMETA(DisplayName = "NoTeam"),

	ET_MAX UMETA(DisplayName = "DefaultMAX")
};