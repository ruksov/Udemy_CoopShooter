// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CSWeapon.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;
class UCameraShake;

UCLASS()
class COOPSHOOTER_API ACSWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACSWeapon();

    void StartFire();

    void StopFire();

protected:
    // Called when the game starts or when spawned
    void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void Fire();

    void PlayFireEffects(const FVector& tracerEndPoint);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    USkeletalMeshComponent* MeshComp;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    TSubclassOf<UDamageType> DamageType;

    // Muzzle effect
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Muzzle Effect")
    FName MuzzleSocketName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Muzzle Effect")
    UParticleSystem* MuzzleEffect;

    /// Impact effect
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Impact Effect")
    UParticleSystem* ImpactDefaultEffect;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Impact Effect")
    UParticleSystem* ImpactFleshEffect;

    /// Tracer effect
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Tracer Effect")
    FName TracerTargetName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Tracer Effect")
    UParticleSystem* TracerEffect;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Tracer Effect")
    TSubclassOf<UCameraShake> FireCameraShake;

    /// Damage config
    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Damage", meta=(DisplayName="Base Damage"))
    float m_baseDamage; 

    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Damage", meta=(DisplayName="Damage Vulnarable Multiplier"))
    float m_vulnerableMultiplier;

    /// Fire config
    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Fire", meta=(DisplayName="Bullets Per Minute"))
    uint32 m_bulletsPerMinute;

private:
    float m_fireRate;
    float m_lastFireTime;
    FTimerHandle m_fireTimer;
};
