#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/GeodeUI.hpp>
using namespace geode::prelude;

#include <regex>

#include <_ImGui.hpp>

namespace fs {
    static std::error_code err;
    using namespace file;
    using namespace std::filesystem;
};

//lol
#define SETTING(type, key_name) Mod::get()->getSettingValue<type>(key_name)

#define MEMBER_BY_OFFSET(type, class, offset) *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(class) + offset)
template<typename T, typename U> constexpr size_t OFFSET_BY_MEMBER(U T::* member) { return (char*)&((T*)nullptr->*member) - (char*)nullptr; }

#define public_cast(value, member) [](auto* v) { \
	class FriendClass__; \
	using T = std::remove_pointer<decltype(v)>::type; \
	class FriendeeClass__: public T { \
	protected: \
		friend FriendClass__; \
	}; \
	class FriendClass__ { \
	public: \
		auto& get(FriendeeClass__* v) { return v->member; } \
	} c; \
	return c.get(reinterpret_cast<FriendeeClass__*>(v)); \
}(value)

#define LOG_THIS_FILE $execute { log::debug("LOG_THIS_FILE: \n\n{}\n", fs::path(__FILE__).filename().string()); }

#ifdef GEODE_IS_ANDROID
#define debug error
#endif // GEODE_IS_ANDROID

namespace geode::cocos {
    inline std::string getFrameName(CCNode* node, bool textureName = false) {
        if (node == nullptr) return "NIL_NODE";
        if (auto textureProtocol = dynamic_cast<CCTextureProtocol*>(node)) {
            if (auto texture = textureProtocol->getTexture()) {
                if (!textureName) if (auto spriteNode = dynamic_cast<CCSprite*>(node)) {
                    auto* cachedFrames = CCSpriteFrameCache::sharedSpriteFrameCache()->m_pSpriteFrames;
                    const auto rect = spriteNode->getTextureRect();
                    for (auto [key, frame] : CCDictionaryExt<std::string, CCSpriteFrame*>(cachedFrames)) {
                        if (frame->getTexture() == texture && frame->getRect() == rect) {
                            return key.c_str();
                        }
                    }
                }
                auto* cachedTextures = CCTextureCache::sharedTextureCache()->m_pTextures;
                for (auto [key, obj] : CCDictionaryExt<std::string, CCTexture2D*>(cachedTextures)) {
                    if (obj == texture) {
                        return key.c_str();
                    }
                }
            }
        }
        auto btnSpriteTry = getFrameName(getChild(node, 0));
        if (
            btnSpriteTry != "NIL_NODE"
            and btnSpriteTry != "CANT_GET_FRAME_NAME"
            ) return btnSpriteTry;
        return "CANT_GET_FRAME_NAME";
    }
};

class DataNode : public CCNode {
public:
    std::string _json_str;
    auto get(std::string_view key) { 
        return matjson::parse(_json_str).unwrapOrDefault()[key]; 
    }
    void set(std::string_view key, matjson::Value val) { 
        auto parse = matjson::parse(_json_str).unwrapOrDefault();
        parse[key] = val;
        _json_str = parse.dump(matjson::NO_INDENTATION);
    }

    CREATE_FUNC(DataNode);

    static auto create(std::string id) {
        auto a = create();
        a->setID(id);
        return a;
    }

    static auto at(CCNode* container, std::string id = "") {
        auto me = typeinfo_cast<DataNode*>(container->getUserObject(id));
        if (!me) {
			me = create();
            container->setUserObject(id, me);
            log::debug("creating data node {} for {}...", me, container);
		}
		return me;
	}
};

#include  <random>
#include  <iterator>
namespace geode::utils {
    template<typename Iter, typename RandomGenerator>
    inline Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
        std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
        std::advance(start, dis(g));
        return start;
    };
    template<typename Iter>
    inline Iter select_randomly(Iter start, Iter end) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        return select_randomly(start, end, gen);
    }
    inline bool rndb(int rarity = 1) {
        auto varsVec = std::vector<bool>();
        varsVec.push_back(true);
        while (rarity > 0) {
            rarity = rarity - 1;
            varsVec.push_back(false);
        }
        auto rtn = *select_randomly(varsVec.begin(), varsVec.end());
        //log::debug("{}({}) = {} of {}", __func__, variants, rtn, varsVec);
        return rtn;
    }
};

struct GJScoreKey {
    inline static auto TotalJumps = "1";
    inline static auto TotalAttempts = "2";
    inline static auto CompletedLevels = "3";
    inline static auto CompletedOnlineLevels = "4";
    inline static auto Demons = "5";
    inline static auto Stars = "6";
    inline static auto CompletedMapPacksCount = "7";
    inline static auto GoldCoins = "8";
    inline static auto PlayersDestroyed = "9";
    inline static auto LikedLevelsCount = "10";
    inline static auto RatedLevelsCount = "11";
    inline static auto UserCoins = "12";
    inline static auto Diamonds = "13";
    inline static auto CurrentOrbs = "14";
    inline static auto DailyCompletionCount = "15";
    inline static auto FireShards = "16";
    inline static auto IceShards = "17";
    inline static auto PoisonShards = "18";
    inline static auto ShadowShards = "19";
    inline static auto LavaShards = "20";
    inline static auto DemonKeys = "21";
    inline static auto TotalOrbs = "22";
    inline static auto EarthShards = "23";
    inline static auto BloodShards = "24";
    inline static auto MetalShards = "25";
    inline static auto LightShards = "26";
    inline static auto SoulShards = "27";
    inline static auto Moons = "28";
    inline static auto DiamondShards = "29";
    inline static auto FirePathStarProgress = "30";
    inline static auto IcePathStarProgress = "31";
    inline static auto PoisonPathStarProgress = "32";
    inline static auto ShadowPathStarProgress = "33";
    inline static auto LavaPathStarProgress = "34";
    inline static auto EarthPathStarProgress = "35";
    inline static auto BloodPathStarProgress = "36";
    inline static auto MetalPathStarProgress = "37";
    inline static auto LightPathStarProgress = "38";
    inline static auto SoulPathStarProgress = "39";
    inline static auto CompletedGauntlets = "40";
    inline static auto ListRewardsCollected = "41";
};

inline auto repo = std::string("user95401/GemetryTrash");
inline auto repobranch = std::string("user95401/GemetryTrash/main");
inline auto repo_lnk = std::string("https://github.com/" + repo);
inline auto raw_content_repo_lnk = std::string("https://raw.githubusercontent.com/" + repobranch);