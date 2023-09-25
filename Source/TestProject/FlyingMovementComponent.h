// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "FlyingMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class TESTPROJECT_API UFlyingMovementComponent : public UFloatingPawnMovement
{
	GENERATED_BODY ()
private:
	float CurrentAltitude = 0.0f;
};
