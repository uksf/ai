#pragma once
#include "common.hpp"

typedef std::tuple<config, std::vector<std::string>> config_group_type;

class uksf_ai_spawning : public singleton<uksf_ai_spawning> {
public:
    uksf_ai_spawning();
    ~uksf_ai_spawning();

private:
    std::unordered_map<std::string, std::vector<config_group_type>> _groupConfigMap{};

    void getGroupConfigs();
    std::vector<std::string> setGroupFlags(config group);
    std::tuple<side, config> selectGroup(std::string faction);
};
