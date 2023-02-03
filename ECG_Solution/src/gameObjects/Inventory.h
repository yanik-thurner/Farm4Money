#pragma once
#include "Item.h"
#include <vector>
#include <memory>
#include "../TextRender.h"
#include "../Utils.h"

class Inventory
{
private:
	int money = 500;
	std::vector<std::unique_ptr<Item>> item_list;
	int max_item_count = 4;
	int max_upgrade = 10;

	int selected_item_slot = 0;

public:
	Inventory();
	~Inventory();
	
	void addMoney(int amount);
	int get_money();
	bool insert(std::unique_ptr<Item> i);
	int insert(std::vector<std::unique_ptr<Item>> is);
	std::unique_ptr<Item> removeFirst();
	std::vector<std::unique_ptr<Item>> removeAll();
	int size();
	int max_size();
	int space_left();
	bool isEmpty();
	bool upgrade_backpack();
	bool contains(std::string name);
	bool is_selected_item_valid();
	Item *get_selected_item();
	std::unique_ptr<Item> remove_selected();
	bool delete_selected();

	void selected_item_change(int by);

	std::string toString();

	void render(TextRender& r, int window_width, int window_height);
};

