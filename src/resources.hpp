#pragma once
#include <_main.hpp>

#include <Geode/modify/GManager.hpp>
class $modify(GManager_resourcescpp, GManager) {
    $override void setup() {
        CCFileUtils::sharedFileUtils()->addPriorityPath(fs::path(
            Mod::get()->getTempDir() / "resources" / Mod::get()->getID() / ""
        ).string().c_str());
        GManager::setup();
    }
};
#include <Geode/modify/CCSpriteFrameCache.hpp>
class $modify(CCSpriteFrameCache_resourcescpp, CCSpriteFrameCache) {
    $override void removeSpriteFramesFromFile(const char* plist) {
        //log::debug("{}(\"{}\")", __func__, plist);
        return CCSpriteFrameCache::removeSpriteFramesFromFile(plist);
    }
    $override void addSpriteFramesWithFile(const char* pszPlist) {

        if (string::contains(CCFileUtils::get()->fullPathForFilename(pszPlist, 0), ""_spr)) {
            log::debug("patching plist \"{}\"", pszPlist);
            auto plist_path = CCFileUtils::get()->fullPathForFilename(pszPlist, 0);
            if (auto plist_ifstream = std::fstream(plist_path)) {
                auto out_strstream = std::stringstream();
                for (std::string line; std::getline(plist_ifstream, line); ) {
                    line = string::replace(line, (Mod::get()->getID() + "/"), "");
                    out_strstream << line << std::endl;
                }
                //log::debug("{}", out_strstream.str());
                std::ofstream(plist_path) << out_strstream.str();
            };
        }

        //log::debug("{}(\"{}\")", __func__, pszPlist);
        return CCSpriteFrameCache::addSpriteFramesWithFile(pszPlist);
    }
    $override void addSpriteFrame(CCSpriteFrame* pobFrame, const char* pszFrameName) {
        //log::debug("{}({}, \"{}\")", __func__, pobFrame, pszFrameName);
        return CCSpriteFrameCache::addSpriteFrame(pobFrame, pszFrameName);
    }
};

#include <Geode/modify/CCFileUtils.hpp>
class $modify(CCFileUtilsExt, CCFileUtils) {
    inline static std::map<std::string, std::vector<fs::path>> rand_globs;
    static auto updateFilePathViaRandFeature(fs::path path) {
        auto mod_resources = Mod::get()->getResourcesDir();
        auto name = (fs::path(path).filename()).replace_extension("");
        auto rand_mark = (mod_resources / name).string() + ".rand";
        if (fs::exists(rand_mark)) {
            auto glob_tar = (mod_resources / name).string() + "_rand*";
            auto paths = rand_globs[glob_tar];
            if (paths.size() > 0) {
                auto nrandval = Mod::get()->getSavedValue<fs::path>(glob_tar);
                if (nrandval.has_filename()) return nrandval;
                auto rndpt = *select_randomly(paths.begin(), paths.end());
                return rndpt;
            }
        }
        return path;
    }
    $override gd::string fullPathForFilename(const char* pszFileName, bool unkBoolean) {
        auto path = fs::path(CCFileUtils::fullPathForFilename(pszFileName, unkBoolean).c_str());
        path = updateFilePathViaRandFeature(path);
        //log::debug("{}.path = {}", __FUNCTION__, path);
        return path.string().c_str();
    }
    $override virtual unsigned char* getFileData(const char* pszFileName, const char* pszMode, unsigned long* pSize) {
        //log::debug("{}.pszFileName = {}", __FUNCTION__, pszFileName);
        if (string::contains(pszFileName, "ui/")) {
            auto myfp = Mod::get()->getID() + "/" + string::replace(pszFileName, "ui/", "ui.");
            //log::debug("{}.myfp = {}", __FUNCTION__, myfp);
            if (cocos::fileExistsInSearchPaths(myfp.c_str())) {
                pszFileName = myfp.c_str();
            }
        }
        //log::debug("{}.path = {}", __FUNCTION__, path);
        return CCFileUtils::getFileData(pszFileName, pszMode, pSize);
    }
};

