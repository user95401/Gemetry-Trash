#pragma once
#include <_main.hpp>
#include <_ImGui.hpp>
#include <misc/cpp/imgui_stdlib.h>

static inline auto RAND_FILENAMES_LIST = std::map<std::string, std::vector<fs::path>>();

#include <Geode/modify/CCFileUtils.hpp>
class $modify(CCFileUtilsResourcesExt, CCFileUtils) {
	virtual gd::string fullPathForFilename(const char* pszFileName, bool skipSuffix) {
		auto name = string::pathToString(pszFileName);
		if (RAND_FILENAMES_LIST.contains(name)) {
			auto names = RAND_FILENAMES_LIST[name];
			name = string::pathToString(
				fs::path("__rand") / name / names[rand() % names.size()]
			).c_str();
		}
		if (pszFileName != name) log::debug(R"(changed "{}" to "{}" for full path)", pszFileName, name);
		return CCFileUtils::get()->fullPathForFilename(name.c_str(), skipSuffix);
	}
};

#include <Geode/modify/FMODAudioEngine.hpp>
class $modify(FMODAudioEngineResourcesExt, FMODAudioEngine) {
	int playEffectAdvanced(gd::string strPath, float speed, float p2, float volume, float pitch, bool fastFourierTransform, bool reverb, int startMillis, int endMillis, int fadeIn, int fadeOut, bool loopEnabled, int p12, bool override, bool p14, int p15, int uniqueID, float minInterval, int sfxGroup) {
		auto name = string::pathToString(strPath.c_str());

		if (RAND_FILENAMES_LIST.contains(name)) {
			auto names = RAND_FILENAMES_LIST[name];
			name = string::pathToString(
				fs::path("__rand") / name / names[rand() % names.size()]
			).c_str();
		}
		if (strPath.c_str() != name) log::debug(R"(changed "{}" to "{}" for full path)", strPath.c_str(), name);

		if (string::contains(strPath.c_str(), "explode_11.ogg")) FMODAudioEngine::playEffectAdvanced(
			rndb() ? strPath : (std::string("/") + strPath.c_str()).c_str(), 
			speed, p2, volume, pitch, fastFourierTransform, reverb, startMillis,
			endMillis, fadeIn, fadeOut, loopEnabled, p12, override, p14, p15, uniqueID, minInterval, sfxGroup
		);

		return FMODAudioEngine::playEffectAdvanced(
			name.c_str(), speed, p2, volume, pitch, fastFourierTransform, reverb, startMillis,
			endMillis, fadeIn, fadeOut, loopEnabled, p12, override, p14, p15, uniqueID, minInterval, sfxGroup
		);
	}
};

