#include "_main.hpp"

#include <Geode/modify/CCSprite.hpp>
class $modify(CCSpriteExt, CCSprite) {
    $override virtual bool initWithTexture(CCTexture2D * pTexture) {
        //log::debug("{}({})", __FUNCTION__, pTexture);
        //if (pTexture) log::debug("pTexture Name: {}", pTexture->getName());
        return pTexture != nullptr ? CCSprite::initWithTexture(pTexture) : init();
    };
};

#include <Geode/modify/GauntletSelectLayer.hpp>
class $modify(GauntletSelectLayerFix, GauntletSelectLayer) {
    $override void setupGauntlets() {
        GauntletSelectLayer::setupGauntlets();
        findFirstChildRecursive<ExtendedLayer>(this,
            [](ExtendedLayer* gauntlet_pages) {
                if (gauntlet_pages->getChildrenCount() > 1) return false;
                gauntlet_pages->setAnchorPoint(CCPointMake(1.f, 0.f));
                gauntlet_pages->ignoreAnchorPointForPosition(false);
                return true;
            }
        );
    };
};

#include <Geode/modify/CCDirector.hpp>
class $modify(ParticleSnow, CCDirector) {
    static inline Ref<CCParticleSnow> shared_ref;
    void sch(float) {
        auto hide = false;
        auto order = getChild(shared_ref->getParent(), -1)->getZOrder();
        if (auto game = GameManager::get()->m_gameLayer) {
            hide = game->isRunning();
            order = 0;
        }
        hide = SETTING(bool, "Add Snow Particles") ? hide : true;
        shared_ref->CCNode::setVisible(not hide);
        shared_ref->setZOrder(order);
    }
    $override void runWithScene(CCScene * pScene) {
        CCDirector::runWithScene(pScene);
        if (!SETTING(bool, "Add Snow Particles")) return;
        shared_ref = CCParticleSnow::create();
        shared_ref->setBlendAdditive(true);
        shared_ref->setID("snow_particle"_spr);
        shared_ref->schedule(schedule_selector(ParticleSnow::sch));
        SceneManager::get()->keepAcrossScenes(shared_ref);
    }
};

#include <Geode/modify/LevelSelectLayer.hpp>
class $modify(CustomPagesColor, LevelSelectLayer) {
    static inline Ref<CCParticleSnow> shared_ref;
    $override cocos2d::ccColor3B colorForPage(int colorID) {
        if (typeinfo_cast<LevelSelectLayer*>(this)) return LevelSelectLayer::colorForPage(colorID);
        auto rgb = Mod::get()->getSettingValue<cocos2d::ccColor3B>("Custom Color");
        return rgb == ccBLACK ? LevelSelectLayer::colorForPage(colorID) : rgb;
    }
};

#include <Geode/modify/MenuGameLayer.hpp>
class $modify(CustomMenuGameColor, MenuGameLayer) {
    static inline Ref<CCParticleSnow> shared_ref;
    $override cocos2d::ccColor3B getBGColor(int colorID) {
        auto rgb = Mod::get()->getSettingValue<cocos2d::ccColor3B>("Custom Color");
        return rgb == ccBLACK ? MenuGameLayer::getBGColor(colorID) : rgb;
    }
};

#include <Geode/modify/CCNode.hpp>
class $modify(UpdateSceneScaleByScreenView, CCNode) {
    $override void visit() {
        CCNode::visit();
        if (!SETTING(bool, "Update Scene Scale By Screen View")) return;
        if (auto game = GameManager::get()->m_gameLayer) if (game->isRunning()) return;
        if (auto gameplay = GameManager::get()->m_playLayer) if (gameplay->isRunning() or gameplay->m_isPaused) return;
        if (auto casted = typeinfo_cast<CCScene*>(this)) {
            this->setScaleX(CCDirector::get()->getScreenRight() / this->getContentWidth());
            this->setScaleY(CCDirector::get()->getScreenTop() / this->getContentHeight());
            this->setAnchorPoint(CCPointZero);
        };
    }
};

#include <Geode/modify/CCNode.hpp>
class $modify(MouseAnchorPointExt, CCNode) {
    $override void visit() {
        CCNode::visit();
        if (auto casted = typeinfo_cast<FLAlertLayer*>(this)) {
            if (casted->m_mainLayer) {
                if (casted->m_mainLayer->getContentSize().equals(CCDirector::get()->getWinSize())) {
                    casted->m_mainLayer->setAnchorPoint(toCocos(ImGui::GetMousePos()) / casted->m_mainLayer->getContentSize());
                }
            };
        };
#ifdef GEODE_IS_WINDOWS
        if (auto casted = typeinfo_cast<MenuGameLayer*>(this)) {
            auto pos = toCocos(ImGui::GetMousePos());
            pos = casted->convertToNodeSpace(pos);
            casted->setAnchorPoint(pos / casted->getContentSize());
            if (SETTING(bool, "Animate Menu Game")) void();
            else casted->getScale() == 1.f ? casted->setScale(1.005f) : void();
        };
#endif
    }
};

#ifdef GEODE_IS_WINDOWS

#include <Geode/modify/AppDelegate.hpp>
class $modify(WindowNameExt, AppDelegate) {
    void updateForegroundWindow() {
        SetWindowTextA(GetForegroundWindow(), fmt::format(
            "{} {}",
            getMod()->getName(), 
            getMod()->getVersion().toVString()
        ).data());
    }
    $override void applicationWillEnterForeground() {
        updateForegroundWindow();
        return AppDelegate::applicationWillEnterForeground();
    };
    $override bool applicationDidFinishLaunching() {
        updateForegroundWindow();
        return AppDelegate::applicationDidFinishLaunching();
    };
};

#include <Geode/modify/CCDirector.hpp>
class $modify(MouseParticle, CCDirector) {
public:
    static inline Ref<CCParticleSystemQuad> shared_ref;
    static inline Ref<CCMoveTo> moveactref = CCMoveTo::create(0.001f, { -10, -10 });
    void sch(float) {
        auto pos = CCScene::get()->convertToNodeSpace(getMousePos());
        moveactref->initWithDuration(moveactref->getDuration(), pos);
        shared_ref->runAction(moveactref);
        auto hide = false;
        auto order = getChild(shared_ref->getParent(), -1)->getZOrder();
        if (auto game = GameManager::get()->m_gameLayer) {
            hide = game->isRunning();
            order = 0;
        }
        hide = SETTING(bool, "Add Cursor Particles") ? hide : true;
        hide ? shared_ref->stopSystem() : shared_ref->resumeSystem();
        shared_ref->setZOrder(order);
    }
    $override void runWithScene(CCScene* pScene) {
        CCDirector::runWithScene(pScene);
        if (!SETTING(bool, "Add Cursor Particles")) return;
        CCSpriteFrameCache::get()->addSpriteFramesWithFile("GJ_ParticleSheet.plist", "GJ_ParticleSheet.png");
        shared_ref = GameToolbox::particleFromString(
            "200a-1a0.54a0.94a-1a90a180a0a20a1a1a0a0a0a0a0a0a1a2a0a0a0.211765a0.1a0.207843a0.1a0.207843a0.1a1a0a0a0a0a0a0a0a0a0a0a0a1a0a0a0a0a0a0a0a10a0a0a0a1a1a1a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0"
            , CCParticleSystemQuad::create(), 0);
        shared_ref->setID("mouse_particle"_spr);
        shared_ref->schedule(schedule_selector(MouseParticle::sch));
        pScene->addChild(shared_ref);
        SceneManager::get()->keepAcrossScenes(shared_ref);
    };
};

#endif
