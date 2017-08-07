#include "spawning.hpp"


uksf_ai_spawning::uksf_ai_spawning() {
    uksf_ai::getInstance().preStart.connect([this]() {
        LOG(DEBUG) << "SPAWNING PRESTART";
        getInstance().setGroupConfigs();
        uksfSpawningGetGroup = client::host::registerFunction(
            "uksfSpawningGetGroup",
            "Gets random group from faction whilst excluding given flags",
            userFunctionWrapper<uksfSpawningGetGroupFunction>,
            types::GameDataType::ARRAY,
            types::GameDataType::ARRAY
        );
    });

    uksf_ai::getInstance().preInit.connect([this]() {
        LOG(DEBUG) << "SPAWNING PREINIT";
    });

    uksf_ai::getInstance().postInit.connect([this]() {
        LOG(DEBUG) << "SPAWNING POSTINIT";
    });

    uksf_ai::getInstance().missionEnded.connect([this]() {
        LOG(DEBUG) << "SPAWNING MISSION ENDED";
    });
};

uksf_ai_spawning::~uksf_ai_spawning() {
    _groupConfigMap.clear();
};

void uksf_ai_spawning::setGroupConfigs() {
    if (_groupConfigMap.size() > 0) return;
    std::vector<config> factions = {};
    auto configFactions = sqf::config_classes("(str _x find \"Empty\") isEqualTo -1", sqf::config_entry() >> "CfgGroups");
    for (auto& factionConfig : configFactions) {
        auto faction = sqf::config_classes("true", factionConfig);
        factions.insert(std::end(factions), std::begin(faction), std::end(faction));
    }
    for (auto& faction : factions) {
        std::vector<config_group_type> groups = {};
        auto configGroups = sqf::config_classes("(str _x find \"Infantry\") != -1", faction);
        for (auto& groupConfig : configGroups) {
            auto groupConfigs = sqf::config_classes("true", groupConfig);
            for (auto& groupConfig : groupConfigs) {
                config_group_type group = { groupConfig, setGroupFlags(groupConfig) };
                groups.insert(std::end(groups), group);
            }
        }
        _groupConfigMap.insert({ sqf::config_name(faction), groups });
    }
}

std::vector<std::string> uksf_ai_spawning::setGroupFlags(config group) {
    std::vector<std::string> flags = {};
    auto units = sqf::config_classes("true", group);
    auto configVehicles = sqf::config_entry() >> "CfgVehicles";
    for (auto& unit : units) {
        auto unitType = sqf::get_text(sqf::config_entry(unit) >> "vehicle");
        auto unitConfig = configVehicles >> unitType;
        if (sqf::is_kind_of(unitType, "CAManBase")) {
            if (checkHasAA(unitConfig)) flags.insert(std::end(flags), "hasAA");
        } else {
            flags.insert(std::end(flags), "hasVehicle");
        }
    }
    flags.erase(std::unique(flags.begin(), flags.end()), flags.end());
    return flags;
}

bool uksf_ai_spawning::checkHasAA(sqf::config_entry unitConfig) {
    auto weapons = sqf::get_array(unitConfig >> "weapons").to_array();
    auto configWeapons = sqf::config_entry() >> "CfgWeapons";
    for (auto& weapon : weapons) {
        auto weaponType = std::string(weapon);
        std::transform(weaponType.begin(), weaponType.end(), weaponType.begin(), ::tolower);
        if (WEAPONS_EXCLUDE.find(weaponType) != WEAPONS_EXCLUDE.end()) continue;
        for (auto& type : WEAPONS_AA) {
            if (weaponType == type) {
                return true;
            }
        }
    }
    return false;
}

game_value uksf_ai_spawning::uksfSpawningGetGroupFunction(game_value params) {
    std::string faction = std::string(params[0]);
    auto_array<game_value> flags = {};
    bool invert = false;
    if (params.size() > 1) flags = params[1].to_array();
    if (params.size() > 2) invert = params[2];

    config groupConfig = game_value();
    side side = sqf::east();
    auto entry = getInstance()._groupConfigMap.find(faction);
    if (entry != getInstance()._groupConfigMap.end()) {
        auto& groups = entry->second;
        std::shuffle(groups.begin(), groups.end(), RENG);
        bool valid = false;
        for (auto& group : groups) {
            auto& groupFlags = std::get<1>(group);
            valid = true && !invert;
            for (std::string flag : flags) {
                if (std::find(groupFlags.begin(), groupFlags.end(), flag) != groupFlags.end()) {
                    valid = false || invert;
                    break;
                }
            }
            if (!valid) continue;
            groupConfig = std::get<0>(group);
            side = uksf_ai_common::getSide((int)(sqf::get_number(sqf::config_entry(groupConfig) >> "side")));
            break;
        }
    }
    return { side, groupConfig };
};
