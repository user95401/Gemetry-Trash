#pragma once

#include <imgui.h>
#include <cocos2d.h>

using namespace cocos2d;

static std::ostream& operator<<(std::ostream& stream, ImVec2 const& vec) {
    return stream << vec.x << ", " << vec.y;
}

static std::ostream& operator<<(std::ostream& stream, ImVec4 const& vec) {
    return stream << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w;
}

static ImVec2 operator+(ImVec2 const& a, ImVec2 const& b) {
    return {
        a.x + b.x,
        a.y + b.y
    };
}

static ImVec2 operator-(ImVec2 const& a, ImVec2 const& b) {
    return {
        a.x - b.x,
        a.y - b.y
    };
}

static ImVec2 operator-(ImVec2 const& a) {
    return { -a.x, -a.y };
}

static ImVec2& operator-=(ImVec2& a, ImVec2 const& b) {
    a.x -= b.x;
    a.y -= b.y;
    return a;
}

static ImVec2 operator/(ImVec2 const& a, ImVec2 const& b) {
    return {
        a.x / b.x,
        a.y / b.y
    };
}

static ImVec2 operator/(ImVec2 const& a, int b) {
    return {
        a.x / b,
        a.y / b
    };
}

static ImVec2 operator/(ImVec2 const& a, float b) {
    return {
        a.x / b,
        a.y / b
    };
}

static bool operator!=(ImVec2 const& a, ImVec2 const& b) {
    return a.x != b.x || a.y != b.y;
}

static ImVec2 operator*(ImVec2 const& v1, float multi) {
    return { v1.x * multi, v1.y * multi };
}

static ImVec2 toVec2(CCPoint const& a) {
    const auto size = ImGui::GetMainViewport()->Size;
    const auto winSize = CCDirector::get()->getWinSize();
    return {
        a.x / winSize.width * size.x,
        (1.f - a.y / winSize.height) * size.y
    };
}

static ImVec2 toVec2(CCSize const& a) {
    const auto size = ImGui::GetMainViewport()->Size;
    const auto winSize = CCDirector::get()->getWinSize();
    return {
        a.width / winSize.width * size.x,
        -a.height / winSize.height * size.y
    };
}

static CCPoint toCocos(const ImVec2& pos) {
    const auto winSize = CCDirector::sharedDirector()->getWinSize();
    const auto size = ImGui::GetMainViewport()->Size;
    return CCPointMake(
        pos.x / size.x * winSize.width,
        (1.f - pos.y / size.y) * winSize.height
    );
};

#include <imgui-cocos.hpp>
#include <Geode/modify/CCDirector.hpp>
class $modify(ImGuiCocosExt, CCDirector) {

    inline static auto id_mapped_drawings = std::unordered_map<std::string, std::function<void()>>();
    inline static auto tag_mapped_next_drawings = std::unordered_map<int, std::function<void()>>();
    inline static auto next_drawings = std::vector<std::function<void()>>();
    inline static auto drawings = std::vector<std::function<void()>>();

    class DrawLayer : public CCLayerRGBA {
    public:
        std::function<void()> m_drawFn;
        virtual void draw() override {
            if (m_drawFn) next_drawings.push_back(
                [__this = Ref(this)] {
                    if (!__this) return;
                    if (!__this->isVisible()) return;

                    auto fontScale = ImGui::GetIO().FontGlobalScale;
                    ImGui::GetIO().FontGlobalScale = __this->getScale();

                    auto vpWorkPos = ImGui::GetMainViewport()->WorkPos;
                    auto nWSp = __this->convertToWorldSpace({});
                    ImGui::GetMainViewport()->WorkPos.x += toVec2(nWSp).x;
					ImGui::GetMainViewport()->WorkPos.y += toVec2(nWSp + __this->getScaledContentSize()).y;

                    auto vpWorkSize = ImGui::GetMainViewport()->WorkSize;
                    ImGui::GetMainViewport()->WorkSize.x = toVec2(__this->getScaledContentSize()).x;
					ImGui::GetMainViewport()->WorkSize.y = toVec2(-__this->getScaledContentSize()).y;

                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, __this->getOpacity() / 255.f);

                    __this->m_drawFn();

                    ImGui::PopStyleVar();

					ImGui::GetIO().FontGlobalScale = fontScale;
					ImGui::GetMainViewport()->WorkPos = vpWorkPos;
					ImGui::GetMainViewport()->WorkSize = vpWorkSize;
                }
            );
        }
        DrawLayer(std::function<void()> drawFn) : m_drawFn(drawFn) {};
        DrawLayer() : m_drawFn([] {}) {};
        CREATE_FUNC(DrawLayer);
        static auto create(std::function<void()> drawFn) {
			auto a = create();
			a->m_drawFn = drawFn;
			return a;
		}
        auto drawFn(std::function<void()> drawFn) {
			m_drawFn = drawFn;
			return this;
		}
    };

    $override void runWithScene(CCScene * pScene) {
        CCDirector::runWithScene(pScene);
        ImGuiCocos::get().setup(
            [] {

            }
        ).draw(
            [] {
                for (auto& draw : ImGuiCocosExt::tag_mapped_next_drawings) draw.second();
                for (auto& draw : ImGuiCocosExt::id_mapped_drawings) draw.second();

                for (auto& draw : ImGuiCocosExt::next_drawings) draw();
                next_drawings.clear();

                for (auto& draw : ImGuiCocosExt::drawings) draw();
            }
        );
    };
};