//preload rand globs
$on_mod(Loaded){
    log::debug("preloading rand globs now");
    auto resources = Mod::get()->getResourcesDir();
    for (auto mark : fs::glob::glob((resources / "").string() + "*.rand")) {
        auto name = (fs::path(mark).filename()).replace_extension("");
        auto glob_tar = (resources / name).string() + "_rand*";
        if (not CCFileUtilsExt::rand_globs.contains(glob_tar)) {
            auto newglob = fs::glob::glob(glob_tar);
            CCFileUtilsExt::rand_globs[glob_tar] = newglob;
            log::debug("rand glob added {}, founded {} files.", glob_tar, newglob.size());
        };
    };
}

//play other songs
#include <Geode/modify/FMODAudioEngine.hpp>
class $modify(FMODAudioEngineExt, FMODAudioEngine) {
    inline static bool dontPlayNextMusic = false;
    $override void playMusic(gd::string strPath, bool shouldLoop, float fadeInTime, int channel) {
        auto path = fs::path(strPath.c_str());
        if (dontPlayNextMusic) {
            dontPlayNextMusic = false;
            return void();//log::debug("{}.dontPlayNextMusic ({} skip)", __FUNCTION__, path.filename());
        }
        path = CCFileUtilsExt::updateFilePathViaRandFeature(path);
        //log::debug("{}.path = {}", __FUNCTION__, path);
        return FMODAudioEngine::playMusic(path.string().c_str(), shouldLoop, fadeInTime, channel);
    }
    $override void playEffectAdvanced(gd::string strPath, float speed, float p2, float volume, float pitch, bool fastFourierTransform, bool reverb, int startMillis, int endMillis, int fadeIn, int fadeOut, bool loopEnabled, int p12, bool override, bool p14, int p15, int uniqueID, float minInterval, int sfxGroup) {
        auto path = fs::path(strPath.c_str());
        path = CCFileUtilsExt::updateFilePathViaRandFeature(path);
        //log::debug("{}.path = {}", __FUNCTION__, path);
        return FMODAudioEngine::playEffectAdvanced(path.string().c_str(), speed, p2, volume, pitch, fastFourierTransform, reverb, startMillis, endMillis, fadeIn, fadeOut, loopEnabled, p12, override, p14, p15, uniqueID, minInterval, sfxGroup);
    }
};
//dont play other songs
#include <Geode/modify/MenuLayer.hpp>
class $modify(MenuLayerMenuLoopFix, MenuLayer) {
    $override bool init() {
        FMODAudioEngineExt::dontPlayNextMusic = true;
        return MenuLayer::init();
    }
};

