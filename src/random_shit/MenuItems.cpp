#pragma once
#include <_main.hpp>

#include <Geode/modify/CCMenuItemSprite.hpp>
class $modify(CCMenuItemSpriteSoundExt, CCMenuItemSprite) {
    $override void selected() {
        FMODAudioEngine::get()->playEffect("btnClick.ogg");
        return CCMenuItemSprite::selected();
    }
};