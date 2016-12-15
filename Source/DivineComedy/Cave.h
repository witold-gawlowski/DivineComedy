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
     GENERATED_BODY()
protected:
     //Number of blocks in X dir. 
     UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Size)
          int NumBlocsX;
     //Number of blocks in Y dir. 
     UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Size)
          int NumBlocsY;
     //Number of generated blocks in Z dir.
     UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Size)
          int NumBlocsZGen;

     //Step of the algorithm
     UFUNCTION(BlueprintCallable, Category = Generation)
          void PerformGenerationStep();

     UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cave)
          TArray<bool> CaveArray;

     UFUNCTION(BlueprintCallable, Category = Cave)
          void GenerateCave();

     UFUNCTION(BlueprintCallable, Category = Cave)
          void FillHoles(bool Ceiling, bool Floor);
     
     //Initial density parameter (0 = no blocks at all, 1 = only blocks, no space)
     UPROPERTY(EditAnywhere, Category = Generation)
          float Density = 0.4f;

     UPROPERTY(EditAnywhere, Category = Generation)
          int NumberOfSteps = 25;

     UPROPERTY(EditAnywhere, Category = Generation)
          int DiesIfHasLowerNumberOfNeighboursThan = 9;

     UPROPERTY(EditAnywhere, Category = Generation)
          int ResurrectsIfHasHigherNumberOfNeighboursThan = 12;


private:

     //Global seed value, constant for an execution. 
     static int seed;

     std::mt19937 generator;
     std::uniform_real_distribution<float> *distribution;

     //Generate maze
     void Clear();
     void SeedRandom();


#pragma region Generation algorithm
     TArray<unsigned short> CurrentStep;
     TArray<unsigned short> PreviousStep;

     //Fill the boundaries with cubes, based on spawn probability
     void FillModelWithCubes();

     int CountNeighbours(int cell_index, bool count_neighbours);

     bool IsAlive(int cell_index);

     void Kill(int cell_index);

     void Resurrect(int cell_index);

#pragma endregion Generation algorithm

     int TotalSize;
public:
     ACave();
     virtual void BeginPlay() override;
     virtual void Tick(float DeltaSeconds) override;
     float Random(int x, int y, int z);

};
