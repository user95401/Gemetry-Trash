#include <_main.hpp>

#include <user95401.game-objects-factory/include/main.hpp>

void SetupObjects();
$on_mod(Loaded) { SetupObjects(); }
inline void SetupObjects() {
	GameObjectsFactory::createTriggerConfig(
		UNIQ_ID("plr-input-crtl"), "edit_ePlayerControlExtBtn_001.png",
		[](EffectGameObject* object, GJBaseGameLayer* game, int, gd::vector<int> const*) {
			GameOptionsTrigger* options = typeinfo_cast<GameOptionsTrigger*>(object);
			if (!options) return log::error("options object cast == {} from {}", options, object);
			//option assignments
			typedef GameOptionsSetting Is;
			auto player = options->m_streakAdditive;
			auto jump = options->m_hideGround;
			auto left = options->m_hideMG;
			auto right = options->m_hideP1;
			//affected players
			std::vector<Ref<PlayerObject>> ps = { game->m_player1, game->m_player2 };
			if (player != Is::Disabled) ps = {
				player == Is::On ? game->m_player1 : game->m_player2
			};
			//jump
			if (jump != Is::Disabled) for (auto p : ps) if (p) p->m_jumpBuffered = jump == Is::On;
			//left
			if (left != Is::Disabled) for (auto p : ps) if (p) p->m_holdingLeft = left == Is::On;
			//right
			if (right != Is::Disabled) for (auto p : ps) if (p) p->m_holdingRight = right == Is::On;
		}
	)->refID(2899)->insertIndex((12 * 7) + 1)->onEditObject(
		[](EditorUI* a, GameObject* aa) -> bool {
			queueInMainThread(
				[a = Ref(a), aa = Ref(aa)] {
					if (!CCScene::get()) return log::error("CCScene::get() == {}", CCScene::get());
					auto popup = CCScene::get()->getChildByType<SetupOptionsTriggerPopup>(0);
					if (!popup) return log::error("popup == {}", popup);
					auto object = typeinfo_cast<EffectGameObject*>(aa.data());
					if (!object) return log::error("object == {} ({})", object, aa);

					auto main = popup->m_mainLayer;
					auto menu = popup->m_buttonMenu;

					if (auto aaa = main->getChildByType<CCLabelBMFont>(0)) aaa->setString("Extended Player Control");

					//xd
					if (auto aaa = main->getChildByType<CCLabelBMFont>(6 - 2)) aaa->setString(R"(Только для игрока)");
					if (auto aaa = main->getChildByType<CCLabelBMFont>(7 - 2)) aaa->setString(R"(1)");
					if (auto aaa = main->getChildByType<CCLabelBMFont>(8 - 2)) aaa->setString(R"(2)");
					//jump buffer
					if (auto aaa = main->getChildByType<CCLabelBMFont>(9 - 2)) aaa->setString(R"(Буфер прыжка)");
					//holding left
					if (auto aaa = main->getChildByType<CCLabelBMFont>(12 - 2)) aaa->setString(R"(Держится лево)");
					//holding right
					if (auto aaa = main->getChildByType<CCLabelBMFont>(15 - 2)) aaa->setString(R"(Держится право)");

					//other shit
					{
						auto low_iq = 18;
						while (auto aaa = main->getChildByType<CCNode>(low_iq++)) aaa->setVisible(false);
					};

					//other shit in menu
					{
						auto low_iq = 13;
						while (auto aaa = menu->getChildByType<CCNode>(low_iq++)) aaa->setVisible(false);
					};

					if (auto aaa = menu->getChildByType<InfoAlertButton>(0)) aaa->m_title = (R"(Оно)");
					if (auto aaa = menu->getChildByType<InfoAlertButton>(0)) aaa->m_description = std::vector<std::string>{
						R"(На самом деле не дёргает за письки, но с его помощью )" 
						R"(ты можешь устанавливать состояние основных инпутов управления игрока.)" 
					}[0].c_str();
				}
			);
			return false;
		}
	)->registerMe();
}

