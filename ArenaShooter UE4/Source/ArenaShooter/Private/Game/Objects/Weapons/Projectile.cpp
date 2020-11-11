// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "BaseCharacter.h"
#include "Components/SphereComponent.h"
#include "FireMode.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "ImpactEffectManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapon.h"

// Sets default values
AProjectile::AProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Actor replicates
	bReplicates = true;
	SetReplicateMovement(true);

	// Create components
	_SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	_SphereCollision->InitSphereRadius(15.0f);
	_SphereCollision->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	RootComponent = _SphereCollision;

	_uProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	_uProjectileMovement->SetUpdatedComponent(_SphereCollision);

	_StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	_StaticMesh->SetupAttachment(_SphereCollision);
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectile::Init(AActor* ProjectileTrackingTarget)
{
	_vProjectileStartingLocation = GetActorLocation();

	// Initialize collision
	_SphereCollision->IgnoreActorWhenMoving(_Weapon, true);
	_SphereCollision->IgnoreActorWhenMoving(_Weapon->GetPawnOwner(), true);

	// Projectile speed
	_uProjectileMovement->InitialSpeed = _fInitialSpeed;
	_uProjectileMovement->MaxSpeed = _fMaximumSpeed;

	// Target tracking
	_uProjectileMovement->bIsHomingProjectile = _bProjectileTracksTargets;
	_uProjectileMovement->HomingAccelerationMagnitude = _fTrackingStrength;
	///if (ProjectileTrackingTarget->bCanBeDamaged) { _uProjectileMovement->HomingTargetComponent = ProjectileTrackingTarget; }
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& HitResult)
{
	///FString string = OtherComponent->GetName();
	///GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Orange, string);
	
	if (GetLocalRole() == ROLE_Authority)
	{
		UFireMode* fireMode = _Weapon->GetCurrentFireMode();
		if (OtherActor != NULL)
		{
			if (OtherActor->CanBeDamaged())
			{
				float damage = 0.0f;

				// Damaging character
				ABaseCharacter* character = Cast<ABaseCharacter>(OtherActor);
				if (character != NULL)
				{
					// Shields remaining
					if (character->GetCurrentShield() >= 0.0f)
					{
						// Determine which damage value (shield / base)
						FString name = HitResult.GetComponent()->GetName();
						bool bSelect = name.Compare(FString(TEXT("HitBox_Head")), ESearchCase::CaseSensitive);
						damage = UKismetMathLibrary::SelectFloat(fireMode->GetDamageShieldHead(), fireMode->GetDamageShieldBase(), bSelect);

						// Multiply damage value by damage curve
						if (fireMode->LosesDamageOverRange() && fireMode->GetDamageCurve() != NULL)
						{ damage *= fireMode->GetDamageCurve()->GetFloatValue(HitResult.Distance); }

						// Apply bleedthrough? (Only possible if damage is GREATER THAN shield remaining
						if (!_bBleedthroughEnabled && character->GetCurrentShield() < damage)
						{
							// Apply pre-damage (take down shields)
							FDamageEvent damageEvent;
							OtherActor->TakeDamage(character->GetCurrentShield(), damageEvent, GetInstigator()->GetController(), this);

							// bleed through damage will have to take into account the shield depletion amount into its final value (to avoid damage from stacking)
							damage -= character->GetCurrentShield();
							damage *= _fBleedthroughMultiplier;
						}
					}

					// Shields depleted -> damage flesh
					else
					{
						// Get initial damage value against hit component
						damage = fireMode->GetDamageByPawnHitComponent(OtherComponent);

						// Multiply damage value by damage curve
						if (fireMode->LosesDamageOverRange() && fireMode->GetDamageCurve() != NULL)
						{ damage *= fireMode->GetDamageCurve()->GetFloatValue(HitResult.Distance); }
					}
				}

				///GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Orange, FString::SanitizeFloat(damage));

				// Apply final damage
				FDamageEvent damageEvent;
				OtherActor->TakeDamage(damage, damageEvent, GetInstigator()->GetController(), this);
			}
		}

		// Play impact effect
		AImpactEffectManager* impactManager = fireMode->GetImpactEffectManager();
		if (impactManager != NULL) { impactManager->Server_Reliable_SpawnImpactEffect(HitResult); }

		// Destroy (re-pool)
		Destroy();
	}
	else
	{ Server_Reliable_OnHit(HitComponent, OtherActor, OtherComponent, NormalImpulse, HitResult); }
}

bool AProjectile::Server_Reliable_OnHit_Validate(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& HitResult)
{ return true; }

void AProjectile::Server_Reliable_OnHit_Implementation(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& HitResult)
{
	OnHit(HitComponent, OtherActor, OtherComponent, NormalImpulse, HitResult);
}
