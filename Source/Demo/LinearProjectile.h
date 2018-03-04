// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LinearProjectile.generated.h"

UCLASS()
class DEMO_API ALinearProjectile : public AActor
{
	GENERATED_BODY()

	/** Sphere component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	class UStaticMeshComponent* SphereMesh;

	/** Firing Position */
	UPROPERTY(Transient)
	FVector m_InitialPosition;

	UPROPERTY(Transient)
	FVector m_CurrentPosition;

	/** Speed at which the projectile left the weapon*/
	UPROPERTY(Transient)
	float m_Speed;

	UPROPERTY(Transient)
	FVector m_VelocityDirection;

	UPROPERTY(Transient)
	float m_CurrentTime;
	
public:	
	
	ALinearProjectile();
	virtual void Tick(float DeltaTime) override;

	/** Set physics parameters from weapon */
	void SetSpeed(float Speed);
	void SetVelocityDirection(FVector VelocityDirection);

protected:
	
	virtual void BeginPlay() override;
	
private:

	void SimulateMovement(float DeltaTime);

};
