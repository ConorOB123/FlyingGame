// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ring1.h"
#include "RingPoints.generated.h"

/**
 * 
 */
UCLASS()
class FLYINGGAME_API ARingPoints : public ARing1
{
	GENERATED_BODY()
	
public:

	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
