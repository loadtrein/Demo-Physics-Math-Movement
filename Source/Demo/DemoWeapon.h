// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DemoWeapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponMode : uint8
{
	WM_Physics,
	WM_Guided
};

class ADemoCharacter;

UCLASS()
class DEMO_API ADemoWeapon : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category=Weapon)
	EWeaponMode WeaponMode;

	UPROPERTY(Transient)
	ADemoCharacter* m_DemoCharacter;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class APhysicsProjectile> PhysicsProjectileClass;

	UPROPERTY(EditAnywhere, Transient, Category=Projectile)
	float m_PhysicsProjectileSpeed;

	UPROPERTY(EditAnywhere, Transient, Category=Projectile)
	float m_GravityValue;

	UPROPERTY(EditDefaultsOnly, Category=Weapon)
	TSubclassOf<class AActor> LandingMarkerClass;

	UPROPERTY(Transient)
	AActor* m_LandingMarker;

public:
	
	ADemoWeapon();
	virtual void Tick(float DeltaTime) override;
	void SetCharacter(ADemoCharacter* DemoCharacter);
	void ChangeFireMode();
	void Fire();

protected:

	virtual void BeginPlay() override;

private:
	void FirePhysicsProjectile();
	void FireGuidedProjectile();
	void CalculateLandingSpot();
};
