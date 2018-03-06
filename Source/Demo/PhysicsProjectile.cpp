// Fill out your copyright notice in the Description page of Project Settings.

#include "PhysicsProjectile.h"
#include "Components/SphereComponent.h"

APhysicsProjectile::APhysicsProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	// Projectile's Sphere Static Mesh Component
	SphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereMesh"));
	RootComponent = SphereMesh;

	InitialLifeSpan = 5.0f;
}

void APhysicsProjectile::BeginPlay()
{
	Super::BeginPlay();

	m_InitialPosition = GetActorLocation();
}

void APhysicsProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SimulateProjectilePhysics(DeltaTime);
}

void APhysicsProjectile::SetPhysicsParameters(float Gravity, float Speed, const FVector& FiringDirection)
{
	m_FiringDirection = FiringDirection;
	m_GravityValue = Gravity;
	m_Speed = Speed;
	m_GravityVector = FVector::ZeroVector;
	m_GravityVector.Z = m_GravityValue;
}

void APhysicsProjectile::SimulateProjectilePhysics(float DeltaTime)
{
	const FVector Velocity = m_FiringDirection * m_Speed * m_CurrentTime;
	const FVector GravityAcceleration = (m_GravityVector * FMath::Square(m_CurrentTime)) / 2.0f;

	const FVector CurrentPosition = m_InitialPosition + Velocity + GravityAcceleration;
	SetActorLocation(CurrentPosition);

	m_CurrentTime += DeltaTime;
}

