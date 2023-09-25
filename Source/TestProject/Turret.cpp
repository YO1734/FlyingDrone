

// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "Projectile.h"
// Sets default values
ATurret::ATurret()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent> ( TEXT ( "StaticMesh" ) );
	RootComponent = BaseMesh;

	MovementMesh = CreateDefaultSubobject<UStaticMeshComponent> ( TEXT ( "MovementMesh" ) );
	MovementMesh->SetupAttachment ( RootComponent );

	CannonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CannonMesh"));
	CannonMesh->SetupAttachment ( MovementMesh );

    ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
    ArrowComponent->SetupAttachment(CannonMesh);



	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent> ( TEXT ( "CapsuleComponent" ) );
	CapsuleComponent->SetupAttachment ( RootComponent );

	 RotationDirection = FMath::RandBool() ? 1 : -1;

     bCanShoot = true;
}

void ATurret::PatrolAir(float DeltaTime)
{
    // Angle for Rotation
    FRotator TurretRotation = MovementMesh->GetRelativeRotation();
    FRotator TargetTurretRotation = TurretRotation;

    // Define the target yaw (e.g., 180 degrees or -190 degrees)
    float TargetYaw = 0.0f; // Initialize with 0 degrees

    // Check if we have reached the target yaw, if yes, reverse direction
    if (FMath::IsNearlyEqual(TurretRotation.Yaw, 180.0f, 1.0f) || FMath::IsNearlyEqual(TurretRotation.Yaw, -180.0f, 1.0f))
    {
        RotationDirection *= -1; // Reverse the rotation direction
    }

    // Calculate the new target yaw based on the rotation direction
    TargetYaw = TurretRotation.Yaw + RotationDirection * 180.0f;

    // Interpolate the turret rotation smoothly and quickly
    float RotationInterpSpeed = 1.0f; // You can adjust the speed as needed
    TargetTurretRotation.Yaw = FMath::FInterpTo(TurretRotation.Yaw, TargetYaw, DeltaTime, RotationInterpSpeed);

    // Apply the new turret rotation
    MovementMesh->SetRelativeRotation(TargetTurretRotation);

    FVector StartLocation = GetActorLocation();
    StartLocation.Z += 150; // Append to CannonMesh level

    // End Location from MovementMesh to Object
    FVector ForwardVector = MovementMesh->GetRightVector();
    FVector EndLocation = StartLocation + (ForwardVector * MaxLineTraceDistance);
    UE_LOG(LogTemp, Warning, TEXT("ArrowComponent Rotation: %s"), *ArrowComponent->GetComponentRotation().ToString());

    // LineTrace Params
    FHitResult HitResult;
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(this);

    if (GetWorld ()->LineTraceSingleByChannel ( HitResult, StartLocation, EndLocation, ECC_Visibility, CollisionParams ))
    {
        AActor* HitActor = HitResult.GetActor ();
        if (HitActor && HitActor->ActorHasTag ( "Drone" ))
        {
            bFoundDron = true;
                if (bFoundDron && bCanShoot)
                {
                    Shoot ( StartLocation );
                    

                    bCanShoot = false;
                    // Timer for Reaload
                    GetWorldTimerManager ().SetTimer ( TimerHandle_Shoot, this, &ATurret::ResetShootState, 0.3f, false );
                
                }
        }
        bFoundDron = false;
    }
}


void ATurret::ResetShootState()
{
    bCanShoot = true;
}

void ATurret::Shoot (FVector SpawnLocation)
{
    if (ProjectileClass)
    {
        FRotator SpawnRotation = FRotationMatrix::MakeFromX ( MovementMesh->GetRightVector()).Rotator ();
        SpawnLocation.Y = ArrowComponent->GetComponentLocation ().Y;
        SpawnLocation.X = ArrowComponent->GetComponentLocation ().X;
        SpawnLocation.Z = ArrowComponent->GetComponentLocation ().Z;
        // Спавним снаряд в мире.
        AProjectile* Projectile = GetWorld ()->SpawnActor<AProjectile> ( ProjectileClass, SpawnLocation, SpawnRotation );
    }
}



// Called when the game starts or when spawned
void ATurret::BeginPlay()
{
    Super::BeginPlay();


}

// Called every frame
void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    PatrolAir ( DeltaTime );
}

