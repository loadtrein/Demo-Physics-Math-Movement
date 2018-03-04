// Fill out your copyright notice in the Description page of Project Settings.

#include "DemoWeapon.h"
#include "DemoCharacter.h"
#include "PhysicsProjectile.h"
#include "LinearProjectile.h"

#pragma optimize("",off)
ADemoWeapon::ADemoWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	m_WeaponFireMode = EWeaponFireMode::FM_Physics;
	
	m_PhysicsProjectileSpeed = 3000.0f;
	m_GravityValue = -980.0f;

	m_GuidedProjectileSpeed = 3000.0f;
}

void ADemoWeapon::BeginPlay()
{
	Super::BeginPlay();

	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	// Spawn Landing Marker Actor
	FActorSpawnParameters ActorSpawnParams;
	m_LandingMarker = World->SpawnActor<AActor>(LandingMarkerClass, GetActorLocation(), GetActorRotation(), ActorSpawnParams);
}

void ADemoWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_WeaponFireMode == EWeaponFireMode::FM_Physics)
	{
		// While on Physics Fire Mode, the weapon calculates the landing spot as an aid for shooting
		UpdateLandingMarkerPosition();
	}

	if (m_WeaponFireMode == EWeaponFireMode::FM_Guided)
	{
		GuideGuidedProjectile();
	}
}

void ADemoWeapon::SetCharacter(ADemoCharacter* DemoCharacter)
{
	m_DemoCharacter = DemoCharacter;
}

void ADemoWeapon::ChangeFireMode()
{
	switch (m_WeaponFireMode)
	{
	case EWeaponFireMode::FM_Physics:
		
		m_WeaponFireMode = EWeaponFireMode::FM_Guided;

		if (m_LandingMarker != nullptr && !m_LandingMarker->bHidden)
		{
			m_LandingMarker->SetActorHiddenInGame(true);
		}

		break;
	case EWeaponFireMode::FM_Guided:
		
		m_WeaponFireMode = EWeaponFireMode::FM_Physics;

		// Landing Marker is only visible while on Physics Fire Mode
		if (m_LandingMarker != nullptr && m_LandingMarker->bHidden)
		{
			m_LandingMarker->SetActorHiddenInGame(false);
		}

		break;
	}
}

bool ADemoWeapon::Fire()
{
	if (m_DemoCharacter == nullptr)
	{
		return false;
	}

	// Obtain rotation from pawn and location from muzzle
	const FRotator SpawnRotation = m_DemoCharacter->GetControlRotation();
	
	// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
	USceneComponent* const MuzzleLocation = m_DemoCharacter->GetFirstPersonMuzzleLocation();
	const FVector SpawnLocation = ((MuzzleLocation != nullptr) ? MuzzleLocation->GetComponentLocation() : m_DemoCharacter->GetActorLocation()) + SpawnRotation.RotateVector(m_DemoCharacter->GunOffset);

	bool fireOk = false;

	switch (m_WeaponFireMode)
	{
	case EWeaponFireMode::FM_Physics:
		
		fireOk = FirePhysicsProjectile(SpawnLocation, SpawnRotation);
		
		break;

	case EWeaponFireMode::FM_Guided:
		
		// Just have one guided projectile at a time
		if (m_GuidedProjectile == nullptr || m_GuidedProjectile->IsPendingKill())
		{
			fireOk = FireGuidedProjectile(SpawnLocation, SpawnRotation);
		}

		break;
	}

	return fireOk;
}

bool ADemoWeapon::FirePhysicsProjectile(const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	UWorld* const World = GetWorld();
	if (PhysicsProjectileClass == nullptr || World == nullptr)
	{
		return false;
	}

	// spawn the projectile at the muzzle
	FActorSpawnParameters ActorSpawnParams;
	APhysicsProjectile* PhysicsProjectile = World->SpawnActor<APhysicsProjectile>(PhysicsProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
	if (PhysicsProjectile == nullptr)
	{
		return false;
	}

	PhysicsProjectile->SetPhysicsParameters(m_GravityValue, m_PhysicsProjectileSpeed, SpawnRotation.Vector());
	return true;
}

void ADemoWeapon::UpdateLandingMarkerPosition()
{
	// We calculate the time it takes for the projectile to reach the ground (Z equals to 0) with a giving Firing Direction

	USceneComponent* const MuzzleLocation = m_DemoCharacter->GetFirstPersonMuzzleLocation();
	const FVector SpawnLocation = ((MuzzleLocation != nullptr) ? MuzzleLocation->GetComponentLocation() : m_DemoCharacter->GetActorLocation()) + m_DemoCharacter->GetControlRotation().RotateVector(m_DemoCharacter->GunOffset);
	const FVector FiringDirection = m_DemoCharacter->GetControlRotation().Vector();

	// We split the solution to the quadratic equation in several steps

	// We first check the equation to have solution and so we check for no negative square roots
	float FactorInsideSqrt = FMath::Square(FiringDirection.Z * m_PhysicsProjectileSpeed) - 2.0f * m_GravityValue * SpawnLocation.Z;
	if (FactorInsideSqrt < 0.0f)
	{
		return;
	}
	
	// Secondly, we obtain the two solutions for the quadratic equation
	float TimeToLand1 = (-(FiringDirection.Z * m_PhysicsProjectileSpeed) + FMath::Sqrt(FactorInsideSqrt)) / m_GravityValue;
	float TimeToLand2 = (-(FiringDirection.Z * m_PhysicsProjectileSpeed) - FMath::Sqrt(FactorInsideSqrt)) / m_GravityValue;

	// Time to land will be the maximum of the two solutions
	float TimeToLand = FMath::Max(TimeToLand1, TimeToLand2);

	// Now since we know the time it takes for the projectile to land, we can calculate the landing spot for that time value
	// Gravity for x and y is 0, for code simplicity it has been deleted from the equations below
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

bool ADemoWeapon::FireGuidedProjectile(const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	UWorld* const World = GetWorld();
	if (PhysicsProjectileClass == nullptr || World == nullptr)
	{
		return false;
	}

	// spawn the projectile at the muzzle
	FActorSpawnParameters ActorSpawnParams;
	m_GuidedProjectile = World->SpawnActor<ALinearProjectile>(GuidedProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
	if (m_GuidedProjectile == nullptr)
	{
		return false;
	}

	m_GuidedProjectile->SetSpeed(m_PhysicsProjectileSpeed);
	m_GuidedProjectile->SetVelocityDirection(SpawnRotation.Vector());
	return true;
}

void ADemoWeapon::GuideGuidedProjectile()
{

}

#pragma optimize("",on)




