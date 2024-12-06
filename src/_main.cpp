#include "_main.hpp"

#ifdef GEODE_IS_WINDOWS

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
class $modify(FLAlertLayerShowupStartPointExt, CCNode) {
    $override void visit() {
        CCNode::visit();
        if (auto casted = typeinfo_cast<FLAlertLayer*>(this)) {
            if (casted->m_mainLayer) {
                if (casted->m_mainLayer->getContentSize().equals(CCDirector::get()->getWinSize())) {
                    casted->m_mainLayer->setAnchorPoint(getMousePos() / casted->m_mainLayer->getContentSize());
                }
            };
        };
    }
};

#include <Geode/modify/AppDelegate.hpp>
class $modify(WindowNameExt, AppDelegate) {
    $override void applicationWillEnterForeground() {
        SetWindowTextA(GetForegroundWindow(), Mod::get()->getName().data());
        return AppDelegate::applicationWillEnterForeground();
    };
    $override bool applicationDidFinishLaunching() {
        SetWindowTextA(GetForegroundWindow(), Mod::get()->getName().data());
        return AppDelegate::applicationDidFinishLaunching();
    };
};

#include <Geode/modify/MenuLayer.hpp>
class mouse_particle : public CCParticleSystemQuad {
public:
    static inline bool created = false;
    static inline Ref<CCParticleSystemQuad> shared_ref;
    static inline Ref<CCMoveTo> moveactref = CCMoveTo::create(0.001f, { -10, -10 });
    static void create() {
        if (created) return;
        created = true;
        shared_ref = Ref<CCParticleSystemQuad>(GameToolbox::particleFromString(
            "200a-1a0.54a0.94a-1a90a180a0a20a1a1a0a0a0a0a0a0a1a2a0a0a0.211765a0.1a0.207843a0.1a0.207843a0.1a1a0a0a0a0a0a0a0a0a0a0a0a1a0a0a0a0a0a0a0a10a0a0a0a1a1a1a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0"
            , CCParticleSystemQuad::create(), 0));
        shared_ref->setID("mouse_particle"_spr);
        shared_ref->schedule(schedule_selector(mouse_particle::sch));
        SceneManager::get()->keepAcrossScenes(shared_ref);
    }
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
    class $modify(MouseParticleExt, MenuLayer) {
        $override bool init() {
            if (SETTING(bool, "Add Cursor Particles")) mouse_particle::create();
            return MenuLayer::init();
        }
    };
};

#endif