#pragma once
#include <_main.hpp>

#include <Geode/modify/DialogLayer.hpp>
class $modify(DialogLayerExt, DialogLayer) {
    static inline CCNode* DialogIcon = nullptr;
    static inline std::function<void(CCSprite*)> HandleDialogIcon = [](CCSprite*) {};
    $override void displayDialogObject(DialogObject * p0) {
        DialogLayer::displayDialogObject(p0);
        //id fix
        this->m_characterSprite->setDisplayFrame(CCSprite::create(
            fmt::format(
                "dialogIcon_{:03d}.png", p0->m_characterFrame
            ).c_str()
        )->displayFrame());
        //DialogIcon
        if (this->m_characterSprite) {
            //custom setup
            HandleDialogIcon(this->m_characterSprite);
            //replace
            if (DialogIcon) {
                this->m_characterSprite->addChild(DialogIcon);
                this->m_characterSprite->setDisplayFrame(CCSpriteFrame::create(
                    cocos::frameName(this->m_characterSprite).c_str(), CCRectZero)
                );
                this->m_characterSprite->setContentSize(DialogIcon->getContentSize());
            }
        };
        DialogIcon = nullptr;
        HandleDialogIcon = [](CCSprite*) {};
    }
    $override bool init(DialogObject* object, cocos2d::CCArray* objects, int background) {
        auto init_result = DialogLayer::init(object, objects, background);
        return init_result;
    }
};

#include <Geode/modify/CCScene.hpp>
class $modify(CCSceneDialogs, CCScene) {
    void random(float) {
        if (auto game = GameManager::get()->m_gameLayer) if (game->isRunning()) return;
        if (auto gameplay = GameManager::get()->m_playLayer) if (gameplay->isRunning() or gameplay->m_isPaused) return;
        auto dailogs = std::vector<CCArray*>();
        //dont do stupid
        {
            CCArray* arr = CCArray::create();
            arr->addObject(DialogObject::create("Scratch", "don't do stupid", 26, 1.f, 0, ccWHITE));
            dailogs.push_back(arr);
        }
        //nevermind
        {
            CCArray* arr = CCArray::create();
            arr->addObject(DialogObject::create("Scratch", "nevermind", 52, 1.f, 0, ccWHITE));
            dailogs.push_back(arr);
        }
        //hi im walking here
        {
            CCArray* arr = CCArray::create();
            arr->addObject(DialogObject::create("Shopkeeper", "Hi!", 5, 1.f, 0, ccWHITE));
            arr->addObject(DialogObject::create("You", "What the fuck are you doing here?", 60, 1.f, 0, ccWHITE));
            arr->addObject(DialogObject::create("Shopkeeper", "I'm fucking walking bruh.", 34, 1.f, 0, ccWHITE));
            arr->addObject(DialogObject::create("You", "Ah well have a nice walk.", 60, 1.f, 0, ccWHITE));
            dailogs.push_back(arr);
        }
        //hi im walking here
        {
            CCArray* arr = CCArray::create();
            arr->addObject(DialogObject::create("Shopkeeper", "me still keeping shop", 5, 1.f, 0, ccWHITE));
            arr->addObject(DialogObject::create("You", "me?", 60, 1.f, 0, ccWHITE));
            dailogs.push_back(arr);
        }
        //hi im walking here
        {
            CCArray* arr = CCArray::create();
            arr->addObject(DialogObject::create("Shopkeeper", "Did you kill some one?", 29, 1.f, 0, ccWHITE));
            arr->addObject(DialogObject::create("You", "rly?", 60, 1.f, 0, ccWHITE));
            arr->addObject(DialogObject::create("Shopkeeper", "Ever?", 32, 1.f, 0, ccWHITE));
            arr->addObject(DialogObject::create("Shopkeeper", "How to huh?", 32, 1.f, 0, ccWHITE));
            arr->addObject(DialogObject::create("You", "You try every sort of drugs e?", 60, 1.f, 0, ccWHITE));
            arr->addObject(DialogObject::create("Shopkeeper", "labidydabbidy", 34, 1.f, 0, ccWHITE));
            dailogs.push_back(arr);
        }
        //I remember one day
        {
            CCArray* arr = CCArray::create();
            arr->addObject(DialogObject::create("You", "I remember one day, maybe was the first day of my life", 60, 1.f, 0, ccWHITE));
            arr->addObject(DialogObject::create("You", "You came to my heart, my eyes wide open to you", 60, 1.f, 0, ccWHITE));
            arr->addObject(DialogObject::create("You", "I feel warm, behind you take my hand inside", 60, 1.f, 0, ccWHITE));
            arr->addObject(DialogObject::create("You", "Kiss me, kiss me, kiss me again", 60, 1.f, 0, ccWHITE));
            arr->addObject(DialogObject::create("You", "I feel you, remember one day I start to love", 60, 1.f, 0, ccWHITE));
            arr->addObject(DialogObject::create("You", "Yes girl, yes, I love more than me, I know", 60, 1.f, 0, ccWHITE));
            arr->addObject(DialogObject::create("You", "Just touch my lips and got this picture, you still remember", 60, 1.f, 0, ccWHITE));
            arr->addObject(DialogObject::create("You", "Kiss you, kiss you, kiss you again", 60, 1.f, 0, ccWHITE));
            dailogs.push_back(arr);
        };
        //days
        {
            CCArray* arr = CCArray::create();
            arr->addObject(DialogObject::create("Scratch", "Have the days Been here?", 13, 1.f, 0, ccWHITE));
            arr->addObject(DialogObject::create("You", "No, I didn't saw them, I just came in.", 60, 1.f, 0, ccWHITE));
            arr->addObject(DialogObject::create("You", "What kind of days are they anyway,", 60, 1.f, 0, ccWHITE));
            arr->addObject(DialogObject::create("You", "what's so special about them?", 60, 1.f, 0, ccWHITE));
            arr->addObject(DialogObject::create("You", "Maybe i know...", 60, 1.f, 0, ccWHITE));
            arr->addObject(DialogObject::create("You", "nevermind", 60, 1.f, 0, ccWHITE));
            arr->addObject(DialogObject::create("Scratch", "ok", 13, 1.f, 0, ccWHITE));
            dailogs.push_back(arr);
        }
        //Who did the bite of 87
        {
            CCArray* arr = CCArray::create();
            arr->addObject(DialogObject::create("Scratch", rndb() ? pLoadingLayerRef->getLoadingString() : "Who did the bite of 87?", 13, 1.f, 0, ccWHITE));
            dailogs.push_back(arr);
        }
        auto dailog = *select_randomly(dailogs.begin(), dailogs.end());
        if (not cocos::findFirstChildRecursive<DialogLayer>(CCScene::get(), [](CCNode*) { return true; })) {
            srand(time(0));
            if ((rand() % 100 < 10)) {
                auto layer = DialogLayer::createWithObjects(dailog, 1);
                layer->animateInRandomSide();
                layer->addToMainScene();
            }
        };
    };
    $override static CCScene* create(void) {
        auto __this = CCScene::create();
        __this->schedule(schedule_selector(CCSceneDialogs::random), 12.f);
        return __this;
    }
};