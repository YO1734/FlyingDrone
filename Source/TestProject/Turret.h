// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"

#include "Turret.generated.h"

UCLASS()
class TESTPROJECT_API ATurret : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATurret();

	UPROPERTY ( VisibleAnywhere, BlueprintReadOnly, Category = "Static Mesh" )
		UStaticMeshComponent* BaseMesh;
	UPROPERTY ( VisibleAnywhere, BlueprintReadOnly, Category = "Static Mesh" )
		UStaticMeshComponent* MovementMesh;

	UPROPERTY ( VisibleAnywhere, BlueprintReadOnly, Category = "Static Mesh" )
		UStaticMeshComponent* CannonMesh;
	UPROPERTY ( VisibleAnywhere, BlueprintReadOnly, Category = "CapsuleComponent" )
	class UCapsuleComponent* CapsuleComponent;

	UPROPERTY ( VisibleAnywhere, BlueprintReadOnly, Category = "Components" )
	UArrowComponent* ArrowComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Settings")
	float MaxLineTraceDistance = 1050.0f;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Settings")
		float RotationSpeed = 0.9f;
	int32 RotationDirection;
	bool bFoundDron = false;

	void PatrolAir (float DeltaTime);
	void Shoot (FVector SpawnLocation);
	void ResetShootState ();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool bCanShoot;
	FTimerHandle TimerHandle_Shoot;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
