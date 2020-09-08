// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/InputComponent.h"
// generated headers must come last!
#include "Grabber.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDINGESCAPE_API UGrabber : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabber();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
		bool bShowDebugLine = true;

	UPROPERTY(EditAnywhere)
		uint16 DebugLineLength = 100;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// initialized with nullpointer
	// as we cannot guarantee the component is going to be loaded yet
	UPhysicsHandleComponent* PhysicsHandle = nullptr;
	UInputComponent* InputComponent = nullptr;
	mutable FVector PlayerViewPointLocation;
	mutable FRotator PlayerViewPointRotation;
	mutable FVector LineTraceEnd;

	// Cast ray and grab what's in reach
	void Grab();

	// Called when grab Input is released
	void Release();

	// Find attached physics handle component
	void FindPhysicsHandleComponent();

	// setup assumed attached input component
	void SetupInputComponent();

	// Return hit for first physics body in reach
	FHitResult GetFirstPhysicsBodyInReach() const;

	// Get player viewPoint
	void GetPlayerViewPoint() const;
};