#include <Geode/modify/EffectGameObject.hpp>
class $modify(MenuItemGameObjectExt, EffectGameObject) {

	class CCMenuItem : public cocos2d::CCMenuItem {
	public:
		CREATE_FUNC(CCMenuItem);
		virtual bool init() {
			log::debug("{}()", __FUNCTION__);
			CCMenuItem::initWithTarget(this, nullptr);
			this->setAnchorPoint({ 0.5f, 0.5f });
			this->setEnabled(true);
			return true; 
		};
		std::function<void(void)> m_onActivate = []() {};
		std::function<void(void)> m_onSelected = []() {};
		std::function<void(void)> m_onUnselected = []() {};
		virtual void activate() { if (m_onActivate) m_onActivate(); };
		virtual void selected() { if (m_onSelected) m_onSelected(); };
		virtual void unselected() { if (m_onUnselected) m_onUnselected(); };
		auto onActivate(std::function<void(void)> onActivate) { m_onActivate = onActivate; return this; }
		auto onSelected(std::function<void(void)> onSelected) { m_onSelected = onSelected; return this; }
		auto onUnselected(std::function<void(void)> onUnselected) { m_onUnselected = onUnselected; return this; }
	};

#define MenuItemObjectData(ring) DataNode::at(ring, "menu-item-data")
	inline static GameObjectsFactory::GameObjectConfig* conf;

	static void setupMenuItemPopup(EditorUI*, EffectGameObject* obj, SetupCollisionStateTriggerPopup * popup) {

		if (popup->getUserObject("got-custom-setup-for-menu-item")) return;
		popup->setUserObject("got-custom-setup-for-menu-item", obj);

		auto main = popup->m_mainLayer;
		auto menu = popup->m_buttonMenu;
		if (auto aaa = main->getChildByType<CCLabelBMFont>(0)) aaa->setString("Menu Item");

		if (auto aaa = main->getChildByType<CCLabelBMFont>(1)) aaa->setVisible(false);
		if (auto aaa = main->getChildByType<CCLabelBMFont>(2)) aaa->setVisible(false);
		if (auto aaa = main->getChildByType<CCScale9Sprite>(1)) aaa->setVisible(false);
		if (auto aaa = main->getChildByType<CCScale9Sprite>(2)) aaa->setVisible(false);
		if (auto aaa = main->getChildByType<CCTextInputNode>(0)) aaa->setVisible(false);
		if (auto aaa = main->getChildByType<CCTextInputNode>(1)) aaa->setVisible(false);

		while (auto aaa = menu->getChildByTag(51)) aaa->removeFromParentAndCleanup(false);
		while (auto aaa = menu->getChildByTag(71)) aaa->removeFromParentAndCleanup(false);

		auto data = MenuItemObjectData(obj);

		//activate
		auto activate = TextInput::create(52.f, "ID");
		activate->setFilter("0123456789");
		activate->getInputNode()->m_allowedChars = "0123456789";
		activate->setString(utils::numToString(data->get("activate").asInt().unwrapOr(0)));
		activate->setPositionY(95.000f);
		activate->setCallback(
			[data = Ref(MenuItemObjectData(obj))](const std::string& p0) {
				data->set("activate", utils::numFromString<int>(p0).unwrapOr(0));
			}
		);
		menu->addChild(activate);
		auto activateLabel = CCLabelBMFont::create("Activate:\n \n \n \n ", "goldFont.fnt");
		activateLabel->setScale(0.5f);
		activate->getInputNode()->addChild(activateLabel);

		//selected
		auto selected = TextInput::create(54.f, "ID");
		selected->setFilter("0123456789");
		selected->getInputNode()->m_allowedChars = "0123456789";
		selected->setString(utils::numToString(data->get("selected").asInt().unwrapOr(0)));
		selected->setPosition(-95.000f, 77.f);
		selected->setCallback(
			[data = Ref(MenuItemObjectData(obj))](const std::string& p0) {
				data->set("selected", utils::numFromString<int>(p0).unwrapOr(0));
			}
		);
		menu->addChild(selected);
		auto selectedLabel = CCLabelBMFont::create("Selected:\n \n \n \n ", "goldFont.fnt");
		selectedLabel->setScale(0.5f);
		selected->getInputNode()->addChild(selectedLabel);

		//unselected
		auto unselected = TextInput::create(48.f, "ID");
		unselected->setFilter("0123456789");
		unselected->getInputNode()->m_allowedChars = "0123456789";
		unselected->setString(utils::numToString(data->get("unselected").asInt().unwrapOr(0)));
		unselected->setPosition(95.000f, 77.f);
		unselected->setCallback(
			[data = Ref(MenuItemObjectData(obj))](const std::string& p0) {
				data->set("unselected", utils::numFromString<int>(p0).unwrapOr(0));
			}
		);
		menu->addChild(unselected);
		auto unselectedLabel = CCLabelBMFont::create("Unselected:\n \n \n \n ", "goldFont.fnt");
		unselectedLabel->setScale(0.5f);
		unselected->getInputNode()->addChild(unselectedLabel);

		if (auto aaa = menu->getChildByType<InfoAlertButton>(0)) aaa->m_title = (R"(Да.)");
		if (auto aaa = menu->getChildByType<InfoAlertButton>(0)) aaa->m_description = std::vector<std::string>{
			R"(Это рил кнопка как в менюшках гд.)""\n"
			R"(Спавнит группу именно когда получает клик на себя!)"
		} [0] .c_str();
	}

