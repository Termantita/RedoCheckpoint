#include <geode.custom-keybinds/include/Keybinds.hpp>

using namespace keybinds;
using namespace geode::prelude;

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