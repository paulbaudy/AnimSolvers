// Created by Paul Baudy

#pragma once

/// UE4
#include "CoreMinimal.h"
#include "AnimGraphNode_SkeletalControlBase.h"
#include "Animation/AnimNodeBase.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"

/// AnimSolvers
#include "AnimSolversRuntime/Public/ASAnimNode_BoneTrace.h"

#include "ASAnimGraphNode_BoneTrace.generated.h"

/**
 * Custom Editor graph node for our Bone trace controller
 */
UCLASS(MinimalAPI)
class UASAnimGraphNode_BoneTrace : public UAnimGraphNode_SkeletalControlBase
{
	GENERATED_UCLASS_BODY()
public:

	// Begin UAnimGraphNode_SkeletalControlBase Interface
	virtual const FAnimNode_SkeletalControlBase* GetNode() const override;
	FLinearColor GetNodeTitleColor() const override;
	FString GetNodeCategory() const override;
	FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	FText GetControllerDescription() const;
	// ~End UAnimGraphNode_SkeletalControlBase Interface


	UPROPERTY(EditAnywhere, Category = Skeletal)
	FAnimNode_BoneTrace Node;

	UPROPERTY(EditAnywhere, Category = Debug)
	bool bEnableDebugDraw = true;
};
