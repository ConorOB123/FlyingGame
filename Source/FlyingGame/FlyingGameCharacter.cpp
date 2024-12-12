// Copyright Epic Games, Inc. All Rights Reserved.

#include "FlyingGameCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "InputActionValue.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AFlyingGameCharacter

AFlyingGameCharacter::AFlyingGameCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->BrakingDecelerationFlying = 1500.0f;
	GetCharacterMovement()->GravityScale = 0.0f;
	GetCharacterMovement()->MaxFlySpeed = 1000.f;
	MaxFlyTime = 100.0f;
	CurrentFlyTime = 100.0f;
	FlyTimeDrain = 0.75f;
	bIsAscending = false;
	bIsDescending = false;
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AFlyingGameCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(TH_CountDown, this, &AFlyingGameCharacter::CountDown, 1.0f, true, 1.0f);


}
void AFlyingGameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Flying)
	{
		FVector FlyDirection = FVector::ZeroVector;

		// Apply upward movement if ascending
		if (bIsAscending)
		{
			FlyDirection.Z += 1.0f;  // Move up
		}

		// Apply downward movement if descending
		if (bIsDescending)
		{
			FlyDirection.Z -= 1.0f;  // Move down
		}

		// Apply the movement to the character
		AddMovementInput(FlyDirection);
	}

	UpdateFlytime();

	WinScreen();

}
//////////////////////////////////////////////////////////////////////////
// Input

void AFlyingGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	// toggling between flying and walking
	PlayerInputComponent->BindKey(EKeys::F, IE_Pressed, this, &AFlyingGameCharacter::ToggleFly);


	// Fast Flying button
	PlayerInputComponent->BindKey(EKeys::LeftShift, IE_Pressed, this, &AFlyingGameCharacter::FastFlyStart);
	PlayerInputComponent->BindKey(EKeys::LeftShift, IE_Released, this, &AFlyingGameCharacter::FastFlyStop);
	// upward movement using Space Bar
	PlayerInputComponent->BindKey(EKeys::SpaceBar, IE_Pressed, this, &AFlyingGameCharacter::StartAscending);
	PlayerInputComponent->BindKey(EKeys::SpaceBar, IE_Released, this, &AFlyingGameCharacter::StopAscending);


	// downward using left ctrl
	PlayerInputComponent->BindKey(EKeys::Q, IE_Pressed, this, &AFlyingGameCharacter::StartDescending);
	PlayerInputComponent->BindKey(EKeys::Q, IE_Released, this, &AFlyingGameCharacter::StopDescending);


	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		// Flying
		EnhancedInputComponent->BindAction(IA_Fly, ETriggerEvent::Started, this, &AFlyingGameCharacter::ToggleFly);
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFlyingGameCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFlyingGameCharacter::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}
void AFlyingGameCharacter::ToggleFly()
{
	if (GetCharacterMovement())
	{
		if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Flying)
		{
			StopFlying();
		}
		else
		{
			StartFlying();
		}
	}
}

void AFlyingGameCharacter::StartFlying()
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	}
}

void AFlyingGameCharacter::StopFlying()
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}
}
void AFlyingGameCharacter::StartAscending() {
	bIsAscending = true;
}

void AFlyingGameCharacter::StopAscending() {
	bIsAscending = false;
}

void AFlyingGameCharacter::StartDescending() {
	bIsDescending = true;
}

void AFlyingGameCharacter::StopDescending() {
	bIsDescending = false;
}
void AFlyingGameCharacter::FastFlyStart() {
	
	if (bHasFlyTime)
	{
		GetCharacterMovement()->MaxFlySpeed = 2000.0f;
		if (GetVelocity().Size() >= 0.5f)
		{
			bIsFastFlying = true;
		}
		else
		{
			bIsFastFlying = false;
		}
	}

}
void AFlyingGameCharacter::FastFlyStop()
{
	
		GetCharacterMovement()->MaxFlySpeed = 1000.f;

		bIsFastFlying = false;
}
void AFlyingGameCharacter::UpdateFlytime()
{
	if (bIsFastFlying)
	{
		CurrentFlyTime -= FlyTimeDrain;
	}

	

	if (CurrentFlyTime <= 0)
	{
		bHasFlyTime = false;
		FastFlyStop();
	}
	else
	{
		bHasFlyTime = true;
	}
}



void AFlyingGameCharacter::AddPoints(int32 PointsToAdd)
{
	PlayersPoints += PointsToAdd;

	if (GEngine)
	{
		/*GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Magenta,
			FString::Printf(TEXT("Points: %d"), PlayersPoints));*/
	}
}

void AFlyingGameCharacter::WinScreen()
{
	if (PlayersPoints >= 100)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, "You Win!");
		
		
	/*	if (IsValid(WidgetClass))
		{
			TextWidget = Cast<UTextWidget>(CreateWidget(GetWorld(), WidgetClass));

			if (TextWidget != nullptr)
			{
				
			}
		}*/

	}
}

void AFlyingGameCharacter::CountDown()
{
	if (seconds > 0)
	{
		--seconds;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Magenta,
			FString::Printf(TEXT("Seconds: %d"), seconds));
	}
	else
	{
		
		
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Magenta,
			FString::Printf(TEXT("Minutes: %d"), minutes));
		if (minutes >= 0)
		{
			--minutes;
			seconds = 59.0f;
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Magenta,
				FString::Printf(TEXT("Game Begin")));
		}
	}
}


void AFlyingGameCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
		
	}
}

void AFlyingGameCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}