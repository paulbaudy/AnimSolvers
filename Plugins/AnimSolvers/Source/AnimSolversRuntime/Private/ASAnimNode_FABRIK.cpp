// Created by Paul baudy

#include "ASAnimNode_FABRIK.h"

/// Animation
#include "Animation/AnimInstanceProxy.h"

/// AnimSolvers
#include "ASBoneData.h"

#if WITH_EDITOR
#include "DrawDebugHelpers.h"
#endif // WITH_EDITOR

DECLARE_STATS_GROUP(TEXT("AnimSolvers"), STATGROUP_ANIMSOLVERS, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("FABRIK_EvaluateSkeletalControl"), STAT_FABRIK_EvaluateSkeletalControl, STATGROUP_ANIMSOLVERS);
DEFINE_LOG_CATEGORY(LogASFABRIK);

namespace FABRIKSolver
{
	void OffsetPoint(FTransform& InMovingBone, float InLength, const FTransform& InStaticBone)
	{
		const FVector Dir = InMovingBone.GetTranslation() - InStaticBone.GetTranslation();
		const FVector DirNormalized = Dir.GetUnsafeNormal();

		// Offset the moving bone maintaining the bone length
		InMovingBone.SetTranslation(InStaticBone.GetTranslation() + DirNormalized * InLength);
	}

	void ForwardPass(const TArray<FASBoneData>& InBoneData, TArray<FTransform>& OutBoneTransforms)
	{
		const int32 NumBones = OutBoneTransforms.Num();
		for (int32 Index = 0; Index < NumBones - 2; ++Index)
		{
			const int32 rIndex = NumBones - 2 - Index;
			const FTransform& ChildBoneTransform = OutBoneTransforms[rIndex + 1];
			FTransform& ParentBoneTransform = OutBoneTransforms[rIndex];

			// We must maintain the bone chain length, so we offset the parent bone to match its distance with his child bone
			OffsetPoint(ParentBoneTransform, InBoneData[rIndex + 1].Length, ChildBoneTransform);

			if (const UASBoneConstraint* Constraint = InBoneData[rIndex - 1].Constraint.Get())
			{
				Constraint->Apply(ParentBoneTransform, rIndex, InBoneData, OutBoneTransforms);
			}
		}
	}

	void BackwardPass(const TArray<FASBoneData>& InBoneData, TArray<FTransform>& OutBoneTransforms)
	{
		const int32 NumBones = InBoneData.Num();
		for (int32 Index = 1; Index < NumBones; ++Index)
		{
			const FTransform& ParentBoneTransform = OutBoneTransforms[Index - 1];
			FTransform& ChildBoneTransform = OutBoneTransforms[Index];

			// Once again, we must maitain the bone chain length, so we offset the child bone to maintain its distance to his parent
			OffsetPoint(ChildBoneTransform, InBoneData[Index].Length, ParentBoneTransform);
		}
	}

	void SolveFABRIK(const TArray<FASBoneData>& InBoneDataArr, const FVector& TargetLocation, const float& InPrecision, const int32 InMaxIteration, TArray<FTransform>& OutBoneTransforms)
	{
		if (InBoneDataArr.Num() <= 1) 
		{
			return;
		}

		OutBoneTransforms.Empty();
		OutBoneTransforms.Reserve(InBoneDataArr.Num());
		for (const FASBoneData& InBoneData : InBoneDataArr)
		{
			OutBoneTransforms.Add(InBoneData.BoneTransform);
		}

		const FTransform& EffectorBoneTransform = OutBoneTransforms.Last();
		float TargetOffset = FVector::Dist(TargetLocation, EffectorBoneTransform.GetLocation());
		if (TargetOffset < InPrecision)
		{
			return;
		}

		// First step : we set the end effector's bone location to the specified location.
		// The solver will then try to modify to bone chain accordingly
		OutBoneTransforms.Last().SetLocation(TargetLocation);

		// Main loop of the FABRIK algorithm.
		// We run as many passes as we can according to our precision & iteration count
		int32 Count = 0;
		while (TargetOffset > InPrecision)
		{
			if (++Count > InMaxIteration)
			{
				break;
			}

			ForwardPass(InBoneDataArr, OutBoneTransforms);
			BackwardPass(InBoneDataArr, OutBoneTransforms);

			TargetOffset = FVector::Dist(TargetLocation, EffectorBoneTransform.GetLocation());
		}
	}
}

