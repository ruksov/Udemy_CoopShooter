// Fill out your copyright notice in the Description page of Project Settings.

#include "CSWeapon.h"

#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ACSWeapon::ACSWeapon()
    : MeshComp(CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp")))
    , MuzzleSocketName("MuzzleSocket")
    , TracerTargetName("Target")
{
    RootComponent = MeshComp;
}

void ACSWeapon::Fire()
{
    AActor* Owner = GetOwner();
    if (Owner)
    {
        FVector EyeLocation;
        FRotator EyeRotation;
        Owner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

        FVector ShootDirection = EyeRotation.Vector();
        FVector EndTrace = EyeLocation + (ShootDirection * 10000);

        FCollisionQueryParams QueryParams;
        QueryParams.AddIgnoredActor(Owner);
        QueryParams.AddIgnoredActor(this);
        QueryParams.bTraceComplex = true;

        FHitResult Hit;
        if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation
            , EndTrace
            , ECollisionChannel::ECC_Visibility
            , QueryParams))
        {
            // True if we block something
            // Do damage logic

            AActor* HitActor = Hit.GetActor();
            UGameplayStatics::ApplyPointDamage(HitActor
                , 20.f
                , ShootDirection
                , Hit
                , Owner->GetInstigatorController()
                , this
                , DamageType);

            if (ImpactEffect)
            {
                UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
            }
        }

        PlayFireEffects(Hit.IsValidBlockingHit() ? Hit.ImpactPoint : EndTrace);
    }
}

void ACSWeapon::PlayFireEffects(const FVector& TraceEndPoint)
{
    if (MuzzleEffect)
    {
        UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
    }

    if (TracerEffect)
    {
        FVector muzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
        auto tracerEffectComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, muzzleLocation);
        if (tracerEffectComp)
        {
            tracerEffectComp->SetVectorParameter(TracerTargetName, TraceEndPoint);
        }
    }

    auto MyOwner = Cast<APawn>(GetOwner());
    if (MyOwner)
    {
        auto PC = Cast<APlayerController>(MyOwner->GetController());
        if (PC)
        {
            PC->ClientPlayCameraShake(FireCameraShake);
        }
    }
}