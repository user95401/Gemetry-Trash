#include <_main.hpp>

//data values
inline static auto globed_server = std::string("147.185.221.22:46644");

//dankmeme.globed2/GlobedServersLayer
class GlobedServersLayer : public CCLayer {
public:
	cocos2d::CCSprite* background;
	CCLayer* serverList;//GlobedServerList
	CCLayer* signupLayer;//GlobedSignupLayer
	void trySetupServer(float asd = 1337.f) {
		if (!typeinfo_cast<GlobedServersLayer*>(this)) return;

		//starter
		if (asd == 1337.f) this->schedule(schedule_selector(GlobedServersLayer::trySetupServer), 0.1f);

		auto scene = typeinfo_cast<CCScene*>(this->getParent());
		if (scene) { if (scene != CCDirector::get()->m_pRunningScene) return; }
		else return log::error("{}.scene = {}", __func__, scene);

		//set current server

		auto switcher = typeinfo_cast<CCMenuItemSpriteExtra*>(this->getChildByIDRecursive("btn-server-switcher"));
		if (switcher) switcher->activate();
		else return log::error("{}.switcher = {}", __func__, switcher);

		auto ipconnect = typeinfo_cast<CCMenuItemSpriteExtra*>(this->getParent()->getChildByIDRecursive("dankmeme.globed2/server-switcher-btn-ipconnect"));
		if (ipconnect) ipconnect->activate();
		else return log::error("{}.ipconnect = {}", __func__, ipconnect);

		auto addr = typeinfo_cast<TextInput*>(this->getParent()->getChildByIDRecursive("dankmeme.globed2/direct-connection-addr"));
		if (addr) addr->setString(globed_server);
		else return log::error("{}.addr = {}", __func__, addr);

		auto connect = typeinfo_cast<CCMenuItemSpriteExtra*>(this->getParent()->getChildByIDRecursive("dankmeme.globed2/connect-btn"));
		if (connect) connect->activate();
		else return log::error("{}.connect = {}", __func__, connect);

		//finish
		this->unschedule(schedule_selector(GlobedServersLayer::trySetupServer));
	}
	void tryCustomSetup(float) {
		if (!typeinfo_cast<GlobedServersLayer*>(this)) return;
		this->trySetupServer();

		//msg
		auto text = MDTextArea::create(
			"### Here was set a server hosted specially for Gemetry Trash!"
			"\n"
			"Hostname: `" + globed_server + "`"
			"\n""\n"
			"<cr>Server **don't working for 24/7** because i can forgot to launch it or smth else...</c>"
			"\n""\n"
			"<cf>You can ask me relaunch it in</c> [discord server](https://discord.gg/UyQytJsrGZ)<cf>!</c>"
			, { serverList->getContentSize().width, 173 }
		);
		public_cast(text, m_bgSprite)->setVisible(0);
		public_cast(text, m_scrollLayer)->m_disableMovement = 1;
		text->ignoreAnchorPointForPosition(1);
		text->setPositionX(10.f);
		serverList->addChild(text, 3);

		auto scroll = typeinfo_cast<ScrollLayer*>(serverList->getChildByIDRecursive("ScrollLayer"));
		if (scroll) scroll->m_disableMovement = 1;
	}
};

#include <Geode/modify/CCLayer.hpp>
class $modify(GlobedServersLayerExt, CCLayer) {
	$override bool init() {
		if (!CCLayer::init()) return false;
		this->scheduleOnce(
			schedule_selector(GlobedServersLayer::tryCustomSetup), 0.f
		);
		return true;
	}
};
