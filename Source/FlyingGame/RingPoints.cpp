// Fill out your copyright notice in the Description page of Project Settings.


#include "RingPoints.h"

void ARingPoints::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AFlyingGameCharacter* Character = Cast<AFlyingGameCharacter>(OtherActor))
	{
		Character->AddPoints(10);

		Destroy();

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Overlap Begin: Points Awarded");
	}
}
