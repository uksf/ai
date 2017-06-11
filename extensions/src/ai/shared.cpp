#include "shared.hpp"

float uksf_shared::getZoom() {
    float safeZoneW = intercept::sqf::safe_zone_w();
    float deltaX = ((intercept::sqf::world_to_screen(intercept::sqf::position_camera_to_world(intercept::types::vector3(5000, 0, 10000))).x) - 0.5f);
    float trigRatio = (((safeZoneW / 2.0f) * 5000.0f) / (10000.0f * deltaX));
    float configFOV = (trigRatio / (safeZoneW / intercept::sqf::safe_zone_h()));
    return (0.75f / configFOV);
}

bool uksf_shared::lineOfSight(intercept::types::object& target, intercept::types::object& source, bool zoomCheck, bool groupCheck) {
    bool los = false;
    bool inScreen = false;
    intercept::types::vector2 screenPosition = intercept::sqf::world_to_screen(intercept::sqf::get_pos(target), inScreen);
    bool onScreen = (inScreen && ((std::abs(screenPosition.x) < 1.5f) && (std::abs(screenPosition.y) < 1.5f)));
    los = (onScreen && (intercept::sqf::check_visibility(source, "VIEW", intercept::sqf::vehicle(source), intercept::sqf::eye_pos(source), intercept::sqf::eye_pos(target)) > 0));

    if (onScreen && !los && groupCheck) {
        std::vector<intercept::types::object> units = intercept::sqf::units(target);
        auto unit = units.begin();
        while (unit != units.end()) {
            bool inScreen = false;
            intercept::types::vector2 screenPosition = intercept::sqf::world_to_screen(intercept::sqf::get_pos(*unit), inScreen);
            bool onScreen = (inScreen && ((std::abs(screenPosition.x) < 1.5f) && (std::abs(screenPosition.y) < 1.5f)));
            los = (onScreen && (intercept::sqf::check_visibility(source, "VIEW", intercept::sqf::vehicle(source), intercept::sqf::eye_pos(source), intercept::sqf::eye_pos(*unit)) > 0));
            if (los) {
                target = std::ref(*unit);
                break;
            }
            ++unit;
        }
    }

    if (los && zoomCheck) {
        float distanceMultiplier = (200 + (4 * 200 * (std::max(0.0f, (float) (intercept::sqf::current_vision_mode(source) - 1)))));
        if (!intercept::sqf::is_kind_of(intercept::sqf::vehicle(target), "CAManBase")) {
            distanceMultiplier *= 2.5f;
        }
        float distanceCheck = std::min(intercept::sqf::get_object_view_distance().object_distance, 1000 + (distanceMultiplier * uksf_shared::getZoom()));
        float distance = (intercept::sqf::get_pos_world(target)).distance(intercept::sqf::get_pos_world(source));
        los = (distance < distanceCheck);
        LOG(DEBUG) << "Distance < distanceCheck : " << distance << " < " << distanceCheck;
    }

    return los;
}
