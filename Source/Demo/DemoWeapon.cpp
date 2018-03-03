// Fill out your copyright notice in the Description page of Project Settings.

#include "DemoWeapon.h"
#include "DemoCharacter.h"
#include "PhysicsProjectile.h"

#pragma optimize("",off)
ADemoWeapon::ADemoWeapon()
{
	WeaponMode = EWeaponMode::WM_Physics;
	PrimaryActorTick.bCanEverTick = true;
	
	m_PhysicsProjectileSpeed = 3000.0f;
	m_GravityValue = -980.0f;
}

void ADemoWeapon::BeginPlay()
{
	Super::BeginPlay();

	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	FActorSpawnParameters ActorSpawnParams;
	m_LandingMarker = World->SpawnActor<AActor>(LandingMarkerClass, GetActorLocation(), GetActorRotation(), ActorSpawnParams);
}

void ADemoWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (WeaponMode == EWeaponMode::WM_Physics)
	{
		// While on Physics mode, the weapon calculates the landing spot as an aid
		CalculateLandingSpot();
	}

	if (WeaponMode == EWeaponMode::WM_Guided)
	{
		
	}
}

void ADemoWeapon::SetCharacter(ADemoCharacter* DemoCharacter)
{
	m_DemoCharacter = DemoCharacter;
}

void ADemoWeapon::ChangeFireMode()
{
	switch (WeaponMode)
	{
	case EWeaponMode::WM_Physics:
		
		WeaponMode = EWeaponMode::WM_Guided;

		if (m_LandingMarker != nullptr && !m_LandingMarker->bHidden)
		{
			m_LandingMarker->SetActorHiddenInGame(true);
		}

		break;
	case EWeaponMode::WM_Guided:
		
		WeaponMode = EWeaponMode::WM_Physics;

		if (m_LandingMarker != nullptr && m_LandingMarker->bHidden)
		{
			m_LandingMarker->SetActorHiddenInGame(false);
		}

		break;
	}
}

void ADemoWeapon::Fire()
{
	switch (WeaponMode)
	{
	case EWeaponMode::WM_Physics:
		FirePhysicsProjectile();
		break;
	case EWeaponMode::WM_Guided:
		FireGuidedProjectile();
		break;
	}
}

void ADemoWeapon::FirePhysicsProjectile()
{
	UWorld* const World = GetWorld();
	if (PhysicsProjectileClass == nullptr || World == nullptr || m_DemoCharacter == nullptr)
	{
		return;
	}

	const FRotator SpawnRotation = m_DemoCharacter->GetControlRotation();
	USceneComponent* const MuzzleLocation = m_DemoCharacter->GetFirstPersonMuzzleLocation();
	// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
	const FVector SpawnLocation = ((MuzzleLocation != nullptr) ? MuzzleLocation->GetComponentLocation() : m_DemoCharacter->GetActorLocation()) + SpawnRotation.RotateVector(m_DemoCharacter->GunOffset);


	// spawn the projectile at the muzzle
	FActorSpawnParameters ActorSpawnParams;
	APhysicsProjectile* PhysicsProjectile = World->SpawnActor<APhysicsProjectile>(PhysicsProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
	if (PhysicsProjectile == nullptr)
	{
		return;
	}

	PhysicsProjectile->SetPhysicsParameters(m_GravityValue, m_PhysicsProjectileSpeed, SpawnRotation.Vector());
}

// Calculate Landing Spot in the ground (Z equals to 0)
void ADemoWeapon::CalculateLandingSpot()
{
	USceneComponent* const MuzzleLocation = m_DemoCharacter->GetFirstPersonMuzzleLocation();
	const FVector SpawnLocation = ((MuzzleLocation != nullptr) ? MuzzleLocation->GetComponentLocation() : m_DemoCharacter->GetActorLocation()) + m_DemoCharacter->GetControlRotation().RotateVector(m_DemoCharacter->GunOffset);
	const FVector FiringDirection = m_DemoCharacter->GetControlRotation().Vector();

	// We calculate the time it takes for the projectile to reach the ground (Z equals to 0) with a giving Firing Direction
	// We split the solution to the quadratic equation in several steps

	// We first check the equation to have solution and so check for no negative square roots
	float FactorInsideSqrt = FMath::Square(FiringDirection.Z * m_PhysicsProjectileSpeed) - 2.0f * m_GravityValue * SpawnLocation.Z;
	if (FactorInsideSqrt < 0.0f)
	{
		return;
	}
	
	// Secondly, we obtain the two solutions for the quadratic equation
	float TimeToLand1 = (-(FiringDirection.Z * m_PhysicsProjectileSpeed) + FMath::Sqrt(FactorInsideSqrt)) / m_GravityValue;
	float TimeToLand2 = (-(FiringDirection.Z * m_PhysicsProjectileSpeed) - FMath::Sqrt(FactorInsideSqrt)) / m_GravityValue;

	// Time to land will be the maximun of the two solutions
	float TimeToLand = FMath::Max(TimeToLand1, TimeToLand2);

	// Now since we know the time it takes for the projectile to land, we can calculate the landing spot for that time
	// Gravity for x and y is 0, therefore it has been deleted from the equations below
	FVector LandingSpot;
	LandingSpot.X = SpawnLocation.X + FiringDirection.X * m_PhysicsProjectileSpeed * (TimeToLand);
	LandingSpot.Y = SpawnLocation.Y + FiringDirection.Y * m_PhysicsProjectileSpeed * (TimeToLand);
	LandingSpot.Z = 0; // ground

	// Update landing marker actor
	if (m_LandingMarker != nullptr)
	{
		m_LandingMarker->SetActorLocation(LandingSpot);
	}	
}

void ADemoWeapon::FireGuidedProjectile()
{

}
#pragma optimize("",on)




