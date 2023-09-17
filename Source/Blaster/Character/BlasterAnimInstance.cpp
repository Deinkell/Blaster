// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterAnimInstance.h"
#include "BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blaster/Weapon/Weapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Blaster/BlasterTypes/CombatState.h"

void UBlasterAnimInstance::NativeInitializeAnimation() 
{
    Super::NativeInitializeAnimation();

    BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaTime) 
{
    Super::NativeUpdateAnimation(DeltaTime);

    if(BlasterCharacter == nullptr)
    {
        BlasterCharacter = BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
    }

    if(BlasterCharacter == nullptr) return;

    FVector Velocity = BlasterCharacter->GetVelocity();
    Velocity.Z = 0.f;
    Speed = Velocity.Size();

    bIsInAir = BlasterCharacter->GetCharacterMovement()->IsFalling();
    bIsAccelerating = BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0 ? true : false;
    bWeaponEquipped = BlasterCharacter->IsWeaponEquipped();
    EquippedWeapon = BlasterCharacter->GetEquippedWeapon();
    bIsCrouch = BlasterCharacter->bIsCrouched;
    bAiming = BlasterCharacter->IsAiming();
    TurningInPlace = BlasterCharacter->GetTurningInPlace();
    bRotateRootBone = BlasterCharacter->ShouldRotateRootBone();
    bElimmed = BlasterCharacter->IsElimmed();
    bHoldingTheFlag = BlasterCharacter->IsHoldingTheFlag();

    FRotator AimRotation = BlasterCharacter->GetBaseAimRotation();
    FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(BlasterCharacter->GetVelocity());
    FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
    DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 6.f);
    YawOffset = DeltaRotation.Yaw;

    CharacterRotationLastFrame = CharacterRotation;
    CharacterRotation = BlasterCharacter->GetActorRotation();
    const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
    const float Target = Delta.Yaw / DeltaTime;
    const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);
    Lean = FMath::Clamp(Interp, -90.f, 90.f);

    AO_Yaw = BlasterCharacter->GetAO_Yaw();
    AO_Pitch = BlasterCharacter->GetAO_Pitch();

    if(bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && BlasterCharacter->GetMesh())
    {
        LefthandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
        FVector OutPosition;
        FRotator OutRotation;
        BlasterCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LefthandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
        LefthandTransform.SetLocation(OutPosition);
        LefthandTransform.SetRotation(FQuat(OutRotation));  

        if(BlasterCharacter->IsLocallyControlled())
        {
            FTransform MuzzleTipTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("MuzzleFlash"), ERelativeTransformSpace::RTS_World);
            FVector MuzzleX(FRotationMatrix(MuzzleTipTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X));

            bLocallyControlled = true;
            FTransform RightHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("hand_r"), ERelativeTransformSpace::RTS_World);
            FRotator LookAtRotation =UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - BlasterCharacter->GetHitTarget()));
            LookAtRotation.Yaw += 10;       
            RightHandRoation = FMath::RInterpTo(RightHandRoation, LookAtRotation, DeltaTime, 30.f);                
        }  
    }

    bUseFABRIK = BlasterCharacter->GetCombatState() == ECombatState::ECS_Unoccupied;
    bool bFABRIKoverride = BlasterCharacter->IsLocallyControlled() && 
                            BlasterCharacter->GetCombatState() != ECombatState::ECS_ThrowingGrenade && 
                            BlasterCharacter->bFinishedSwapping;
                           
    if(bFABRIKoverride)
    {
        bUseFABRIK = !(BlasterCharacter->IsLocallyReloading());
    }
    bUseAimOffset = BlasterCharacter->GetCombatState() == ECombatState::ECS_Unoccupied && !BlasterCharacter->GetDisableGameplay();
    bTransformRightHand = BlasterCharacter->GetCombatState() == ECombatState::ECS_Unoccupied && !BlasterCharacter->GetDisableGameplay();
}