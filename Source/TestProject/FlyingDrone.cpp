// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingDrone.h"
#include "Components/CapsuleComponent.h"
#include "Math/UnrealMathUtility.h"
#include "HealthWidget.h"
#include "Projectile.h"

// Sets default values
AFlyingDrone::AFlyingDrone()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    //DronCollisionComponent = CreateDefaultSubobject<UCapsuleComponent> ( TEXT ( "DronCollision" ) );
    //RootComponent = DronCollisionComponent;


	DroneMeshComponent = CreateDefaultSubobject<UStaticMeshComponent> ( TEXT ( "DroneMesh" ) );
    RootComponent = DroneMeshComponent;
	FlyingMovementComponent = CreateDefaultSubobject<UFlyingMovementComponent>(TEXT("FlyingMovementComponent"));
    FlyingMovementComponent->UpdatedComponent = RootComponent; // Привязываем к корневому компоненту
    FlyingMovementComponent->MovementState.bCanFly;
    FlyingMovementComponent->GetPathFollowingBrakingDistance ( 50 );
    
    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
    SpringArmComponent->SetupAttachment ( RootComponent );
    SpringArmComponent->TargetArmLength = 300.0f;
    SpringArmComponent->SetRelativeRotation ( FRotator ( -50.0f, 0.0f, 0.0f ) ); //Starter Rotation
    SpringArmComponent->bDoCollisionTest = true;
	SpringArmComponent->bClampToMaxPhysicsDeltaTime = false;
    SpringArmComponent->bInheritPitch = false;
    SpringArmComponent->bInheritRoll = false;
    SpringArmComponent->bInheritYaw = false;
	SpringArmComponent->CameraLagMaxDistance = 300.0f;
	SpringArmComponent->CameraLagMaxTimeStep = 0.2f;

	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
    ThirdPersonCamera->SetupAttachment(SpringArmComponent);
	ThirdPersonCamera->SetRelativeRotation ( FRotator ( 0.0f, 0.0f, 0.0f ) );

    CameraCollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CameraCollisionComponent"));
    CameraCollisionComponent->SetupAttachment ( ThirdPersonCamera );
    CameraCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
    ArrowComponent->SetupAttachment ( RootComponent ); 
  
    HealthPoint = 50.0f;

    MainWidget = nullptr;
    
}

// Called when the game starts or when spawned
void AFlyingDrone::BeginPlay ()
{
    Super::BeginPlay ();
    DroneMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &AFlyingDrone::OnDroneOverlap);
    MainWidget->UpdateHealthBar ( HealthPoint );
    MainWidget->UpdateAmmoBar ( Ammo );
}

// Called every frame
void AFlyingDrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFlyingDrone::Turn(float Value)
{
    bool bShouldInterpolateCameraRollBack;
    float RollInterpolationSpeed = 0.8f;

    FRotator SpringArmRotation = SpringArmComponent->GetComponentRotation();

    // Changes Only for Yaw
    SpringArmRotation.Yaw += Value * RotationSpeed;

    //// Устанавливаем новую ротацию SpringArm с измененным Yaw и Roll
    SpringArmComponent->SetWorldRotation(SpringArmRotation);

    // Check Roll Angle, if it non 0,  start Interpolate block
    if (!FMath::IsNearlyZero(SpringArmRotation.Roll, 1e-2f))
    {
    
        bShouldInterpolateCameraRollBack = true;
    }
    if (bShouldInterpolateCameraRollBack)
    {
        float InterpolatedRoll = FMath::FInterpTo(SpringArmRotation.Roll, 0.0f, GetWorld()->GetDeltaSeconds(), RollInterpolationSpeed);
        SpringArmRotation.Roll = InterpolatedRoll;


        if (FMath::IsNearlyZero(SpringArmRotation.Roll, 1e-2f))
        {
            bShouldInterpolateCameraRollBack = false;
        }


        SpringArmComponent->SetWorldRotation(SpringArmRotation);
    }
    //End
}


