#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include <math.h>

using namespace geode::prelude;

float hitboxBorderWidth = 1.f;
ccColor4F hazardHitboxColor = {1.f, 0.f, 0.f, 1.f};
ccColor4F primaryHitboxColor = {0.f, 0.f, 1.f, 1.f};
ccColor4F extraHitboxColor = {1.f, 1.f, 0.f, 1.f};
ccColor4F playerHitboxColor = {0.f, 1.f, 0.f, 1.f};

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
		if (!object->m_active) 
			return;

		ccColor4F color;
		switch (object->getType()) {
			case GameObjectType::Solid:
				color = primaryHitboxColor;
				break;
			case GameObjectType::Hazard:
				color = hazardHitboxColor;
				break;
			case GameObjectType::Slope:
				return; //tba
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

		float radius = object->getObjectRadius();
		if (radius > 0) {
			auto pos = object->getPosition();
			this->drawHitboxFromCircle(pos, radius, color);
			return;
		}

		auto OBB2D = object->m_objectOBB2D;
		if (OBB2D) {
			this->drawHitboxFromOBB2D(OBB2D, color);
			return;
		}

		this->drawHitboxFromRect(object->getObjectRect(), color);
	}

	void drawPlayerHitbox(PlayerObject *player) {
		float radius = player->getObjectRadius();
		if (radius > 0) { // temporary
			auto pos = player->getPosition();
			this->drawHitboxFromCircle(pos, radius, playerHitboxColor);
			return;
		}
		
		auto OBB2D = player->m_objectOBB2D;
		if (OBB2D) {
			this->drawHitboxFromOBB2D(OBB2D, playerHitboxColor);
		}

		this->drawHitboxFromRect(player->getObjectRect(), playerHitboxColor);
		this->drawHitboxFromRect(player->getObjectRect(.25f, .25f), playerHitboxColor);
	}

	void drawHitboxFromRect(const CCRect &rect, const ccColor4F &color) {
		float offset = hitboxBorderWidth; //temporary
		float minX = rect.getMinX(); //+ offset;
		float minY = rect.getMinY(); //+ offset;
		float maxX = rect.getMaxX(); //- offset;
		float maxY = rect.getMaxY(); //- offset;
		CCPoint points[] = {
			{minX, minY},
			{maxX, minY},
			{maxX, maxY},
			{minX, maxY}
		};
		this->drawPolygon(points, 4, {0.f, 0.f, 0.f, 0.f}, hitboxBorderWidth, color);
	}

	void drawHitboxFromOBB2D(OBB2D *OBB2D, const ccColor4F &color) {
		CCPoint points[] = {
			OBB2D->m_p1_1,
			OBB2D->m_p1_2,
			OBB2D->m_p1_3,
			OBB2D->m_p1_4
		};
		this->drawPolygon(points, 4, {0.f, 0.f, 0.f, 0.f}, hitboxBorderWidth, color);
	}

	void drawHitboxFromCircle(const CCPoint &center, float radius, const ccColor4F &color) {
		// temporary
		constexpr size_t N = 32;
		CCPoint points[N];
		for (size_t i = 0; i < N; i++) {
			points[i] = center + CCPoint(std::cos(2 * M_PI * i / N), std::sin(2 * M_PI * i / N)) * radius;
		}
		this->drawPolygon(points, N, {0.f, 0.f, 0.f, 0.f}, hitboxBorderWidth, color);
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
		if (this->m_isDualMode) {
			hitboxNode->drawPlayerHitbox(this->m_player2);
		}
	}
};
