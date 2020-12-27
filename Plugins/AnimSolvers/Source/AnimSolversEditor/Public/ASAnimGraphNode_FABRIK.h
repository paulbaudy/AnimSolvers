// Created by Paul baudy

#pragma once

/// UE4
#include "CoreMinimal.h"
#include "AnimGraphNode_SkeletalControlBase.h"
#include "Animation/AnimNodeBase.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"

/// AnimSolvers
#include "AnimSolversRuntime/Public/ASAnimNode_FABRIK.h"

#include "ASAnimGraphNode_FABRIK.generated.h"

/**
 *  Custom Editor graph node for our custom FABRIK Skeletal controller
 */
UCLASS(MinimalAPI)
class UASAnimGraphNode_FABRIK : public UAnimGraphNode_SkeletalControlBase
{
	GENERATED_UCLASS_BODY()
public:

	// Begin UAnimGraphNode_SkeletalControlBase Interface
	virtual const FAnimNode_SkeletalControlBase* GetNode() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FString GetNodeCategory() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	FText GetControllerDescription() const;
	// ~End UAnimGraphNode_SkeletalControlBase Interface

	/** The FABRIK controller this Graph node is holding */
	UPROPERTY(EditAnywhere, Category = Skeletal)
	FASAnimNode_FABRIK Node;
};
