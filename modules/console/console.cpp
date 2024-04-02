#include "console.h"

#include <filesystem>
#include <fstream>

String Console::output;
Vector<String> aliases;

Console::Console() {
}

Console::~Console() {
}

int Console::exec(const String &file) noexcept {
	/*std::ifstream source(file.get_data());
	if (!source.is_open()) {
		Console::print_line("Couldn't open stream:", strerror(errno));
		return errno;
	}*/
	return 0;
}

void Console::submit(const String &text) {
	auto args = text.split_spaces();

	if (args.is_empty()) {
		return;
	}

	ConsoleCommand *cmd;
	if ((cmd = ConsoleCommand::find(args[0]))) {
		cmd->invoke(args);
	} else {
		Console::print_line("Unknown command:", args[0]);
	}
}

String* Console::find_alias(const String&name) {
	for (auto& alias : aliases) {
		if (alias == name) {
			return &alias;
		}
	}
	return nullptr;
}

void Console::_bind_methods() {
	ClassDB::bind_static_method("Console", "submit", &Console::submit);
}

ConsoleCommand *ConsoleCommand::first;

ConsoleCommand::ConsoleCommand() {
}

ConsoleCommand::ConsoleCommand(const char *p_name, function_t p_function, flags_t p_flags, const char *p_description) :
		name(p_name), function(p_function), flags(p_flags), description(p_description), next(first) {
	first = this;

	printf("ConsoleCommand %s\n", p_name);
}

ConsoleCommand::~ConsoleCommand() {
}

void ConsoleCommand::_bind_methods() {
	ClassDB::bind_static_method("ConsoleCommand", "find", &ConsoleCommand::find);
}

int ConsoleCommand::invoke(const PackedStringArray &args) {
	DEV_ASSERT(ConsoleCommand::function != nullptr);

	return function(args);
}

ConsoleCommand *ConsoleCommand::find(const String &name) {
	for (ConsoleCommand *it = first; it; it = it->next) {
		if (it->name == name) {
			return it;
		}
	}
	return nullptr;
}

/* commands */

static int cplusplus_fn(const PackedStringArray &args) {
	print_line("__cplusplus is", (int)__cplusplus);

	return (int)__cplusplus;
}
ConsoleCommand cplusplus_cmd("__cplusplus", cplusplus_fn, 0, "C++ version");

static int help_fn(const PackedStringArray &args) {
	int num = 0;
	for (ConsoleCommand *it = ConsoleCommand::first; it; it = it->next) {
		Console::print_line(it->name);
		++num;
	}
	Console::print_line(num, "commands");
	return num;
}
ConsoleCommand help_cmd("help", help_fn);

static int exec_fn(const PackedStringArray &args) {
	if (args.size() == 2) {
		return Console::exec(args[1]);
	} else {
		Console::print_line("Usage:", args[0], "<file>");
		return -1;
	}
}
ConsoleCommand exec_cmd("exec", exec_fn);

static int bind_fn(const PackedStringArray &args) {
	return 0;
}
ConsoleCommand bind_cmd("bind", bind_fn);

static int exit_fn(const PackedStringArray &args) {
	// TODO proper
	exit(EXIT_SUCCESS);
	return EXIT_SUCCESS;
}
ConsoleCommand exit_cmd("exit", exit_fn, CMD_FL_C, "C");

static int parse_fn(const PackedStringArray &args) {
	for (int64_t i = 0; i < args.size(); ++i) {
		// TODO how to not print space?
		Console::print_line(i, ": ", args[i]);
	}
	return 0;
}
ConsoleCommand parse_cmd("parse", parse_fn);

static int errno_fn(const PackedStringArray &args) {
	Console::print_line(errno);
	return 0;
}
ConsoleCommand errno_cmd("errno", errno_fn, CMD_FL_C, "C");

static int strerror_fn(const PackedStringArray &args) {
	Console::print_line(strerror(errno));
	return 0;
}
ConsoleCommand strerror_cmd("strerror", strerror_fn, CMD_FL_C, "C");

// UNIX
// https://en.wikipedia.org/wiki/List_of_POSIX_commands

static int alias_fn(const PackedStringArray &args) {
	if (args.size() == 2) {
		auto* const alias = Console::find_alias(args[1]);
		if (alias) {
			Console::print_line(args[1], "is", *alias);
			return 0;
		} else {
			Console::print_line("No such alias");
			return 0;
		}
	} else if (args.size() == 3) {
		auto alias = Console::find_alias(args[1]);
		if (alias) {
			return 0;
		} else {
			Console::print_line("No such alias");
			return 0;
		}
	} else {
		Console::print_line(aliases.size(), "aliases");
		return 0;
	}
}
ConsoleCommand alias_cmd("alias", alias_fn, CMD_FL_UNIX, "POSIX");

static int cat_fn(const PackedStringArray &args) {
	if (args.size() == 2) {
	} else {
		Console::print_line("Usage:", args[0], "<file>");
	}

	return 0;
}
ConsoleCommand cat_cmd("cat", cat_fn, CMD_FL_UNIX, "POSIX");

