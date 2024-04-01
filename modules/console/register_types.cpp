#include "register_types.h"

#include "core/object/class_db.h"
#include "console.h"

void initialize_console_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	ClassDB::register_class<Console>();
	ClassDB::register_class<ConsoleCommand>();
}

void uninitialize_console_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}
