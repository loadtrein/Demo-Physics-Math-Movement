// Fill out your copyright notice in the Description page of Project Settings.

#include "LinearProjectile.h"

ALinearProjectile::ALinearProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereMesh"));
	RootComponent = SphereMesh;

	InitialLifeSpan = 5.0f;
}

void ALinearProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	m_InitialPosition = GetActorLocation();
}

void ALinearProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	SimulateMovement(DeltaTime);
}

void ALinearProjectile::SetSpeed(float Speed)
{
	m_Speed = Speed;
}

void ALinearProjectile::SetVelocityDirection(FVector VelocityDirection)
{
	m_VelocityDirection = VelocityDirection;
}

void ALinearProjectile::SimulateMovement(float DeltaTime)
{
	FVector Velocity = m_VelocityDirection * m_Speed;

	m_CurrentPosition = m_InitialPosition + Velocity * m_CurrentTime;
	SetActorLocation(m_CurrentPosition);

	m_CurrentTime += DeltaTime;
}

