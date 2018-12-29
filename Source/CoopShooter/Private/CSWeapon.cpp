// Fill out your copyright notice in the Description page of Project Settings.

#include "CSWeapon.h"

#include "CoopShooter.h"

#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TimerManager.h"

// Sets default values
ACSWeapon::ACSWeapon()
    : MeshComp(CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp")))
    , MuzzleSocketName("MuzzleSocket")
    , TracerTargetName("Target")
    , m_baseDamage(20.f)
    , m_vulnerableMultiplier(4.f)
    , m_bulletsPerMinute(10)
    , m_fireRate(0.f)
    , m_lastFireTime(0.f)
{
    RootComponent = MeshComp;
}

void ACSWeapon::BeginPlay()
{
    Super::BeginPlay();
    m_fireRate = 60.f / m_bulletsPerMinute;
}

void ACSWeapon::StartFire()
{
    float now = GetWorld()->GetTimeSeconds();
    if (now - m_lastFireTime < m_fireRate)
    {
        return;
    }

    GetWorld()->GetTimerManager().SetTimer(m_fireTimer, this, &ACSWeapon::Fire, m_fireRate, true, 0.f);
}

void ACSWeapon::StopFire()
{
    GetWorld()->GetTimerManager().ClearTimer(m_fireTimer);
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
        QueryParams.bReturnPhysicalMaterial = true;

        FHitResult Hit;
        if (GetWorld()->LineTraceSingleByChannel(Hit
            , EyeLocation
            , EndTrace
            , collision_channel::gWeapon
            , QueryParams))
        {
            // True if we block something
            // Do damage logic

            auto SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
            float CurrentDamage = m_baseDamage;
            if (SurfaceType == surface::gFleshVulnerable)
            {
                CurrentDamage *= m_vulnerableMultiplier;
            }

            AActor* HitActor = Hit.GetActor();
            UGameplayStatics::ApplyPointDamage(HitActor
                , CurrentDamage
                , ShootDirection
                , Hit
                , Owner->GetInstigatorController()
                , this
                , DamageType);

            /// Select impact effect according to surface type
            UParticleSystem* SelectedImpactEffect = nullptr;
            switch (SurfaceType)
            {
            case surface::gFleshDefault:
            case surface::gFleshVulnerable:
                SelectedImpactEffect = ImpactFleshEffect;
                break;

            default:
                SelectedImpactEffect = ImpactDefaultEffect;
                break;
            }

            if (SelectedImpactEffect)
            {
                UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
            }
        }

        PlayFireEffects(Hit.IsValidBlockingHit() ? Hit.ImpactPoint : EndTrace);

        // Save time in seconds from last fire
        m_lastFireTime = GetWorld()->GetTimeSeconds();
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