void AFlyingDrone::LookUp(float Value)
{
    if(Value!=0)
    {
        FRotator CameraRotation = SpringArmComponent->GetComponentRotation ();

        FVector Start = ThirdPersonCamera->GetComponentLocation ();
        FVector ForwardVector = ThirdPersonCamera->GetForwardVector ();
        FVector End = Start + ForwardVector * CameraCollisionDistance;

        FHitResult HitResult;
        FCollisionQueryParams CollisionParams;
        CollisionParams.AddIgnoredActor ( this );

        // Checking the collision of the beam with the camera
        if (!GetWorld ()->LineTraceSingleByChannel ( HitResult, Start, End, ECC_Visibility, CollisionParams ))
        {
            CameraRotation.Pitch += Value * RotationSpeed;
            
        }
        else
        {
            // If there is a collision, interpolate the camera position to a safe location
            FVector SafeLocation = HitResult.ImpactPoint - ForwardVector * CameraCollisionDistance;
            FVector NewLocation = FMath::VInterpTo ( Start, SafeLocation, GetWorld ()->GetDeltaSeconds (), CameraCollisionInterpSpeed );
            ThirdPersonCamera->SetWorldLocation ( NewLocation );
            
        }

        // Limit the Pitch angle within specified limits (for example, from -80 to 80 degrees)
        CameraRotation.Pitch = FMath::ClampAngle ( CameraRotation.Pitch, -80.0f, 80.0f );

        // Set new Angle for Camera
        SpringArmComponent->SetWorldRotation ( CameraRotation );
    }
}




void AFlyingDrone::MoveUp(float Value)
{
    if(Value!=0)
    {
        FRotator CurrentRotation = DroneMeshComponent->GetRelativeRotation ();
        CurrentRotation.Normalize ();
        DroneMeshComponent->SetRelativeRotation ( CurrentRotation );
        if (CurrentRotation.Roll > 90 || CurrentRotation.Roll < -90 || CurrentRotation.Pitch > 80 || CurrentRotation.Pitch < -80)
        {
            return;
        }
        FVector UpVector ( 0.0f, 0.0f, 1.0f ); // Movement Vector Z
        FVector CurrentLocation = GetActorLocation ();
        float CurrentHeight = CurrentLocation.Z;
        if(Value>0)
            AddMovementInput ( GetActorUpVector (), Value, true );
    }
   
}


void AFlyingDrone::MoveForward(float Value)
{
    // Maximum drone angle (in degrees)
    const float MaxTiltAngle = 40.0f;

    // Drone tilt speed (in degrees per second)
    const float TiltSpeed = 5.0f;

    
    FRotator CurrentRotation = GetActorRotation();

    // Calculate the angle of inclination, limiting it to the maximum value
    float TargetTilt = FMath::Clamp(Value, -1.0f, 1.0f) * MaxTiltAngle;

    // Interpolate the current tilt angle to the target angle
    float InterpolatedTilt = FMath::FInterpTo(CurrentRotation.Roll, TargetTilt, GetWorld()->GetDeltaSeconds(), TiltSpeed);

    // Set a new drone rotation taking into account the tilt
    SetActorRotation(FRotator(CurrentRotation.Pitch, CurrentRotation.Yaw, InterpolatedTilt));

    // If there is no input, the drone returns to its original position
    if (FMath::IsNearlyZero(Value))
    {
        // Interpolate the current tilt angle back to 0
        float InterpolatedTiltBack = FMath::FInterpTo(InterpolatedTilt, 0.0f, GetWorld()->GetDeltaSeconds(), TiltSpeed);

      
        SetActorRotation(FRotator(CurrentRotation.Pitch, CurrentRotation.Yaw, InterpolatedTiltBack));
    }

    // Apply forward and backward movement
    FVector ForwardVector = GetActorRightVector();
    AddMovementInput(ForwardVector, Value * 0.7);
}



void AFlyingDrone::RotateRight ( float Value )
{
    AddActorLocalRotation( FRotator (0.0f,  Value, 0.0f ) );
}

