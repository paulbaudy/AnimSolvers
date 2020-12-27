// Created by Paul baudy

#include "ASAnimGraphNode_FABRIK.h"

#define LOCTEXT_NAMESPACE "IKSolverNodes"

UASAnimGraphNode_FABRIK::UASAnimGraphNode_FABRIK(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

const FAnimNode_SkeletalControlBase* UASAnimGraphNode_FABRIK::GetNode() const
{
	return &Node;
}

FLinearColor UASAnimGraphNode_FABRIK::GetNodeTitleColor() const
{
	return FLinearColor::Yellow;
}

FString UASAnimGraphNode_FABRIK::GetNodeCategory() const
{
	return TEXT("IKSolver");
}

FText UASAnimGraphNode_FABRIK::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("Custom FABRIK", "Custom FABRIK");
}

FText UASAnimGraphNode_FABRIK::GetControllerDescription() const
{
	return LOCTEXT("Custom FABRIK", "Custom FABRIK");
}

#undef LOCTEXT_NAMESPACE