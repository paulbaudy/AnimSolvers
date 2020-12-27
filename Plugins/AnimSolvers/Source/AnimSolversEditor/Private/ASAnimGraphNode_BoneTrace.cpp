// Created by Paul baudy

#include "ASAnimGraphNode_BoneTrace.h"

#define LOCTEXT_NAMESPACE "IKSolverNodes"

UAnimGraphNode_BoneTrace::UAnimGraphNode_BoneTrace(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

const FAnimNode_SkeletalControlBase* UAnimGraphNode_BoneTrace::GetNode() const
{
	return &Node;
}

FLinearColor UAnimGraphNode_BoneTrace::GetNodeTitleColor() const
{
	return FLinearColor::Yellow;
}

FString UAnimGraphNode_BoneTrace::GetNodeCategory() const
{
	return TEXT("IKSolver");
}

FText UAnimGraphNode_BoneTrace::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("Bone Trace", "Bone Trace");
}

FText UAnimGraphNode_BoneTrace::GetControllerDescription() const
{
	return LOCTEXT("Bone Trace", "Bone trace");
}

#undef LOCTEXT_NAMESPACE