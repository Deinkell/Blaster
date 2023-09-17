// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Projectile.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
    Super::Fire(HitTarget);
  
    APawn* InstigatorPawn = Cast<APawn>(GetOwner());
    const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
    UWorld* World = GetWorld();
    if(MuzzleFlashSocket && World)
    {
        FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
        FVector ToTarget = HitTarget - SocketTransform.GetLocation();
        FRotator TargetRotation = ToTarget.Rotation();

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = GetOwner();
        SpawnParams.Instigator = InstigatorPawn;

        AProjectile* SpawnProjectile = nullptr;
        if(bUseServerSideRewind)
        {   
            if(InstigatorPawn->HasAuthority()) //server
            {
                if(InstigatorPawn->IsLocallyControlled()) //server, host : use replicated projectile
                {
                    SpawnProjectile = World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
                    SpawnProjectile->bUseServerSideRewind = false;
                    SpawnProjectile->Damage = Damage;
                    SpawnProjectile->HeadShotDamage = HeadShotDamage;                   
                }
                else //server, not locally controlled : spawn non-replicated projectiles, no ssr
                {
                    SpawnProjectile = World->SpawnActor<AProjectile>(ServerSidRewindProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
                    SpawnProjectile->bUseServerSideRewind = true;
                }
            }
            else //client, using ssr
            {
                if(InstigatorPawn->IsLocallyControlled()) //client, locally controlled - Spawn non-replicated projectile, use ssr
                {
                    SpawnProjectile = World->SpawnActor<AProjectile>(ServerSidRewindProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
                    SpawnProjectile->bUseServerSideRewind = true;
                    SpawnProjectile->TraceStart = SocketTransform.GetLocation();
                    SpawnProjectile->InitialVelocity = SpawnProjectile->GetActorForwardVector() * SpawnProjectile->InitialSpeed;
                }
                else //client not locally controlled - spawn non-replicated projectile, no ssr 
                {
                    SpawnProjectile = World->SpawnActor<AProjectile>(ServerSidRewindProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
                    SpawnProjectile->bUseServerSideRewind = false;
                }
            }
        }
        else // weapon not using ssr
        {
            if(InstigatorPawn->HasAuthority())
            {
                SpawnProjectile = World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
                SpawnProjectile->bUseServerSideRewind = false;
                SpawnProjectile->Damage = Damage;
                SpawnProjectile->HeadShotDamage = HeadShotDamage;               
            }
        }       
    }
}
