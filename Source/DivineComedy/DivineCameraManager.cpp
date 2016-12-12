// Fill out your copyright notice in the Description page of Project Settings.

#include "DivineComedy.h"
#include "DivineCameraManager.h"
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>


#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White,text);

bool ADivineCameraManager::JumpFrame () {
  return WasJumpingLastFrame == 0 && GetViewTargetPawn ()->GetMovementComponent ()->IsFalling ();

}

void ADivineCameraManager::ProcessViewRotation (float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot) {
  //Vector pointing player direction when jump starts. Parallel to the XY plane.
  if ( JumpFrame () ) {
    JumpOrientation = GetViewTargetPawn ()->GetActorForwardVector ();
    JumpOrientation.Z = 0;
  }

  if ( GetViewTargetPawn ()->GetMovementComponent ()->IsFalling () ) {
    print (TEXT ("falling"));
  } else {
    print (TEXT ("standing"));
  }

  //Calculate the pitch rotation axis.
  float ZSpeedParam = FMath::Max (-GetViewTargetPawn ()->GetVelocity ().Z / PitchAxisMultiplier, 0.f);
  if ( GetViewTargetPawn ()->GetMovementComponent ()->IsFalling () ) {
    ZSpeedParam = 1000;
  }
  else {
    ZSpeedParam = 0;
  }
  FVector rotAxis = JumpOrientation*ZSpeedParam;
  if ( !GetViewTargetPawn ()->GetMovementComponent ()->IsFalling () ) {
    rotAxis = FVector (0, 0, 1);
  }
  rotAxis.Normalize ();

  //Calculate the rotation.
  FQuat OutQuat (OutViewRotation);
  FQuat YawQuat (rotAxis, OutDeltaRot.Yaw / 100);
  FQuat PitchQuat (FVector (0, 1, 0), -OutDeltaRot.Pitch / 100);
  FRotator result = FRotator (YawQuat*OutQuat*PitchQuat);
  //TODO: If a change from last frame is non-infinitesimal, don't do it.
  OutViewRotation = FRotator (result);

  //when standing, make view horizontal.
  if ( !GetViewTargetPawn ()->GetMovementComponent ()->IsFalling () ) {
    if ( OutViewRotation.Roll > RollToleranceMargin ) {
      OutViewRotation.Roll -= DeltaTime * RollRecoverySpeedParameter;
    } else if ( OutViewRotation.Roll < -RollToleranceMargin ) {
      OutViewRotation.Roll += DeltaTime * RollRecoverySpeedParameter;
    }
  }


  OutDeltaRot = FRotator::ZeroRotator;
  WasJumpingLastFrame = GetViewTargetPawn ()->GetMovementComponent ()->IsFalling ();
}


