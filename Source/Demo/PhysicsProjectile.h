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

	/** Physics Parameters */
	UPROPERTY(Transient)
	FVector m_InitialPosition;

	UPROPERTY(Transient)
	FVector m_CurrentPosition;

	UPROPERTY(Transient)
	float m_Speed;

	UPROPERTY(Transient)
	FVector m_FiringDirection;

	UPROPERTY(Transient)
	float m_GravityValue;

	UPROPERTY(Transient)
	FVector m_GravityVector;

	UPROPERTY(Transient)
	float m_CurrentTime;

public:	

	APhysicsProjectile();
	virtual void Tick(float DeltaTime) override;
	void SetPhysicsParameters(float Gravity, float Speed, FVector FiringDirection);

protected:

	virtual void BeginPlay() override;

private:

	void SimulateProjectilePhysics(float DeltaTime);
	
};
