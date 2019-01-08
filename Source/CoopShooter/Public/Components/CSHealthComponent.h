// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CSHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, UCSHealthComponent*, OwningHealthComp, float, Health, float, DeltaHealth, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS( ClassGroup=(COOP), meta=(BlueprintSpawnableComponent) )
class COOPSHOOTER_API UCSHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCSHealthComponent();

protected:
    // Called when the game starts or when spawned
    void BeginPlay() override;

    UFUNCTION()
    void HandleDamage(AActor* DamagedActor,
        float Damage,
        const class UDamageType* DamageType,
        class AController* InstigatedBy,
        AActor* DamageCauser);

public:
    UPROPERTY(BlueprintAssignable, Category="Events")
    FOnHealthChangedSignature OnHealthChanged;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Health", meta=(DisplayName="Default Health"))
    float m_defaultHealth;

private:
    float m_currentHealth;
};
