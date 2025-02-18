// BP_SQLitePlugin.h
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FBP_SQLitePluginModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};