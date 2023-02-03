#include "Inventory.h"
Inventory::Inventory()
{
}


Inventory::~Inventory()
{
}

void Inventory::addMoney(int amount)
{
	this->money += amount;
}

int Inventory::get_money()
{
	return money;
}

bool Inventory::insert(std::unique_ptr<Item> i)
{
	if (item_list.size() < max_item_count) {
		item_list.push_back(std::move(i));
		return true;
	}
	return false;
}

/*
	inserts items into inventory, returns number of successfully inserted items
*/
int Inventory::insert(std::vector<std::unique_ptr<Item>> is)
{
	int i = 0;
	for (; i < is.size(); i++) {
		if (space_left() > 0) {
			item_list.push_back(std::move(is[i]));
		}
		else {
			break;
		}
	}
	return i;
}

std::unique_ptr<Item> Inventory::removeFirst()
{
	std::unique_ptr<Item> first = std::move(item_list[0]);
	item_list.erase(item_list.begin());
	
	return first;
}


std::vector<std::unique_ptr<Item>> Inventory::removeAll()
{
	std::vector<std::unique_ptr<Item>> return_value;
	item_list.swap(return_value);
	return return_value;
}

std::unique_ptr<Item> Inventory::remove_selected()
{
	std::unique_ptr<Item> selected = std::move(item_list[selected_item_slot]);
	item_list.erase(item_list.begin()+selected_item_slot);
	return selected;
}

bool Inventory::delete_selected()
{
	if (is_selected_item_valid()) {

		std::unique_ptr<Item> selected = std::move(item_list[selected_item_slot]);
		Item *selected_raw = selected.release();
		delete selected_raw;

		item_list.erase(item_list.begin() + selected_item_slot);
		return true;
	}
	return false;
}

int Inventory::size()
{
	return item_list.size();
}

int Inventory::max_size()
{
	return max_item_count;
}

int Inventory::space_left()
{
	return max_item_count - item_list.size();
}

bool Inventory::isEmpty()
{
	return item_list.size() == 0;
}

bool Inventory::upgrade_backpack()
{
	if (max_item_count < max_upgrade) {
		max_item_count += 1;
		return true;
	}
	return false;
}

bool Inventory::contains(std::string name)
{
	bool contains = false;

	for (int i = 0; i < item_list.size(); i++) {
		if (item_list[i]->toString() == name)
			contains = true;
	}
	return contains;
}

std::string Inventory::toString()
{
	std::string s = "====================\n";
	s += "Selected Item Slot: " + std::to_string(selected_item_slot) + "; ";
	
	if (selected_item_slot < item_list.size()) {
		s += item_list[selected_item_slot]->toString() +"\n";
	}
	else {
		s += "[      ]\n";
	}
	s += "--------------------\n";
	s += "[ Money " + std::to_string(money) + "$ ]\n";
	for (int i = 0; i < max_item_count; i++) {
		if (i < item_list.size()) {
			s += "[ " + item_list[i]->toString() + " ]\n";
		}
		else {
			s += "[      ]\n";
		}
	}
	s += "====================\n";
	return s;
}

void Inventory::render(TextRender& r, int window_width, int window_height)
{
	int y_offset = 20;
	int size = 50;
	int border = 5;
	int char_length = 29;
	int char_height = 34;

	glm::vec4 selected_color = glm::vec4(0.627, 0.443, 0.471, 0.8);
	glm::vec4 quad_color = glm::vec4(0.824,0.835,0.867, 0.5);
	glm::vec4 bar_color = glm::vec4(0.722, 0.729, 0.812, 0.5);
	glm::vec3 money_color = glm::vec3(0.6, 0.761, 0.635);

	std::string money_string = "$" + std::to_string(money);
	int gap = 2 * border;
	
	int bar_length = (size + gap)*max_item_count + gap + money_string.size() * char_length;
	int x_offset = (window_width - bar_length) / 2 + gap/2;
	
	// bar
	r.renderquad("quad", glm::vec2(x_offset - border, y_offset - border), glm::vec2(bar_length, size+2*border), bar_color);

	// money
	r.renderquad("quad", glm::vec2(x_offset + max_item_count * size + max_item_count * gap, y_offset), glm::vec2(money_string.size() * char_length, size), quad_color);
	r.renderText(money_string, x_offset + max_item_count * size + max_item_count * gap, y_offset + (size - char_height) / 2, 1, money_color);

	// selected item
	r.renderquad("quad", glm::vec2(x_offset + selected_item_slot * size + selected_item_slot * gap - border, y_offset - border), glm::vec2(size+2*border, size+2*border), selected_color);

	for (int i = 0; i < max_item_count; i++) {
		// slots
		r.renderquad("quad", glm::vec2(x_offset + i*size + i*gap, y_offset), glm::vec2(size, size), quad_color);

		// items from inventory
		if (i < item_list.size()) {
			r.renderquad(item_list[i]->toString(), glm::vec2(x_offset + i * size + i * gap, y_offset), glm::vec2(size, size), glm::vec4(1, 1, 1, 1));
		}
	}

	

}

bool Inventory::is_selected_item_valid()
{
	return get_selected_item() != nullptr;
}

Item * Inventory::get_selected_item()
{
	if (selected_item_slot < item_list.size()) 
		return item_list[selected_item_slot].get();
	else
		return nullptr;

}



void Inventory::selected_item_change(int by) {
	int new_item_slot = (selected_item_slot + by) % max_item_count;

	if (new_item_slot < 0) {
		selected_item_slot = max_item_count + new_item_slot;
	}
	else {
		selected_item_slot = new_item_slot;
	}
}
