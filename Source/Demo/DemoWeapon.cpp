// Fill out your copyright notice in the Description page of Project Settings.

#include "DemoWeapon.h"
#include "DemoCharacter.h"
#include "PhysicsProjectile.h"
#include "LinearProjectile.h"
#include "DrawDebugHelpers.h"

ADemoWeapon::ADemoWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	// Weapon Default values
	m_WeaponFireMode = EWeaponFireMode::FM_Physics;
	
	m_PhysicsProjectileSpeed = 3000.0f;
	m_GravityValue = -980.0f;

	m_GuidedProjectileSpeed = 2000.0f;
	m_GuidedProjectileDistanceToObjectiveThreshold = 25.0f;
	m_GuidedProjectileAngularVelocity = 10.0f;
	m_GuidedProjectileRange = 4010.0f;
	m_GuidedProjectileTraceCameraOffset = 100.0f;
	m_GuidedProjectileTraceLength = 4000.0f;
}

void ADemoWeapon::BeginPlay()
{
	Super::BeginPlay();

	UWorld* const World = GetWorld();
	if (World != nullptr)
	{
		// Spawn Landing Marker Actor
		FActorSpawnParameters ActorSpawnParams;
		m_LandingMarker = World->SpawnActor<AActor>(LandingMarkerClass, GetActorLocation(), GetActorRotation(), ActorSpawnParams);
	}
}

void ADemoWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_WeaponFireMode == EWeaponFireMode::FM_Physics)
	{
		// While on Physics Fire Mode, weapon calculates the landing spot as an aid for shooting
		UpdateLandingMarkerPosition();
	}

	if (m_WeaponFireMode == EWeaponFireMode::FM_Guided)
	{
		// While on Guided Fire Mode, weapon guides projectile
		GuideGuidedProjectile(DeltaTime);
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

		// When changing to Guided Fire Mode Landing Marker is made invisible
		if (m_LandingMarker != nullptr && !m_LandingMarker->bHidden)
		{
			m_LandingMarker->SetActorHiddenInGame(true);
		}

		if (GEngine != nullptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Guided Fire Mode"));
		}

		break;
	case EWeaponFireMode::FM_Guided:
		
		m_WeaponFireMode = EWeaponFireMode::FM_Physics;

		// Landing Marker is only visible while on Physics Fire Mode
		if (m_LandingMarker != nullptr && m_LandingMarker->bHidden)
		{
			m_LandingMarker->SetActorHiddenInGame(false);
		}

		if (GEngine != nullptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Physics Fire Mode"));
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
	
	// GunOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
	USceneComponent* const MuzzleComponent = m_DemoCharacter->GetFirstPersonMuzzleComponent();
	const FVector SpawnLocation = ((MuzzleComponent != nullptr) ? MuzzleComponent->GetComponentLocation() : m_DemoCharacter->GetActorLocation()) + SpawnRotation.RotateVector(m_DemoCharacter->GunOffset);

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

	// Set physics parameters to simulate projectile physics
	PhysicsProjectile->SetPhysicsParameters(m_GravityValue, m_PhysicsProjectileSpeed, SpawnRotation.Vector());
	return true;
}

void ADemoWeapon::UpdateLandingMarkerPosition()
{
	// All the calculations are based on the general projectile motion formulae

	// First, we calculate the time it takes for the projectile to reach the ground (Z equals to 0) with a giving Firing Direction

	USceneComponent* const MuzzleComponent = m_DemoCharacter->GetFirstPersonMuzzleComponent();
	const FVector SpawnLocation = ((MuzzleComponent != nullptr) ? MuzzleComponent->GetComponentLocation() : m_DemoCharacter->GetActorLocation()) + 
		m_DemoCharacter->GetControlRotation().RotateVector(m_DemoCharacter->GunOffset);
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

	// Now since we know the time it takes for the projectile to land, we can calculate the landing spot for x and y for that time value (z is already known and equals to 0)
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

	// Set parameters to simulate linear movement
	m_GuidedProjectile->SetSpeed(m_GuidedProjectileSpeed);
	m_GuidedProjectile->SetVelocityDirection(SpawnRotation.Vector());
	return true;
}

void ADemoWeapon::GuideGuidedProjectile(float DeltaTime)
{
	UWorld* const World = GetWorld();
	if (World == nullptr || m_DemoCharacter == nullptr)
	{
		return;
	}

	if (m_GuidedProjectile == nullptr || m_GuidedProjectile->IsPendingKill())
	{
		return;
	}

	// Get the pawn's camera transform.
	FVector CameraLocation;
	FRotator CameraRotation;
	m_DemoCharacter->GetActorEyesViewPoint(CameraLocation, CameraRotation);

	// Calculate Start and End trace values
	FVector Start = CameraLocation + CameraRotation.Vector() * m_GuidedProjectileTraceCameraOffset;
	FVector End = Start + CameraRotation.Vector() * m_GuidedProjectileTraceLength;
	FVector TargetLocation = End;

	FCollisionQueryParams TraceParams;
	TraceParams.bTraceComplex = true;
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.AddIgnoredActor(this);
	TraceParams.AddIgnoredActor(m_GuidedProjectile);
	TraceParams.AddIgnoredActor(m_DemoCharacter);
	TraceParams.AddIgnoredActor(m_LandingMarker);

	// Check for hits, and if so, update TargetLocation
	FHitResult Hit(ForceInit);
	if (World->LineTraceSingleByChannel(Hit, Start, End, ECC_Pawn, TraceParams))
	{
		TargetLocation = Hit.Location;
	}

	// Draw Sphere at TargetLocation to show where the projectile will be guided to
	DrawDebugSphere(World, TargetLocation, 20.0F, 100, FColor::Red);

	// Check for missile being close to objective, if so, we let it go
	float CurrentDistanceToObjective = FVector::DistSquared(m_GuidedProjectile->GetActorLocation(), TargetLocation);
	if (CurrentDistanceToObjective <= FMath::Square(m_GuidedProjectileDistanceToObjectiveThreshold))
	{
		return;
	}

	// Check for missile not being in range. If not in range, we let it go
	float DistanceBetweenWeaponAndProjectile = FVector::DistSquared(m_GuidedProjectile->GetActorLocation(), m_DemoCharacter->GetActorLocation());
	if ( DistanceBetweenWeaponAndProjectile >= FMath::Square(m_GuidedProjectileRange))
	{
		return;
	}
	
	// Obtain projectile's current direction and the desired one
	FVector ProjectileDirection = m_GuidedProjectile->GetVelocityDirection();
	ProjectileDirection.Normalize();
	FVector DesiredVelocity = TargetLocation - m_GuidedProjectile->GetActorLocation();
	DesiredVelocity.Normalize();

	// Check for projectile not already having desired direction
	if (ProjectileDirection.Equals(DesiredVelocity))
	{
		return;
	}

	// Interpolate between Current and Desired direction overtime
	FVector CurrentVelocity = ProjectileDirection + (DesiredVelocity - ProjectileDirection) * DeltaTime * m_GuidedProjectileAngularVelocity;
	CurrentVelocity.Normalize();

	// Set interpolated value as the projectile's current normalized velocity vector
	m_GuidedProjectile->SetVelocityDirection(CurrentVelocity);
}
