#ifndef ITEMDATA_HPP
#define ITEMDATA_HPP

#include "tinyxml/tinyxml.h"

class ItemData
{
public:
	enum Type
	{
		ARMOR,
		ENGINE,
		SHIELD,
		HEATSINK,
		LASER1,
		LASER2,
		_COUNT
	};
	static int const MAX_ITEM_LEVEL = 3;

	ItemData();

	virtual bool LoadFromXml(TiXmlElement* elem);

	int GetPrice() const;
	int GetLevel() const;

private:
	int price_;
	int level_;
	Type type_;
};

#endif // ITEMDATA_HPP