static int echo_fn(const PackedStringArray &args) {
	// whatever
	String str;

	for (int64_t i = 1; i < args.size(); ++i) {
		str += args[i];
	}
	Console::print_line(str);

	return 0;
}
ConsoleCommand echo_cmd("echo", echo_fn, CMD_FL_UNIX, "POSIX");

static int ls_fn(const PackedStringArray &args) {
	std::filesystem::directory_iterator dir(std::filesystem::current_path());

	for (const auto &file : dir) {
		Console::print_line(file.path().generic_string().c_str());
	}

	return 0;
}
ConsoleCommand ls_cmd("ls", ls_fn, CMD_FL_UNIX, "POSIX");

#ifdef _WIN32
#include <windows.h>
#endif

static int who_fn(const PackedStringArray &args) {
#ifdef _WIN32
	CHAR username[256];
	DWORD username_len = sizeof(username);
	GetUserNameA(username, &username_len);
	Console::print_line(username);
	return 0;
#else
	Console::print_line("TODO");
	return -1;
#endif
}
ConsoleCommand who_cmd("who", who_fn, CMD_FL_UNIX, "POSIX");

#include "core/config/engine.h"

static int architecture_fn(const PackedStringArray &args) {
	Engine *const engine = Engine::get_singleton();

	Console::print_line(engine->get_architecture_name());

	return 0;
}
ConsoleCommand architecture_cmd("architecture", architecture_fn, 0, "CPU architecture");

static int compiler_fn(const PackedStringArray &args) {
#ifdef __GNUC__
	Console::print_line("compiler is GCC %d.%d.%d", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#elif defined(_MSC_VER)
	Console::print_line("compiler is MSVC");
#else
#error "Unknown compiler"
#endif

	return 0;
}
ConsoleCommand compiler_cmd("compiler", compiler_fn, 0, "C++ compiler");

static int max_fps_fn(const PackedStringArray &args) {
	Engine *const engine = Engine::get_singleton();

	if (args.size() >= 2) {
		engine->set_max_fps((int)args[0].to_int());
	} else {
		Console::print_line(args[0], "is", engine->get_max_fps());
	}

	return 0;
}
// Godot.
ConsoleCommand max_fps_cmd("max_fps", max_fps_fn, 0, "framerate limit");
// Source? Not GoldSrc.
ConsoleCommand fps_max_cmd("fps_max", max_fps_fn, 0, "framerate limit");

#include "servers/display_server.h"

static int vsync_fn(const PackedStringArray &args) {
	DisplayServer *const display_server = DisplayServer::get_singleton();

	if (args.size() >= 2) {
		// nice!
		if (args[1].is_numeric()) {
			const auto vsync = args[1].to_int();
			if (vsync >= 0 && vsync <= 3) {
				display_server->window_set_vsync_mode(static_cast<DisplayServer::VSyncMode>(vsync));
			} else {
				Console::print_line("Invalid argument");
			}
		} else {
			if (args[1].casecmp_to("VSYNC_DISABLED") == 0) {
				display_server->window_set_vsync_mode(DisplayServer::VSYNC_DISABLED);
			} else if (args[1].casecmp_to("VSYNC_ENABLED") == 0) {
				display_server->window_set_vsync_mode(DisplayServer::VSYNC_ENABLED);
			} else if (args[1].casecmp_to("VSYNC_ADAPTIVE") == 0) {
				display_server->window_set_vsync_mode(DisplayServer::VSYNC_ADAPTIVE);
			} else if (args[1].casecmp_to("VSYNC_MAILBOX") == 0) {
				display_server->window_set_vsync_mode(DisplayServer::VSYNC_MAILBOX);
			} else {
				Console::print_line("Invalid argument");
			}
		}
	} else {
		Console::print_line(args[0], "is", display_server->window_get_vsync_mode((DisplayServer::WindowID)0));
	}

	return 0;
}
ConsoleCommand vsync_cmd("vsync", vsync_fn, 0, "vertical synchronization");

static int audio_output_latency_fn(const PackedStringArray &args) {
	Engine *const engine = Engine::get_singleton();

	if (args.size() >= 2) {
		engine->set_audio_output_latency((int)args[1].to_int());
	} else {
		Console::print_line(args[0], "is", engine->get_audio_output_latency());
	}

	return 0;
}
ConsoleCommand audio_output_latency_cmd("audio_output_latency", audio_output_latency_fn, 0, "msec");

static int time_scale_fn(const PackedStringArray &args) {
	Engine *const engine = Engine::get_singleton();

	if (args.size() >= 2) {
		engine->set_time_scale((double)args[1].to_float());
	} else {
		Console::print_line(args[0], "is", engine->get_time_scale());
	}

	return 0;
}
ConsoleCommand time_scale_cmd("time_scale", time_scale_fn, CMD_FL_CHEAT, "time speed");
