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
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    RootComponent = MeshComp;
}

void ACSWeapon::Fire()
{
    AActor* owner = GetOwner();
    if (owner)
    {
        FVector eyeLocation;
        FRotator eyeRotation;
        owner->GetActorEyesViewPoint(eyeLocation, eyeRotation);

        FVector shootDirection = eyeRotation.Vector();
        FVector endTrace = eyeLocation + (shootDirection * 10000);

        // End point for tracer effect
        FVector tracerEndPoint = endTrace;

        FCollisionQueryParams queryParams;
        queryParams.AddIgnoredActor(owner);
        queryParams.AddIgnoredActor(this);
        queryParams.bTraceComplex = true;

        FHitResult hit;
        if (GetWorld()->LineTraceSingleByChannel(hit, eyeLocation
            , endTrace
            , ECollisionChannel::ECC_Visibility
            , queryParams))
        {
            // True if we block something
            // Do damage logic

            AActor* hitActor = hit.GetActor();
            UGameplayStatics::ApplyPointDamage(hitActor
                , 20.f
                , shootDirection
                , hit
                , owner->GetInstigatorController()
                , this
                , DamageType);

            if (ImpactEffect)
            {
                UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, hit.ImpactPoint, hit.ImpactNormal.Rotation());
            }

            tracerEndPoint = hit.ImpactPoint;
        }

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
                tracerEffectComp->SetVectorParameter(TracerTargetName, tracerEndPoint);
            }
        }
    }
}

// Called when the game starts or when spawned
void ACSWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACSWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

