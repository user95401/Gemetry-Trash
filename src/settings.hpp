#pragma once
#include <_main.hpp>

#include <Geode/loader/SettingEvent.hpp>

#define mod(id) Loader::get()->getInstalledMod(id)

template <class T>
inline std::monostate listenForSettingChanges(
    std::string modID, std::string const& settingKey, void (*callback)(T)
) {
    (void)new EventListener(
        callback, GeodeSettingChangedFilter<T>(modID, settingKey)
    );
    return std::monostate();
}

inline auto settingIsForcedPopup(auto settingName = "thats") {
    return createQuickPopup(
        "You cant change this",
        fmt::format(
            "{} will be changed by gtps mod at game loading.",
            settingName
        ),
        "Ok", nullptr,
        [](auto, auto) {}, false);
}

#define setListenerToNtfyForcedSetting(id, key) listenForSettingChanges(id, key,+[](bool value) {    settingIsForcedPopup(key)->show();    });

$execute{
	mod("geode.loader")->setSettingValue("disable-last-crashed-popup", true);
    setListenerToNtfyForcedSetting("geode.loader", "disable-last-crashed-popup");
	mod("geode.loader")->setSettingValue("enable-geode-theme", true);
    setListenerToNtfyForcedSetting("geode.loader", "enable-geode-theme");
}