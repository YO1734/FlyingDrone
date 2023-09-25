// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthWidget.h"
#include "FlyingDrone.h"

void UHealthWidget::UpdateHealthBar ( float Value )
{
	OnHealthUpdated.Broadcast (Value);
}

void UHealthWidget::UpdateAmmoBar ( int Value )
{
	OnAmmoUpdated.Broadcast ( Value );
}
