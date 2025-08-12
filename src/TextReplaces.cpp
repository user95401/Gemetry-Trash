#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <alphalaneous.alphas_geode_utils/include/Utils.h>
using namespace geode::prelude;

#include <regex>

$execute{ GEODE_WINDOWS(SetConsoleOutputCP(65001)); };

//data values
inline static matjson::Value replaces;

enum TreeMapType {
    IDxNames,  // type##id
    TypeNames, // type
    IDs        // id or type
};

auto getNodeParentsTree(CCNode* node, TreeMapType mapType = TreeMapType::IDs) {
    auto map = std::map<std::string, unsigned int>(); // Используем std::string вместо const char*
    if (!node) return map; // Проверка на nullptr

    while (node->m_pParent) {
        auto name = AlphaUtils::Cocos::getClassName(node);
        auto id = node->getID(); // Исправлено: получаем ID правильно

        std::string key;
        switch (mapType) {
        case IDxNames:
            key = name + "##" + id;
            break;
        case TypeNames:
            key = name;
            break;
        case IDs:
            key = id.empty() ? name : id;
            break;
        }
        map[key] = map.size() + 1;
        node = node->m_pParent;
    }
    return map;
}

bool fontHasAllCharsForStr(const std::string& file, const std::string& str) {

    static std::unordered_map<std::string, std::unordered_set<uint32_t>> charsets;

    if (not charsets.contains(file)) {
        auto content = file::readString(
            CCFileUtils::get()->fullPathForFilename(file.c_str(), 0).c_str()
        ).unwrapOrDefault();
        std::unordered_set<uint32_t> charset;
        std::istringstream stream(content);
        std::string line;

        while (std::getline(stream, line)) {
            if (line.find("char id=") == 0) {
                size_t pos = line.find("id=") + 3;
                size_t end = line.find(' ', pos);
                if (end == std::string::npos) end = line.length();

                uint32_t charId = std::stoul(line.substr(pos, end - pos));
                charset.insert(charId);
            }
        }
        charsets[file] = std::move(charset);
    }

    auto decodeUtf8 = [](const char*& ptr) -> uint32_t {
        unsigned char b = *ptr++;
        if (b < 0x80) return b;
        if ((b & 0xE0) == 0xC0) return ((b & 0x1F) << 6) | (*ptr++ & 0x3F);
        if ((b & 0xF0) == 0xE0) return ((b & 0x0F) << 12) | ((*ptr++ & 0x3F) << 6) | (*ptr++ & 0x3F);
        if ((b & 0xF8) == 0xF0) return ((b & 0x07) << 18) | ((*ptr++ & 0x3F) << 12) | ((*ptr++ & 0x3F) << 6) | (*ptr++ & 0x3F);
        return 0;
        };

    const char* ptr = str.c_str();
    while (*ptr) {
        uint32_t cp = decodeUtf8(ptr);
        if (!charsets[file].contains(cp)) {
            static std::unordered_set<std::string> logged;
            logged.contains(file) ? void() : log::error(
                "Font \"{}\" doesn't have char \"{}\" in {}", file, cp, charsets[file]
            );
            logged.insert(file);
            return false;
        }
    }

    return true;
}

