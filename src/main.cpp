#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>

#include <stack>
#include "Keybind.hpp"

#define TOCHECKPOINT(x) static_cast<CheckpointObject*>(x)

using namespace geode::prelude;


struct CustomPlayLayer : Modify<CustomPlayLayer, PlayLayer> {
	struct Fields {
		std::stack<Ref<CheckpointObject>> m_deletedCheckpoints;
	};

	bool init(GJGameLevel* p0, bool p1, bool p2) {
		if (!PlayLayer::init(p0, p1, p2))
			return false;
		
		this->template addEventListener<InvokeBindFilter>([&](InvokeBindEvent* event) {
			if (event->isDown()) {
				redoCheckpoint();
			}
			return ListenerResult::Propagate;
		}, "redo-checkpoint"_spr);
		
#ifdef GEODE_IS_ANDROID
		auto practiceMenu = getChildByIDRecursive("checkpoint-menu");

		if (practiceMenu) {
			auto redoButton = CCMenuItemSpriteExtra::create(
				CircleButtonSprite::createWithSprite("redo.png"_spr, 1, CircleBaseColor::Green, CircleBaseSize::Big),
				this,
				menu_selector(CustomPlayLayer::onRedoCheckpoint)
			);
			
			auto placeCheckpointBtn = getChildByIDRecursive("add-checkpoint-button");

			redoButton->setID("redo-checkpoint-button"_spr);
			redoButton->setPositionX(placeCheckpointBtn->getPositionX() - 70);

			practiceMenu->addChild(redoButton);
		}
#endif
		return true;
	}

	bool isLastCheckpoint(CheckpointObject* checkpoint) {
		if (!m_checkpointArray->count()) return true;

		return TOCHECKPOINT(m_checkpointArray->lastObject())->m_player1Checkpoint->m_position.x < checkpoint->m_player1Checkpoint->m_position.x;
	}

	void enableCheckpoint(CheckpointObject* checkpoint) {
		static_cast<CheckpointGameObject*>(checkpoint->m_physicalCheckpointObject)->triggerObject(this, 0, nullptr); // DANK THE GOAT
	}

	void clearDeletedCheckpoints() {
		while (!m_fields->m_deletedCheckpoints.empty())
			m_fields->m_deletedCheckpoints.pop();
	}

	void addDeletedCheckpoint(CheckpointObject* checkpoint) {
		storeCheckpoint(checkpoint);
			
		m_fields->m_deletedCheckpoints.pop();
	}

	void onRedoCheckpoint(CCObject* sender) {
		redoCheckpoint();
	}

	void redoCheckpoint() {
		if (!m_isPracticeMode || m_fields->m_deletedCheckpoints.empty()) {
			if (getChildByIDRecursive("redo-checkpoint-notification") || Mod::get()->getSettingValue<bool>("disable-notification"))
				return;
			
			auto notice = Notification::create("No checkpoints to redo", NotificationIcon::Error);
			notice->setID("redo-checkpoint-notification"_spr);
			notice->show();
			
			return;
		}
		
		auto checkpoint = m_fields->m_deletedCheckpoints.top();
		log::debug("stack size: {}", m_fields->m_deletedCheckpoints.size());
		if (isLastCheckpoint(checkpoint)) {
			addDeletedCheckpoint(checkpoint);
			enableCheckpoint(checkpoint);
		} else {
			clearDeletedCheckpoints();
		}
	}

	void removeCheckpoint(bool p0) {
		if (!m_checkpointArray->count()) return;

		auto lastObj = TOCHECKPOINT(m_checkpointArray->lastObject());
		m_fields->m_deletedCheckpoints.push(std::move(lastObj));
		
		PlayLayer::removeCheckpoint(p0);
	}
};