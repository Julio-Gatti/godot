#pragma once

#include "core/object/ref_counted.h"

class ConsoleCommand;

class Console : public Object {
	GDCLASS(Console, Object);

public:
	static String output;

	Console();
	virtual ~Console();

	static int exec(const String &file) noexcept;

	/// Console output
	static void print_line(const Variant &v) {
		auto str = ::stringify_variants(v);
		output += str;
		::__print_line(str);
	}

	/// Console output
	template <typename... Args>
	static void print_line(const Variant &p_var, Args... p_args) {
		auto str = ::stringify_variants(p_var, p_args...);
		output += str;
		::__print_line(str);
	}

	static void submit(const String &text);

	static String *find_alias(const String &name);

protected:
	static void _bind_methods();
};

enum ConsoleCommandFlags : int64_t {
	CMD_FL_CHEAT = 1 << 0,
	CMD_FL_C = 1 << 1,
	CMD_FL_UNIX = 1 << 2,
};

/// @todo `Resource`?
class ConsoleCommand : public RefCounted {
	GDCLASS(ConsoleCommand, RefCounted);

public:
	typedef int (*function_t)(const PackedStringArray &args);
	using flags_t = std::underlying_type<ConsoleCommandFlags>::type;

	ConsoleCommand();
	ConsoleCommand(const char *name, function_t function, flags_t flags = 0, const char *description = "");
	virtual ~ConsoleCommand();

	int invoke(const PackedStringArray &args);

	static void _bind_methods();

	static ConsoleCommand *find(const String &name);

	String name;
	function_t function = nullptr;
	String description;
	flags_t flags = 0;
	ConsoleCommand *next = nullptr;

	static ConsoleCommand *first;
};

class ConsoleVariable : public ConsoleCommand {
	GDCLASS(ConsoleVariable, ConsoleCommand);

public:
};
