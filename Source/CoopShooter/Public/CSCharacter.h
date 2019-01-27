// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CSCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ACSWeapon;
class UCSHealthComponent;

UCLASS()
class COOPSHOOTER_API ACSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACSCharacter();

public:
    // Called every frame
    void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // Returns CameraComp location if it's set
    FVector GetPawnViewLocation() const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    void BeginCrouch();
    void EndCrouch();

    /// Zoom functions
    void BeginZoom();
    void EndZoom();

    /// Fire functions();
    void BeginFire();
    void EndFire();

    // Function to bind move action to forward
    void MoveForward(float value);

    // Function to bind move action to right
    void MoveRight(float value);

    UFUNCTION()
    void OnHealthChanged(UCSHealthComponent* OwningHealthComp,
            float Health,
            float DeltaHealth,
            const class UDamageType* DamageType,
            class AController* InstigatedBy,
            AActor* DamageCauser);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USpringArmComponent* SpringArmComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UCameraComponent* CameraComp;

    /// Zoom variables
    bool bWantsToZoom;
    float DefaultFOV;
    
    UPROPERTY(EditDefaultsOnly, Category = "Player")
    float ZoomedFOV;

    UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100))
    float ZoomInterpSpeed;

    /// Weapon variables
    UPROPERTY(Replicated)
    ACSWeapon* CurrentWeapon;

    UPROPERTY(EditDefaultsOnly, Category = "Player")
    TSubclassOf<ACSWeapon> SpawnWeaponClass;

    UPROPERTY(VisibleDefaultsOnly, Category = "Player")
    FName WeaponAttachSocketName;

    /// Health part
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player")
    UCSHealthComponent* HealthComp;

    UPROPERTY(BlueprintReadOnly, Category="Player")
    bool bDied;
};
