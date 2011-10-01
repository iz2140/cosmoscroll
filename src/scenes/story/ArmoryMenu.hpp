#ifndef ARMORY_HPP
#define ARMORY_HPP

#include "CreditCounterBase.hpp"
#include "UpgradeItem.hpp"


class ArmoryMenu: public CreditCounterBase
{
public:
	ArmoryMenu();

private:
	// override
	void EventCallback(int id);

	void ShowDialog(bool visible);

	bool BuyItem(ItemData::Type type);

	void LoadItem(ItemData::Type type);

	struct Dialog
	{
		static const int WIDTH = 360;
		static const int HEIGHT = 180;
		int x, y;
		gui::Image* background;
		gui::Label* lab_item;
		gui::Label* current_level;
		gui::Label* current_level_details;
		gui::Label* next_level;
		gui::Label* next_level_details;
		gui::Button* but_back;
		gui::Button* but_buy;
	} dialog_;

	UpgradeItem* items_[ItemData::_COUNT];

};

#endif // ARMORY_HPP
