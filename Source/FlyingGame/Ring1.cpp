// Fill out your copyright notice in the Description page of Project Settings.


#include "Ring1.h"
#include "Components/BoxComponent.h"
#include "Engine/Engine.h"
// Sets default values
ARing1::ARing1()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Creating the Static mesh for the rings
	//StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	//RootComponent = StaticMesh;
	// Creating Collision box for the Rings
	
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	CollisionBox->SetBoxExtent(FVector(32.f, 32.f, 32.f));
	CollisionBox->SetCollisionProfileName("Trigger");
	RootComponent = CollisionBox;

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ARing1::OnOverlapBegin);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &ARing1::OnOverlapEnd);
}

// Called when the game starts or when spawned
void ARing1::BeginPlay()
{
	Super::BeginPlay();
	


}

// Called every frame
void ARing1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARing1::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Overlap Begin Function Called");
	
	if (AFlyingGameCharacter* Character = Cast<AFlyingGameCharacter>(OtherActor))
	{
		Character->AddPoints(10);

		Character->CurrentFlyTime = 100.0f;

		Destroy();

		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Overlap Begin: Points Awarded");
	}	

}

void ARing1::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Overlap End Function Called");
}

