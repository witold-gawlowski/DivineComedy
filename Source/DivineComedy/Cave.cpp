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
     //Build();
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

     //int ResurrectionsNumber = 0;
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

void ACave::GenerateCave()
{
     Clear();
     Generate();
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

     //Improvements
     //if (parallel_neighbours > 1 && diagonal_neighbours > 7)||(parallel_neighbours > 3)
     //     return 26;
     
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
     CurrentStep[cell_index] = 26;// CountNeighbours(cell_index, true);
}
#pragma endregion Cellular automata algorithm

//
//void ACave::CreateBlock(UClass *block_type, FVector location, int n_n)
//{
//     AActor *temp = DivineUtils::SpawnBP<AActor>(
//          GetWorld(),
//          block_type,
//          location,
//          FRotator(0, 0, 0), false, this, NULL
//          );
//
//     temp->SetActorScale3D(FVector(BlocksScale, BlocksScale, BlocksScale));
//
//     FAttachmentTransformRules rules(
//          EAttachmentRule::KeepRelative,
//          EAttachmentRule::KeepRelative,
//          EAttachmentRule::KeepRelative,
//          true
//          );
//     temp->AttachToActor(this, rules);
//     cubes.Add(temp);
//}

void ACave::Generate()
{
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
     //for (AActor *a : cubes)
     //{
     //     if (a && !a->IsPendingKillPending())
     //          a->Destroy();
     //}
     PreviousStep.Empty();
     CurrentStep.Empty();
}
//
//void ACave::Build()
//{
//     FVector position = GetTransform().GetLocation() - FVector(NumBlocsX*BlockSize, NumBlocsY*BlockSize, 0) / 2.0f;
//
//     //position = FVector(0, 0, 0);
//     int x, y, z;
//     for (int i = 0; i < PreviousStep.Num(); i++)
//     {
//          x = i / (NumBlocsY*NumBlocsZGen);
//          y = i / NumBlocsZGen % NumBlocsY;
//          z = i%NumBlocsZGen;
//          if (IsAlive(i))
//          {
//               /*int neighbours_number = CountNeighbours(i, false);
//               UClass *block_type = NULL;
//               switch (neighbours_number)
//               {
//               case 1:
//                    block_type = OneSided;
//                    break;
//               case 2:
//                    block_type = TwoSided_Together;
//                    break;
//               case 3:
//                    block_type = ThreeSided_AllConnected;
//                    break;
//               case 4:
//                    block_type = FourSided_Together;
//                    break;
//               case 5:
//                    block_type = FiveSided;
//                    break;
//               case 6:
//                    block_type = SixSided;
//                    break;
//
//               }
//               if (block_type != NULL)*/
//               CreateBlock(SixSided, position + FVector(x, y, -z) * BlockSize, PreviousStep[i]);
//          }
//     }
//
//     CaveSize3D = BlockSize*BlocksScale*FVector(NumBlocsX, NumBlocsY, NumBlocsZGen);
//
//}
