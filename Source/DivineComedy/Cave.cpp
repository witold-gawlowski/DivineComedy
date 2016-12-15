#include "DivineComedy.h"
#include "Cave.h"
#include <time.h>
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::White,text)

ACave::ACave()
{
     PrimaryActorTick.bCanEverTick = true;
     distribution = new std::uniform_real_distribution<float>(0, 1);
}

void ACave::BeginPlay()
{
     Super::BeginPlay();
     GenerateCave();
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
          PreviousStep.Add(Random(i / (NumBlocsY*NumBlocsZGen), i / NumBlocsZGen, i%NumBlocsZGen) <= Density ? 26 : 0);
}

void ACave::PerformGenerationStep()
{
     for (int i = 0; i < TotalSize; i++)
     {
          int neighbours_number = CountNeighbours(i,true);
          if (IsAlive(i))
          {
               if (neighbours_number < DiesIfHasLowerNumberOfNeighboursThan)
                    Kill(i);
          }
          else if (neighbours_number > ResurrectsIfHasHigherNumberOfNeighboursThan)
          {
               Resurrect(i);
          }
     }
}

int ACave::CountNeighbours(int cell_index, bool count_diagonal)
{
     int diagonal_neighbours = 0,
          parallel_neighbours = 0,
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
                         {
                              //exclude center
                              if (i != j || j != k || i != k)
                              {
                                   if (k + z >= 0 && k + z < NumBlocsZGen)
                                   {
                                        if (IsAlive((i + x)*(NumBlocsY*NumBlocsZGen) + (y + j)*NumBlocsZGen + z + k))
                                        {
                                             if ((i + j + k) % 2 == 1)
                                                  diagonal_neighbours++;
                                             else
                                                  parallel_neighbours++;
                                        }
                                   }
                                   else
                                   {
                                        if ((i + j) % 2 == 1)
                                             diagonal_neighbours++;
                                        else
                                             parallel_neighbours++;
                                   }
                              }
                         }
                    }
                    else
                    {
                         diagonal_neighbours += 2;
                         parallel_neighbours++;
                    }
          }
          else
          {
               diagonal_neighbours += 8;
               parallel_neighbours++;
          }
     
     if (count_diagonal)
          return (diagonal_neighbours + parallel_neighbours);
     else
          return parallel_neighbours;
}

bool ACave::IsAlive(int cell_index)
{
     if (PreviousStep[cell_index] != 0)
          return true;
     else
          return false;
}

void ACave::Kill(int cell_index)
{
     CurrentStep[cell_index] = 0;
}

void ACave::Resurrect(int cell_index)
{
     CurrentStep[cell_index] = 26;
}
#pragma endregion Cellular automata algorithm

void ACave::GenerateCave()
{
     Clear();

     TotalSize = NumBlocsX*NumBlocsY*NumBlocsZGen;
     FillModelWithCubes();
     CurrentStep = PreviousStep;

     for (int i = 0; i < NumberOfSteps; i++)
     {
          PerformGenerationStep();
          PreviousStep = CurrentStep;
     }

     for (int i = 0; i < TotalSize; i++)
     {
          CaveArray.Add(IsAlive(i));
     }
}

void ACave::Clear()
{
     CaveArray.Empty();
     PreviousStep.Empty();
     CurrentStep.Empty();
}