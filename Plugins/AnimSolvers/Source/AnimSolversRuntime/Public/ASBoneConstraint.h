// Created by Paul Baudy

#pragma once

#include "ASBoneConstraint.generated.h"

struct FASBoneData;

DECLARE_LOG_CATEGORY_EXTERN(LogASConstraint, Log, All);

/**
*   Base abstract class used to define a bone constraint that the solver should fulfil during computation.
*   Derive from this class in order to apply a new specific mathematical constraint
*/
UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class UASBoneConstraint : public UObject
{
	GENERATED_BODY()

public:
	/** Override this to apply additional transformation to the bone */
	virtual void Apply(FTransform& InBoneTransform, int32 Index, const TArray<FASBoneData>& InBoneData, TArray<FTransform>& OutBoneTransforms) const;
};

/** Constraint used to limit a specific bone's rotation from its base orientation */
UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class UASBoneConstraint_AngularLimit : public UASBoneConstraint
{
	GENERATED_BODY()

public:

	/** Maximum angle in degrees the specified bone can deviate from its initial rotation */
	UPROPERTY(EditAnywhere, Category = IK)
	float MaxAngle;

	virtual void Apply(FTransform& InBoneTransform, int32 Index, const TArray<FASBoneData>& InBoneData, TArray<FTransform>& OutBoneTransforms) const override;
};

/** 
*  Helper struct used to wrap together a bone reference with an instanced object
*  @note : FBoneReference struct cannot be a data member of an EditInlineNew object
*/
USTRUCT(BlueprintType, Blueprintable)
struct FASBoneConstraintWrapper
{
	GENERATED_BODY()

	/** Bone to apply this constraint on */
	UPROPERTY(EditAnywhere, Category = Skeletal)
	FBoneReference Bone;

	UPROPERTY(EditAnywhere, instanced)
	UASBoneConstraint* Constraint;
};

/** 
*  Constraint used to limit rotation to a specific axis 
*  @todo : this is still WIP
*/
UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class UIKSBoneConstraint_PlanarRotation : public UASBoneConstraint
{
	GENERATED_BODY()

public:
	/** Rotation axis in mesh space */
	UPROPERTY(EditAnywhere)
	FVector RotationAxis;

	/** */
	UPROPERTY(EditAnywhere)
	FVector BaseRotation;

	UPROPERTY(EditAnywhere)
	float MaxAngle;

	virtual void Apply(FTransform& InBoneTransform, int32 Index, const TArray<FASBoneData>& InBoneData, TArray<FTransform>& OutBoneTransforms) const override;
};