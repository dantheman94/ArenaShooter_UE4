// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraCinematic.h"

#include "BaseCharacter.h"
#include "BasePlayerController.h"
#include "CameraSpline.h"
#include "Components/ArrowComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "IngameGameState.h"
#include "Kismet/KismetMathLibrary.h"
#include "UnrealNetwork.h"

// Startup ********************************************************************************************************************************

/**
* @summary:	Sets default values for this component's properties.
*
* @return:	Constructor
*/
ACameraCinematic::ACameraCinematic()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

/**
* @summary:	Called when the game starts or when spawned.
*
* @return:	virtual void
*/
void ACameraCinematic::BeginPlay()
{
	Super::BeginPlay();

	// Get GameState reference
	if (_pIngameGameState == NULL)
	{
		AGameStateBase* gameState = GetWorld()->GetGameState();
		_pIngameGameState = Cast<AIngameGameState>(gameState);
	}
	
	// Add cinematic to array
	if (_pIngameGameState != NULL) { _pIngameGameState->Server_Reliable_AddCinematicToArray(this); }
}

///////////////////////////////////////////////

void ACameraCinematic::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	///DOREPLIFETIME(ACameraCinematic, _bCanTryToVentCooldown);
}

// Current Frame **************************************************************************************************************************

/**
* @summary:	Called every frame.
*
* @param:	float DeltaTime
*
* @return:	virtual void
*/
void ACameraCinematic::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (_CameraSpline != NULL && _ViewCameraActor != NULL)
	{
		if (_CameraSpline->GetPreDelay() > 0.0f)
		{
			_CameraSpline->SetPreDelay(_CameraSpline->GetPreDelay() - DeltaTime);
		}
		else
		{
			// Head zoom movement
			if (_bZoomIntoHead == true)
			{
				if (_fLerpTime < _fZoomDuration)
				{
					_fLerpTime += DeltaTime;
					FVector loc = UKismetMathLibrary::VLerp(_ZoomStartLocation, _HeadLocation, _fLerpTime / _fZoomDuration);
					FRotator rot = UKismetMathLibrary::RLerp(_ZoomStartRotation, _HeadRotation, _fLerpTime / _fZoomDuration, true);
					_ViewCameraActor->SetActorLocation(loc);
					//_ViewCameraActor->SetActorRotation(rot);
				} else
				{

				}

				// Start fade in
				if ((_fLerpTime / _fZoomDuration) >= _fZoomFadeThresholdPercent)
				{
					_OnHeadZoomThreshold.Broadcast();
				}
			}

			// Camera spline movement
			else
			{
				if (_iCurrentSplinePoint < _CameraSpline->GetSpline()->GetNumberOfSplinePoints())
				{
					// Set camera facing direction
					FVector v = _ViewCameraActor->GetActorLocation() - _CameraSpline->GetActorLocation();
					v.Normalize();
					v *= -1;
					FRotator r = UKismetMathLibrary::MakeRotFromX(v);
					_ViewCameraActor->SetActorRotation(r);

					// Get lerp time
					float lerpDuration = _CameraSpline->GetLerpDuration();
					_fLerpTime += DeltaTime;
					if (_fLerpTime < lerpDuration)
					{
						// Move camera along spline
						FVector pointALoc = _CameraSpline->GetSpline()->GetLocationAtSplinePoint(_iCurrentSplinePoint, ESplineCoordinateSpace::World);
						FVector pointBLoc = _CameraSpline->GetSpline()->GetLocationAtSplinePoint(_iCurrentSplinePoint + 1, ESplineCoordinateSpace::World);
						FVector currentLoc = UKismetMathLibrary::VLerp(pointALoc, pointBLoc, _fLerpTime / lerpDuration);
						_ViewCameraActor->SetActorLocation(currentLoc);
					} else
					{
						// Check if next spline is valid
						if (_iCurrentSplinePoint + 1 < _CameraSpline->GetSpline()->GetNumberOfSplinePoints() - 1)
						{
							// Proceed to next spline point
							_iCurrentSplinePoint++;
						}
					}

					// Camera reached end of transition for this spline
					if (_fLerpTime >= _CameraSpline->GetTransitionDelay())
					{
						// Another spline in the list
						if (_iCurrentSpline + 1 < _tCameraSplines.Num())
						{
							_iCurrentSpline++;
							_CameraSpline = _tCameraSplines[_iCurrentSpline];
							_fLerpTime = 0.0f;
							_iCurrentSplinePoint = 0;
						}

						// Cinematic complete
						else
						{
							_OnCinematicComplete.Broadcast();

							// Do that really fast zoom into the local player
							if (_eCinematicType == E_CinematicType::eGT_Opening)
							{
								// Get local player controller (to get their pawn)
								APlayerController* playerController = GetWorld()->GetFirstPlayerController();
								if (playerController != NULL)
								{
									// Get pawn reference
									APawn* pawn = playerController->GetPawn();
									if (pawn != NULL)
									{
										// Get pawn's head position
										ABaseCharacter* character = Cast<ABaseCharacter>(pawn);
										USkeletalMeshComponent* mesh = character->GetMesh();
										_HeadLocation = mesh->GetSocketLocation("head");

										// Get pawn's head facing direction
										FVector dir = mesh->GetForwardVector();
										_HeadRotation = UKismetMathLibrary::MakeRotFromX(dir);
										// This is because the ue4 mesh is rotated -90 degrees on the Z axis (YAW)
										_HeadRotation -= FRotator(0.0f, -90.0f, 0.0f);

										// Initialize lerp to location/rotation
										_ZoomStartLocation = _ViewCameraActor->GetActorLocation();
										_ZoomStartRotation = _ViewCameraActor->GetActorRotation();
										_fLerpTime = 0.0f;
										_bZoomIntoHead = true;
									}
								}
							}
						}
					}
				}
			}
		}	
	}
}

///////////////////////////////////////////////

/*
*
*/
void ACameraCinematic::StartCinematic()
{
	if (_ViewCameraActor == NULL) { return; }

	// This is so in blueprint, we can use custom UMG for fade ins or black bars without explicitly calling them in code
	_OnCinematicStart.Broadcast();

	// Initialize player controller view to starting actor
	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	if (playerController != NULL && _tCameraSplines.Num() > 0)
	{
		// Attach view to camera actor
		playerController->SetViewTarget(_ViewCameraActor);

		// Start at first spline point
		_fLerpTime = 0.0f;
		_iCurrentSpline = 0;
		_iCurrentSplinePoint = 0;
		_CameraSpline = _tCameraSplines[_iCurrentSpline];

		// Get starting location
		FVector loc = _CameraSpline->GetSpline()->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
		FRotator rot = _CameraSpline->GetSpline()->GetRotationAtSplinePoint(0, ESplineCoordinateSpace::World);
		_ViewCameraActor->SetActorLocation(loc, false, nullptr, ETeleportType::TeleportPhysics);

		// Get facing direction
		FVector v = _ViewCameraActor->GetActorLocation() - _CameraSpline->GetActorLocation();
		v.Normalize();
		v *= -1;
		FRotator r = UKismetMathLibrary::MakeRotFromX(v);
		_ViewCameraActor->SetActorRotation(r);
	}
	else
	{
		FString message = TEXT("No Camera Spline References found in _tCameraSplines [" + this->GetName() + "]. Add spline references for this cinematic to rectify this issue.");
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, message);
	}
}

