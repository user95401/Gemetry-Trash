#pragma once
#include <_main.hpp>
#include <_ImGui.hpp>

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
		return FMODAudioEngine::playEffectAdvanced(name.c_str(), speed, p2, volume, pitch, fastFourierTransform, reverb, startMillis, endMillis, fadeIn, fadeOut, loopEnabled, p12, override, p14, p15, uniqueID, minInterval, sfxGroup);
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
			ImGuiCocosExt::id_mapped_drawings["RAND_FILENAMES_LIST"] = (
				[] -> void {
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
				}
				);
		};
	}
	bool init(bool penis) {
		resourceSetup();
		return LoadingLayer::init(penis);
	}
};

#include <Geode/modify/MenuLayer.hpp>
class $modify(MenuLayerResourcesExt, MenuLayer) {
	bool init() {
		auto deps = getMod()->getMetadata().getDependencies();
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
			};
		}
		return MenuLayer::init();
	}
};