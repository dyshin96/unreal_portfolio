// Copyright Epic Games, Inc. All Rights Reserved.

#include "WarriorsCharacter.h"
#include "WarriorsCharacterSettings.h"
#include "Item.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "WarriorsMovementSettings.h"
#include "WarriorsCharacterMovementComponent.h"
#include "WarriorsConstants.h"
#include "WarriorsGamePlayTags.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "Net/Core/PushModel/PushModel.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "IImageWrapperModule.h"
#include "ObjectTools.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AWarriorsCharacter

AWarriorsCharacter::AWarriorsCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UWarriorsCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 98.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	HeadMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Head"));
	HandsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hands"));
	LegsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Legs"));
	FeetMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Feet"));
	EyesMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Eyes"));
	HairMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hair"));
	HelmetMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Helmet"));

	InitSubMeshs(HeadMesh);
	InitSubMeshs(HandsMesh);
	InitSubMeshs(LegsMesh);
	InitSubMeshs(FeetMesh);
	InitSubMeshs(EyesMesh);
	InitSubMeshs(HairMesh);
	InitSubMeshs(HelmetMesh);
}

void AWarriorsCharacter::PostRegisterAllComponents()
{
	Super::PostRegisterAllComponents();

	ViewState.Rotation = GetViewRotation();
	ViewState.PreviousYawAngle = ViewState.Rotation.Yaw;
}

void AWarriorsCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DetectInterationObject();
	RefreshInput(DeltaTime);
	RefreshViewState(DeltaTime);
	RefreshGroundedRotation(DeltaTime);
	RefreshLocomotionLocationAndRotation(DeltaTime);
	RefreshLocomotion();
	RefreshGait();
}

void AWarriorsCharacter::DetectInterationObject()
{
	FVector Start = GetActorLocation();
	FVector ForwardVector = GetActorForwardVector();
	float MaxDistance = 300.0f;
	float MAXAngle = 30.0f;

	FHitResult HitResult;
	FVector End = Start + (ForwardVector * MaxDistance);
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	TArray<AItem*> Items;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);
	if (bHit)
	{
		if (AActor* HitActor = Cast<AActor>(HitResult.GetActor()))
		{
			if (AItem* Item = Cast<AItem>(HitActor))
			{
				if (DetectedItems.Contains(Item) == false)
				{
				    Item->OnDetected.Broadcast();
				}

				Items.Add(Item);
			}
		}
	}

	TArray<AItem*> UnDetectedItem = DetectedItems.FilterByPredicate([Items](const AItem* Item)
	{
		return Items.Contains(Item) == false;
	});

	for (AItem* Item : UnDetectedItem)
	{
		Item->OnUnDetected.Broadcast();
	}

	DetectedItems = Items;
}

void AWarriorsCharacter::SetInputDirection(FVector NewInputDirection)
{
	NewInputDirection = NewInputDirection.GetSafeNormal();
	InputDirection = NewInputDirection;
}

void AWarriorsCharacter::SetTargetYawAngle(const float TargetYawAngle)
{
	LocomotionState.TargetYawAngle = FMath::UnwindDegrees(TargetYawAngle);
	LocomotionState.SmoothTargetYawAngle = LocomotionState.TargetYawAngle;
}

void AWarriorsCharacter::RefreshViewState(const float DeltaTime)
{
	ViewState.PreviousYawAngle = ViewState.Rotation.Yaw;
	ViewState.Rotation = GetViewRotation();

	if (DeltaTime > UE_SMALL_NUMBER)
	{
		ViewState.YawSpeed = FMath::Abs(UE_REAL_TO_FLOAT(ViewState.Rotation.Yaw - ViewState.PreviousYawAngle)) / DeltaTime;
	}
}