	static void setup() {
		conf = GameObjectsFactory::createRingConfig(
			UNIQ_ID("menu-item"), "edit_eMenuItemBtn_001.png"
		)->refID(3640)->tab(12)->insertIndex((12*6)+3)->onEditObject(
			[](EditorUI* a, GameObject* aa) -> bool {
				queueInMainThread(
					[a = Ref(a), aa = Ref(aa)] {
						if (!CCScene::get()) return log::error("CCScene::get() == {}", CCScene::get());
						auto popup = CCScene::get()->getChildByType<SetupCollisionStateTriggerPopup>(0);
						if (!popup) return log::error("popup == {}", popup);
						auto object = typeinfo_cast<EffectGameObject*>(aa.data());
						if (!object) return log::error("object == {} ({})", object, aa);
						setupMenuItemPopup(a, object, popup);
					}
				);
				return false;
			}
		)->saveString(
			[](std::string str, GameObject* object, GJBaseGameLayer* level)
			{
				str += ",228,";
				str += ZipUtils::base64URLEncode(MenuItemObjectData(object)->_json_str.c_str()).c_str();
				return str;
			}
		)->objectFromVector(
			[](GameObject* object, gd::vector<gd::string>& p0, gd::vector<void*>& p1, GJBaseGameLayer* p2, bool p3)
			{
				auto parsed = matjson::parse(
					ZipUtils::base64URLDecode(p0[228].c_str()).c_str()
				).unwrapOrDefault();
				for (auto& [key, value] : parsed) MenuItemObjectData(object)->set(key, value);
				return object;
			}
		)->resetObject(
			[](GameObject* __this) {

				if (!GameManager::get()->m_gameLayer) return;

				CCMenu* ui_menu = typeinfo_cast<CCMenu*>(GameManager::get()->m_gameLayer->getUserObject("ui-objects-menu"));
				if (!ui_menu) {
					ui_menu = CCMenu::create();
					ui_menu->setID("ui-objects-menu");
					ui_menu->setPosition(CCSizeZero);
					ui_menu->setAnchorPoint(CCPointZero);
					GameManager::get()->m_gameLayer->setUserObject("ui-objects-menu", ui_menu);
					GameManager::get()->m_gameLayer->m_uiTriggerUI->addChild(ui_menu);
				}
				CCMenu* ol_menu = typeinfo_cast<CCMenu*>(GameManager::get()->m_gameLayer->getUserObject("ol-objects-menu"));
				if (!ol_menu) {
					ol_menu = CCMenu::create();
					ol_menu->setID("ol-objects-menu");
					ol_menu->setPosition(CCSizeZero);
					ol_menu->setContentSize(CCSizeZero);
					ol_menu->setAnchorPoint(CCPointZero);
					GameManager::get()->m_gameLayer->setUserObject("ol-objects-menu", ol_menu);
					GameManager::get()->m_gameLayer->m_objectLayer->addChild(ol_menu);
				}

				for (auto& item : CCArrayExt<CCMenuItem*>(ui_menu->getChildren())) {
					if (item->getUserObject("menu-item-object") == __this) ui_menu->removeChild(item, true);
				}
				for (auto& item : CCArrayExt<CCMenuItem*>(ol_menu->getChildren())) {
					if (item->getUserObject("menu-item-object") == __this) ol_menu->removeChild(item, true);
				}

				auto item = CCMenuItem::create();
				//virtual void spawnGroup(int group, bool ordered, double delay, gd::vector<int> const& remapKeys, int triggerID, int controlID);
				item->onActivate([game = Ref(GameManager::get()->m_gameLayer), data = Ref(MenuItemObjectData(__this))] {
					if (game) game->spawnGroup(data->get("activate").asInt().unwrapOr(0), false, 0, gd::vector<int>(), -1, -1);
					});
				item->onSelected([game = Ref(GameManager::get()->m_gameLayer), data = Ref(MenuItemObjectData(__this))] {
					if (game) game->spawnGroup(data->get("selected").asInt().unwrapOr(0), false, 0, gd::vector<int>(), -1, -1);
					});
				item->onUnselected([game = Ref(GameManager::get()->m_gameLayer), data = Ref(MenuItemObjectData(__this))] {
					if (game) game->spawnGroup(data->get("unselected").asInt().unwrapOr(0), false, 0, gd::vector<int>(), -1, -1);
					});
				item->setUserObject("menu-item-object", __this);

				auto menu = __this->m_isUIObject ? ui_menu : ol_menu; //m_isUIObject lies here
				menu->setTouchEnabled(false);
				menu->setTouchEnabled(true);

				menu->runAction(CCRepeatForever::create(CCSequence::create(CallFuncExt::create(
					[__this = Ref(__this), item = Ref(item), ui_menu = Ref(ui_menu), ol_menu = Ref(ol_menu)] {
						if (!GameManager::get()->m_gameLayer) return;
						if (!__this) return;
						if (!item) return;
						if (!ui_menu) return;
						if (!ol_menu) return;
						auto menu = __this->m_isUIObject ? ui_menu : ol_menu;
						if (item->getParent() != menu) {
							item->removeFromParentAndCleanup(false);
							menu->addChild(item);
							menu->setTouchEnabled(false);
							menu->setTouchEnabled(true);
						}
						menu->setVisible(GameManager::get()->m_gameLayer->m_uiLayer->isVisible());
						item->setPosition(__this->getPosition());
						item->setContentSize(__this->boundingBox().size);

					}
				), nullptr)));
			}
		);
		conf->registerMe();
	}
	static void onModify(auto&) { setup(); }

