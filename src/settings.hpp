#pragma once
#include <_main.hpp>

#include <Geode/loader/SettingV3.hpp>

#define mod(id) Loader::get()->getInstalledMod(id)

$execute{
	mod("geode.loader")->setSettingValue("disable-last-crashed-popup", true);
	mod("geode.loader")->setSettingValue("enable-geode-theme", false);
}