// Created by Paul baudy

#include "ASAnimGraphNode_BoneTrace.h"

#define LOCTEXT_NAMESPACE "IKSolverNodes"

UASAnimGraphNode_BoneTrace::UASAnimGraphNode_BoneTrace(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

const FAnimNode_SkeletalControlBase* UASAnimGraphNode_BoneTrace::GetNode() const
{
	return &Node;
}

FLinearColor UASAnimGraphNode_BoneTrace::GetNodeTitleColor() const
{
	return FLinearColor::Yellow;
}

FString UASAnimGraphNode_BoneTrace::GetNodeCategory() const
{
	return TEXT("IKSolver");
}

FText UASAnimGraphNode_BoneTrace::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("Bone Trace", "Bone Trace");
}

FText UASAnimGraphNode_BoneTrace::GetControllerDescription() const
{
	return LOCTEXT("Bone Trace", "Bone trace");
}

#undef LOCTEXT_NAMESPACE