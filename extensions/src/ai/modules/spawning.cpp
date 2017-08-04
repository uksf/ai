#include "spawning.hpp"


uksf_ai_spawning::uksf_ai_spawning() {
    uksf_ai::getInstance().preStart.connect([this]() {
        LOG(DEBUG) << "SPAWNING PRESTART";
    });

    uksf_ai::getInstance().preInit.connect([this]() {
        LOG(DEBUG) << "SPAWNING PREINIT";
        getInstance().getGroupConfigs();
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

void uksf_ai_spawning::getGroupConfigs() {
    if (_groupConfigMap.size() > 0) return;
    std::vector<config> factions = {};
    auto one = sqf::config_entry() >> "CfgGroups";
    auto two = sqf::config_classes("true", sqf::config_entry() >> "CfgGroups");
    auto configFactions = sqf::config_classes("(str _x find ""Empty"") isEqualTo -1", sqf::config_entry() >> "CfgGroups");
    for (auto& factionConfig : configFactions) {
        auto faction = sqf::config_classes("true", factionConfig);
        factions.insert(std::end(factions), std::begin(faction), std::end(faction));
    }
    for (auto& faction : factions) {
        std::vector<config_group_type> groups = {};
        auto configGroups = sqf::config_classes("(str _x find ""Infantry"") != -1", faction);
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
            auto weapons = sqf::get_array(unitConfig >> "weapons");
            auto configWeapons = sqf::config_entry() >> "CfgWeapons";
            for (auto& weapon : weapons.to_array()) {
                auto canLock = sqf::get_number(configWeapons >> weapon >> "canLock");
                if (canLock == 2) {
                    flags.insert(std::end(flags), "hasAA");
                }
            }
        } else {
            flags.insert(std::end(flags), "hasVehicle");
        }
    }
    flags.erase(std::unique(flags.begin(), flags.end()), flags.end());
    return flags;
}

std::tuple<side, config> uksf_ai_spawning::selectGroup(std::string faction) {
    sqf::config_entry group = game_value();
    auto entry = _groupConfigMap.find(faction);
    if (entry != _groupConfigMap.end()) {
        auto groups = entry->second;
        auto group = std::next(std::begin(groups), uksf_ai_common::getInstance().rand->randomBetween(0, groups.size()));
    } else {
        return { sqf::east(), group };
    }
    side side = uksf_ai_common::getSide((int)(sqf::get_number(group >> "side")));
    return { side, group };
}