bool shouldUpdateWithTranslation(CCNode* node, const char* str) {
    if (!node || !str) return false; // nullptr любимый

    // Загружаем replaces
    if (!CCFileUtils::get()->m_fullPathCache.contains("_loc.json")) replaces = file::readJson(
        CCFileUtils::get()->fullPathForFilename("_loc.json", 0).c_str()
    ).unwrap();

    auto translation = replaces[str].asString().unwrapOrDefault();
    if (translation.empty()) {
        translation = str;

#define part_translate(orig, repl, ...)                               \
if (string::contains(translation, orig) __VA_ARGS__)                                   \
translation = string::replace(translation, orig, repl)

#define _add_translate(orig, repl) replaces[orig] = (repl)

#define has(...) string::containsAny(translation, { __VA_ARGS__ })

        auto hasNumbers = has("0", "1", "2", "3", "4", "5", "6", "7", "8", "9");

        part_translate("Uninstall ", R"(Снести )");
        part_translate("Are you sure you want to \n<cr>uninstall</c> <cy>", "Ты рил хочешь <cr>удалить</c> этот\n<cy>");
        _add_translate("Delete the Mod's save data", R"(Удалить сохранённые данные мода)");

        part_translate("Support ", R"(Поддержка )");

        _add_translate("Search Filters", R"(Фильтры поиска)"); //"Search Filters"
        _add_translate("Enabled Only", "Только\nдозволенные"); //"Enabled Only"
        _add_translate("Enabled First", "Сначала\nдозволенные"); //"Enabled First"

        part_translate("Settings for ", R"(Настройки для )"); //"Settings for "
        _add_translate("Search Settings...", R"(Поиск настроек...)"); //"Search Settings..."
        _add_translate("Reset All", R"(Сбросить все)"); //"Reset All"
        part_translate("There is <cg>", R"(У нас тут <cg>)", and hasNumbers and has("</c> updat", " available!")); //"There is <cg>"
        part_translate("There are <cg>", R"(У нас тут <cg>)", and hasNumbers and has("</c> updat", " available!")); //"There is <cg>"
        part_translate("</c> update available!", R"(</c> обнова!)", and hasNumbers and has("There ", " <cg>"));
        part_translate("</c> updates available!", R"(</c> обнов!)", and hasNumbers and has("There ", " <cg>"));
        _add_translate("Show Update", R"(Покаж обнову)"); //"Show Update"
        _add_translate("Hide Update", R"(Скрыть обнову)"); //"Hide Update"
        _add_translate("Update All", R"(Обнови все)"); //"Update All"
        _add_translate("Show Updates", R"(Покаж обновы)"); //"Show Update"
        _add_translate("Hide Updates", R"(Скрыть обновы)"); //"Hide Update"
        _add_translate("Details", R"(Детали)");
        _add_translate("Search Mods", R"(Поиск модов)"); //Search Mods
        _add_translate("There were <cy>errors</c> loading some mods", R"(Были <cy>ошибочки</c> на загрузке модов)");
        _add_translate("Show Errors Only", R"(Покаж тольк ошибки)"); //"Show Errors Only"
        _add_translate("Hide Errors Only", R"(Скрыть эти ошибки)"); //"Hide Errors Only"
        _add_translate("Update Found", R"(Есть обнова)"); //"Update Found"
        _add_translate("Not Installed", R"(Не установлено)"); //"Not Installed"

        part_translate("Attempt ", R"(Пытка )", and hasNumbers);
        part_translate("Opacity: 1.", R"(Прозрачность: 1.)");
        part_translate("Opacity: 0.", R"(Прозрачность: 0.)");

        part_translate("Version: ", R"(Версия: )", and hasNumbers);

        part_translate("Uploaded: ", R"(Загружено: )", and hasNumbers);
        part_translate("Updated: ", R"(Обновлено: )", and hasNumbers);
        part_translate("Stars Requested: ", R"(Хуёв отсосано: )", and hasNumbers);
        part_translate("Moons Requested: ", R"(Пёзд отлизано: )", and hasNumbers);

        auto inLevelInf = string::contains(translation, "Total Attempts</c>: ");
        part_translate("Total Attempts</c>: ", R"(Всего пыток</c>: )", and inLevelInf and hasNumbers);
        part_translate("Total Jumps</c>: ", R"(Всего прыжков</c>: )", and inLevelInf and hasNumbers);
        part_translate("Jumps</c>: ", R"(Прыжков</c>: )", and inLevelInf and hasNumbers);
        part_translate("Normal</c>: ", R"(На нормале</c>: )", and inLevelInf and hasNumbers);
		part_translate("Practice</c>: ", R"(На практике</c>: )", and inLevelInf and hasNumbers);

        auto isPageLabel = string::contains(translation, "Page ");
        part_translate("Page ", R"(Стр. )", and hasNumbers and isPageLabel);
        part_translate(" (Total ", R"( (Всего )", and hasNumbers and isPageLabel);
        part_translate(" to ", R"( на )", and hasNumbers);
        part_translate(" of ", R"( из )", and hasNumbers);

        part_translate(" days ago", R"( деньков нзд)", and hasNumbers);
        part_translate(" day ago",  R"( день назад)", and hasNumbers);
        part_translate(" hours ago", R"( часов назад)", and hasNumbers);
		part_translate(" hour ago", R"( час назад)", and hasNumbers);
        part_translate("Jan", R"(Янв. )");
        part_translate("Feb ", R"(Фев. )");
		part_translate("Mar ", R"(Мар. )");
		part_translate("Apr ", R"(Апр. )");
		part_translate("May ", R"(Май. )");
		part_translate("Jun ", R"(Юн. )");
		part_translate("Jul ", R"(Юл. )");
		part_translate("Aug ", R"(Авг. )");
		part_translate("Sep ", R"(Сеп. )");
		part_translate("Oct ", R"(Окт. )");
		part_translate("Nov ", R"(Нов. )");
		part_translate("Dec ", R"(Дек. )");

#undef part_translate

        if (translation == str) return false;
		replaces[str] = translation;
    };

    //chars test
    if (auto label = findFirstChildRecursive<CCLabelBMFont>(node, [](void*) {return 1; })) {
        if (!fontHasAllCharsForStr(label->getFntFile(), translation)) return false;
    };

    auto parentsTree = getNodeParentsTree(node, TreeMapType::TypeNames);

    if (parentsTree.contains("MultilineBitmapFont")) return false;

    auto isInpPlaceholder = false;
    if (auto label = typeinfo_cast<CCLabelBMFont*>(node)) {
        auto col = label->getColor();
        isInpPlaceholder = ccc3BEqual(col, { 120,170,240 }) ? true : isInpPlaceholder;
        isInpPlaceholder = ccc3BEqual(col, { 108,153,216 }) ? true : isInpPlaceholder;
        isInpPlaceholder = ccc3BEqual(col, { 150,150,150 }) ? true : isInpPlaceholder;
    }
    if (parentsTree.contains("CCTextInputNode") and !isInpPlaceholder) return false;

    std::vector<std::string> wIDS = {
        "creator-name",
        "level-name",
        "username-button",
        "song-name-label",
        "artist-label",
    };

    std::string nodeID = node->getID();
    if (string::containsAny(nodeID, wIDS)) return false;

    if (auto parent = node->getParent()) {
        std::string parentID = parent->getID();
        if (string::containsAny(parentID, wIDS)) return false;
    }

    return true;
}

