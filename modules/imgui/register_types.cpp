#include "register_types.h"

#include "core/object/class_db.h"
#include "imgui.h"
#include "servers/display_server.h"

#ifdef _WIN32
#include "imgui_impl_win32.h"
#endif

void initialize_imgui_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	ClassDB::register_class<IMGUI>();

	print_line("initialize_imgui_module");

	DisplayServer *const display_server = DisplayServer::get_singleton();
	DEV_ASSERT(display_server != nullptr);

	// https://docs.godotengine.org/en/stable/classes/class_displayserver.html#class-displayserver-method-window-get-native-handle
	// https://docs.godotengine.org/en/stable/classes/class_displayserver.html#enum-displayserver-handletype

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();

#ifdef _WIN32
	if (ImGui_ImplWin32_InitForOpenGL(reinterpret_cast<void *>(display_server->window_get_native_handle(DisplayServer::HandleType::WINDOW_HANDLE)))) {
		print_line("ImGui_ImplWin32_InitForOpenGL");
	}
#endif
}

void uninitialize_imgui_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	print_line("uninitialize_imgui_module");

#ifdef _WIN32
	ImGui_ImplWin32_Shutdown();
#endif

	ImGui::DestroyContext();
}
