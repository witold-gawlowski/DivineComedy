// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**
*
*/
class DIVINECOMEDY_API DivineUtils {
public:
  template <typename ASpawnBP>
  FORCEINLINE static ASpawnBP* SpawnBP (
    UWorld* TheWorld,
    UClass* TheBP,
    const FVector& Loc,
    const FRotator& Rot,
    const bool bNoCollisionFail,
    AActor* Owner,
    APawn* Instigator
  ) {
    if ( !TheWorld ) return NULL;
    if ( !TheBP ) return NULL;
    //~~~~~~~~~~~

    FActorSpawnParameters SpawnInfo;
    SpawnInfo.bNoFail = bNoCollisionFail;
    SpawnInfo.Owner = Owner;
    SpawnInfo.Instigator = Instigator;
    SpawnInfo.bDeferConstruction = false;

    return TheWorld->SpawnActor<ASpawnBP> (TheBP, Loc, Rot, SpawnInfo);
  };

  DivineUtils ();
  ~DivineUtils ();
};
