#include <Geode/Geode.hpp>
#include <Geode/ui/GeodeUI.hpp>
using namespace geode::prelude;

#include <Geode/modify/PlayLayer.hpp>
class $modify(PlayLayerExt, PlayLayer) {
};

#include <Geode/modify/GJBaseGameLayer.hpp>
class $modify(GJBaseGameLayerExt, GJBaseGameLayer) {
	$override void gameEventTriggered(GJGameEvent p0, int p1, int p2) {
		//log::debug("{}({}, {}, {})", __FUNCTION__, (int)p0, p1, p2);
		GJBaseGameLayer::gameEventTriggered(p0, p1, p2);

		auto eventID = static_cast<int>(p0);

		PlayerObject* player = nullptr;
		if (p2 == 0) return;//a
		if (p2 == 1) player = m_player1;//a
		if (p2 == 2) player = m_player2;//a

		if (SETTING(bool, "jelly cube")) {

			if (p0 == GJGameEvent::NormalJump) player->animatePlatformerJump(1.0f);

			if (p0 == GJGameEvent::PinkOrb) player->animatePlatformerJump(0.950f);
			if (p0 == GJGameEvent::YellowOrb) player->animatePlatformerJump(1.0f);
			if (p0 == GJGameEvent::RedOrb) player->animatePlatformerJump(1.1f);
			if (p0 == GJGameEvent::DropOrb) player->animatePlatformerJump(1.25f);

			if (p0 == GJGameEvent::HitHead) player->animatePlatformerJump(0.7f);

		}

		if (SETTING(bool, "Boost Rotation Thing")) {

			if (p0 == GJGameEvent::PinkOrb) player->addChild(createDataNode("rotate_multiplier_while_flying"), 0, -20);
			if (p0 == GJGameEvent::YellowOrb) player->addChild(createDataNode("rotate_multiplier_while_flying"), 0, -20);
			if (p0 == GJGameEvent::RedOrb) player->addChild(createDataNode("rotate_multiplier_while_flying"), 0, -20);
			if (p0 == GJGameEvent::DropOrb) player->addChild(createDataNode("rotate_multiplier_while_flying"), 0, -20);

		};

	}
};

#include <Geode/modify/PlayerObject.hpp>
class $modify(PlayerObjectExt, PlayerObject) {
	$override void switchedToMode(GameObjectType p0) {
		//log::debug("{}->{}({})", this, __FUNCTION__, (int)p0);

		if (p0 == GameObjectType::ShipPortal or p0 == GameObjectType::UfoPortal) {
			//if animatePlatformerJump still runnig and sets scale after setup for related mode:
			//13 and 14 is action tags i found in animatePlatformerJump
			this->m_iconSprite->stopActionByTag(13);
			this->m_iconSprite->stopActionByTag(14);
			this->m_iconGlow->stopActionByTag(13);
			this->m_iconGlow->stopActionByTag(14);
		}

		return PlayerObject::switchedToMode(p0);
	}
	$override void bumpPlayer(float p0, int p1, bool p2, GameObject * p3) {

		if (SETTING(bool, "jelly cube")) this->animatePlatformerJump(p0);

		if (SETTING(bool, "Boost Rotation Thing")) this->addChild(
			createDataNode("rotate_multiplier_while_flying"), 0, (p0 + 1.0f) * -10
		);

		PlayerObject::bumpPlayer(p0, p1, p2, p3);
	}
    $override void updateRotation(float p0) {

		if ((m_isRobot or m_isSpider) and !m_isOnGround) {
			auto plat = m_isPlatformer;

			m_isPlatformer = 0;

			if (SETTING(bool, "Funny Robot Rotation")) m_isShip = 1;
			else m_isBird = 1;

			PlayerObject::updateRotation(p0);

			m_isBird = 0;
			m_isShip = 0;
			m_isPlatformer = plat;

			return;
		};

		if (SETTING(bool, "Break Player Rotations At Plat")) if (m_isPlatformer) {

			m_isPlatformer = 0;
			PlayerObject::updateRotation(p0);
			m_isPlatformer = 1;

			return;
		};

		if (auto rotate_multiplier_while_flying = this->getChildByID("rotate_multiplier_while_flying")) {
			if ((m_isUpsideDown ? m_yVelocity >= 0 : m_yVelocity <= 0) or m_isOnGround) rotate_multiplier_while_flying->removeFromParent();
			return PlayerObject::updateRotation(p0 * (rotate_multiplier_while_flying->getTag() / 10));
		}

		PlayerObject::updateRotation(p0);

	}
};
