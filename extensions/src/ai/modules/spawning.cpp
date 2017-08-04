#pragma once
#include "common.hpp"

typedef std::tuple<config, std::vector<std::string>> config_group_type;

class uksf_ai_spawning : public singleton<uksf_ai_spawning> {
public:
    uksf_ai_spawning() {
        uksf_ai::getInstance()->preStart.connect([this]() {
            LOG(DEBUG) << "SPAWNING PRESTART";
            getInstance()->getGroupConfigs();
        });

        uksf_ai::getInstance()->preInit.connect([this]() {
            LOG(DEBUG) << "SPAWNING PREINIT";

        });

        uksf_ai::getInstance()->postInit.connect([this]() {
            LOG(DEBUG) << "SPAWNING POSTINIT";

        });

        uksf_ai::getInstance()->missionEnded.connect([this]() {
            LOG(DEBUG) << "SPAWNING MISSION ENDED";

        });
    };

    ~uksf_ai_spawning() {
        groupConfigMap.clear();
    };

private:
    std::unordered_map<std::string, std::vector<config_group_type>> groupConfigMap{};

    void getGroupConfigs() {
        std::vector<config> factions = {};
        auto configFactions = sqf::config_classes("(str _x find ""Empty"") isEqualTo -1", sqf::config_entry() >> "CfgGroups");
        for (auto factionConfig : configFactions) {
            auto faction = sqf::config_classes("true", factionConfig);
            factions.insert(std::end(factions), std::begin(faction), std::end(faction));
        }
        for (auto faction : factions) {
            std::vector<config_group_type> groups = {};
            auto configGroups = sqf::config_classes("(str _x find ""Infantry"") != -1", faction);
            for (auto groupConfig : configGroups) {
                auto groupConfigs = sqf::config_classes("true", groupConfig);
                for (auto groupConfig : groupConfigs) {
                    config_group_type group = { groupConfig, setGroupFlags(groupConfig) };
                    groups.insert(std::end(groups), group);
                }
            }
            groupConfigMap.insert({ sqf::config_name(faction), groups });
        }
    }

    std::vector<std::string> setGroupFlags(config group) {
        std::vector<std::string> flags = {};
        auto units = sqf::config_classes("true", group);
        for (auto unit : units) {
            auto unitType = sqf::get_text(sqf::config_entry(unit) >> "vehicle");
            auto unitConfig = sqf::config_entry() >> "CfgVehicles" >> unitType;
            if (sqf::is_kind_of(unitType, "CAManBase")) {
                auto weapons = sqf::get_array(unitConfig >> "weapons");
                for (auto weapon : weapons.to_array()) {
                    auto canLock = sqf::get_number(sqf::config_entry() >> "CfgWeapons" >> weapon >> "canLock");
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

    std::tuple<side, config> selectGroup(std::string faction) {
        sqf::config_entry group = game_value();
        auto entry = groupConfigMap.find(faction);
        if (entry != groupConfigMap.end()) {
            auto groups = entry->second;
            auto group = std::next(std::begin(groups), uksf_ai_common::getInstance()->rand->randomBetween(0, groups.size()));
        } else {
            return { sqf::east(), group };
        }
        side side = uksf_ai_common::getSide((int)(sqf::get_number(group >> "side")));
        return { side, group };
    }
};
