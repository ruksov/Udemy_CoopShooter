// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CSCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class COOPSHOOTER_API ACSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACSCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    void BeginCrouch();

    void EndCrouch();

    // Function to bind move action to forward
    void MoveForward(float value);

    // Function to bind move action to right
    void MoveRight(float value);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USpringArmComponent* SpringArmComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UCameraComponent* CameraComp;
};
