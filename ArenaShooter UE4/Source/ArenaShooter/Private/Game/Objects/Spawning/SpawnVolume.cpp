// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnVolume.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetTextLibrary.h"

// Startup ********************************************************************************************************************************

/**
* @summary:	Sets default values for this component's properties.
*
* @return:	Constructor
*/
ASpawnVolume::ASpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create components
	_SpawnBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnBounds"));
	_SpawnBounds->SetupAttachment(GetRootComponent());
	_Text_Team = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TEXT_Team"));
	_Text_Team->SetupAttachment(_SpawnBounds);
	_Text_Gamemode = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TEXT_Gamemode"));
	_Text_Gamemode->SetupAttachment(_SpawnBounds);
	_Text_SpawnVolume = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TEXT_Spawn"));
	_Text_SpawnVolume->SetupAttachment(_SpawnBounds);

	_SpawnBounds->SetBoxExtent(_BoundsHalfExtents, true);
}

/**
* @summary:	Called when the game starts or when spawned.
*
* @return:	virtual void
*/
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();
	
}

// Settings *******************************************************************************************************************************

void ASpawnVolume::SetupGraphics(UTextRenderComponent* GametypeName, UTextRenderComponent* TeamName, UTextRenderComponent* SpawnName)
{
	// Set colour
	FColor col = FColor::White;
	switch (_Team)
	{
	case E_TeamNames::eTN_Alpha:
	{
		col = FColor::Red;
		break;
	}
	case E_TeamNames::eTN_Bravo:
	{
		col = FColor::Blue;
		break;
	}
	case E_TeamNames::eTN_Charlie:
	{
		col = FColor::Green;
		break;
	}
	case E_TeamNames::eTN_Delta:
	{
		col = FColor::Yellow;
		break;
	}
	}

	// Set text values
	E_GameTypes gametype = E_GameTypes::eGT_TDM;
	FString gametypeEnum = TEXT("") + EnumToString(TEXT("E_GameTypes"), static_cast<uint8>(gametype));
	FString gametypeText = UKismetMathLibrary::SelectString(TEXT("Multiple Gamemodes"), TEXT("" + gametypeEnum), _GameModes.Num() > 1);
	GametypeName->Text = UKismetTextLibrary::Conv_StringToText(gametypeText);

	FString enumString = TEXT("") + EnumToString(TEXT("E_TeamNames"), static_cast<uint8>(_Team));
	TeamName->Text = UKismetTextLibrary::Conv_StringToText(enumString);

	FString spawnString = UKismetMathLibrary::SelectString(TEXT("Starting Spawn"), TEXT("Respawn Volume"), _bStartingSpawnVolume);
	SpawnName->Text = UKismetTextLibrary::Conv_StringToText(spawnString);

	// Set text colour
	GametypeName->SetTextRenderColor(col);
	TeamName->SetTextRenderColor(col);
	SpawnName->SetTextRenderColor(col);
}

// Current Frame **************************************************************************************************************************

/**
* @summary:	Called every frame.
*
* @param:	float DeltaTime
*
* @return:	virtual void
*/
void ASpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Spawning *******************************************************************************************************************************

/*
*
*/
FVector ASpawnVolume::CalculateSpawnLocation(FVector TraceStartLocation, bool TraceComplex)
{
	// We can calculate the end of the trace as an offset of the Z axis 
	// which is extended to the bottom of the volume. 
	// Or in the case of overriding the ZExtent, to that value.
	FVector traceEnd;
	bool bLocalTraceFailed = false;
	float f = _fTraceZExtentOffset + (_BoundsHalfExtents.Z * 2);
	f *= -1;
	if (_bTraceOverrideZExtent)
	{ traceEnd = TraceStartLocation + (GetActorUpVector() * f); }
	else
	{ traceEnd = TraceStartLocation + ((_BoundsHalfExtents.Z * 2) * -1); }

	// Fire line trace
	FHitResult hitResult;
	FCollisionQueryParams params;
	params.bTraceComplex = TraceComplex;
	params.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(hitResult, TraceStartLocation, traceEnd, ECollisionChannel::ECC_Visibility, params);

	if (hitResult.GetComponent() != NULL)
	{
		// If our trace is inside a collider and we can use the Half Height Trace, adjust our trace to try and avoid the collider.
		if (hitResult.bStartPenetrating)
		{
			if (_bUseHalfTraceOnFail)
			{
				if (_bUsedHalfHeight)
				{
					FString name;
					this->GetName(name);
					FString message = TEXT("Warning: Hit failed - is the Volume intersecting or able to trace to an Actor component with the correct Tag? DISPLAY NAME: " + name);
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("") + message);
					return FVector::ZeroVector;
				}
				else
				{
					_bUsedHalfHeight = true;
					FVector v = CalculateSpawnLocation(TraceStartLocation, TraceComplex);
					if (v.Equals(FVector::ZeroVector))
					{
						FString name;
						this->GetName(name);
						FString message = TEXT("Warning: Hit failed - is the Volume intersecting or able to trace to an Actor component with the correct Tag? DISPLAY NAME: " + name);
						GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("") + message);
						return FVector::ZeroVector;
					}
					else
					{
						// Borat: Great success
						_bUsedHalfHeight = false;
						return v;
					}
				}
			}
			else
			{
				FString name;
				this->GetName(name);
				FString message = TEXT("Warning: Hit failed - is the Volume intersecting or able to trace to an Actor component with the correct Tag? DISPLAY NAME: " + name);
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("") + message);
				return FVector::ZeroVector;
			}
		}

		// Borat: Great success
		else  { return hitResult.Location; }
	}
	else
	{
		FString name;
		this->GetName(name);
		FString message = TEXT("Warning: Hit failed No line trace hit component is valid. DISPLAY NAME: " + name);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("") + message);
		return FVector::ZeroVector;
	}	
}

/*
*
*/
FVector ASpawnVolume::GetRandomLocationWithinVolumeBounds()
{
	FVector location = FVector::ZeroVector;

	// Calculate a random starting point to begin a line trace within the spawning volume bounds
	FRotator rot = GetRootComponent()->GetComponentRotation();
	float x = UKismetMathLibrary::RandomFloatInRange(_BoundsHalfExtents.X * -1.0, _BoundsHalfExtents.X);
	float y = UKismetMathLibrary::RandomFloatInRange(_BoundsHalfExtents.Y * -1.0, _BoundsHalfExtents.Y);
	FVector v = FVector(x, y, _BoundsHalfExtents.Z);
	UKismetMathLibrary::Quat_RotateVector(FQuat(rot), v);
	FVector traceStart = v + GetRootComponent()->GetComponentLocation();

	if (_bUseMultitraceOnFail)
	{
		for (int i = 0; i < _iMaxMultitraceIterations; i++)
		{
			// Get spawn location
			FString message = TEXT("Iteration: ") + i;
			location = CalculateSpawnLocation(traceStart, _bUseComplexTrace);
			if (!location.Equals(FVector::ZeroVector)) { break; }
		}
	}

	// Get spawn location
	else { location = CalculateSpawnLocation(traceStart, _bUseComplexTrace); }
	return location;
}

/*
*
*/
bool ASpawnVolume::SpawnActorWithinVolume(TSubclassOf<AActor> ActorClass, float ActorVerticalOffset)
{
	FVector loc = GetRandomLocationWithinVolumeBounds();
	if (loc.Equals(FVector::ZeroVector))
	{
		// Failed to get a valid spawning location
		FString name;
		this->GetName(name);
		FString message = TEXT("Warning: Failed to spawn actor from volume. DISPLAY NAME: " + name);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("") + message);
		return false;
	}




	return true;
}
