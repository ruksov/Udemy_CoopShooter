// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CSWeapon.h"
#include "CSProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class COOPSHOOTER_API ACSProjectileWeapon : public ACSWeapon
{
	GENERATED_BODY()

public:
    void Fire() override;

protected:
    // Projectile class to spawn
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<AActor> ProjectileClass;
	
};
