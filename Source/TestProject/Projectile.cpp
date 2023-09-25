// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Components/CapsuleComponent.h"
// Sets default values
AProjectile::AProjectile()
{
    Tags.Add ( "Projectile" );
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent> ( TEXT ( "Mesh Component" ) );
	RootComponent = ProjectileMesh;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/StarterContent/Shapes/Shape_Sphere"));
	if (MeshAsset.Succeeded())
	{
		ProjectileMesh->SetStaticMesh(MeshAsset.Object);
	}

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
    ProjectileMovementComponent->SetUpdatedComponent(ProjectileMesh); // Укажите компонент, который будет двигаться
    ProjectileMovementComponent->InitialSpeed = 5000.0f; // Начальная скорость снаряда
    ProjectileMovementComponent->MaxSpeed = 7000.0f; // Максимальная скорость
    ProjectileMovementComponent->ProjectileGravityScale = 0.0f; // Масштаб гравитации (если это необходимо)

    ProjectileCollision = CreateDefaultSubobject<UCapsuleComponent> ( TEXT ( "Collision" ) );

    ProjectileCollision->SetupAttachment ( ProjectileMesh );
    ProjectileCollision->SetRelativeScale3D ( FVector ( 0.2f, 0.2f, 0.2f ) );
    ProjectileMesh->SetRelativeScale3D ( FVector ( 0.1f, 0.1f, 0.1f ) );
    ProjectileMesh->SetSimulatePhysics ( true );
    
}

void AProjectile::OnProjectileOverlap ( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{
    if (OtherActor)
    {
        if (OtherActor->ActorHasTag ( "Turret" ))
        {
            
            OtherActor->Destroy ();
            Destroy ();
        }
        else if (OtherActor->ActorHasTag ( "Drone" ))
        {
            Destroy ();
        }
    }
    
}

void AProjectile::DestroyProjectile ()
{
    Destroy ();
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
    Super::BeginPlay();
    ProjectileCollision->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnProjectileOverlap);

      float DestroyDelay = 2.0f;
    GetWorldTimerManager().SetTimer(TimerHandleDestroy, this, &AProjectile::DestroyProjectile, DestroyDelay, false);
}


// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