void AWarriorsCharacter::RefreshInput(const float DeltaTime)
{
	if (GetLocalRole() >= ROLE_AutonomousProxy)
	{
		SetInputDirection(GetCharacterMovement()->GetCurrentAcceleration() / GetCharacterMovement()->GetMaxAcceleration());
	}

	LocomotionState.bHasInput = InputDirection.SizeSquared() > UE_KINDA_SMALL_NUMBER;

	if (LocomotionState.bHasInput)
	{
		LocomotionState.InputYawAngle = UE_REAL_TO_FLOAT(FMath::RadiansToDegrees(FMath::Atan2(InputDirection.Y, InputDirection.X)));
	}
}

void AWarriorsCharacter::RefreshLocomotionLocationAndRotation(const float DeltaTime)
{
	const auto& ActorTransform{ GetActorTransform() };

	// If network smoothing is disabled, then return regular actor transform.

	if (GetCharacterMovement()->NetworkSmoothingMode == ENetworkSmoothingMode::Disabled)
	{
		LocomotionState.Location = ActorTransform.GetLocation();
		LocomotionState.Rotation = GetActorRotation();
	}
	else if (GetMesh()->IsUsingAbsoluteRotation())
	{
		LocomotionState.Location = ActorTransform.TransformPosition(GetMesh()->GetRelativeLocation() - GetBaseTranslationOffset());
		LocomotionState.Rotation = GetActorRotation();
	}
	else
	{
		//�ִϸ��̼��̳� �� Ʈ�������� ���� �޽� ������Ƽ�� �����̽����� ��ȯ�� ���� �� �����Ƿ� SmoothTransform�� ����մϴ�. 
		const FTransform SmoothTransform{ ActorTransform * FTransform{
			GetMesh()->GetRelativeRotationCache().RotatorToQuat_ReadOnly(
			GetMesh()->GetRelativeRotation()) * GetBaseRotationOffset().Inverse(),
			GetMesh()->GetRelativeLocation() - GetBaseTranslationOffset()
		}
		};

		LocomotionState.Location = SmoothTransform.GetLocation();
		LocomotionState.Rotation = SmoothTransform.Rotator();
	}
}

void AWarriorsCharacter::RefreshLocomotion()
{
	LocomotionState.Velocity = GetVelocity();
	LocomotionState.HorizontalSpeed = UE_REAL_TO_FLOAT(GetVelocity().Size2D());
	LocomotionState.Quat = GetActorRotation().Quaternion();

	static constexpr float bHasHorizontalSpeedTheshold{ 1.0f };
	LocomotionState.bHasVelocity = LocomotionState.HorizontalSpeed > bHasHorizontalSpeedTheshold;
	if (LocomotionState.bHasVelocity)
	{
		LocomotionState.VelocityYawAngle = UE_REAL_TO_FLOAT(FMath::RadiansToDegrees(FMath::Atan2(LocomotionState.Velocity.Y, LocomotionState.Velocity.X)));
	}
	
	LocomotionState.bMoving = (LocomotionState.bHasInput && LocomotionState.bHasVelocity) || LocomotionState.HorizontalSpeed > Settings->MovingSpeedThreshold;
}	

void AWarriorsCharacter::RefreshGroundedRotation(const float DeltaTime)
{
	if (LocomotionMode != WarriorsLocomotionModeTags::Grounded)
	{
		return;
	}

	if (HasAnyRootMotion())
	{
		RefreshTargetYawAngleUsingLocomotionRotation();
		return;
	}
	
	//Not moving
	if (!LocomotionState.bMoving)
	{
		ApplyRotationYawSpeedAnimationCurve(DeltaTime);
		RefreshTargetYawAngleUsingLocomotionRotation();
		return;
	}
	
	//moving

	float TargetYawAngle{ UE_REAL_TO_FLOAT(ViewState.Rotation.Yaw) };
	if (Gait == WarriorsGaitTags::Sprinting)
	{
		//Todo
		TargetYawAngle = LocomotionState.VelocityYawAngle;
	}
	else
	{
		TargetYawAngle = UE_REAL_TO_FLOAT(ViewState.Rotation.Yaw + GetMesh()->GetAnimInstance()->GetCurveValue(UWarriorsConstants::RotationYawOffsetCurveName()));
	}

	const float RotationInterpolationSpeed{ CalculateGroundedMovingRotationInterpolationSpeed() };
	static constexpr float TargetYawAngleRotationSpeed{ 500.0f };

	SetRotationExtraSmooth(TargetYawAngle, DeltaTime, RotationInterpolationSpeed, TargetYawAngleRotationSpeed);
}