#include <Geode/modify/LoadingLayer.hpp>
class $modify(LoadingLayerResourcesExt, LoadingLayer) {
	static auto pathQ(fs::path path) {
		auto name = string::pathToString(path);
		size_t qstart = name.find('[');
		size_t qend = name.find(']', qstart);
		auto query = (qstart != std::string::npos && qend != std::string::npos) ?
			name.substr(qstart + 1, qend - qstart - 1) : "";
		query = string::replace(query, "..", "/");
		query = string::replace(query, "{game}", string::pathToString(dirs::getGeodeDir()));
		query = string::replace(query, "{save}", string::pathToString(dirs::getSaveDir()));
		query = string::replace(query, "{resources}", string::pathToString(getMod()->getResourcesDir()));
		return query;
	}
	static void resourceSetup() {
		auto resources_dir = getMod()->getResourcesDir();

		RAND_FILENAMES_LIST.clear();

		auto tp = CCTexturePack();
		tp.m_paths = { string::pathToString(resources_dir).c_str() };
		tp.m_id = "resources"_spr;
		CCFileUtils::get()->addTexturePack(tp);

		for (const auto& entry : std::filesystem::recursive_directory_iterator(resources_dir, fs::err)) {
			if (!(entry.is_regular_file())) continue;
			if (!(entry.path().parent_path() == resources_dir)) continue;

			auto query = pathQ(entry.path());
			auto path = string::pathToString(entry.path());
			auto name = string::pathToString(entry.path().filename());
			auto ext = string::toLower(string::pathToString(entry.path().extension()));

			if (string::containsAll(name, { "__unzip", ".zip" })) {
				auto unzipPath = query;

				auto forced = string::contains(name, "_forced");

				fs::create_directories(unzipPath, fs::err);

				if (auto a = fs::Unzip::create(entry.path()); auto unzip = &a.unwrap()) {

					std::set<fs::path> unzipPaths;
					for (const auto& entry : unzip->getEntries()) unzipPaths.insert(unzipPath / entry);

					auto allFilesExtracted = true;
					for (const auto& entry : unzipPaths) if (not fs::exists(entry, fs::err)) {
						allFilesExtracted = false;
						break;
					};

					if (!allFilesExtracted or forced) {
						if (auto err = unzip->extractAllTo(unzipPath).err()) log::error("{}", err);
						else log::info("{} extracted to {}", entry.path(), unzipPath);
					};

				}
			}
		}

		for (const auto& entry : std::filesystem::recursive_directory_iterator(resources_dir, fs::err)) {
			if (!(entry.is_regular_file())) continue;
			if (!(entry.path().parent_path() == resources_dir)) continue;

			auto query = pathQ(entry.path());
			auto path = string::pathToString(entry.path());
			auto name = string::pathToString(entry.path().filename());
			auto ext = string::toLower(string::pathToString(entry.path().extension()));

			auto relativePath = string::pathToString(std::filesystem::relative(path, resources_dir));
			auto asSubFolder = resources_dir / getMod()->getID() / entry.path().filename();

			if (string::containsAll(name, { "__rand", ".zip" })) {
				auto fileName = query;

				fs::path unzipPath = resources_dir / "__rand" / fileName / "";
				fs::create_directories(unzipPath, fs::err);
				if (auto a = fs::Unzip::create(path); auto unzip = &a.unwrap()) {
					
					std::set<fs::path> unzipPaths;
					for (const auto& entry : unzip->getEntries()) unzipPaths.insert(unzipPath / entry);

					auto allFilesExtracted = true;
					for (const auto& entry : unzipPaths) if (!fs::exists(entry, fs::err)) {
						allFilesExtracted = false;
						break;
					};

					if (allFilesExtracted) {
						for (const auto& entry : unzip->getEntries()) RAND_FILENAMES_LIST[fileName].push_back(entry);
					}
					else {
						if (auto err = unzip->extractAllTo(unzipPath).err()) log::error("{}", err);
						else {
							for (const auto& entry : unzip->getEntries()) RAND_FILENAMES_LIST[fileName].push_back(entry);
							log::info(
								"{} extracted to {}, \nrand list for {}: {}",
								path, unzipPath, fileName, RAND_FILENAMES_LIST[fileName]
							);
						}
					};
				}
			}

			if (string::containsAll(path, { ".." })) {
				fs::path newpath = string::replace(path, "..", "/");
				fs::create_directories(newpath.parent_path(), fs::err);
				fs::rename(path, newpath, fs::err);
				if (string::containsAny(ext, { ".fnt", ".plist" })) {
					auto asd = fs::readString(newpath).unwrapOr(""); 
					asd = string::replace(asd, "..", "/");
					if (ext == ".fnt") asd = string::replace(asd, "unicode=1", "unicode=0");
					if (ext == ".fnt") asd = string::replace(asd, "geode.loader/", "");
					if (auto err = fs::writeString(newpath, asd).err()) log::error("{}", err);
				}
			}

			if (string::contains(path, "Sheet") and string::containsAny(path, { "GJ_", "SecretSheet" })) {
				fs::create_directories(asSubFolder.parent_path(), fs::err);
				fs::rename(path, asSubFolder, fs::err);
				if (fs::err) log::error("{}", fs::err.message());
				if (string::containsAny(ext, { ".fmt", ".plist" })) {
					if (auto err = fs::writeString(asSubFolder, string::replace(
						fs::readString(asSubFolder).unwrapOr(""),
						fmt::format("<key>{}/", getMod()->getID()), "<key>"
					)).err()) log::error("{}", err);
				}
			}

			if (string::containsAny(ext, { ".png", ".jpg", ".jpeg" })) {
				auto sprite = CCSprite::create();
				sprite->initWithFile(relativePath.c_str());
				CCSpriteFrameCache::get()->addSpriteFrame(
					sprite->displayFrame(), relativePath.c_str()
				);
			}
		}
		if (CCKeyboardDispatcher::get()->getControlKeyPressed()) {
			ImGuiCocosExt::id_mapped_drawings.erase("RAND_FILENAMES_LIST");
			ImGuiCocosExt::id_mapped_drawings["RAND_FILENAMES_LIST"] = [] -> void
				{
					bool windopn = true;
					ImGui::Begin("RAND_FILENAMES_LIST", &windopn);

					if (!windopn) return (void)ImGuiCocosExt::id_mapped_drawings.erase("RAND_FILENAMES_LIST");

					for (const auto& entry : RAND_FILENAMES_LIST) {
						if (ImGui::TreeNode(entry.first.c_str())) {
							for (const auto& name : entry.second) {
								ImGui::Text("%s", string::pathToString(name).c_str());
							}
							ImGui::TreePop();
						}
					}

					ImGui::End();
				};
			ImGuiCocosExt::id_mapped_drawings["RESOURCE_TOOLS"] = [] -> void
				{
					bool windopn = true;
					ImGui::Begin("RESOURCE_TOOLS", &windopn);
					if (!windopn) return (void)ImGuiCocosExt::id_mapped_drawings.erase("RESOURCE_TOOLS");
					
					auto targetDir = dirs::getModsDir();
					auto shouldDoArchives = false;

					ImGui::SeparatorText("Archives creator");

					auto archiveFilePrefix = getMod()->getSavedValue<std::string>("archiveFilePrefix", "__unzip[{game}..mods] --part");
					if (ImGui::InputText("file prefix", &archiveFilePrefix)) getMod()->setSavedValue("archiveFilePrefix", archiveFilePrefix);

					if (ImGui::Button("run for mods dir")) {
						shouldDoArchives = true;
						targetDir = dirs::getModsDir();
					}

					if (ImGui::Button("run for temp dir")) {
						shouldDoArchives = true;
						targetDir = dirs::getTempDir();
					}


					if (shouldDoArchives) {
						getMod()->uninstall();

						constexpr uintmax_t MB24 = 24 * 1024 * 1024;
						std::vector<std::vector<fs::path>> parts;
						uintmax_t currentPartSize = 0;
						std::vector<fs::path> currentPart;

						auto files = fs::readDirectory(targetDir).unwrapOrDefault();

						files.erase(std::remove_if(files.begin(), files.end(), 
							[](const fs::path& p) { return fs::is_directory(p); }
						), files.end());

						for (const auto& entry : files) {
							uintmax_t fileSize = fs::file_size(entry, fs::err);

							if (fileSize > MB24) continue;

							if (currentPartSize + fileSize > MB24) {
								parts.push_back(std::move(currentPart));
								currentPart.clear();
								currentPartSize = 0;
							}

							currentPart.push_back(entry);
							currentPartSize += fileSize;
						}

						if (!currentPart.empty()) {
							parts.push_back(std::move(currentPart));
						}

						auto zipOutDir = dirs::getTempDir().parent_path() / "makdzipsingtps";
						fs::remove_all(zipOutDir, fs::err);
						fs::create_directories(zipOutDir, fs::err);
						for (int i = 0; i < parts.size(); i++) {
							auto zipper = fs::Zip::create(zipOutDir / fmt::format("{}{}.zip", archiveFilePrefix, i + 1));
							if (zipper.isOk()) for (const auto& file : parts[i]) zipper.unwrap().addFrom(file);
						}
						fs::openFolder(zipOutDir);
					}

					ImGui::End();
				};
		};
	}
	bool init(bool penis) {
		// fix black textures while image plus installed (force-autodetect)
		if (auto mod = Loader::get()->getInstalledMod("prevter.imageplus")) mod->setSettingValue("force-autodetect", false);
		// setup
		resourceSetup();
		// yes
		return LoadingLayer::init(penis);
	}
};

