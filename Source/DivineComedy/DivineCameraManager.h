// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera/PlayerCameraManager.h"
#include "DivineCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class DIVINECOMEDY_API ADivineCameraManager : public APlayerCameraManager
{
  GENERATED_BODY ()
  bool WasJumpingLastFrame = 0;
  bool JumpFrame ();
  void ProcessViewRotation (float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot) override;
public:
  //How quickly axis approaches horizontal one when falling. 
  UPROPERTY (EditAnywhere)
    float PitchAxisMultiplier = 4000.f;
  UPROPERTY (EditAnywhere)
    float RollRecoverySpeedParameter = 150.0f;
  //In degrees.
  UPROPERTY (EditAnywhere)
    float RollToleranceMargin = 2.0f;
  
  UPROPERTY (VisibleAnywhere)
    bool DownFallCameraMode = true;
  FVector JumpOrientation;
	
  UPROPERTY (EditAnywhere)
    float CameraModeTriggerAngleLow = 15;

  UPROPERTY (EditAnywhere)
    float CameraModeTriggerAngleHigh = 65;
};
