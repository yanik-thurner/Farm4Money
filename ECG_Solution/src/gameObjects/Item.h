#pragma once
#include <string>

class Item
{
protected:
	Item(int value) : _value(value) {}

	const int _value = -1;
public:
	int get_value() {
		return _value;
	}
	static const int value = -1;
	virtual ~Item() {}
	virtual std::string toString() = 0;
	static std::string toString(bool is_static) {
		return "item";
	};
};

