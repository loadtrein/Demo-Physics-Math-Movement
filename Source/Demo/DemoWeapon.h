// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DemoWeapon.generated.h"

/** Weapon Fire Modes */
UENUM(BlueprintType)
enum class EWeaponFireMode : uint8
{
	FM_Physics,
	FM_Guided
};

class ADemoCharacter;

UCLASS()
class DEMO_API ADemoWeapon : public AActor
{
	GENERATED_BODY()
	
	/** Current Weapon FireMode */
	UPROPERTY(EditAnywhere, Category=Weapon)
	EWeaponFireMode m_WeaponFireMode;

	/** Pawn holding weapon */
	UPROPERTY(Transient)
	ADemoCharacter* m_DemoCharacter;

	/** Physics Projectile Class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class APhysicsProjectile> PhysicsProjectileClass;

	/** Physics Projectile Initial Speed */
	UPROPERTY(EditAnywhere, Transient, Category=Projectile)
	float m_PhysicsProjectileSpeed;

	/** Gravity Value */
	UPROPERTY(EditAnywhere, Transient, Category=Projectile)
	float m_GravityValue;

	/** Landing Marker Class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Weapon)
	TSubclassOf<class AActor> LandingMarkerClass;

	/** Landing Marker Actor */
	UPROPERTY(Transient)
	AActor* m_LandingMarker;

public:
	
	ADemoWeapon();
	virtual void Tick(float DeltaTime) override;

	/** Sets weapon owner */
	void SetCharacter(ADemoCharacter* DemoCharacter);

	/** Chages weapon fire mode */
	void ChangeFireMode();

	/** Weapon firing */
	void Fire();

protected:

	virtual void BeginPlay() override;

private:

	/** When shooting on Physics Fire Mode */
	void FirePhysicsProjectile();

	/** Update Landking Marker Position while on Physics Fire Mode */
	void UpdateLandingMarkerPosition();

	void FireGuidedProjectile();

};
