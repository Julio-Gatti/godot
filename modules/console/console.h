#ifndef CONSOLE_H
#define CONSOLE_H

#include "core/object/ref_counted.h"

class Console : public Object {
	GDCLASS(Console, Object);

public:
	static Vector<String> output;

	Console();
	virtual ~Console();

	static void submit(const String &text);

protected:
	static void _bind_methods();
};

class ConsoleCommand : public RefCounted {
	GDCLASS(ConsoleCommand, RefCounted);

public:
	typedef int (*function_t)(const PackedStringArray &args);

	ConsoleCommand();
	ConsoleCommand(const char *name, function_t function);
	virtual ~ConsoleCommand();

	int invoke(const PackedStringArray &args);

	static ConsoleCommand *find(const String &name);

	String name;
	function_t function;
	ConsoleCommand *next = nullptr;

	static ConsoleCommand *first;
};

#endif