void FASAnimNode_FABRIK::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
	SCOPE_CYCLE_COUNTER(STAT_FABRIK_EvaluateSkeletalControl);

	const USkeletalMeshComponent* SkmCmp = Output.AnimInstanceProxy->GetSkelMeshComponent();
	if (!ensure(SkmCmp))
	{
		return;
	}
	
	// Get all bone indices to compose our bone chain
	TArray<FCompactPoseBoneIndex> BoneIndices;
	if(!FillBoneIndices(Output, BoneIndices))
	{
		return;
	}

	// Build bones data used by the FABRIK Solver
	TArray<FASBoneData> BonesToModify;
	BuildBoneData(BoneIndices, Output, BonesToModify);

	TArray<FTransform> ModifiedBoneTransforms;
	FABRIKSolver::SolveFABRIK(BonesToModify, TargetLocation, Tolerance, MaxIteration, ModifiedBoneTransforms);

	// Once the FABRIK algorithm has computed the new locations for our bone chain,
	// We need to adjust the modified bone angles to re-build our bone hierarchy 
	for (int32 Index = 0; Index < BoneIndices.Num() - 1; ++Index)
	{
		const FTransform& OldParentTransform = BonesToModify[Index].BoneTransform;
		const FTransform& OldChildTransform = BonesToModify[Index + 1].BoneTransform;

		const FVector OldDir = (OldChildTransform.GetTranslation() - OldParentTransform.GetTranslation()).GetUnsafeNormal();

		const FTransform& ParentTransform = ModifiedBoneTransforms[Index];
		const FTransform& ChildTransform = ModifiedBoneTransforms[Index + 1];

		const FVector NewDir = (ChildTransform.GetTranslation() - ParentTransform.GetTranslation()).GetUnsafeNormal();
		const FVector Axis = FVector::CrossProduct(OldDir, NewDir).GetSafeNormal();
		const float Angle = FMath::Acos(FVector::DotProduct(OldDir, NewDir));
		const FQuat DeltaRot(Axis, Angle);

		ModifiedBoneTransforms[Index].SetRotation(DeltaRot * ModifiedBoneTransforms[Index].GetRotation());
		ModifiedBoneTransforms[Index].NormalizeRotation();
	}

	// Send the new bone transforms
	for (int32 Index = 0; Index < BoneIndices.Num(); ++Index)
	{
		OutBoneTransforms.Add(FBoneTransform(BoneIndices[Index], ModifiedBoneTransforms[Index]));
	}

#if WITH_EDITOR && UE_ALLOW_DEBUG
	if (bDrawDebug)
	{
		const UWorld* World = SkmCmp->GetWorld();
		const FMatrix SkmToWorld = SkmCmp->GetComponentToWorld().ToMatrixNoScale();

		if (BonesToModify.Num() > 0)
		{
			const FVector FirstBoneLocation = BonesToModify[0].BoneTransform.GetTranslation();
			DrawDebugSphere(World, SkmToWorld.TransformPosition(FirstBoneLocation), 5.f, 10, FColor::Blue);

			const int32 BoneCount = BonesToModify.Num();
			for (int32 Index = 1; Index < BoneCount; ++Index)
			{
				const FVector& Start = BonesToModify[Index - 1].BoneTransform.GetTranslation();
				const FVector& End = BonesToModify[Index].BoneTransform.GetTranslation();
				DrawDebugSphere(World, SkmToWorld.TransformPosition(End), 5.f, 10, FColor::Blue);
				DrawDebugLine(World, SkmToWorld.TransformPosition(Start), SkmToWorld.TransformPosition(End), FColor::Yellow);
			}
		}

		if (OutBoneTransforms.Num() > 0)
		{
			const FVector FirstBoneLocation = BonesToModify[0].BoneTransform.GetTranslation();
			DrawDebugSphere(World, SkmToWorld.TransformPosition(FirstBoneLocation), 5.f, 10, FColor::Red);

			const int32 BoneCount = OutBoneTransforms.Num();
			for (int32 Index = 1; Index < BoneCount; ++Index)
			{
				const FVector& Start = OutBoneTransforms[Index - 1].Transform.GetTranslation();
				const FVector& End = OutBoneTransforms[Index].Transform.GetTranslation();
				DrawDebugSphere(World, SkmToWorld.TransformPosition(End), 5.f, 10, FColor::Red);
				DrawDebugLine(World, SkmToWorld.TransformPosition(Start), SkmToWorld.TransformPosition(End), FColor::Yellow);
			}
		}
	}
