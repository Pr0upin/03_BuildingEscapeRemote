// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDoor.h"
#include "Gameframework/Actor.h"
#include "Math/Rotator.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Controller.h"
#include "Components/PrimitiveComponent.h"

#define OUT

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
	Owner = GetOwner();
	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("%s missing pressure plate"), *Owner->GetName())
	}
}

void UOpenDoor::OpenDoor()
{
	// Owner->SetActorRotation(FRotator(0.0f, OpenAngle, 0.0f));
	OnOpenRequest.Broadcast();
}

void UOpenDoor::CloseDoor()
{
	Owner->SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Poll the trigger volume every frame
	if (GetTotalMassOfActorsOnPlate() > 50.0f) // TODO make into a parameter
	{
		OpenDoor();
		LastDoorOpenTime = 	GetWorld()->GetTimeSeconds();
	}

	//check if its time to close the door
	if (GetWorld()->GetTimeSeconds()> LastDoorOpenTime + DoorCloseDelay)
	{
		CloseDoor();
	}

}

float UOpenDoor::GetTotalMassOfActorsOnPlate()
{
	float TotalMass = 0.0f;

	// Find all the overlapping actors
	TArray<AActor*> OverlappingActors;
	if (!PressurePlate) { return TotalMass; }
	PressurePlate->GetOverlappingActors(OUT OverlappingActors); 

	// Iterate through them adding their masses
	for (const auto& AnActor : OverlappingActors)
	{
		UPrimitiveComponent& TheComponent = *AnActor->FindComponentByClass<UPrimitiveComponent>();
		//UE_LOG(LogTemp, Error, TEXT("Pressure plate has overlapped with: %s"), *AnActor->GetName());
		TotalMass += TheComponent.GetMass();
	}
	//UE_LOG(LogTemp, Error, TEXT("Total Mass: %f"), TotalMass);
	return TotalMass;
}