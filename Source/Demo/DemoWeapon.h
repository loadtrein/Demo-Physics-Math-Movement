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
	UPROPERTY(EditDefaultsOnly, Category=PhysicsProjectile)
	TSubclassOf<class APhysicsProjectile> PhysicsProjectileClass;

	/** Physics Projectile Initial Speed */
	UPROPERTY(EditAnywhere, Transient, Category=PhysicsProjectile)
	float m_PhysicsProjectileSpeed;

	/** Gravity Value */
	UPROPERTY(EditAnywhere, Transient, Category=PhysicsProjectile)
	float m_GravityValue;

	/** Landing Marker Class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Weapon)
	TSubclassOf<class AActor> LandingMarkerClass;

	/** Landing Marker Actor */
	UPROPERTY(Transient)
	AActor* m_LandingMarker;

	/** Guided Projectile Class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=GuidedProjectile)
	TSubclassOf<class ALinearProjectile> GuidedProjectileClass;

	/** Current Guided Projectile Actor */
	UPROPERTY(Transient)
	ALinearProjectile* m_GuidedProjectile;

	/** Guided Projectile Speed */
	UPROPERTY(EditAnywhere, Transient, Category=GuidedProjectile)
	float m_GuidedProjectileSpeed;

	/** Guided Projectile Distance To Objective Threshold */
	UPROPERTY(EditAnywhere, Transient, Category=GuidedProjectile)
	float m_GuidedProjectileDistanceToObjectiveThreshold;

	/** Guided Projectile Turn Rate */
	UPROPERTY(EditAnywhere, Transient, Category=GuidedProjectile)
	float m_GuidedProjectileAngularVelocity;

	/** GuidedProjectile-Weapon Range distance */
	UPROPERTY(EditAnywhere, Transient, Category=GuidedProjectile)
	float m_GuidedProjectileRange;

	/** GuidedProjectile Trace offset from camera that will define the trace start */
	UPROPERTY(EditAnywhere, Transient, Category=GuidedProjectile)
	float m_GuidedProjectileTraceCameraOffset;

	/** Guided Projectile Trace Length */
	UPROPERTY(EditAnywhere, Transient, Category=GuidedProjectile)
	float m_GuidedProjectileTraceLength;

public:
	
	ADemoWeapon();
	virtual void Tick(float DeltaTime) override;

	/** Sets weapon owner */
	void SetCharacter(ADemoCharacter* DemoCharacter);

	/** Chages weapon fire mode */
	void ChangeFireMode();

	/** Weapon firing */
	bool Fire();

protected:

	virtual void BeginPlay() override;

private:

	/** When shooting on Physics Fire Mode */
	bool FirePhysicsProjectile(const FVector& SpawnLocation, const FRotator& SpawnRotation);

	/** Update Landking Marker Position while on Physics Fire Mode */
	void UpdateLandingMarkerPosition();

	/** When shooting on Guided Fire Mode */
	bool FireGuidedProjectile(const FVector& SpawnLocation, const FRotator& SpawnRotation);

	/** Update Guided Projectile Direction while on Guided Fire Mode */
	void GuideGuidedProjectile(float DeltaTime);

};
