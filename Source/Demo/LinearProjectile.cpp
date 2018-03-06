// Fill out your copyright notice in the Description page of Project Settings.

#include "LinearProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"

ALinearProjectile::ALinearProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->SetNotifyRigidBodyCollision(true);
	CollisionComp->OnComponentHit.AddDynamic(this, &ALinearProjectile::OnHit); // set up a notification for when this component hits something blocking

	RootComponent = CollisionComp;

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

void ALinearProjectile::SetVelocityDirection(const FVector& VelocityDirection)
{
	m_VelocityDirection = VelocityDirection;
}

void ALinearProjectile::SimulateMovement(float DeltaTime)
{
	// Simulate Linear Movement
	const FVector Velocity = m_VelocityDirection * m_Speed;
	const FVector CurrentPosition = GetActorLocation() + Velocity * DeltaTime;
	
	// Setting actor location with sweep to obtain collision hit events and so trigger sound and destroy the object
	SetActorLocation(CurrentPosition, true);
}

const FVector& ALinearProjectile::GetVelocityDirection() const
{
	return m_VelocityDirection;
}

void ALinearProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// On Hit play Destroy sound and destroy the projectile

	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
	{
		if (m_DestroySound != NULL)
		{
			UGameplayStatics::PlaySoundAtLocation(this, m_DestroySound, GetActorLocation());
		}

		Destroy();
	}
}

