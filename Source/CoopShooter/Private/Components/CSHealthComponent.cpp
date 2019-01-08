// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/CSHealthComponent.h"

#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UCSHealthComponent::UCSHealthComponent()
    : m_defaultHealth(0.f)
    , m_currentHealth(0.f)
{

}

// Called when the game starts
void UCSHealthComponent::BeginPlay()
{
	Super::BeginPlay();

    m_currentHealth = m_defaultHealth;

    auto owner = GetOwner();
    if (owner)
    {
        owner->OnTakeAnyDamage.AddDynamic(this, &UCSHealthComponent::HandleDamage);
    }
}

void UCSHealthComponent::HandleDamage(AActor * DamagedActor, 
    float Damage, 
    const UDamageType * DamageType, 
    AController * InstigatedBy, 
    AActor * DamageCauser)
{
    if (Damage <= 0.f)
    {
        return;
    }

    m_currentHealth = m_currentHealth < Damage ? 0.f : m_currentHealth - Damage;
    UE_LOG(LogTemp, Log, TEXT("Health was changed: %f"), m_currentHealth);

    OnHealthChanged.Broadcast(this, m_currentHealth, Damage, DamageType, InstigatedBy, DamageCauser);
}
