// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "FlyingGameCharacter.h"
#include "Ring1.generated.h"


UCLASS()
class FLYINGGAME_API ARing1 : public AActor
{
	GENERATED_BODY()

	
public:	
	// Sets default values for this actor's properties
	ARing1();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UBoxComponent* CollisionBox;
	
	/*UPROPERTY(VisibleAnywhere, BlueprintReadWrite);
	UStaticMeshComponent* StaticMesh;*/
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite);
	AFlyingGameCharacter* GameCharacter;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:

};
