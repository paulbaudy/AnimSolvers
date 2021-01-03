// Created by Paul Baudy

#include "ASBoneConstraint.h"

/// AnimSolvers
#include "ASBoneData.h"

DECLARE_STATS_GROUP(TEXT("AnimSolvers_Constraints"), STATGROUP_ANIMSOLVERS_CONSTRAINTS, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("BoneConstraint_AngularLimit_Apply"), STAT_BoneConstraint_AngularLimit_Apply, STATGROUP_ANIMSOLVERS_CONSTRAINTS);
DECLARE_CYCLE_STAT(TEXT("BoneConstraint_PlanarRotation_Apply"), STAT_BoneConstraint_PlanarRotation_Apply, STATGROUP_ANIMSOLVERS_CONSTRAINTS);
DEFINE_LOG_CATEGORY(LogASConstraint);

void UASBoneConstraint::Apply(FTransform& InBoneTransform, int32 Index, const TArray<FASBoneData>& InBoneData, TArray<FTransform>& OutBoneTransforms) const
{
	// @note Implement in subclasses
	// No generic behavior
	UE_LOG(LogASConstraint, Log, TEXT("Apply method was not overriden in a Bone constraint subclass."))
}

void UASBoneConstraint_AngularLimit::Apply(FTransform& InBoneTransform, int32 Index, const TArray<FASBoneData>& InBoneData, TArray<FTransform>& OutBoneTransforms) const
{
	SCOPE_CYCLE_COUNTER(STAT_BoneConstraint_AngularLimit_Apply);

	const FTransform& OutParentTransform = OutBoneTransforms[Index - 1];
	const FTransform& OutChildTransform = OutBoneTransforms[Index];
	FVector PostDir = OutChildTransform.GetTranslation() - OutParentTransform.GetTranslation();
	PostDir.Normalize();

	const FTransform& InParentTransform = InBoneData[Index - 1].BoneTransform;
	const FTransform& InChildTransform = InBoneData[Index].BoneTransform;
	FVector PreDir = InChildTransform.GetTranslation() - InParentTransform.GetTranslation();
	PreDir.Normalize();

	const float AngleOffset = FMath::Acos(PreDir | PostDir);

	if (FMath::RadiansToDegrees(AngleOffset) > MaxAngle)
	{
		const FVector RotationAxis = FVector::CrossProduct(PreDir, PostDir);
		const FVector DirToApply = PreDir.RotateAngleAxis(MaxAngle, RotationAxis);
		const float Length = InBoneData[Index].Length;
		InBoneTransform.SetTranslation(InParentTransform.GetTranslation() + DirToApply * Length);
	}
}

void UIKSBoneConstraint_PlanarRotation::Apply(FTransform& InBoneTransform, int32 Index, const TArray<FASBoneData>& InBoneData, TArray<FTransform>& OutBoneTransforms) const
{
	SCOPE_CYCLE_COUNTER(STAT_BoneConstraint_PlanarRotation_Apply);

	// The idea here is to only allow rotation around the RotationAxis vector

	const FTransform& OutParentTransform = OutBoneTransforms[Index - 1];
	const FTransform& OutChildTransform = OutBoneTransforms[Index];
	const FVector PostDir = OutChildTransform.GetTranslation() - OutParentTransform.GetTranslation();

	FVector BoneOnPlane = FVector::VectorPlaneProject(PostDir, RotationAxis);

	// Compare the newly found location with our base "model" rotation
	const float Angle = FMath::Acos(BoneOnPlane | BaseRotation);

	const float ClampedAngle = FMath::Max(FMath::RadiansToDegrees(Angle), MaxAngle);

	BoneOnPlane = BaseRotation.RotateAngleAxis(ClampedAngle, RotationAxis);
	const float Length = InBoneData[Index].Length;
	BoneOnPlane *= Length;

	// @todo finish up this constraint
	//InBoneTransform.SetTranslation()
}