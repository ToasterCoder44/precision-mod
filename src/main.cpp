#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

float hitboxBorderWidth = 1.f;
ccColor4F hazardHitboxColor = {1.f, 0.f, 0.f, 1.f};
ccColor4F primaryHitboxColor = {0.f, 0.f, 1.f, 1.f};
ccColor4F extraHitboxColor = {1.f, 1.f, 0.f, 1.f};

class HitboxNode : public CCDrawNode {
public:
	static HitboxNode* create() {
		auto ret = new HitboxNode;
		if (!ret)
			return nullptr;

		ret->init();
		hitboxBorderWidth = Mod::get()->getSettingValue<double>("hitbox-border-width");
		return ret;
	}

	void drawObjectHitbox(GameObject *object) {
		ccColor4F color;
		switch (object->getType()) {
			case GameObjectType::Solid:
				color = primaryHitboxColor;
				break;
			case GameObjectType::Hazard:
				color = hazardHitboxColor;
				break;
			case GameObjectType::Slope:
				break; //tba
			case GameObjectType::InverseGravityPortal:
			case GameObjectType::YellowJumpPad:
			case GameObjectType::YellowJumpRing:
			case GameObjectType::InverseMirrorPortal:
			case GameObjectType::DualPortal:
			case GameObjectType::NormalGravityPortal:
			case GameObjectType::NormalMirrorPortal:
			case GameObjectType::SoloPortal:
			case GameObjectType::PinkJumpPad:
			case GameObjectType::PinkJumpRing:
			case GameObjectType::GravityPad:
			case GameObjectType::GravityRing:
			case GameObjectType::RedJumpPad:
			case GameObjectType::RedJumpRing:
			case GameObjectType::GreenRing:
			case GameObjectType::DashRing:
			case GameObjectType::RegularSizePortal:
			case GameObjectType::GravityDashRing:
			case GameObjectType::MiniSizePortal:
			case GameObjectType::SecretCoin:
			case GameObjectType::UserCoin:
			case GameObjectType::CustomRing:
			case GameObjectType::Collectible:
			case GameObjectType::DropRing:
			case GameObjectType::CubePortal:
			case GameObjectType::ShipPortal:
			case GameObjectType::BallPortal:
			case GameObjectType::UfoPortal:
			case GameObjectType::WavePortal:
			case GameObjectType::RobotPortal:
			case GameObjectType::SpiderPortal:
				color = extraHitboxColor;
				break;
			default:
				return;
		}

		if (object->getObjectRadius() > 0) {
			auto pos = object->getPosition();
			this->drawCircularHitbox(pos, object->getObjectRadius(), color);
			return;
		}
		auto rect = object->getObjectRect();
		this->drawRectangularHitbox(rect, color);
	}

	void drawPlayerHitbox(PlayerObject *player) {
		auto rect = player->getObjectRect();
		this->drawRectangularHitbox(rect, extraHitboxColor);
	}

	void drawRectangularHitbox(const CCRect &rect, const ccColor4F &color) {
		float offset = hitboxBorderWidth; //temporary
		float minX = rect.getMinX() + offset;
		float minY = rect.getMinY() + offset;
		float maxX = rect.getMaxX() - offset;
		float maxY = rect.getMaxY() - offset;
		CCPoint points[] = {
			{minX, minY},
			{maxX, minY},
			{maxX, maxY},
			{minX, maxY}
		};
		this->drawPolygon(points, 4, {0.f, 0.f, 0.f, 0.f}, hitboxBorderWidth, color);
	}

	void drawCircularHitbox(const CCPoint &center, float radius, const ccColor4F &color) {
		this->drawDot(center, radius, color);
	}
};

class $modify(PlayLayer) {
	bool init(GJGameLevel* p0) {
		if (!PlayLayer::init(p0))
			return false;

		auto hitboxNode = HitboxNode::create();
		hitboxNode->setID("hitbox-node"_spr);
		this->getObjectLayer()->addChild(hitboxNode, 32);

		return true;
	}

	void update(float p0) {
		PlayLayer::update(p0);

		auto hitboxNode = static_cast<HitboxNode*>(this->getObjectLayer()->getChildByID("hitbox-node"_spr));
		hitboxNode->clear();

		int section = sectionForPos(this->m_player1->getPositionX());
		int middleSectionID = sectionForPos(this->m_player1->getPositionX());
		int sectionEndID = std::min(
			middleSectionID + 6,
			static_cast<int>(this->m_sectionObjects->count())
		);

		for (int sectionIndex = std::max(0, middleSectionID - 5); sectionIndex < sectionEndID; sectionIndex++) {
			auto section = static_cast<CCArray*>(this->m_sectionObjects->objectAtIndex(sectionIndex));
			for (unsigned int objectIndex = 0; objectIndex < section->count(); objectIndex++) {
				auto object = static_cast<GameObject*>(section->objectAtIndex(objectIndex));
				hitboxNode->drawObjectHitbox(object);
			}
		}

		hitboxNode->drawPlayerHitbox(this->m_player1);
		
	}
};
