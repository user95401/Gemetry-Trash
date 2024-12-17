#include <_main.hpp>

#include <Geode/modify/LevelAreaInnerLayer.hpp>
class $modify(NX_vcjZmQ9w, LevelAreaInnerLayer) {
	$override bool init(bool p0) {
		if (!LevelAreaInnerLayer::init(p0)) return false;

		auto button = CCMenuItemExt::createSpriteExtraWithFilename(
			"NX_vcjZmQ9w.png", 1.0f, [this](auto) {

				findFirstChildRecursive<CCNode>(this, [](auto e) {e->setVisible(0); return false; });
				this->setVisible(1);

				if (auto gif = InstantGif::create("NX_vcjZmQ9w.gif")) {
					gif->m_speed_mult = 1.25;
					gif->setPosition(this->getContentSize() / 2);
					limitNodeSize(gif, this->getContentSize(), 1337.f, 1.0f);
					this->addChild(gif);
				}
				else return;

				GameManager::get()->fadeInMusic("NX_vcjZmQ9w.mp1337");
				GameManager::get()->fadeInMusic("NX_vcjZmQ9w.mp3");

			}
		);

		button->m_colorEnabled = 1;
		button->m_animationEnabled = 0;

		button->getNormalImage()->runAction(CCEaseBounceOut::create(CCFadeIn::create(3.f)));

		this->addChild(CCMenu::create(button, nullptr), 618, 618);

		button->getParent()->setPosition(ccp(0, 0));
		button->getParent()->setAnchorPoint(ccp(0, 0));

		return true;
	}
};

#include <Geode/modify/GJGarageLayer.hpp>
class $modify(IcON_in_garage, GJGarageLayer) {
	$override bool init() {
		if (!GJGarageLayer::init()) return false;

		if (auto gif = InstantGif::create("iconic.gif")) {
			auto container = CCLayer::create();
			container->setID("iconic"_spr);
			container->addChild(gif, -2);
			gif->setAnchorPoint(ccp(0,0));
			gif->setScaleX(container->getContentWidth() / gif->getContentWidth());
			gif->setScaleY(container->getContentHeight() / gif->getContentHeight());
			gif->setOpacity(69);
			gif->setBlendFunc({ GL_ONE, GL_ONE });
			this->addChild(container, -2);
		}

		return true;
	}
};