	virtual void resetObject() {
		EffectGameObject::resetObject();
	};

};

#include <Geode/modify/UILayer.hpp>
class $modify(UILayerKeysExt, UILayer) {
	void customUpdate(float) {
		this->setKeyboardEnabled(false);
		this->setKeyboardEnabled(this->isVisible());
	}
	bool init(GJBaseGameLayer * p0) {
		if (!UILayer::init(p0)) return false;
		this->schedule(schedule_selector(UILayerKeysExt::customUpdate));
		return true;
	};
	void handleKeypress(cocos2d::enumKeyCodes key, bool p1) {
		UILayer::handleKeypress(key, p1);

		auto eventID = 120000 + (int)key;
		m_gameLayer->gameEventTriggered((GJGameEvent)eventID, 0, 0);
		m_gameLayer->gameEventTriggered((GJGameEvent)eventID, 0, 1 + !p1);
	}
};

class KeyEventListener : public CCNode, public CCKeyboardDelegate {
public:
	KeyEventListener() { CCKeyboardDispatcher::get()->addDelegate(this); };
	void onExit() override {
		CCNode::onExit();
		CCKeyboardDispatcher::get()->removeDelegate(this);
	}
	CREATE_FUNC(KeyEventListener);
	void keyDown(enumKeyCodes key) override {
		if (m_keyDown) m_keyDown(key);
	}
	void keyUp(enumKeyCodes key) override {
		if (m_keyUp) m_keyUp(key);
	}
	auto onKeyDown(std::function<void(enumKeyCodes key)> keyDown) { m_keyDown = keyDown; return this; }
	auto onKeyUp(std::function<void(enumKeyCodes key)> keyUp) { m_keyUp = keyUp; return this; }

