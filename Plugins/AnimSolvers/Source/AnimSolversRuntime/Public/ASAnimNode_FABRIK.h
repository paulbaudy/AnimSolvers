// Created by Paul baudy

#pragma once

/// UE4
#include "CoreMinimal.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"

/// AnimSolvers
#include "ASBoneConstraint.h"

#include "ASAnimNode_FABRIK.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogASFABRIK, Log, All);

namespace FABRIKSolver
{
	/** 
	*   Implementation of the FABRIK algorithm for Unreal Engine, which is a fast iterative solver for the Inverse Kinematics problem.
	*   See more details at : http://andreasaristidou.com/publications/papers/FABRIK.pdf
	* 
	* @param	InBoneDataArr : The bone chain data
	* @param	TargetLocation : The location our end effector should go to
	* @param	InPrecision : Precision of the algorithm, which is the distance we allow between the end effector and our target location
	* @param	InMaxIteration : Maximum number of passes
	* @return	OutBoneTransforms : The modified bone transforms
	*/
	void SolveFABRIK(const TArray<FASBoneData>& InBoneDataArr, const FVector& TargetLocation, const float& InPrecision, const int32 InMaxIteration, TArray<FTransform>& OutBoneTransforms);
	
	/**
	* Forward pass of the FABRIK algorithm.
	* This is the first stage where we iterate from the end effector (already offset to the target location) to the root bone,
	* while applying corrections to the bone locations
	*
	* @param	InBoneData : The bone chain data
	* @return	OutBoneTransforms : The transforms of the newly modified bones
	*/
	void ForwardPass(const TArray<FASBoneData>& InBoneData, TArray<FTransform>& OutBoneTransforms);
	
	/** 
	* Backward pass of the FABRIK algorithm.
	* This is the second stage where we iterate from the root bone to the end effector to preserve the bone chain,
	* while applying corrections to the bone locations
	*
	* @param	InBoneData : The bone chain data
	* @return	OutBoneTransforms : The transforms of the newly modified bones
	*/
	void BackwardPass(const TArray<FASBoneData>& InBoneData, TArray<FTransform>& OutBoneTransforms);

	/** 
	* Main computation of the FABRIK algorithm.
	* This function is making sure that the length between the two bones is preserved during the FABRIK passes
	* 
	* @param	InStaticBone : Adjacent bone that could be either child or parent depending on the current FABRIK pass
	* @param	InLength : The length that should be maintained between the two bones
	* @return	InMovingBone : The bone that should maintain its distance with InParentBone
	*/
	void OffsetPoint(FTransform& InMovingBone, float InLength, const FTransform& InStaticBone);
}

/**
*	Skeletal controller used to implement the runtime logic of the FABRIK Anim Node
*/
USTRUCT(BlueprintInternalUseOnly)
struct ANIMSOLVERSRUNTIME_API FASAnimNode_FABRIK : public FAnimNode_SkeletalControlBase
{
	GENERATED_USTRUCT_BODY()

public:
	// Begin FAnimNode_SkeletalControlBase Interface
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;
	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override;
	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override;
	// ~End FAnimNode_SkeletalControlBase Interface

	/** Location we're trying to reach with our effector bone */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bones, meta = (PinShownByDefault))
	FVector TargetLocation;

	/** Source bone. This will be the root of our chain */
	UPROPERTY(EditAnywhere, Category = IK)
	FBoneReference FromBone;

	/** Effector bone. This is the bone that will be moved near the target location */
	UPROPERTY(EditAnywhere, Category = IK)
	FBoneReference ToBone;

	/** List of constraints the solver must apply */
	UPROPERTY(EditAnywhere, Category = IK)
	TArray<FASBoneConstraintWrapper> Constraints;

	/** Tolerance for final tip location delta from target location */
	UPROPERTY(EditAnywhere, Category = Solver)
	float Tolerance = 1.f;

	/** Maximum number of iterations allowed for the solver */
	UPROPERTY(EditAnywhere, Category = Solver)
	int32 MaxIteration = 20;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = Debug)
	bool bDrawDebug = false;
#endif // WITH_EDITORONLY_DATA

private:
	/** Cached constraints to allow quick access */
	TMap<int32, const class UASBoneConstraint*> BoneIndexToConstraint;

	/** 
	*  Helper function used to build a bone chain from the source bone to the end effector. 
	*  @param	InPoseContext : The owning pose associated with our skeletal component
	*  @return	OutBoneIndices : An array of FCompactPoseBoneIndex providing all the bones from source bone to the end effector
	*/
	bool FillBoneIndices(const FComponentSpacePoseContext& InPoseContext, TArray<FCompactPoseBoneIndex>& OutBoneIndices) const;

	/**
	*  Helper function used to provide additional data to the FABRIK solver
	*  @param	InBoneIndices : An array representing our bone chain
	*  @param	InPoseContext : The owning pose associated with our skeletal component. @note not const because of GetComponentSpaceTransform not being const either.
	*  @return	OutBoneData : An array of FASBoneData providing meta data on bones for the FABRIK solver, such as bone lengths
	*/
	void BuildBoneData(const TArray<FCompactPoseBoneIndex>& InBoneIndices, FComponentSpacePoseContext& InPoseContext, TArray<FASBoneData>& OutBoneData) const;
};
