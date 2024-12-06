#pragma once
#include <_main.hpp>
#include <Geode/utils/web.hpp>

inline std::map<int, std::string> mods_list;
inline auto mods_list_version = std::string();
inline auto mods_list_ver_file = dirs::getModsDir() / ".list_version";

inline Ref<Slider> loadingBar = nullptr;

inline void downloadModsFromList(int id = 0, int attempts = 0, bool failed = 0) {
    log::debug("{}(id {})", __func__, id);
    auto url = mods_list[id];
    auto filename = fs::path(url).filename();
    auto req = web::WebRequest();
    auto listener = new EventListener<web::WebTask>;
    listener->bind(
        [id, attempts, failed, filename](web::WebTask::Event* e) {
            auto gonext = [id, attempts, failed](bool retry = false) {
                auto nextid = id + 1;
                auto isfailed = failed;
                if (retry and attempts <= 6) nextid = id;
                else if (retry) isfailed = true;
                if (mods_list.contains(nextid)) {
                    downloadModsFromList(nextid, attempts + 1, isfailed);
                }
                else if (!isfailed) {
                    std::ofstream(mods_list_ver_file) << mods_list_version;
                    game::restart();
                }
                };
            if (web::WebResponse* res = e->getValue()) {
                std::string data = res->string().unwrapOr("no res");
                if ((res->code() < 399) and (res->code() > 10)) {
                    res->into(dirs::getModsDir() / filename);
                    auto msg = fmt::format("{}\n installed! id: {}", filename, id);
                    AchievementNotifier::sharedState()->notifyAchievement(
                        "", msg.data(), "GJ_infoIcon_001.png", 1);
                    AchievementNotifier::sharedState()->showNextAchievement();
                    log::info("{}", string::replace(msg, "\n", ""));
                    return gonext();
                }
                else {
                    auto msg = fmt::format(
                        "{} \nFail! Error: {}", 
                        filename, res->string().unwrapOr(res->string().isErr() ? res->string().unwrapErr() : "")
                    );
                    AchievementNotifier::sharedState()->notifyAchievement(
                        "", msg.data(), "miniSkull_001.png", 1);
                    AchievementNotifier::sharedState()->showNextAchievement();
                    log::error("{}", string::replace(msg, "\n", ""));
                    return gonext(true);
                }
            }
            if (auto prog = e->getProgress()) {
                if (loadingBar) loadingBar->setValue(prog->downloadProgress().value_or(0.f) / 100);
            }
        }
    );
    listener->setFilter(req.send("GET", url));
}

inline void getListAndStartDownloadingMods() {
    log::debug("{}()", __func__);
    auto req = web::WebRequest();
    auto listener = new EventListener<web::WebTask>;
    listener->bind(
        [](web::WebTask::Event* e) {
            if (web::WebResponse* res = e->getValue()) {
                std::string data = res->string().unwrapOr("no res");
                auto parse = res->json();
                if ((res->code() < 399) and (res->code() > 10) and parse.isOk()) {
                    
                    auto json = parse.unwrapOrDefault();
                    log::debug("{}", json.dump(4));

                    if (auto list = json["list"].asArray()) {
                        auto temp_id = 0;
                        for (auto url : list.unwrap()) {
                            mods_list[temp_id] = url.asString().unwrapOrDefault();
                            log::debug("debug: mods_list[{}] = {}", temp_id, mods_list[temp_id]);
                            temp_id++;
                        }
                    };

                    mods_list_version = json["version"].asString().unwrapOrDefault();

                    if (fs::exists(mods_list_ver_file)) {
                        auto current_ver = fs::read(mods_list_ver_file);

                        log::debug("mods_list_version=\"{}\"", mods_list_version);
                        log::debug("current_ver=\"{}\"", current_ver);

                        if (std::string(mods_list_version) == std::string(current_ver))
                            return log::debug("last list was installed");
                    }
                    auto popup = createQuickPopup(
                        "Downloading mods...",
                        "\n"
                        "<cr>DON'T CLOSE THE GAME</c>""\n"
                        "game will be restarted after finish",
                        "a", "a", [](auto, auto) {}, false
                    );
                    popup->m_button1->setVisible(0);
                    popup->m_button2->setVisible(0);
                    loadingBar = Slider::create(popup, nullptr);
                    popup->m_buttonMenu->addChild(loadingBar);
                    popup->setContentSize(CCSize(1, 1) * 2222);
                    SceneManager::get()->keepAcrossScenes(popup);
                    downloadModsFromList();
                }
            }
        }
    );
    listener->setFilter(req.send(
        "GET", (raw_content_repo_lnk + "/data/" + "/mods.list.json")
    ));
}

//$on_mod(Loaded){
//    getListAndStartDownloadingMods();
//};
#include <Geode/modify/MenuLayer.hpp>
class $modify(DownloadModsMenuLayerExt, MenuLayer) {
    inline static auto enabled = true;
    $override bool init() {
        if (enabled) {
            enabled = false;
            getListAndStartDownloadingMods();
        }
        return MenuLayer::init();
    }
};