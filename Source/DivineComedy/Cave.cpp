#include "DivineComedy.h"
#include "Cave.h"
#include <time.h>

#define print1(XXX) UE_LOG (LogTemp, Warning, XXX);
#define print(XXX, YYY)  UE_LOG (LogTemp, Warning, XXX, YYY);

ACave::ACave(){
	PrimaryActorTick.bCanEverTick = true;
  distribution = new std::uniform_real_distribution<float> (0, 1);
}
void ACave::OnConstruction (const FTransform & Transform) {
  Clear ();
  Generate ();
  Build ();
}
void ACave::BeginPlay(){
	Super::BeginPlay(); 
}
void ACave::Tick( float DeltaTime ){
	Super::Tick( DeltaTime );
}
//This function sets global seed value. 
void ACave::SeedRandom () {
  seed = time (NULL);
}

//This function assigns same random float from (0, 1) to each triple (x, y, z)
float ACave::Random (int x, int y, int z) {
  generator.seed (seed + x+y*WidthX+z*WidthX*WidthY);
  return (*distribution) (generator);
}
int& ACave::GridAt (int x, int y, int z) {
  check (x < WidthX)
    check (y < WidthY)
    check (z < GLength)
  return grid[x+y*WidthX+z*WidthX*WidthY];
}

void ACave::Clear () {
  for ( AActor *a : cubes ) {
    if(a && ! a->IsPendingKillPending())
      a->Destroy ();
  }
  grid.Empty ();
}

void ACave::Generate () {
  for ( int i = 0; i < WidthX; i++ ) 
    for ( int j = 0; j < WidthY; j++ ) 
      for ( int k = 0; k < GLength; k++ ) {
   
        grid.Add (Random (i, j, k) <= density ? 1 : 0);
      }
}

void ACave::Build () {
  FVector position = GetTransform ().GetLocation ();
  for ( int i = 0; i < WidthX; i++ ) 
    for ( int j = 0; j < WidthY; j++ ) 
      for ( int k = 0; k < GLength; k++ ) {
        
        if ( GridAt (i, j, k) ) {
          print (TEXT ("%d"), GridAt (i, j, k))
          AActor *temp = DivineUtils::SpawnBP<AActor> (
            GetWorld (),
            AtomicBlock,
            position + FVector (i, j, k)*BlockSize,
            FRotator (), true, NULL, NULL
            );
          cubes.Add (temp);
        }
      }
}

int ACave::seed;