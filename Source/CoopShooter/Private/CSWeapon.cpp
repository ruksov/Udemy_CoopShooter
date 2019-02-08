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
#include "Net/UnrealNetwork.h"

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
    SetReplicates(true);

    // 
    // UE reduces network update tick rate if the actor
    // doesn't very often changes. So we change this minimal
    // reduce value to 33 fps (by default it was 2 fps).
    //
    NetUpdateFrequency = 66.f;
    MinNetUpdateFrequency = 33.f;
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
    if (Role != ROLE_Authority)
    {
        ServerFire();
    }

    /// This code will be ran only on server.
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

        FVector ImpactPoint = EndTrace;
        EPhysicalSurface SurfaceType = SurfaceType_Default;

        FHitResult Hit;
        if (GetWorld()->LineTraceSingleByChannel(Hit
            , EyeLocation
            , EndTrace
            , collision_channel::gWeapon
            , QueryParams))
        {
            // True if we block something
            // Do damage logic

            SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
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

            ImpactPoint = Hit.ImpactPoint;

            PlayImpactEffects(SurfaceType, ImpactPoint);
        }

        PlayFireEffects(ImpactPoint);

        if (Role == ROLE_Authority)
        {
            HitScanTrace.TraceTo = ImpactPoint;
            HitScanTrace.SurfaceType = SurfaceType;

            // Change this field to prevent bug with not replicating shoots, which were made to the same place.
            // Unreal call OnRep_* functions only if their underlying feild was changed.
            ++HitScanTrace.AlwaysChangedField;
        }

        // Save time in seconds from last fire
        m_lastFireTime = GetWorld()->GetTimeSeconds();
    }
}

void ACSWeapon::ServerFire_Implementation()
{
    Fire();
}

bool ACSWeapon::ServerFire_Validate()
{
    return true;
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

void ACSWeapon::PlayImpactEffects(EPhysicalSurface SurfaceType, const FVector & TraceTo)
{
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
        FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

        FVector ShotDiraction = TraceTo - MuzzleLocation;
        ShotDiraction.Normalize();

        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedImpactEffect, TraceTo, ShotDiraction.Rotation());
    }
}

void ACSWeapon::OnRep_HitScanTrace()
{
    // Play cosmetic FX
    PlayFireEffects(HitScanTrace.TraceTo);
    PlayImpactEffects(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}

void ACSWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(ACSWeapon, HitScanTrace, COND_SkipOwner);
}