void AFlyingDrone::MoveRight(float Value)
{
    // Maximum drone angle (in degrees)
    const float MaxTiltAngle = 45.0f;

    // Drone tilt speed (in degrees per second)
    const float TiltSpeed = 10.0f;


    FRotator CurrentRotation = GetActorRotation();

    // Calculate the angle of inclination, limiting it to the maximum value
    float TargetTilt = FMath::Clamp(Value, -1.0f, 1.0f) * MaxTiltAngle;

    // Interpolate the current tilt angle to the target angle
    float InterpolatedTilt = FMath::FInterpTo(CurrentRotation.Pitch, TargetTilt, GetWorld()->GetDeltaSeconds(), TiltSpeed);

    // Set a new drone rotation taking into account the tilt
    SetActorRotation(FRotator(InterpolatedTilt, CurrentRotation.Yaw, CurrentRotation.Roll));

    // If there is no input, the drone returns to its original position
    if (FMath::IsNearlyZero(Value))
    {
        // Interpolate the current tilt angle back to 0
        float InterpolatedTiltBack = FMath::FInterpTo(CurrentRotation.Pitch, 0.0f, GetWorld()->GetDeltaSeconds(), TiltSpeed);

        // Set a new rotation for the drone to return to its original position
        SetActorRotation(FRotator(InterpolatedTiltBack, CurrentRotation.Yaw, CurrentRotation.Roll));
    }

    // Apply movement in the selected direction
    FVector RightVector = GetActorForwardVector();
    AddMovementInput(RightVector, -Value);
}






void AFlyingDrone::PlaneDown ( float Value )
{
    FVector DownVector  (0.0f, 0.0f, -1.0f);
    if (Value != 0)
    {
        AddMovementInput ( DownVector, Value * HeightInterpolationFactor );
    }
}

void AFlyingDrone::UpdateHealth (float Value)
{
    if (HealthPoint+Value <= 0)
    {
        HealthPoint += Value;
        MainWidget->UpdateHealthBar ( HealthPoint );
        Destroy ();
        Restart ();
    }
    else
    {
        HealthPoint += Value;
        if (MainWidget)
        {
            MainWidget->UpdateHealthBar ( HealthPoint );
        }


    }

}

void AFlyingDrone::UpdateAmmo ()
{
    MainWidget->UpdateAmmoBar ( Ammo );
}

void AFlyingDrone::Shoot ()
{
    if (Ammo > 0)
    {
       // Check that ProjectileClass is installed in the editor.
        if (ProjectileClass)
        {
          // Get the current position and direction of the drone.
            
            FVector SpawnLocation = ArrowComponent->GetComponentLocation();
            
            FRotator SpawnRotation ( -90.0f, 0.0f, 0.0f );
            
           // Spawn the projectile in the world.
            AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);

            // Check that the projectile has spawned successfully.
            if (Projectile)
            {
                // Reduce the number of Ammo
                Ammo--;
                UpdateAmmo ();
            }
        }
    }
}


void AFlyingDrone::OnDroneOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // Check which object overlapped
    if (OtherActor)
    {
        if (OtherActor->ActorHasTag("HealthPickup"))
        {
            
            UpdateHealth (25);
            OtherActor->Destroy ();
        }
        else if (OtherActor->ActorHasTag ( "Ammo" ))
        {
            Ammo += 4;
            UpdateAmmo ();
            OtherActor->Destroy ();
        }
        else if (OtherActor->ActorHasTag ( "Projectile" ))
        {
            UpdateHealth ( -25 );
        }
    }
}



// Called to bind functionality to input
void AFlyingDrone::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis ( "LookUp", this, &AFlyingDrone::LookUp );
	PlayerInputComponent->BindAxis ( "Turn", this, &AFlyingDrone::Turn );
	PlayerInputComponent->BindAxis ( "Fly", this, &AFlyingDrone::MoveUp );
	PlayerInputComponent->BindAxis ( "MoveForward", this, &AFlyingDrone::MoveForward );
	PlayerInputComponent->BindAxis ( "RotateRight", this, &AFlyingDrone::RotateRight );
    PlayerInputComponent->BindAxis ( "MoveRight", this, &AFlyingDrone::MoveRight );
    PlayerInputComponent->BindAxis ( "PlaneDown", this, &AFlyingDrone::PlaneDown);
	PlayerInputComponent->BindAction ( TEXT ( "Shoot" ), EInputEvent::IE_Released, this, &AFlyingDrone::Shoot );
}

