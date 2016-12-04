// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <random>
#include "DivineUtils.h"
#include "GameFramework/Actor.h"
#include "Cave.generated.h"


UCLASS()
class DIVINECOMEDY_API ACave : public AActor
{
  //Class responsible for cave generation.
  GENERATED_BODY ()

  //Number of blocks in X dir. 
  UPROPERTY (EditAnywhere)
  int NumBlocsX;
  //Number of blocks in Y dir. 
  UPROPERTY (EditAnywhere)
  int NumBlocsY;
  //Number of generated blocks in Z dir.
  UPROPERTY (EditAnywhere)
  int NumBlocsZGen;
  //Atomic block size.
  UPROPERTY (EditAnywhere)
  float BlockDistance;
  //Initial density parameter (0 = no blocks at all, 1 = only blocks, no space)
  UPROPERTY (EditAnywhere)
  float density;
  //Atomic block blueprint.
  UPROPERTY (EditAnywhere)
  UClass *AtomicBlock;

  TArray<int> grid;
  TArray<AActor*> cubes;
  //Global seed value, constant for an execution. 
  static int seed;
  
  std::mt19937 generator;
  std::uniform_real_distribution<float> *distribution;
  void SeedRandom ();
public:	
	ACave();
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;
  float Random (int x, int y, int z);
  int& GridAt (int x, int y, int z);

  void Generate ();
  void Build ();
  void Clear ();
	
	
};
