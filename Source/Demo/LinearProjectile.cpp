// Fill out your copyright notice in the Description page of Project Settings.

#include "LinearProjectile.h"
#include "Kismet/GameplayStatics.h"

ALinearProjectile::ALinearProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereMesh"));
	RootComponent = SphereMesh;

	InitialLifeSpan = 10.0f;
}

void ALinearProjectile::BeginPlay()
{
	Super::BeginPlay();
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
	m_CurrentPosition = GetActorLocation() + Velocity * DeltaTime;
	
	SetActorLocation(m_CurrentPosition);
}

FVector ALinearProjectile::GetVelocityDirection()
{
	return m_VelocityDirection;
}

void ALinearProjectile::DestroyProjectile()
{
	// try and play the sound if specified
	if (m_DestroySound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, m_DestroySound, GetActorLocation());
	}

	Destroy();
}

