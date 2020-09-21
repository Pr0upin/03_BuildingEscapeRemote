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
		DrawDebugLine(
			GetWorld(),
			GetPlayerViewPointStart(),
			GetPlayerViewPointEnd(),
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
		PhysicsHandle->SetTargetLocation(GetPlayerViewPointEnd());
	}
}

// Find Physics Handle Component
void UGrabber::FindPhysicsHandleComponent()
{
	/// Look for attached physics handle component
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle != nullptr)
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
		GetPlayerViewPointStart(),
		GetPlayerViewPointEnd(),
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

// Update Current Viewpoint's location and rotation
void UGrabber::UpdateViewPoint() const
{
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);
}

// Calculate vector reaching out of the player view
FVector UGrabber::GetPlayerViewPointStart() const
{
	UpdateViewPoint();
	return PlayerViewPointLocation;
}

// Calculate vector reaching out of the player view
FVector UGrabber::GetPlayerViewPointEnd() const
{
	UpdateViewPoint();
	return PlayerViewPointLocation + PlayerViewPointRotation.Vector() * GrabDistance;
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

		if (!PhysicsHandle) { return; }
		/// With rotation un-constrained
		/*PhysicsHandle->GrabComponentAtLocation(
			ComponentToGrab,
			NAME_None,
			ComponentToGrab->GetOwner()->GetActorLocation()
		);*/

		/// With Rotation Constrained
		PhysicsHandle->GrabComponentAtLocationWithRotation(
			ComponentToGrab,
			NAME_None,
			ComponentToGrab->GetOwner()->GetActorLocation(),
			PlayerViewPointRotation
		);
	}
}

// Release grabbed physics object
void UGrabber::Release()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab Released"))
	if (!PhysicsHandle) { return; }
	PhysicsHandle->ReleaseComponent();
}