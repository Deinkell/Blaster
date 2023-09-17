// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class BLASTER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:		
	AProjectile();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

	//Used with Server-side rewind

	bool bUseServerSideRewind = false;
	FVector_NetQuantize TraceStart;
	FVector_NetQuantize100 InitialVelocity;

	UPROPERTY(EditAnywhere)
	float InitialSpeed = 8000;

	UPROPERTY(EditAnywhere)
	float Damage = 20.f;
	
	UPROPERTY(EditAnywhere)
	float HeadShotDamage = 40.f;

protected:
	virtual void BeginPlay() override;
	void StartDestroyTimer();
	void DestroyTimerFinished();
	void SpawnTrailSystem();
	void ExplodeDamage();

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditAnywhere)
	class UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere)
	class USoundCue* ImpactSound;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* TrailSystem;

	UPROPERTY()
	class UNiagaraComponent* TrailSystemComponent;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(EditAnywhere)
	float DamageInnerRadius = 200;
	
	UPROPERTY(EditAnywhere)
	float DamageOuterRadius = 500;

private:

	UPROPERTY(EditAnywhere)
	class UParticleSystem* Tracer;

	UPROPERTY()
	class UParticleSystemComponent* TracerComponent;	

	FTimerHandle DestroyTimer;

	UPROPERTY(EditAnywhere)
	float DestroyTime = 3.f;

public:	

};
