// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CSExplosiveActor.generated.h"

class UCSHealthComponent;
class USphereComponent;

UCLASS()
class COOPSHOOTER_API ACSExplosiveActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACSExplosiveActor();

protected:
    // Called when the game starts or when spawned
    void BeginPlay() override;
    
    UFUNCTION()
    void OnHealthChanged(UCSHealthComponent* OwningHealthComp,
            float Health,
            float DeltaHealth,
            const class UDamageType* DamageType,
            class AController* InstigatedBy,
            AActor* DamageCauser);

    virtual void Explose();

protected:
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* MeshComp;

    UPROPERTY(VisibleAnywhere)
    USphereComponent* SphereComp;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UCSHealthComponent* HealthComp;

    UPROPERTY(EditDefaultsOnly, Category = "Explose Config")
    float ExplosionStrength;

    UPROPERTY(EditDefaultsOnly, Category = "Explose Config")
    float SelfExplosionStrength;

    UPROPERTY(EditDefaultsOnly, Category = "Explose Config")
    float ExplosionDamage;

    UPROPERTY(EditDefaultsOnly, Category = "Explose Config")
    UMaterialInterface* ExplodedMaterial;

    UPROPERTY(EditDefaultsOnly, Category = "Explose Config")
    UParticleSystem* ExplosionEffect;

    UPROPERTY(EditDefaultsOnly, Category = "Explose Config")
    TSubclassOf<UDamageType> DamageType;

private:
    bool m_isExploded;
};
