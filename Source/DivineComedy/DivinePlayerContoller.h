// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "DivinePlayerContoller.generated.h"

/**
 * 
 */
UCLASS()
class DIVINECOMEDY_API ADivinePlayerContoller : public APlayerController
{
  GENERATED_BODY ()
    void UpdateRotation (float DeltaTime) override;
	
	
	
};
