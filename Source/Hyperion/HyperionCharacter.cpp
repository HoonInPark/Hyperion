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

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AHyperionCharacter::StartCharStatus_AIR);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AHyperionCharacter::StopCharStatus_MOUSE);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AHyperionCharacter::Move);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Started, this, &AHyperionCharacter::StartCharStatus_WASD);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AHyperionCharacter::StopCharStatus_WASD);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AHyperionCharacter::Look);
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
		m_CharStates,
		GetActorLocation(),
		GetActorRotation(),
		false);
}

// in engine loop, rhi funcs called after this tick func returned
void AHyperionCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (int i = 0; i < static_cast<int>(ECharStatus::E_MAX); ++i)
	{
		switch (static_cast<ECharStatus>(i))
		{
		case ECharStatus::E_WASD:
		{
			if (m_CharStates[i])
				VecToSend = GetActorLocation();

			break;
		}
		case ECharStatus::E_MOUSE:
		{
			if (m_CharStates[i])
				RotToSend = GetActorRotation();

			break;
		}
		case ECharStatus::E_AIR:
		{
			if (m_CharStates[i])
			{
				m_CharStates[static_cast<int>(ECharStatus::E_WASD)] = true;
				
				VecToSend = GetActorLocation();
				bInAirToSend = true;
			}

			break;
		}
		default:
			break;
		}
	}

	m_pObservable->UpdateData(
		m_CharStates,
		VecToSend,
		RotToSend,
		bInAirToSend);

	m_CharStates.Init(false, static_cast<int32>(ECharStatus::E_MAX));
	VecToSend = FVector(0.f, 0.f, 0.f);
	RotToSend = FRotator(0.f, 0.f, 0.f);
	bInAirToSend = false;
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
