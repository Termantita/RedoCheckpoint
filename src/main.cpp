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
		std::stack<CCPoint> m_deletedCheckpoints;
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

		auto oldPlayerPos = m_player1->getPosition();
		m_player1->setPosition(m_fields->m_deletedCheckpoints.top());
		auto ch = PlayLayer::createCheckpoint();

		log::debug("{} < {}?", typeinfo_cast<CheckpointObject*>(m_checkpointArray->lastObject())->m_player1Checkpoint->m_position, ch->m_player1Checkpoint->m_position);
		
		if (typeinfo_cast<CheckpointObject*>(m_checkpointArray->lastObject())->m_player1Checkpoint->m_position.x < ch->m_player1Checkpoint->m_position.x) {
			storeCheckpoint(ch);
			m_fields->m_deletedCheckpoints.pop();	
		} else {
			while (!m_fields->m_deletedCheckpoints.empty())
				m_fields->m_deletedCheckpoints.pop();
		}
		m_player1->setPosition(oldPlayerPos);
	}

	void removeCheckpoint(bool p0) {
		if (!m_checkpointArray->count()) return;
		// TODO: copy more attributes from original checkpoint (best would be all of them)
		auto lastObj = typeinfo_cast<CheckpointObject*>(m_checkpointArray->lastObject());
		m_fields->m_deletedCheckpoints.push(lastObj->m_player1Checkpoint->m_position);

		PlayLayer::removeCheckpoint(p0);
	}
};