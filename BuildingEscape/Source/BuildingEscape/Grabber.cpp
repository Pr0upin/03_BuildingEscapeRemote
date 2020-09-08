// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabber.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "CollisionQueryParams.h"
#include "Components/PrimitiveComponent.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	FindPhysicsHandleComponent();
	SetupInputComponent();
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/// bShowDebugLine is an editable parameter of this component
	/// Draws a red line from the pawn until LineTraceEnd
	if (bShowDebugLine)
	{
		GetPlayerViewPoint();
		DrawDebugLine(
			GetWorld(),
			PlayerViewPointLocation,
			LineTraceEnd,
			FColor(255, 0, 0),
			false,
			0.0f,
			0,
			5.0f
		);
	}

	// snap grabbed object to LineTraceEnd
	if (PhysicsHandle != nullptr)
	{
		PhysicsHandle->SetTargetLocation(LineTraceEnd);
	}
}

// Find Physics Handle Component
void UGrabber::FindPhysicsHandleComponent()
{
	/// Look for attached physics handle component
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle)
	{
		UE_LOG(LogTemp, Error, TEXT("Physics Handle Component found on: %s"), *GetOwner()->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Physics Handle Component not found on: %s"), *GetOwner()->GetName());
	}
}

void UGrabber::SetupInputComponent()
{
	/// Look for attached Input component
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("Input Handle Component found on: %s"), *GetOwner()->GetName());
		/// Bind Input Axis
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Input Handle Component not found on: %s"), *GetOwner()->GetName());
	}
}

// Line Trace (ray-cast) from viewer's POV and return the first hit physics body
FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{
	GetPlayerViewPoint();

	/// Setup query parameters
	FHitResult Hit;
	FCollisionQueryParams TraceParameters(
		FName(TEXT("")),
		false,
		GetOwner()
	);

	//Line-Trace (AKA raycast) out to reach distance
	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		PlayerViewPointLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters
	);

	if (Hit.GetActor())
	{
		UE_LOG(LogTemp, Warning, TEXT("Traced Object: %s"),
			*Hit.GetActor()->GetName()
		)
	}
	return Hit;
}


// Calculate vector reaching out of the player view
void UGrabber::GetPlayerViewPoint() const
{
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);
	LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * DebugLineLength;
}

// Grab physics object
void UGrabber::Grab()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab Pressed"))

	// Check if any Physics object is in reach
	UPrimitiveComponent* ComponentToGrab = GetFirstPhysicsBodyInReach().GetComponent();
	
	// If a physics body was found, 'grab' it
	if (ComponentToGrab)
	{
		UE_LOG(LogTemp, Warning, TEXT("ComponentToGrab: %s"),
				*ComponentToGrab->GetOwner()->GetName()
			)

		/// With rotation un-constrained
		PhysicsHandle->GrabComponentAtLocation(
			ComponentToGrab,
			NAME_None,
			ComponentToGrab->GetOwner()->GetActorLocation()
		);

		/// With Rotation Constrained
		/*PhysicsHandle->GrabComponentAtLocationWithRotation(
			ComponentToGrab,
			NAME_None,
			ComponentToGrab->GetOwner()->GetActorLocation(),
			PlayerViewPointRotation
		);*/
	}
}

// Release grabbed physics object
void UGrabber::Release()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab Released"))
	PhysicsHandle->ReleaseComponent();
}