#include "Geode/modify/CCLabelBMFont.hpp"
class $modify(GDL_CCLabelBMFont, CCLabelBMFont) {
    bool tryUpdateWithTranslation(char const* str) {
        if (!str || !shouldUpdateWithTranslation(this, str)) return false;

        if (this->getString() != std::string(str)) return false;

        auto translation = replaces[str].asString().unwrapOrDefault();
        if (translation.empty()) return false;

        auto point = typeinfo_cast<CCNode*>(this->getUserObject("translation-point"_spr));
        if (point) {
            if (translation != point->getID()) {
                this->setContentSize(point->getContentSize());
                this->setScale(point->getScale());
                this->setUserObject("translation-point"_spr, nullptr);
                return tryUpdateWithTranslation(str);
            }
            else {
				this->setString(translation.c_str());
			}
        }
        else {
            auto size = this->getScaledContentSize();
            auto scale = this->getScale();

            this->setString(translation.c_str());
            limitNodeSize(this, size, this->getScale(), 0.3f);

            point = CCNode::create();
            if (point) { //кековщина
                point->setContentSize(size);
                point->setScale(scale);
                point->setID(translation);
                this->setUserObject("translation-point"_spr, point);
            }
        }
        return true;
    }

    bool initWithString(const char* pszstr, const char* font, float a3, cocos2d::CCTextAlignment a4, cocos2d::CCPoint a5) {
        if (!CCLabelBMFont::initWithString(pszstr, font, a3, a4, a5)) return false;
        // отложенная
        if (pszstr) {
            std::string str = pszstr; // паранорман
            queueInMainThread([__this = Ref(this), str] {
                if (__this && !str.empty()) {
                    __this->tryUpdateWithTranslation(str.c_str());
                }
                });
        }
        return true;
    }

