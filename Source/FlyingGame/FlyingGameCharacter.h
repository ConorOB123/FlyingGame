// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "FlyingGameCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AFlyingGameCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Flying Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Fly;

	UInputMappingContext* InputMapping;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

public:
	AFlyingGameCharacter();
	
	bool bIsFastFlying;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float MaxFlyTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CurrentFlyTime;

	// Initialise points for player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Points")
	int32 PlayersPoints = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	FTimerHandle TH_CountDown;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "seconds")
	int seconds = 20;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "minutes")
	int minutes = 1;

	// function to add points
	void AddPoints(int32 PointsToAdd);

	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			
	// flying
	//void fly();

	void ToggleFly();

	void StartFlying();

	void StopFlying();

	void StartAscending();

	void StopAscending();

	void StartDescending();
	
	void StopDescending();

	void FastFlyStart();
	void FastFlyStop();

	bool bIsAscending;
	bool bIsDescending;

	

	void UpdateFlytime();


	UPROPERTY(EditAnywhere, Category = "Movement")
	float FlyTimeDrain;


	bool bHasFlyTime;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime);

	// Win State
	virtual void WinScreen();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	void CountDown();
};

