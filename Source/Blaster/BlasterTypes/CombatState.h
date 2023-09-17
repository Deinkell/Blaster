// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied UMETA(DisplayName="Unoccuipied"),
	ECS_Reloading UMETA(DisplayName="Reloading"),
	ECS_ThrowingGrenade UMETA(DisplayName="Throwing Grenade"),
	ECS_SwappingWeapons UMETA(DisplayName="Swapping Weapons"),

	ECS_MAX UMETA(DisplayName="DefaultMAX")
};