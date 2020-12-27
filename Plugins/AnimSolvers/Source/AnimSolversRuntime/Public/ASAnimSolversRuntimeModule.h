// Created by Paul baudy

#pragma once

#include "Modules/ModuleManager.h"


class FAnimSolversRuntimeModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
