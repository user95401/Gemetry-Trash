#include "_main.hpp"

#include <Geode/modify/GJGameLoadingLayer.hpp>
class $modify(GJGameLoadingLayerWhatTheF, GJGameLoadingLayer) {
    inline static Ref<EditLevelLayer> sex;
    void xdddd(float) {
        if (!sex) return;
        if (!this) return;
        if (!typeinfo_cast<GJGameLoadingLayer*>(this)) return;
        if (!this->isRunning()) return;
        this->m_editor ? sex->onEdit(sex) : sex->onPlay(sex);
    }
    void xd(float) {
        if (!this) return;
        if (!typeinfo_cast<GJGameLoadingLayer*>(this)) return;
        if (!this->isRunning()) return;
        if (this->m_level) {
            sex = EditLevelLayer::create(this->m_level);
            this->getParent()->addChild(sex, -999);
            this->scheduleOnce(schedule_selector(GJGameLoadingLayerWhatTheF::xdddd), 0.01f);
		}
    }
    virtual void onEnter() {
		GJGameLoadingLayer::onEnter();
        if (!this) return;
        if (!typeinfo_cast<GJGameLoadingLayer*>(this)) return;
        if (this->m_level) {
            GameManager::get()->playMenuMusic();
            this->addChild(geode::createLayerBG());
            this->scheduleOnce(schedule_selector(GJGameLoadingLayerWhatTheF::xd), 5.f);
            auto text = CCLabelBMFont::create(
                R"(Если ты застреваешь тут более 5 секунд 
то РобТоп хуесоооооссс)", "bigFont.fnt"
            );
            text->setOpacity(0);
            text->runAction(CCFadeIn::create(5.f));
            text->setAlignment(kCCTextAlignmentCenter);
            text->setPosition(this->getContentSize() / 2);
            limitNodeSize(text, this->getContentSize(), 0.6f, 0.3f);
            this->addChild(text);
        }
    };
};

#include <Geode/modify/GameManager.hpp>
class $modify(GameManagerSetsForGV, GameManager) {
    bool getGameVariable(char const* p0) {
        if (p0 == std::string("0095")) return true; //just dont
        if (p0 == std::string("0024")) return true; //show mouse
        return GameManager::getGameVariable(p0);
    };
};

#include <Geode/modify/CCSprite.hpp>
class $modify(CCSpriteNilTextureFix, CCSprite) {
    bool initWithTexture(CCTexture2D * pTexture) {
        //log::debug("{}({})", __FUNCTION__, pTexture);
        //if (pTexture) log::debug("pTexture Name: {}", pTexture->getName());
        return pTexture != nullptr ? CCSprite::initWithTexture(pTexture) : init();
    };
};

#include <Geode/modify/CCString.hpp>
class $modify(CCStringNilFix, CCString) {
    const char* getCString() {
        //log::debug("{}(int:{})->{}", this, (int)this, __func__);
        if ((size_t)this == 0) log::error("{}(int:{})->{}", this, (size_t)this, __func__);
        return (size_t)this != 0 ? CCString::getCString() : CCString::createWithFormat("")->getCString();
    }
};

#include <Geode/modify/GauntletSelectLayer.hpp>
class $modify(GauntletSelectLayerPosFix, GauntletSelectLayer) {
    void setupGauntlets() {
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

#include <Geode/modify/CCNode.hpp>
class $modify(UpdateSceneScaleByScreenView, CCNode) {
    void visit() {
        CCNode::visit();
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
    void visit() {
        CCNode::visit();
        if (auto casted = typeinfo_cast<FLAlertLayer*>(this)) {
            if (auto layer = casted->m_mainLayer) {
                if (layer->getContentSize().equals(CCDirector::get()->getWinSize())) {
                    layer->setAnchorPoint(toCocos(ImGui::GetMousePos()) / layer->getContentSize());
                }
            };
        };
    }
};

#ifdef GEODE_IS_WINDOWS

#include <Geode/modify/CCDirector.hpp>
class $modify(EnumWindowsExt, CCDirector) {
public:
    static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
        DWORD pid;
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid == GetCurrentProcessId()) {
            char title[256];
            GetWindowTextA(hwnd, title, sizeof(title));
            if (strlen(title) > 0) {
                if (title == std::string("Geometry Dash")) SetWindowTextW(
                    hwnd, string::utf8ToWide(fmt::format(
                        "{} {}",
                        getMod()->getName(),
                        getMod()->getVersion().toVString()
                    )).c_str()
                );
            }
        }
        return TRUE;
    }
    void runWithScene(CCScene* pScene) {
        EnumWindows(EnumWindowsProc, 0);
        CCDirector::runWithScene(pScene);
    };
};
$execute{ SetWindowTextW(GetConsoleWindow(), LR"(консось)"); }

#endif
