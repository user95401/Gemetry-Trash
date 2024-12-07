#pragma once
#include <_main.hpp>

#include <Geode/modify/CCScene.hpp>
class $modify(PopupRandomMeme, CCScene) {
    inline static bool can_asd = false;
    static void loadAndShow() {

        web::WebRequest req = web::WebRequest();
        req.header(
            "x-rapidapi-key",//:>
            ZipUtils::base64URLDecode("MWI0NzE0YTFiNW1zaDY0YmM3NDdmODg3YjcyOHAxNTViMjNqc25jN2UyN2I2N2ExOTM=")
        );

        auto listener = new EventListener<web::WebTask>;
        listener->bind(
            [listener](web::WebTask::Event* e) {
                if (web::WebResponse* res = e->getValue()) {

                    //log::info("headers: {}", res->headers());
                    if (string::contains(res->header("content-type").value_or("asd"), "image")) {
                        //log::info("image");
                        auto path = getMod()->getTempDir() / ".rand_meme_image";
                        auto save_result = res->into(path);
                        auto sprite = CCSprite::create(path.string().c_str());
                        CCTextureCache::get()->removeTextureForKey(path.string().c_str());
                        fs::remove(path, fs::last_err_code);

                        auto popup = createQuickPopup("", "", "", "", [](auto, auto) {});

                        popup->m_mainLayer->removeAllChildrenWithCleanup(0);
                        popup->m_mainLayer->addChild(popup->m_buttonMenu);
                        popup->m_button1->getParent()->setContentSize(CCSize(1, 1) * 7777);
                        popup->m_button1->setVisible(0);
                        popup->m_button2->setVisible(0);

                        popup->m_mainLayer->addChild(sprite);
                        sprite->setPosition(popup->getContentSize() / 2);
                        limitNodeSize(sprite, popup->getContentSize() - CCSize(60, 40), 8.f, 0.1f);

                        return;
                    }

                    auto json = res->json();
                    auto value = matjson::Value(json.unwrapOr(json.isErr() ? json.unwrapErr() : ""));
                    //log::info("{}", value);

                    if (json.isErr()) return;
                    if (value.isArray() == false) return;
                    //log::info("{}", "as_array");
                    //log::info("value {}", value);
                    auto the_memes_list = value;
                    auto rand_meme = *select_randomly(the_memes_list.begin(), the_memes_list.end());
                    //log::info("rand_meme {}", rand_meme);
                    web::WebRequest req = web::WebRequest();
                    listener->setFilter(req.get(rand_meme["image"].asString().unwrapOr("no meme?(")));
                }
            }
        );
        listener->setFilter(req.get("https://programming-memes-images.p.rapidapi.com/v1/memes"));
    };
    void asd(float) {

        if (this->getChildByIDRecursive("LoadingLayer")) {
            can_asd = true;
            return;
        }
        if (not can_asd) return;

        loadAndShow();
        can_asd = false;
    };
    $override static CCScene* create(void) {
        auto __this = CCScene::create();
        if (SETTING(bool, "Show Random Meme After Loading")) {
            __this->scheduleOnce(schedule_selector(PopupRandomMeme::asd), 0.0f);
        };
        return __this;
    }
};