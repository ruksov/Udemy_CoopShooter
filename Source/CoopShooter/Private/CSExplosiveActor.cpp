// Fill out your copyright notice in the Description page of Project Settings.

#include "CSExplosiveActor.h"

#include "Components/CSHealthComponent.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACSExplosiveActor::ACSExplosiveActor()
    : MeshComp(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp")))
    , SphereComp(CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp")))
    , HealthComp(CreateDefaultSubobject<UCSHealthComponent>(TEXT("HealthComp")))
    , ExplosionStrength(200.f)
    , SelfExplosionStrength(50.f)
    , ExplosionDamage(50.f)
    , m_isExploded(false)
{
    MeshComp->SetSimulatePhysics(true);
    RootComponent = MeshComp;
    SphereComp->SetupAttachment(RootComponent);
}

void ACSExplosiveActor::BeginPlay()
{
    Super::BeginPlay();
    
    HealthComp->OnHealthChanged.AddDynamic(this, &ACSExplosiveActor::OnHealthChanged);
}

void ACSExplosiveActor::OnHealthChanged(UCSHealthComponent * OwningHealthComp,
    float Health,
    float DeltaHealth,
    const UDamageType * DamageType,
    AController * InstigatedBy,
    AActor * DamageCauser)
{
    if (Health <= 0 && !m_isExploded && ExplodedMaterial)
    {
        Explose();
    }
}

void ACSExplosiveActor::Explose()
{
    UE_LOG(LogTemp, Log, TEXT("Start explosion"));
    m_isExploded = true;
    MeshComp->SetMaterial(0, ExplodedMaterial);

    UGameplayStatics::SpawnEmitterAtLocation(GetWorld()
        , ExplosionEffect
        , GetActorLocation());

    /// Launch it self
    auto SelfLaunchVelocity = FVector(0.f, 0.f, 1.f) * SelfExplosionStrength;
    MeshComp->AddImpulse(SelfLaunchVelocity, NAME_None, true);

    
    /// Launch all components in explosive sphere
    const float ExplosionRadius = SphereComp->GetScaledSphereRadius();
    TArray<UPrimitiveComponent*> OverlappingComponents;
    SphereComp->GetOverlappingComponents(OverlappingComponents);
    
    for (auto Comp : OverlappingComponents)
    {
        if (Comp && Comp->IsSimulatingPhysics() && MeshComp != Comp)
        {
            Comp->AddRadialImpulse(GetActorLocation()
                , ExplosionRadius
                , ExplosionStrength
                , ERadialImpulseFalloff::RIF_Constant
                , true);

            /// Apply damage to components
            UGameplayStatics::ApplyRadialDamage(Comp
                , ExplosionDamage
                , GetActorLocation()
                , ExplosionRadius
                , DamageType
                , TArray<AActor*>()
                , this);
        }
    }
}
