// src/Plugin.cpp
#include "PCH.h"

#include <spdlog/sinks/basic_file_sink.h>

void SetActorCollision(RE::StaticFunctionTag*, RE::Actor* a_actor, bool a_enable) {
    if (!a_actor) {
        return;
    }
    auto controller = a_actor->GetCharController();
    if (!controller) {
        return;
    }

    if (a_enable) {
        controller->flags.reset(RE::CHARACTER_FLAGS::kNoCharacterCollisions);
    } else {
        controller->flags.set(RE::CHARACTER_FLAGS::kNoCharacterCollisions);
    }
}

void InitLog() {
    auto pluginName = SKSE::PluginDeclaration::GetSingleton()->GetName();

    auto logsFolder = SKSE::log::log_directory();
    if (!logsFolder)
        SKSE::stl::report_and_fail(std::format("{}: failed to init log - logs folder not found", pluginName));

    auto logFilePath = *logsFolder / std::format("{}.log", pluginName);
    auto fileLoggerPtr = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath.string(), true);
    auto loggerPtr = std::make_shared<spdlog::logger>("log", std::move(fileLoggerPtr));
    spdlog::set_default_logger(std::move(loggerPtr));

#ifdef _DEBUG
    spdlog::set_level(spdlog::level::trace);
    spdlog::flush_on(spdlog::level::trace);
#else
    spdlog::set_level(spdlog::level::info);
    spdlog::flush_on(spdlog::level::warn);
#endif
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse) {
    SKSE::Init(a_skse);

    const auto plugin = SKSE::PluginDeclaration::GetSingleton();
    const auto pluginName = plugin->GetName();
    const auto pluginVersion = plugin->GetVersion().string(".");
    const auto gameVersion = REL::Module::get().version().string(".");
    const auto authorName = plugin->GetAuthor();
    const auto supportEmail = plugin->GetSupportEmail();

    InitLog();

    SKSE::log::info("==================================================");
    SKSE::log::info("Loading plugin: {}", pluginName);
    SKSE::log::info("Plugin version: {}", pluginVersion);
    SKSE::log::info("Game version: {}", gameVersion);
    SKSE::log::info("Author: {}", authorName);
    SKSE::log::info("Support: {}", supportEmail);
    SKSE::log::info("==================================================");

    auto papyrusInterface = SKSE::GetPapyrusInterface();
    if (!papyrusInterface->Register([](RE::BSScript::IVirtualMachine* a_vm) {
            a_vm->RegisterFunction("SetActorCollision", "ActorCollisionManager", SetActorCollision);
            return true;
        })) {
        SKSE::log::critical("Failed to register Papyrus functions");
        return false;
    }

    SKSE::log::info("Plugin loaded successfully");
    return true;
}