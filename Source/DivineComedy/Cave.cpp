
#include "DivineComedy.h"
#include "Cave.h"
#include <time.h>

#define print1(XXX) UE_LOG (LogTemp, Warning, XXX);
#define print(XXX, YYY)  UE_LOG (LogTemp, Warning, XXX, YYY);

ACave::ACave () {
  PrimaryActorTick.bCanEverTick = true;
  distribution = new std::uniform_real_distribution<float> (0, 1);
}

void ACave::BeginPlay () {
  Super::BeginPlay ();
  Clear ();
  Generate ();
  Build ();
}
void ACave::Tick (float DeltaTime) {
  Super::Tick (DeltaTime);
}

//This function sets global seed value. 
void ACave::SeedRandom () {
  seed = time (NULL);
}

void ACave::FillModelWithCubes () {
  for ( int i = 0; i < TotalSize; i++ )
    PreviousStep.Add (Random (i / (NumBlocsY*NumBlocsZGen), i / NumBlocsZGen, i%NumBlocsZGen) <= Density ? true : false);
}

void ACave::PerformGenerationStep (bool RecreateModel = false) {
  for ( int i = 0; i < TotalSize; i++ ) {
    int neighbours_number = CountNeighbours (i);
    if ( IsAlive (i) ) {
      if ( neighbours_number <= LowerNeighboursCountDieThreshold ||
        neighbours_number > UpperNeighboursCountDieThreshold )
        Kill (i);
    } else if ( neighbours_number >= ResurrectionGapLowerLimit && neighbours_number <= ResurrectionGapUpperLimit )
      Resurrect (i);
  }

  //recreate model if from blueprint called
  if ( RecreateModel ) {
    Clear ();
    Build ();
  }
}

int ACave::CountNeighbours (int cell_index) {
  int neighbours = 0,
    x = cell_index / (NumBlocsY*NumBlocsZGen),
    y = cell_index / NumBlocsZGen % NumBlocsY,
    z = cell_index%NumBlocsZGen;
  for ( int i = -1; i <= 1; i++ )
    if ( i + x >= 0 && i + x < NumBlocsX ) {
      for ( int j = -1; j <= 1; j++ )
        if ( j + y >= 0 && j + y < NumBlocsY ) {
          for ( int k = -1; k <= 1; k++ )
            if ( k + z >= 0 && k + z < NumBlocsZGen ) {
              if ( PreviousStep[cell_index] )
                neighbours++;
            } else
              neighbours++;
        } else
          neighbours++;
    } else
      neighbours++;
    return neighbours;
}

bool ACave::IsAlive (int cell_index) {
  return PreviousStep[cell_index];
}

void ACave::Kill (int cell_index) {
  CurrentStep[cell_index] = false;
}

void ACave::Resurrect (int cell_index) {
  CurrentStep[cell_index] = true;
}

//This function assigns same random float from (0, 1) to each triple (x, y, z)
float ACave::Random (int x, int y, int z) {
  generator.seed (seed + x + y*NumBlocsX + z*NumBlocsX*NumBlocsY);
  return (*distribution) (generator);
}

void ACave::Generate () {
  TotalSize = NumBlocsX*NumBlocsY*NumBlocsZGen;
  FillModelWithCubes ();
  CurrentStep = PreviousStep;
  for ( int i = 0; i < NumberOfSteps; i++ )
    PerformGenerationStep ();
  PreviousStep = CurrentStep;
}

void ACave::Clear () {
  for ( AActor *a : cubes ) {
    if ( a && !a->IsPendingKillPending () )
      a->Destroy ();
  }
  PreviousStep.Empty ();
  CurrentStep.Empty ();
}

void ACave::Build () {
  //BlockSize = AtomicBlock->getsize
  FVector position = GetTransform ().GetLocation () - FVector (NumBlocsX*BlockSize, NumBlocsY*BlockSize, 0) / 2.0f;
  position = FVector (0, 0, 0);
  int x, y, z;
  for ( int i = 0; i < PreviousStep.Num (); i++ ) {
    x = i / (NumBlocsY*NumBlocsZGen);
    y = i / NumBlocsZGen % NumBlocsY;
    z = i%NumBlocsZGen;
    if ( IsAlive (i) ) {
      //print(TEXT("%d"), GridAt(x, y, z))
      if ( AtomicBlock != NULL) {
        FVector location =  (position + FVector (x, y, -z) * BlockSize - FVector (NumBlocsX*BlockSize / 2.f, NumBlocsY*BlockSize / 2.f, 0));
        AActor *temp = DivineUtils::SpawnBP<AActor> (
          GetWorld (),
          AtomicBlock,
          location,
          FRotator (0, 0, 0), false, this, NULL
          );
        FAttachmentTransformRules rules (
          EAttachmentRule::KeepRelative,
          EAttachmentRule::KeepRelative,
          EAttachmentRule::KeepRelative,
          true
        );
        temp->AttachToActor (this, rules);
        cubes.Add (temp);
      }
    }
  }
}

int ACave::seed;