// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LinearProjectile.generated.h"

UCLASS()
class DEMO_API ALinearProjectile : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	class USphereComponent* CollisionComp;

	/** Speed at which the projectile left the weapon*/
	UPROPERTY(Transient)
	float m_Speed;

	/** Current Velocity Direction */
	UPROPERTY(Transient)
	FVector m_VelocityDirection;

public:

	/** Sound to play on destroy */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Sound)
	class USoundBase* m_DestroySound;
	
public:	
	
	ALinearProjectile();
	virtual void Tick(float DeltaTime) override;

	/** Set physics parameters from weapon */
	void SetSpeed(float Speed);
	void SetVelocityDirection(FVector VelocityDirection);

	FVector GetVelocityDirection();

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:
	
	virtual void BeginPlay() override;
	
private:

	void SimulateMovement(float DeltaTime);

};
