// Copyright Maanav Rao 2020

#include "Grabber.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	//Checking for PhysicsHandleComponent
	FindPhysicsHandle();

	SetupInputComponent();
}

void UGrabber::SetupInputComponent()
{
	Input = GetOwner()->FindComponentByClass<UInputComponent>();
	if (Input)
	{
		Input->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		Input->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s has the Grabber component on it, but not the input component"), *GetOwner()->GetName());
	}
}

//Finds the physics handle component
void UGrabber::FindPhysicsHandle()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has the Grabber component on it, but not the Physics Handle component"), *GetOwner()->GetName());
	}
}

void UGrabber::Grab()
{
	FHitResult HitResult = GetFirstPhysicsBodyInReach();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();

	// See if we reach any actors with physics body collision channel set
	if (HitResult.GetActor())
	{
		// todo attach a physics handle
		PhysicsHandle->GrabComponentAtLocation
		(
			ComponentToGrab,
			NAME_None,
			GetPlayersViewpoint()
		);
	}
}


void UGrabber::Release()
{
	PhysicsHandle->ReleaseComponent();
	// todo remove/release the physics handle
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->SetTargetLocation(GetPlayersViewpoint());
	}
}

FVector UGrabber::GetPlayersWorldPos() const
{
	// Get player's viewpoint
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint
	(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);
	return PlayerViewPointLocation;
}

FVector UGrabber::GetPlayersViewpoint() const
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint
	(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);
	return PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;
}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{
	FHitResult Hit;
	// Raycast out to a certain distance (Reach)
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	GetWorld()->LineTraceSingleByObjectType
	(
		OUT Hit,
		GetPlayersWorldPos(),
		GetPlayersViewpoint(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
	);
	return Hit;
}