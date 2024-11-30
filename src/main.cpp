#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <geode.custom-keybinds/include/Keybinds.hpp>

#include <stack>

using namespace geode::prelude;
using namespace keybinds;

$execute {
	BindManager::get()->registerBindable({
	// ID, should be prefixed with mod ID
	"redo-checkpoint"_spr,
	
	// Name
	"Redo Checkpoint",
	
	// Description, leave empty for none
	"Key for redoing checkpoint",
	
	// Default binds
	{Keybind::create(KEY_Z, Modifier::Shift)},
	
	// Category; use slashes for specifying subcategories. See the
	// Category class for default categories
	Category::PLAY});
}

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

		return true;
	}

	void redoCheckpoint() {
		if (!m_isPracticeMode || !m_checkpointArray->count() || m_fields->m_deletedCheckpoints.empty()) 
			return;
		
		auto checkpoint = m_fields->m_deletedCheckpoints.top();

		if (static_cast<CheckpointObject*>(m_checkpointArray->lastObject())->m_player1Checkpoint->m_position.x < checkpoint->m_player1Checkpoint->m_position.x) {
			storeCheckpoint(checkpoint);
			static_cast<CheckpointGameObject*>(checkpoint->m_physicalCheckpointObject)->triggerObject(this, 0, nullptr); // DANK THE GOAT
			
			m_fields->m_deletedCheckpoints.pop();
		} else {
			while (!m_fields->m_deletedCheckpoints.empty())
				m_fields->m_deletedCheckpoints.pop();
		}
	}

	void removeCheckpoint(bool p0) {
		if (!m_checkpointArray->count()) return;

		auto lastObj = static_cast<CheckpointObject*>(m_checkpointArray->lastObject());

		m_fields->m_deletedCheckpoints.push(std::move(lastObj));
		PlayLayer::removeCheckpoint(p0);
	}
};