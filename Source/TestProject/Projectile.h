// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Projectile.generated.h"


UCLASS()
class TESTPROJECT_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

	UPROPERTY ( EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh Component" )
	UStaticMeshComponent* ProjectileMesh;

    UProjectileMovementComponent* ProjectileMovementComponent;

	UFUNCTION()
	void OnProjectileOverlap (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void DestroyProjectile ();

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Capsule Component")
	class UCapsuleComponent* ProjectileCollision;
	
	FTimerHandle TimerHandleDestroy;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
