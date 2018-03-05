// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PhysicsProjectile.generated.h"

UCLASS()
class DEMO_API APhysicsProjectile : public AActor
{
	GENERATED_BODY()
	
	/** Sphere component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	class UStaticMeshComponent* SphereMesh;

	/** Firing Position */
	UPROPERTY(Transient)
	FVector m_InitialPosition;

	UPROPERTY(Transient)
	FVector m_CurrentPosition;

	/** Speed at which the projectile left the weapon*/
	UPROPERTY(Transient)
	float m_Speed;

	/** Direction the weapon was fired in */
	UPROPERTY(Transient)
	FVector m_FiringDirection;

	UPROPERTY(Transient)
	float m_GravityValue;

	/** Vector holding the gravity value on the z component */
	UPROPERTY(Transient)
	FVector m_GravityVector;

	UPROPERTY(Transient)
	float m_CurrentTime;

public:	

	APhysicsProjectile();
	virtual void Tick(float DeltaTime) override;

	/** Set physics parameters from weapon */
	void SetPhysicsParameters(float Gravity, float Speed, FVector FiringDirection);

protected:

	virtual void BeginPlay() override;

private:

	void SimulateProjectilePhysics(float DeltaTime);
	
};
