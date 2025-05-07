// Copyright Epic Games, Inc. All Rights Reserved.

#include "HyperionCharacter.h"

#include "HyperionBase/ObservableBase.h"
#include "HyperionPlayerController.h"
#include "HyperionProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AHyperionCharacter

AHyperionCharacter::AHyperionCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	m_pObservable = CreateDefaultSubobject<UObservableBase>(TEXT("Observable"));

	m_CharStates.Init(false, static_cast<int32>(ECharStatus::E_MAX));
}

//////////////////////////////////////////////////////////////////////////// Input

void AHyperionCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AHyperionCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AHyperionCharacter::Move);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Started, this, &AHyperionCharacter::StartCharStatus_WASD);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AHyperionCharacter::StopCharStatus_WASD);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AHyperionCharacter::Look);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Started, this, &AHyperionCharacter::StartCharStatus_MOUSE);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Completed, this, &AHyperionCharacter::StopCharStatus_MOUSE);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error,
			TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."),
			*GetNameSafe(this));
	}
}

void AHyperionCharacter::BeginPlay()
{
	Super::BeginPlay();

	check(m_pObservable);

	m_pObservable->UpdateData(
		{ true, true },
		GetActorLocation(),
		GetActorRotation(),
		false);
}

// in engine loop, rhi funcs called after this tick func returned
void AHyperionCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_TimeSinceLastUpdate < 0.2f) return;

	m_pObservable->UpdateData(
		m_CharStates,
		GetActorLocation(),
		GetControlRotation(), // if GetActorRotation() is called, not GetControlRotation(), it only returns yaw changes
		GetCharacterMovement()->IsFalling());

	m_TimeSinceLastUpdate = 0.f;

	//UE_LOG(LogTemp, Warning, TEXT("Player Location Updated: %s"), *(GetCharacterMovement()->IsFalling() ? FString("IsFalling : TRUE") : FString("IsFalling : FALSE")));
}

void AHyperionCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	TScriptInterface<IObserverBase> ObserverInterface;
	ObserverInterface.SetObject(NewController);
	ObserverInterface.SetInterface(Cast<IObserverBase>(NewController));

	m_pObservable->Subscribe(ObserverInterface);
}

void AHyperionCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (!Controller) return;

	// add movement 
	AddMovementInput(GetActorForwardVector(), MovementVector.Y);
	AddMovementInput(GetActorRightVector(), MovementVector.X);
}

void AHyperionCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (!Controller) return;

	// add yaw and pitch input to controller
	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}
