// Fill out your copyright notice in the Description page of Project Settings.

#include "CSProjectileWeapon.h"

#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"

void ACSProjectileWeapon::Fire()
{
    AActor* owner = GetOwner();
    if (owner && ProjectileClass)
    {
        FVector eyeLocation;
        FRotator eyeRotation;
        owner->GetActorEyesViewPoint(eyeLocation, eyeRotation);

        auto muzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

        //Set Spawn Collision Handling Override
        FActorSpawnParameters actorSpawnParams;
        actorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        // spawn the projectile at the muzzle
        GetWorld()->SpawnActor<AActor>(ProjectileClass, muzzleLocation, eyeRotation, actorSpawnParams);
    }
}