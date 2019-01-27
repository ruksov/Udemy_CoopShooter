// Fill out your copyright notice in the Description page of Project Settings.

#include "CSCharacter.h"

#include "CSWeapon.h"
#include "Components/CSHealthComponent.h"

#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACSCharacter::ACSCharacter()
    : SpringArmComp(CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp")))
    , CameraComp(CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp")))
    , ZoomedFOV(65.f)
    , ZoomInterpSpeed(20.f)
    , WeaponAttachSocketName("WeaponSocket")
    , HealthComp(CreateDefaultSubobject<UCSHealthComponent>(TEXT("HealthComp")))
    , bDied(false)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    SpringArmComp->bUsePawnControlRotation = true;
    SpringArmComp->SetupAttachment(RootComponent);

    CameraComp->SetupAttachment(SpringArmComp);

    // Enable crouching/jumping for character
    auto MovementComp = GetMovementComponent();
    MovementComp->GetNavAgentPropertiesRef().bCanCrouch = true;
    MovementComp->GetNavAgentPropertiesRef().bCanJump = true;
}

// Called when the game starts or when spawned
void ACSCharacter::BeginPlay()
{
	Super::BeginPlay();
	
    DefaultFOV = CameraComp->FieldOfView;
    HealthComp->OnHealthChanged.AddDynamic(this, &ACSCharacter::OnHealthChanged);

    if (Role == ROLE_Authority)
    {
        /// Run this code only on server ACSCharacter instances.
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        CurrentWeapon = GetWorld()->SpawnActor<ACSWeapon>(SpawnWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
        if (CurrentWeapon)
        {
            CurrentWeapon->SetOwner(this);
            CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
        }
    }
}

void ACSCharacter::BeginCrouch()
{
    Crouch();
}

void ACSCharacter::EndCrouch()
{
    UnCrouch();
}

void ACSCharacter::BeginZoom()
{
    bWantsToZoom = true;
}

void ACSCharacter::EndZoom()
{
    bWantsToZoom = false;
}

void ACSCharacter::BeginFire()
{
    if (CurrentWeapon)
    {
        CurrentWeapon->StartFire();
    }
}

void ACSCharacter::EndFire()
{
    if (CurrentWeapon)
    {
        CurrentWeapon->StopFire();
    }
}

void ACSCharacter::MoveForward(float value)
{
    AddMovementInput(GetActorForwardVector(), value);
}

void ACSCharacter::MoveRight(float value)
{
    AddMovementInput(GetActorRightVector(), value);
}

void ACSCharacter::OnHealthChanged(UCSHealthComponent * OwningHealthComp,
    float Health,
    float DeltaHealth,
    const UDamageType * DamageType,
    AController * InstigatedBy,
    AActor * DamageCauser)
{
    if (Health <= 0.f && !bDied)
    {
        // Handle some die
        bDied = true;

        GetMovementComponent()->StopMovementImmediately();
        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

        DetachFromControllerPendingDestroy();
        SetLifeSpan(10.f);
    }
}

// Called every frame
void ACSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    /// Zoom logic
    float targetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;
    float currentFOV = FMath::FInterpTo(CameraComp->FieldOfView, targetFOV, DeltaTime, ZoomInterpSpeed);
    CameraComp->SetFieldOfView(currentFOV);
}

// Called to bind functionality to input
void ACSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &ACSCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ACSCharacter::MoveRight);

    PlayerInputComponent->BindAxis("LookUp", this, &ACSCharacter::AddControllerPitchInput);
    PlayerInputComponent->BindAxis("Turn", this, &ACSCharacter::AddControllerYawInput);

    PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &ACSCharacter::BeginCrouch);
    PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Released, this, &ACSCharacter::EndCrouch);

    // Zoom actions
    PlayerInputComponent->BindAction("Zoom", EInputEvent::IE_Pressed, this, &ACSCharacter::BeginZoom);
    PlayerInputComponent->BindAction("Zoom", EInputEvent::IE_Released, this, &ACSCharacter::EndZoom);

    // Fire
    PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &ACSCharacter::BeginFire);
    PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &ACSCharacter::EndFire);

    // CHALANGE CODE
    PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ACSCharacter::Jump);
}

FVector ACSCharacter::GetPawnViewLocation() const
{
    if (CameraComp)
    {
        return CameraComp->GetComponentLocation();
    }
    return Super::GetPawnViewLocation();
}

void ACSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ACSCharacter, CurrentWeapon);
}