    void setString(const char* newString) {
        CCLabelBMFont::setString(newString ? newString : "");
        this->tryUpdateWithTranslation(newString);
    }
};

#if !defined(GEODE_IS_IOS)

#include "Geode/modify/MultilineBitmapFont.hpp"
class $modify(GDL_MultilineBitmapFont, MultilineBitmapFont) {
    struct Fields {
        float m_textScale = 1.0f;
        std::string m_fontName;
        float m_maxWidth = 0.0f;
    };

    static uint32_t nextUTF8(std::string::iterator & it, std::string::iterator end) {
        if (it >= end)
            return 0;

        unsigned char c1 = static_cast<unsigned char>(*it++);
        if (c1 < 0x80)
            return c1;

        if ((c1 & 0xE0) == 0xC0 && it < end) {
            unsigned char c2 = static_cast<unsigned char>(*it++);
            return ((c1 & 0x1F) << 6) | (c2 & 0x3F);
        }
        if ((c1 & 0xF0) == 0xE0 && it + 1 < end) {
            unsigned char c2 = static_cast<unsigned char>(*it++);
            unsigned char c3 = static_cast<unsigned char>(*it++);
            return ((c1 & 0x0F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
        }
        if ((c1 & 0xF8) == 0xF0 && it + 2 < end) {
            unsigned char c2 = static_cast<unsigned char>(*it++);
            unsigned char c3 = static_cast<unsigned char>(*it++);
            unsigned char c4 = static_cast<unsigned char>(*it++);
            return ((c1 & 0x07) << 18) | ((c2 & 0x3F) << 12) | ((c3 & 0x3F) << 6) | (c4 & 0x3F);
        }
        return c1;
    }
    static void appendUTF8(uint32_t cp, std::string & str) {
        if (cp < 0x80) {
            str += static_cast<char>(cp);
        }
        else if (cp < 0x800) {
            str += static_cast<char>(0xC0 | (cp >> 6));
            str += static_cast<char>(0x80 | (cp & 0x3F));
        }
        else if (cp < 0x10000) {
            str += static_cast<char>(0xE0 | (cp >> 12));
            str += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
            str += static_cast<char>(0x80 | (cp & 0x3F));
        }
        else {
            str += static_cast<char>(0xF0 | (cp >> 18));
            str += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
            str += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
            str += static_cast<char>(0x80 | (cp & 0x3F));
        }
    }

    gd::string readColorInfo(gd::string s) {
        std::string str = s;
        std::string str2;

        for (auto it = str.begin(); it != str.end();) {
            auto cp = nextUTF8(it, str.end());
            str2 += (cp < 128) ? static_cast<char>(cp) : 'W';
        }

        return MultilineBitmapFont::readColorInfo(str2);
    }
    bool initWithFont(const char* p0, gd::string p1, float p2, float p3, cocos2d::CCPoint p4, int p5, bool colorsDisabled) {
        if (!p0) return false;

        m_fields->m_textScale = p2;
        m_fields->m_fontName = p0;
        m_fields->m_maxWidth = p3;

        std::string translatedText = p1;
        if (shouldUpdateWithTranslation(this, p1.c_str())) {
            translatedText = replaces[p1.c_str()].asString().unwrapOr(p1);
        }

        std::string notags = translatedText;
        std::regex tagRegex("(<c.>)|(<\\/c>)|(<d...>)|(<s...>)|(<\\/s>)|(<i...>)|(<\\/i>)");
        notags = std::regex_replace(translatedText, tagRegex, "");

        if (!MultilineBitmapFont::initWithFont(p0, notags, p2, p3, p4, p5, true))
            return false;

        if (!colorsDisabled) {
            m_specialDescriptors = CCArray::create();
            if (!m_specialDescriptors)
                return false;

            m_specialDescriptors->retain();

            MultilineBitmapFont::readColorInfo(translatedText);

            if (m_specialDescriptors && m_characters) {
                for (auto i = 0u; i < m_specialDescriptors->count(); i++) {
                    auto tag = static_cast<TextStyleSection*>(m_specialDescriptors->objectAtIndex(i));
                    if (!tag) continue;

                    if (tag->m_endIndex == -1 && tag->m_styleType == TextStyleType::Delayed) {
                        if (tag->m_startIndex >= 0 && tag->m_startIndex < static_cast<int>(m_characters->count())) {
                            auto child = static_cast<CCFontSprite*>(m_characters->objectAtIndex(tag->m_startIndex));
                            if (child) {
                                child->m_fDelay = tag->m_delay;
                            }
                        }
                    }
                    else {
                        int startIndex = std::max(0, tag->m_startIndex);
                        int endIndex = std::min(tag->m_endIndex, static_cast<int>(m_characters->count() - 1));

                        for (int j = startIndex; j <= endIndex && j >= 0; j++) {
                            if (j < static_cast<int>(m_characters->count())) {
                                auto child = static_cast<CCFontSprite*>(m_characters->objectAtIndex(j));
                                if (!child) continue;

                                switch (tag->m_styleType) {
                                case TextStyleType::Colored: {
                                    child->setColor(tag->m_color);
                                } break;
                                case TextStyleType::Instant: {
                                    child->m_bUseInstant = true;
                                    child->m_fInstantTime = tag->m_instantTime;
                                } break;
                                case TextStyleType::Shake: {
                                    child->m_nShakeIndex = j;
                                    child->m_fShakeIntensity = static_cast<float>(tag->m_shakeIntensity);
                                    child->m_fShakeElapsed = tag->m_shakesPerSecond <= 0 ? 0.0f : 1.0f / tag->m_shakesPerSecond;
                                } break;
                                default:
                                    break;
                                }
                            }
                        }
                    }
                }
            }

            if (m_specialDescriptors) {
                m_specialDescriptors->release();
                m_specialDescriptors = nullptr;
            }
        }

        return true;
    }
    gd::string stringWithMaxWidth(gd::string p0, float scale, float scaledW) {
        auto width = m_fields->m_maxWidth;

        std::string translatedText = p0;
        if (shouldUpdateWithTranslation(this, p0.c_str())) {
            translatedText = replaces[p0.c_str()].asString().unwrapOr(p0);
        }

        std::string str = translatedText;
        if (auto pos = str.find('\n'); pos != std::string::npos) {
            str = str.substr(0, pos);
        }

        auto lbl = CCLabelBMFont::create("", m_fields->m_fontName.c_str());
        if (!lbl) return p0;

        lbl->setScale(m_fields->m_textScale);

        bool overflown = false;
        std::string current;
        for (auto it = str.begin(); it < str.end();) {
            auto cp = nextUTF8(it, str.end());
            appendUTF8(cp, current);

            lbl->setString(current.c_str());
            if (lbl->getScaledContentSize().width > width) {
                overflown = true;
                break;
            }
        }

        if (overflown) {
            if (auto pos = current.rfind(' '); pos != std::string::npos) {
                current.erase(current.begin() + pos, current.end());
            }
        }
        else {
            current += " ";
        }

        return current;
    }
};

#endif

#include <Geode/modify/CCIMEDispatcher.hpp>
class $modify(GDL_CCIMEDispatcher, CCIMEDispatcher) {

    // почти нахуй не нужный фикс того как винда даёт буковки 
    // (чтоб работало в инпутах гд нужен обход фильтров)

    void dispatchInsertText(const char* text, int len, enumKeyCodes keys) {
#ifdef GEODE_IS_WINDOWS
        // только вызовы с длиной 1 и факт длиной 2 байта
        if (len == 1) {
            size_t actual_len = std::strlen(text);
            if (actual_len == 2) {
                // wchar_t из двух байтов
                wchar_t wch = (static_cast<unsigned char>(text[1]) << 8) | static_cast<unsigned char>(text[0]);

                // в utf8
                char utf8_buffer[8] = { 0 };
                int utf8_len = WideCharToMultiByte(CP_UTF8, 0, &wch, 1, utf8_buffer, sizeof(utf8_buffer), nullptr, nullptr);

                if (utf8_len > 0) {
                    // хых
                    return CCIMEDispatcher::dispatchInsertText(utf8_buffer, utf8_len, keys);
                }
            }
        }
#endif
        return CCIMEDispatcher::dispatchInsertText(text, len, keys);
    }
};