//norand selector
#include <Geode/modify/GJGarageLayer.hpp>
class $modify(GJGarageLayerNoRandSelector, GJGarageLayer) {
    $override bool init() {
        if (!GJGarageLayer::init()) return 0;

        if (auto shards_menu = typeinfo_cast<CCMenu*>(this->getChildByIDRecursive("shards-menu"))) {
            auto openupbtnimage = CCLabelBMFont::create("Rand\nSel.", "goldFont.fnt");
            openupbtnimage->setAlignment(kCCTextAlignmentCenter);
            openupbtnimage->setScale(0.5f);
            auto openupbtn = CCMenuItemExt::createSpriteExtra(openupbtnimage,
                [](auto) {
                    auto popup = FLAlertLayer::create(
                        "No Rand Selector", 
                        "\n \n \n \n          \n \n \n \n ", 
                        "Close"
                    );
                    popup->show();

                    auto scroll = ScrollLayer::create({ 244.f, 176.f });
                    scroll->m_contentLayer->setLayout(ColumnLayout::create()
                        ->setGap(0.f)
                        ->setAxisAlignment(AxisAlignment::End)
                        ->setAxisReverse(true)
                    );
                    scroll->m_contentLayer->setContentHeight(0.f);
                    scroll->setPosition(scroll->getContentWidth() / -2, 24.f);
                    popup->m_buttonMenu->addChild(scroll);

                    auto sbar = Scrollbar::create(scroll);
                    sbar->setPosition(
                        8.f + scroll->getContentWidth() / 2, 
                        scroll->getPositionY() + scroll->getContentHeight() / 2
                    );
                    popup->m_buttonMenu->addChild(sbar);

                    handleTouchPriority(popup);

                    auto addToScroll = [scroll](CCNode* node) {
                        auto content = scroll->m_contentLayer;
                        content->addChild(node);
                        content->setContentHeight(
                            content->getContentHeight() + node->getContentHeight()
                        );
                        content->updateLayout();
                        };

                    auto createItemHeader = [scroll](std::string name) {
                        auto layer = CCLayerColor::create({ 0, 0, 0, 110 });

                        layer->setContentWidth(scroll->getContentWidth());
                        layer->setContentHeight(38.f);

                        auto label = CCLabelBMFont::create(name.c_str(), "geode.loader/mdFontB.fnt");
                        label->setAnchorPoint({ 0.f, 0.5f });
                        label->setScale(0.8);
                        layer->addChildAtPosition(label, Anchor::Left, { 12,0 });

                        return layer;
                        };

                    auto itemAltBG = CCNode::create();
                    auto createItem = [itemAltBG, scroll](std::string of, fs::path file) {
                        auto layer = CCLayerColor::create({0, 0, 0, 70});

                        if (itemAltBG->isVisible()) layer->setOpacity(90);
                        itemAltBG->setVisible(!itemAltBG->isVisible());

                        layer->setContentWidth(scroll->getContentWidth());
                        layer->setContentHeight(32.f);

                        auto paddingx = 12.f;

                        auto label = CCLabelBMFont::create(
                            fs::path(file).filename().string().c_str(),
                            "chatFont.fnt"
                        );
                        label->setScale(0.7);
                        label->limitLabelWidth(
                            scroll->getContentWidth() - (paddingx * 2),
                            0.7, 0.1
                        );
                        auto menuitem = CCMenuItemExt::createSpriteExtra(
                            label, [layer, scroll, of, file](CCNode* item) {
                                Ref<CCNode> selBG = scroll->getChildByIDRecursive("selBG." + of);
                                if (selBG) {
                                    if (layer->getChildByID(selBG->getID())) {
                                        selBG->removeFromParentAndCleanup(0);
                                        scroll->m_verticalScrollbar->addChild(selBG);
                                        Mod::get()->getSaveContainer().erase(of);
                                    }
                                    else {
                                        selBG->removeFromParentAndCleanup(0);
                                        layer->addChild(selBG);
                                        Mod::get()->setSavedValue(of, file);
                                    }
                                };
                                if (string::contains(of, "menuLoop")) {
                                    GameManager::get()->fadeInMenuMusic();
                                }
                                if (string::contains(of, "explode_11")) {
                                    FMODAudioEngine::get()->playEffect("explode_11.ogg");
                                }
                            }
                        );
                        menuitem->setAnchorPoint({ 0.f, 0.5f });
                        menuitem->m_animationEnabled = 0;
                        menuitem->m_colorEnabled = 1;

                        auto menu = CCMenu::create(menuitem, nullptr);
                        layer->addChildAtPosition(menu, Anchor::Left, { paddingx,0 });
                        auto selected_file = Mod::get()->getSavedValue<fs::path>(of);
                        if (selected_file == file) {
                            Ref<CCNode> selBG = scroll->getChildByIDRecursive("selBG." + of);
                            if (selBG) {
                                selBG->removeFromParentAndCleanup(0);
                                layer->addChild(selBG);
                            };
                        };

                        return layer;
                        };

                    auto createSelBG = [createItem](std::string id) {
                        auto layer = CCScale9Sprite::create("groundSquare_01_001.png");
                        layer->setColor({ 255 , 255 ,0 });
                        layer->setOpacity(72);
                        layer->setID("selBG." + id);
                        layer->setScale(0.2f);
                        layer->setContentSize(createItem("asd", "asd")->getContentSize() * 5);
                        layer->setAnchorPoint(CCPointZero);
                        layer->setZOrder(-10);
                        return layer;
                        };

                    for (auto ew : CCFileUtilsExt::rand_globs) {
                        auto ew1expl = string::explode(Mod::get()->getID(), ew.first);
                        addToScroll(createItemHeader(ew1expl.at(ew1expl.size() - 1)));
                        scroll->m_verticalScrollbar->addChild(createSelBG(ew.first));
                        for (auto ps : ew.second) addToScroll(createItem(ew.first, ps));
                    }

                }
            );
            shards_menu->addChild(openupbtn);
            shards_menu->updateLayout();
        }

        return 1;
    }
};