	std::function<void(enumKeyCodes key)> m_keyDown = [](enumKeyCodes) {};
	std::function<void(enumKeyCodes key)> m_keyUp = [](enumKeyCodes) {};
};

#include <Geode/modify/SelectEventLayer.hpp>
class $modify(SelectEventLayerKeysExt, SelectEventLayer) {
	bool init(SetupEventLinkPopup* p0, gd::set<int>&p1) {
		if (!SelectEventLayer::init(p0, p1)) return false;
		
		auto keyEventsExpandBtn = CCMenuItemExt::createToggler(
			ButtonSprite::create("Keys", "goldFont.fnt", "GJ_button_04.png", 0.6f),
			ButtonSprite::create("Keys", "goldFont.fnt", "GJ_button_02.png", 0.6f),
			[popup = Ref(this)](CCMenuItemToggler* keyEventsExpandBtn) {

				while (auto a = popup->m_buttonMenu->getChildByID("key-list-item")) a->removeFromParent();
				while (auto a = popup->getChildByType<KeyEventListener>(0)) a->removeFromParent();

				if (!keyEventsExpandBtn->isOn()) return;

				auto posY = keyEventsExpandBtn->getPositionY() + keyEventsExpandBtn->getContentSize().height + 4.f;

				for (auto eventID : popup->m_eventIDs) if (eventID >= 120000 and eventID < 130000) {
					std::string name = CCKeyboardDispatcher::get()->keyToString((enumKeyCodes)(eventID - 120000));
					auto item = CCMenuItemExt::createSpriteExtra(
						ButtonSprite::create((" " + name + " ").c_str(), "goldFont.fnt", "GJ_button_05.png", 0.5f)
						, [popup, eventID, keyEventsExpandBtn](void*) {
							popup->m_eventIDs.erase(eventID);
							popup->m_eventsChanged = true;
							keyEventsExpandBtn->activate();
							keyEventsExpandBtn->activate();
						}
					);
					item->setID("key-list-item");
					item->setPositionY(posY);
					item->setPositionX(keyEventsExpandBtn->getPositionX());
					popup->m_buttonMenu->addChild(item, 999);

					posY += item->getContentSize().height + 4.f;
				}

				auto inf = CCLabelBMFont::create(" \nPress any key...", "chatFont.fnt");
				inf->setID("key-list-item");
				inf->setScale(0.625f);
				inf->setZOrder(999);
				popup->m_buttonMenu->addChildAtPosition(
					inf, Anchor::BottomLeft, { keyEventsExpandBtn->getPositionX(), posY }, false
				);

				popup->addChild(KeyEventListener::create()->onKeyDown(
					[popup, keyEventsExpandBtn](enumKeyCodes key) {
						popup->m_eventIDs.insert(120000 + (int)key);
						popup->m_eventsChanged = true;
						keyEventsExpandBtn->activate();
						keyEventsExpandBtn->activate();
					}
				), 999);
			}
		);
		keyEventsExpandBtn->setID("key-events-expand-btn");
		keyEventsExpandBtn->setPositionX(142.000f);
		keyEventsExpandBtn->setPositionY(0.f);
		keyEventsExpandBtn->activate();
		Ref(this)->m_buttonMenu->addChild(keyEventsExpandBtn);

		return true;
	}
};