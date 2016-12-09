#include "DivineComedy.h"
#include "Cave.h"
#include <time.h>

#define print1(XXX) UE_LOG (LogTemp, Warning, XXX);
#define print(XXX, YYY)  UE_LOG (LogTemp, Warning, XXX, YYY);

ACave::ACave()
{
     PrimaryActorTick.bCanEverTick = true;
     distribution = new std::uniform_real_distribution<float>(0, 1);
}

void ACave::BeginPlay()
{
     Super::BeginPlay();
     Clear();
     Generate();
     Build();
}

void ACave::Tick(float DeltaTime)
{
     Super::Tick(DeltaTime);
}

#pragma region Random number generation
//This function sets global seed value. 
void ACave::SeedRandom()
{
     seed = time(NULL);
}

//This function assigns same random float from (0, 1) to each triple (x, y, z)
float ACave::Random(int x, int y, int z)
{
     generator.seed(seed + x + y*NumBlocsX + z*NumBlocsX*NumBlocsY);
     return (*distribution) (generator);
}

int ACave::seed;
#pragma endregion Random number generation





#pragma region Cellular automata algorithm
void ACave::FillModelWithCubes()
{
     for (int i = 0; i < TotalSize; i++)
          PreviousStep.Add(Random(i / (NumBlocsY*NumBlocsZGen), i / NumBlocsZGen, i%NumBlocsZGen) <= Density ? true : false);
}

void ACave::PerformGenerationStep(bool RecreateModel = false)
{
     for (int i = 0; i < TotalSize; i++)
     {
          int neighbours_number = CountNeighbours(i);
          if (IsAlive(i))
          {
               if (neighbours_number < LowerNeighboursCountDieThreshold ||
                    neighbours_number > UpperNeighboursCountDieThreshold)
                    Kill(i);
          }
          else if (neighbours_number >= ResurrectionGapLowerLimit && neighbours_number <= ResurrectionGapUpperLimit)
               Resurrect(i);
     }

     //recreate model if from blueprint called
     if (RecreateModel)
     {
          Clear();
          Build();
     }
}

int ACave::CountNeighbours(int cell_index)
{
     int neighbours = 0,
          x = cell_index / (NumBlocsY*NumBlocsZGen),
          y = cell_index / NumBlocsZGen % NumBlocsY,
          z = cell_index%NumBlocsZGen;
     for (int i = -1; i <= 1; i++)
          if (i + x >= 0 && i + x < NumBlocsX)
          {
               for (int j = -1; j <= 1; j++)
                    if (j + y >= 0 && j + y < NumBlocsY)
                    {
                         for (int k = -1; k <= 1; k++)
                              if (k + z >= 0 && k + z < NumBlocsZGen)
                              {
                                   if (PreviousStep[(i + x)*(NumBlocsY*NumBlocsZGen) + (y+j)*NumBlocsZGen + z + k])
                                        neighbours++;
                              }
                              else
                                   neighbours++;
                    }
                    else
                         neighbours++;
          }
          else
               neighbours++;
     return neighbours;
}

bool ACave::IsAlive(int cell_index)
{
     return PreviousStep[cell_index];
}

void ACave::Kill(int cell_index)
{
     CurrentStep[cell_index] = false;
}

void ACave::Resurrect(int cell_index)
{
     CurrentStep[cell_index] = true;
}
#pragma endregion Cellular automata algorithm

void ACave::CreateBlock(FVector location)
{
     AActor *temp = DivineUtils::SpawnBP<AActor>(
          GetWorld(),
          AtomicBlock[0],
          location,
          FRotator(0, 0, 0), false, this, NULL
          );
     FAttachmentTransformRules rules(
          EAttachmentRule::KeepRelative,
          EAttachmentRule::KeepRelative,
          EAttachmentRule::KeepRelative,
          true
          );
     temp->AttachToActor(this, rules);
     cubes.Add(temp);
}

void ACave::Generate()
{
     TotalSize = NumBlocsX*NumBlocsY*NumBlocsZGen;
     FillModelWithCubes();
     CurrentStep = PreviousStep;
     for (int i = 0; i < NumberOfSteps; i++)
          PerformGenerationStep();
     PreviousStep = CurrentStep;
}

void ACave::Clear()
{
     for (AActor *a : cubes)
     {
          if (a && !a->IsPendingKillPending())
               a->Destroy();
     }
     PreviousStep.Empty();
     CurrentStep.Empty();
}

void ACave::Build()
{
     FVector position = GetTransform().GetLocation() - FVector(NumBlocsX*BlockSize, NumBlocsY*BlockSize, 0) / 2.0f;
     //position = FVector(0, 0, 0);
     int x, y, z;
     for (int i = 0; i < PreviousStep.Num(); i++)
     {
          x = i / (NumBlocsY*NumBlocsZGen);
          y = i / NumBlocsZGen % NumBlocsY;
          z = i%NumBlocsZGen;
          if (IsAlive(i))
          {
               if (AtomicBlock.Num() != 0)
               {
                    CreateBlock(position + FVector(x, y, -z) * BlockSize);
               }
          }
     }

     CaveSize3D = FVector(BlockSize*NumBlocsX, BlockSize*NumBlocsY, BlockSize*NumBlocsZGen);
     GenerationComplete = true;
}


void ACave::CreateWalls(FVector origin)
{
     //X - walls
     for (int i = 0; i < NumBlocsY; i++)
          for (int j = 0; j < NumBlocsZGen; j++)
          {
               CreateBlock(origin + FVector(-1, i, j)*BlockSize);
               CreateBlock(origin + FVector(NumBlocsX, i, j)*BlockSize);
          }

     //Y - walls
     for (int i = 0; i < NumBlocsX; i++)
          for (int j = 0; j < NumBlocsZGen; j++)
          {
               CreateBlock(origin + FVector(i, -1, j)*BlockSize);
               CreateBlock(origin + FVector(i, NumBlocsY, j)*BlockSize);
          }

     //X - walls
     for (int i = 0; i < NumBlocsX; i++)
          for (int j = 0; j < NumBlocsY; j++)
          {
               CreateBlock(origin + FVector(i, j, -1)*BlockSize);
               CreateBlock(origin + FVector(i, j, NumBlocsZGen)*BlockSize);
          }
}