void AWarriorsCharacter::SetRotationExtraSmooth(const float TargetYawAngle, const float DeltaTime, 
	const float InterpolationSpeed, const float TargetYawAngleRotationSpeed) 
{
	SetTargetYawAngleSmooth(TargetYawAngle, DeltaTime, TargetYawAngleRotationSpeed);

	FRotator NewRotation{ GetActorRotation() };
	if (InterpolationSpeed > 0.0f)
	{
		float Delta = LocomotionState.SmoothTargetYawAngle - FMath::UnwindDegrees(NewRotation.Yaw);
		float ThresholdValue = 5.0f;
		float Ratio = 1.0f - FMath::InvExpApprox(InterpolationSpeed * DeltaTime);
		if (Delta > 180.0f - ThresholdValue)
		{
			Delta = Delta - 360.0f;
		}
		NewRotation.Yaw = FMath::UnwindDegrees(NewRotation.Yaw + Delta * Ratio);
	}
	else
	{
		NewRotation.Yaw = LocomotionState.TargetYawAngle;
	}

	SetActorRotation(NewRotation);
	RefreshLocomotionLocationAndRotation(DeltaTime);
}

void AWarriorsCharacter::SetTargetYawAngleSmooth(const float TargetYawAngle, const float DeltaTime, const float RotationSpeed)
{
	LocomotionState.TargetYawAngle = FMath::UnwindDegrees(TargetYawAngle);

	//SmoothTargetYawAngle�� �� �ִ��� �𸣰���??
	LocomotionState.SmoothTargetYawAngle = TargetYawAngle;

	if (RotationSpeed <= 0.0f || FMath::IsNearlyEqual(LocomotionState.SmoothTargetYawAngle, TargetYawAngle))
	{
		LocomotionState.SmoothTargetYawAngle = TargetYawAngle;
	}
}

float AWarriorsCharacter::CalculateGroundedMovingRotationInterpolationSpeed() const
{
	const FWarriorsMovementGaitSettings& GaitSettings{ WarriorsCharacterMovementComponent->GetGaitSettings() };
	TObjectPtr<UCurveFloat> InterpolationCurve = GaitSettings.RotationInterpolationSpeedCurve.Get();
	
	static constexpr float DefaultInterpolationSpeed{ 5.0f };

	//ȸ���ӵ��� GaitAmount�� ���� Ƽ���� �ϰ� �ְ� ������ ��� 
	const float InterpolationSpeed{ IsValid(InterpolationCurve) ?
		InterpolationCurve->GetFloatValue(WarriorsCharacterMovementComponent->GetGaitAmount()) : DefaultInterpolationSpeed };

	static constexpr auto MaxInterpolationSpeedMultiplier{ 3.0f };
	static constexpr auto ReferenceViewYawSpeed{ 300.0f };

	//ī�޶��� ȸ���ӵ��� ���� �ӵ� ���Ŀ� ���Խ��� ī�޶� ���� ȸ���Ҽ��� ĳ���͵� ���� ȸ���ϰ��Ͽ� �ڿ��������� ���Դϴ�. 
	return InterpolationSpeed * (1.0f + (MaxInterpolationSpeedMultiplier - 1.0f) * FMath::Clamp(ViewState.YawSpeed / ReferenceViewYawSpeed, 0.0f, 1.0f));
}

