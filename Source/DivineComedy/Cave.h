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
          int NumBlocsX = 10;
     //Number of blocks in Y dir. 
     UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Size)
          int NumBlocsY = 10;
     //Number of generated blocks in Z dir.
     UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Size)
          int NumBlocsZGen = 10;     
private:
     //Total number of blocks
     UPROPERTY(VisibleAnywhere, Category = Size)
          int TotalSize;
     
     //Initial density parameter (0 = no blocks at all, 1 = only blocks, no space)
     UPROPERTY(EditAnywhere, Category = Generation)
          float Density = 0.4f;

     UPROPERTY(EditAnywhere, Category = Generation)
          int NumberOfSteps = 25;

     UPROPERTY(EditAnywhere, Category = Generation)
          int DiesIfHasLowerNumberOfNeighboursThan = 9;

     UPROPERTY(EditAnywhere, Category = Generation)
          int ResurrectsIfHasHigherNumberOfNeighboursThan = 12;

     //Atomic block blueprint.
     //UPROPERTY(EditAnywhere)
     //     TArray<UClass*> AtomicBlock;

     TArray<AActor*> cubes;
     //Global seed value, constant for an execution. 
     static int seed;

     std::mt19937 generator;
     std::uniform_real_distribution<float> *distribution;

     //Generate maze
     void Generate();
     void Build();
     void Clear();
     void SeedRandom();


#pragma region Generation algorithm
     TArray<unsigned short> CurrentStep;
     TArray<unsigned short> PreviousStep;

     //Fill the boundaries with cubes, based on spawn probability
     void FillModelWithCubes();

protected:
     //Step of the algorithm
     UFUNCTION(BlueprintCallable, Category = Generation)
     void PerformGenerationStep();

     UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cave)
          TArray<bool> CaveArray;

     UFUNCTION(BlueprintCallable, Category = Cave)
          void GenerateCave();

private:

     int CountNeighbours(int cell_index, bool count_neighbours);

     bool IsAlive(int cell_index);

     void Kill(int cell_index);

     void Resurrect(int cell_index);
     
     //void CreateBlock(UClass *block_type, FVector location, int n_n);

#pragma endregion Generation algorithm


public:
     ACave();
     virtual void BeginPlay() override;
     virtual void Tick(float DeltaSeconds) override;
     float Random(int x, int y, int z);

};