#endif // WITH_EDITOR && UE_ALLOW_DEBUG
}

bool FASAnimNode_FABRIK::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones)
{
	return FromBone.IsValidToEvaluate(RequiredBones) && ToBone.IsValidToEvaluate(RequiredBones);
}

void FASAnimNode_FABRIK::InitializeBoneReferences(const FBoneContainer& RequiredBones)
{
	ToBone.Initialize(RequiredBones);
	FromBone.Initialize(RequiredBones);

	for (FASBoneConstraintWrapper& ConstraintWrapper : Constraints)
	{
		if (nullptr != ConstraintWrapper.Constraint)
		{
			ConstraintWrapper.Bone.Initialize(RequiredBones);
			BoneIndexToConstraint.Add(ConstraintWrapper.Bone.BoneIndex, ConstraintWrapper.Constraint);
		}
	}
}

bool FASAnimNode_FABRIK::FillBoneIndices(const FComponentSpacePoseContext& InPoseContext, TArray<FCompactPoseBoneIndex>& OutBoneIndices) const
{
	const FBoneContainer& BoneContainer = InPoseContext.Pose.GetPose().GetBoneContainer();
	const FCompactPoseBoneIndex FromBoneIndex = FromBone.GetCompactPoseIndex(BoneContainer);
	FCompactPoseBoneIndex ToBoneIndex = ToBone.GetCompactPoseIndex(BoneContainer);

	while (FromBoneIndex != ToBoneIndex && !ToBoneIndex.IsRootBone())
	{
		OutBoneIndices.Insert(ToBoneIndex, 0);
		ToBoneIndex = InPoseContext.Pose.GetPose().GetParentBoneIndex(ToBoneIndex);
	}

#if UE_ALLOW_DEBUG
	if (FromBoneIndex != ToBoneIndex)
	{
		UE_LOG(LogASFABRIK, Log, TEXT("Invalid bone hierarchy. Please provide a proper bone chain."))
	}
#endif // UE_ALLOW_DEBUG

	OutBoneIndices.Insert(ToBoneIndex, 0);
	return FromBoneIndex == ToBoneIndex;
}

void FASAnimNode_FABRIK::BuildBoneData(const TArray<FCompactPoseBoneIndex>& InBoneIndices, FComponentSpacePoseContext& InPoseContext, TArray<FASBoneData>& OutBoneData) const
{
	for (int32 Index = 0; Index < InBoneIndices.Num(); ++Index)
	{
		const FCompactPoseBoneIndex& BoneIndex = InBoneIndices[Index];

		const FTransform& BoneTransform = InPoseContext.Pose.GetComponentSpaceTransform(BoneIndex);
		FASBoneData& BoneData = OutBoneData.AddDefaulted_GetRef();
		BoneData.BoneTransform = BoneTransform;
		BoneData.Constraint = BoneIndexToConstraint.FindRef(BoneIndex.GetInt());

		if (Index > 0)
		{
			const FCompactPoseBoneIndex& ParentBoneIndex = InBoneIndices[Index - 1];
			const FTransform& ParentBoneTransform = InPoseContext.Pose.GetComponentSpaceTransform(ParentBoneIndex);
			BoneData.Length = FVector::Dist(ParentBoneTransform.GetTranslation(), BoneTransform.GetTranslation());
		}
		else
		{
			BoneData.Length = 0.f;
		}
	}
}