void AWarriorsCharacter::RefreshTargetYawAngleUsingLocomotionRotation()
{
	SetTargetYawAngle(UE_REAL_TO_FLOAT(LocomotionState.Rotation.Yaw));
}

void AWarriorsCharacter::ApplyRotationYawSpeedAnimationCurve(float DeltaTime)
{
	const auto DeltaYawAngle{ GetMesh()->GetAnimInstance()->GetCurveValue(UWarriorsConstants::RotationYawSpeedCurveName()) * DeltaTime };
	if (FMath::Abs(DeltaYawAngle) > UE_SMALL_NUMBER)
	{
		FRotator NewRotation{ GetActorRotation() };
		NewRotation.Yaw += DeltaYawAngle;
		SetActorRotation(NewRotation);

		RefreshLocomotionLocationAndRotation(DeltaTime);
		RefreshTargetYawAngleUsingLocomotionRotation();
	}
}

void AWarriorsCharacter::InitSubMeshs(USkeletalMeshComponent* SkeletalMeshComponent)
{
	SkeletalMeshComponent->SetupAttachment(GetMesh());	
	SkeletalMeshComponent->SetLeaderPoseComponent(GetMesh());
}

void AWarriorsCharacter::RefreshGait()
{
	const auto MaxAllowedGait{ CalculateMaxAllowedGait() };

	// Update the character max walk speed to the configured speeds based on the currently max allowed gait.

	//#todo �ʿ��ϸ� �߰����� 
	//WarriorsCharacterMovementComponent->SetMaxAllowedGait(MaxAllowedGait);

	const auto ActualGait{ CalculateActualGait(MaxAllowedGait) };

	SetGait(ActualGait);
}

void AWarriorsCharacter::SetGait(const FGameplayTag& NewGait)
{
	if (Gait != NewGait)
	{
		const auto PreviousGait{ Gait };

		Gait = NewGait;

		OnGaitChanged(PreviousGait);
	}
}

void AWarriorsCharacter::OnGaitChanged_Implementation(const FGameplayTag& PreviousGait) {}

FGameplayTag AWarriorsCharacter::CalculateMaxAllowedGait() const
{
	//���� ���� ���� 
	return WarriorsGaitTags::Sprinting;
}

FGameplayTag AWarriorsCharacter::CalculateActualGait(const FGameplayTag& MaxAllowedGait) const
{
	const auto& GaitSettings{ WarriorsCharacterMovementComponent->GetGaitSettings() };

	if (LocomotionState.HorizontalSpeed < GaitSettings.GetMaxWalkSpeed() + 10.0f)
	{
		return WarriorsGaitTags::Walking;
	}

	if (LocomotionState.HorizontalSpeed < GaitSettings.GetMaxRunSpeed() + 10.0f || MaxAllowedGait != WarriorsGaitTags::Sprinting)
	{
		return WarriorsGaitTags::Running;
	}

	return WarriorsGaitTags::Sprinting;
}

void AWarriorsCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	//SaveSkeletalMeshThumbnailToDisk();
	TSoftObjectPtr<USkeletalMesh> MeshPtr(FSoftObjectPath(TEXT("/Game/MRPGT/SkeletalMeshes/Humans/Male_D/SK_Chest_Arms.SK_Chest_Arms")));
	TSoftObjectPtr<USkeletalMesh> HeadMeshPtr(FSoftObjectPath(TEXT("/Game/MRPGT/SkeletalMeshes/Humans/Male_D/SK_Head_Teeth_Tongue.SK_Head_Teeth_Tongue")));
	TSoftObjectPtr<USkeletalMesh> HandsMeshPtr(FSoftObjectPath(TEXT("/Game/MRPGT/SkeletalMeshes/Humans/Male_D/SK_Hands.SK_Hands")));
	TSoftObjectPtr<USkeletalMesh> LegsMeshPtr(FSoftObjectPath(TEXT("/Game/MRPGT/SkeletalMeshes/Humans/CivilianClothing_A/SK_Civilian_Trousers_A_Green.SK_Civilian_Trousers_A_Green")));
	TSoftObjectPtr<USkeletalMesh> FeetMeshPtr(FSoftObjectPath(TEXT("/Game/MRPGT/SkeletalMeshes/Humans/Male_D/SK_Feet.SK_Feet")));
	TSoftObjectPtr<USkeletalMesh> EyesMeshPtr(FSoftObjectPath(TEXT("/Game/MRPGT/SkeletalMeshes/Humans/Male_D/SK_Eyes.SK_Eyes")));
	TSoftObjectPtr<USkeletalMesh> HairMeshPtr(FSoftObjectPath(TEXT("/Game/MRPGT/SkeletalMeshes/Humans/Male_D/SK_Hair.SK_Hair")));

	GetMesh()->SetSkeletalMesh(MeshPtr.LoadSynchronous());
	HeadMesh->SetSkeletalMesh(HeadMeshPtr.LoadSynchronous());
	HandsMesh->SetSkeletalMesh(HandsMeshPtr.LoadSynchronous());
	LegsMesh->SetSkeletalMesh(LegsMeshPtr.LoadSynchronous());
	FeetMesh->SetSkeletalMesh(FeetMeshPtr.LoadSynchronous());
	EyesMesh->SetSkeletalMesh(EyesMeshPtr.LoadSynchronous());
	HairMesh->SetSkeletalMesh(HairMeshPtr.LoadSynchronous());

	WarriorsCharacterMovementComponent = Cast<UWarriorsCharacterMovementComponent>(GetCharacterMovement());

	if (ensure(IsValid(Settings.Get())))
	{
	    Settings = NewObject<UWarriorsCharacterSettings>(this);
	}
}
//////////////////////////////////////////////////////////////////////////
// Input

void AWarriorsCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
		else
		{
			UE_LOG(LogTemplateCharacter, Warning, TEXT("Fail to Find SubSystem"));
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AWarriorsCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AWarriorsCharacter::Look);

		EnhancedInputComponent->BindAction(InteractionAction, ETriggerEvent::Triggered, this, &AWarriorsCharacter::Interaction);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AWarriorsCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AWarriorsCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AWarriorsCharacter::Interaction(const FInputActionValue& Value)
{
	bool bPress = Value.Get<bool>();
	if (bPress)
	{
		
	}
}

bool AWarriorsCharacter::SaveSkeletalMeshThumbnailToDisk(USkeletalMesh* SkeletalMesh, const FString& SavePath)
{
	if (!SkeletalMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid SkeletalMesh!"));
		return false;
	}

	//// ������� �����մϴ�.
	FObjectThumbnail* Thumbnail = ThumbnailTools::GetThumbnailForObject(SkeletalMesh);
	if (Thumbnail == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to generate thumbnail!"));
		return false;
	}

	Thumbnail->CompressImageData();

	int32 Width = Thumbnail->GetImageWidth();
	int32 Height = Thumbnail->GetImageHeight();

	// �̹��� ������ ���� ����
	const TArray<uint8>& ImageData = Thumbnail->GetUncompressedImageData();

	// PNG�� ������ �غ� �մϴ�.
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

	if (ImageWrapper.IsValid() && ImageWrapper->SetRaw(ImageData.GetData(), ImageData.Num(), Width, Height, ERGBFormat::BGRA, 8))
	{
		const TArray64<uint8>& PngData = ImageWrapper->GetCompressed(100);
		if (FFileHelper::SaveArrayToFile(PngData, *SavePath))
		{
			UE_LOG(LogTemp, Log, TEXT("Thumbnail saved successfully: %s"), *SavePath);
			return true;
		}
	}

	UE_LOG(LogTemp, Error, TEXT("Failed to save thumbnail!"));
	return false;
}

UWarriorsMovementSettings* AWarriorsCharacter::GetWarriorsMovementSettings() const
{
	return WarriorsMovementSettings.Get();
}
