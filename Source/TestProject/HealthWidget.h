// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam ( FOnHealthUpdated, float, Health );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam ( FOnAmmoUpdated, int, Ammo );

UCLASS()
class TESTPROJECT_API UHealthWidget : public UUserWidget
{
	GENERATED_BODY ()
	

public:

	UFUNCTION ()
		void UpdateHealthBar ( float Value );

	UFUNCTION ()
		void UpdateAmmoBar ( int Value );

	UPROPERTY(BlueprintAssignable, BlueprintReadWrite, Category="Hp")
	FOnHealthUpdated OnHealthUpdated;
	
	UPROPERTY(BlueprintAssignable, BlueprintReadWrite, Category="Hp")
	FOnAmmoUpdated OnAmmoUpdated;

};
