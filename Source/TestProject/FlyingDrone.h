// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "FlyingMovementComponent.h"
#include "Components/ArrowComponent.h"
#include "FlyingDrone.generated.h"

UCLASS()
class TESTPROJECT_API AFlyingDrone : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AFlyingDrone();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	UStaticMeshComponent* DroneMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* ThirdPersonCamera;

	UPROPERTY ( VisibleAnywhere, BlueprintReadOnly, Category = "Camera" )
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    UFlyingMovementComponent* FlyingMovementComponent;

	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* CameraCollisionComponent;

	UPROPERTY(EditAnywhere, Category = "Projectile")
		TSubclassOf<class AProjectile> ProjectileClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* ArrowComponent;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
		float MaxFlightHeight = 10000.0f;

	UPROPERTY ( EditAnywhere, BlueprintReadOnly, Category = "Movement" )
	float HeightInterpolationFactor = 1.5f;

	UPROPERTY ( EditAnywhere, BlueprintReadWrite, Category = "Widget" )
	class UHealthWidget* MainWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HealthPoint;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	float  RotationSpeed = 1.5f;
	float  UpSpeed = 1.0f;
	float CameraCollisionDistance = 50.0f;
	float CameraCollisionInterpSpeed = 15.0f;

	int Ammo = 5;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void Turn ( float Value );
	void LookUp ( float Value );
	void MoveUp (float Value);
	void MoveForward ( float Value );
	void RotateRight ( float Value );
	void MoveRight ( float Value );
	void PlaneDown ( float Value );
	void UpdateHealth (float Value);
	void UpdateAmmo ();
	void Shoot ();
	void InterpolateCamera ();
	UFUNCTION ()
	float GetCurrentHP(){ return HealthPoint; };

	UFUNCTION()
	void OnDroneOverlap (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
