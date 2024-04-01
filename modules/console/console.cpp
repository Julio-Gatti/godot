#include "console.h"

#include <filesystem>

Console::Console() {
}

Console::~Console() {

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
		print_line("Unknown command");
	}
}

void Console::_bind_methods() {
	ClassDB::bind_static_method("Console", "submit", &Console::submit);
}

ConsoleCommand *ConsoleCommand::first;

ConsoleCommand::ConsoleCommand() {
}

ConsoleCommand::ConsoleCommand(const char *p_name, function_t p_function) :
		name(p_name), function(p_function), next(first) {
	first = this;

	printf("ConsoleCommand %s\n", p_name);
}

ConsoleCommand::~ConsoleCommand() {
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

static int help_fn(const PackedStringArray &args) {
	int num = 0;
	for (ConsoleCommand *it = ConsoleCommand::first; it; it = it->next) {
		print_line(it->name);
		++num;
	}
	print_line(num, " commands");
	return num;
}
ConsoleCommand help_cmd("help", help_fn);

static int exit_fn(const PackedStringArray &args) {
	// TODO proper
	exit(EXIT_SUCCESS);
	return EXIT_SUCCESS;
}
ConsoleCommand exit_cmd("exit", exit_fn);

static int parse_fn(const PackedStringArray &args) {
	for (int64_t i = 0; i < args.size(); ++i) {
    print_line(i, ": '", args[i], '\'');
  }
	return 0;
}
ConsoleCommand parse_cmd("parse", parse_fn);

static int errno_fn(const PackedStringArray &args) {
	print_line(errno);
	return 0;
}
ConsoleCommand errno_cmd("errno", errno_fn);

static int strerror_fn(const PackedStringArray &args) {
	print_line(strerror(errno));
	return 0;
}
ConsoleCommand strerror_cmd("strerror", strerror_fn);

static int ls_fn(const PackedStringArray &args) {
  std::filesystem::directory_iterator dir(std::filesystem::current_path());

  for (const auto& file : dir) {
    print_line(file.path().generic_string().c_str());
  }

	return 0;
}
ConsoleCommand ls_cmd("ls", ls_fn);