#include <Geode/modify/MenuLayer.hpp>
class $modify(MenuLayerResourcesExt, MenuLayer) {
	bool init() {

		auto deps = getMod()->getMetadataRef().getDependencies();
		for (const auto& dep : deps) {
			if (!Loader::get()->isModInstalled(dep.id)) game::restart(
				[dep] { log::warn("{} isn't installed, restarting", dep.id); return 1; }()
			);
			if (!Loader::get()->isModLoaded(dep.id)) {
				auto _ = Notification::create(
					" The " + dep.id + " isn't loaded!\n Enable it please..",
					NotificationIcon::Error, 0.f
				);
				_->show();
				auto menu = CCMenu::create();
				_->addChild(menu);
				menu->setPosition(CCPointZero);
				menu->addChild(CCMenuItemExt::createSpriteExtra(CCLayer::create(), [=](void*) {
					createQuickPopup(
						"Ready to restart?", "", "Restart", "Later", [=](void*, bool a) {
							if (!a) game::restart();
						}
					);
					geode::openInfoPopup(dep.id);
					}));
				menu->setAnchorPoint(CCPointZero);
				menu->setScaleX(1.200f);
				menu->setScaleY(0.175f);
			};
		}

		auto hmmm = fileExistsInSearchPaths((getMod()->getTempDir() / GEODE_MOD_ID".android64.so").string().c_str());
		if (CCKeyboardDispatcher::get()->getControlKeyPressed() or hmmm) {
			auto get_release_data_listener = new EventListener<web::WebTask>;

			get_release_data_listener->bind(
				[this, get_release_data_listener](web::WebTask::Event* e) {
					if (web::WebProgress* prog = e->getProgress()) {
						//log::debug("{}", prog->downloadTotal());

						if (prog->downloadTotal() > 0) void(); else return;

						auto installed_size = fs::file_size(getMod()->getPackagePath(), fs::err);
						auto actual_size = prog->downloadTotal();

						if (installed_size == actual_size) return;

						auto pop = geode::createQuickPopup(
							"Update!",
							fmt::format(
								"Latest release size mismatch with installed one :D"
								"\n" "Download latest release of mod?"
							),
							"Later.", "Yes", [this](CCNode* pop, bool Yes) {
								if (!Yes) return;

								this->setVisible(0);

								auto req = web::WebRequest();

								auto state_win = Notification::create("Downloading... (///%)");
								state_win->setTime(1337.f);
								state_win->show();

								if (state_win->m_pParent) {
									state_win->m_pParent->addChild(geode::createLayerBG());
								}

								auto listener = new EventListener<web::WebTask>;
								listener->bind(
									[state_win](web::WebTask::Event* e) {
										if (web::WebProgress* prog = e->getProgress()) {
											auto strr = fmt::format("Downloading... ({}%)", (int)prog->downloadProgress().value_or(000));
											static std::string last_str;
											if (last_str != strr) { last_str = strr; state_win->setString(strr); }
										}
										if (web::WebResponse* res = e->getValue()) {
											std::string data = res->string().unwrapOr("no res");
											if (res->code() < 399) {
												log::debug("{}", res->into(getMod()->getPackagePath()).err());
												game::restart();
											}
											else {
												auto asd = geode::createQuickPopup(
													"Request exception",
													data,
													"Nah", nullptr, 420.f, nullptr, false
												);
												asd->show();
											};
										}
									}
								);

								listener->setFilter(req.send(
									"GET",
									repo_lnk + "/releases/latest/download/" GEODE_MOD_ID ".geode"
								));

							}, false
						);
						pop->m_scene = this;
						pop->show();

						e->cancel();
						get_release_data_listener->disable();
						delete get_release_data_listener;
					}
				}
			);
			get_release_data_listener->setFilter(
				web::WebRequest().get(repo_lnk + "/releases/latest/download/" GEODE_MOD_ID ".geode")
			);
		}
		else Notification::create("Update check was aborted because it's a dev build...")->show();

		return MenuLayer::init();
	}
};