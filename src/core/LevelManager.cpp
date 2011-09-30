#include <iostream>

#include "LevelManager.hpp"
#include "entities/EntityManager.hpp"
#include "entities/Asteroid.hpp"
#include "entities/EvilBoss.hpp"
#include "entities/BossTentacles.hpp"

#define DEFAULT_STARS_COUNT 33


LevelManager& LevelManager::GetInstance()
{
	static LevelManager self;
	return self;
}


LevelManager::LevelManager()
{
	last_insert_time_ = 0.f;
	current_level_ = last_unlocked_level_ = 1;
}


LevelManager::~LevelManager()
{
	ClearWaitingLine();
}


void LevelManager::LoadCurrent()
{
	ParseLevel(GetLevelElement(current_level_));
}


Entity* LevelManager::GiveNextEntity(float timer)
{
	if (!waiting_line_.empty())
	{
		if (waiting_line_.front().spawntime < timer)
		{
			Entity* next = waiting_line_.front().entity;
			waiting_line_.pop();
			return next;
		}
	}
	return NULL;
}


void LevelManager::SetCurrent(int level)
{
	if (level < 1 || level > last_unlocked_level_ || level > (int) levels_.size() * 2)
	{
		std::cerr << " [levels] level " << level << " is undefined, using level 1" << std::endl;
		level = 1;
	}

	current_level_ = level;
}


int LevelManager::GetCurrent() const
{
	return current_level_;
}


int LevelManager::UnlockNextLevel()
{
	if (last_unlocked_level_ < (int) levels_.size() * 2)
	{
		++last_unlocked_level_;
		current_level_ = last_unlocked_level_;
	}
	return current_level_;
}


int LevelManager::GetLastUnlocked() const
{
	return last_unlocked_level_;
}


void LevelManager::SetLastUnlocked(int level)
{
	if (level < 1 || level > (int) levels_.size() * 2)
	{
		level = 1;
	}
	last_unlocked_level_ = level;
}


const char* LevelManager::GetDescription() const
{
	return GetLevelElement(current_level_)->Attribute("desc");
}


sf::Color LevelManager::GetTopColor() const
{
	const char* p = GetLevelElement(current_level_)->Attribute("bg_top");
	return p != NULL ? HexaToColor(p) : sf::Color::Black;
}


sf::Color LevelManager::GetBottomColor() const
{
	const char* p = GetLevelElement(current_level_)->Attribute("bg_bottom");
	return p != NULL ? HexaToColor(p) : sf::Color::Black;
}


int LevelManager::GetStarsCount() const
{
	int nb_stars = DEFAULT_STARS_COUNT;
	GetLevelElement(current_level_)->QueryIntAttribute("stars", &nb_stars);
	return nb_stars;
}


int LevelManager::GetDuration() const
{
	if (waiting_line_.empty())
	{
		return 0;
	}
	return waiting_line_.back().spawntime;
}


int LevelManager::GetTotalPoints() const
{
	// TODO (itérer sur waiting line ? précalculer ?)
	return 0;
}


int LevelManager::CountLevel() const
{
	return levels_.size();
}


bool LevelManager::AllLevelsCompleted() const
{
	return last_unlocked_level_ > (int) levels_.size();
}


bool LevelManager::IsHardcoreEnabled() const
{
	return current_level_ > (int) levels_.size();
}


void LevelManager::EnableHardcore(bool hardcore)
{
	if (hardcore)
		current_level_ = last_unlocked_level_;
	else
		current_level_ = AllLevelsCompleted() ? (int) levels_.size() : last_unlocked_level_;
}


void LevelManager::ParseFile(const char* file)
{
	printf("* loading levels... ");

	if (!doc_.LoadFile(file))
	{
		std::cerr << "cannot load file " << file << std::endl;
		std::cerr << "error #" << doc_.ErrorId() << ": " << doc_.ErrorDesc() << std::endl;
	}

	// Constitution de la map de pointeurs vers les fonctions
	TiXmlNode* node = doc_.RootElement()->FirstChild("functions")->FirstChild();
	while (node != NULL)
	{
		TiXmlElement* element = node->ToElement();
		const char* name = element->Attribute("name");
		if (name != NULL)
			functions_[name] = element;
		else
			puts("warning: function doesn't have a proper name");
		node = node->NextSibling();
	}

	// Constitution de la liste de pointeurs vers les niveaux
	node = doc_.RootElement()->FirstChild("levels")->FirstChild();
	while (node != NULL)
	{
		levels_.push_back(node->ToElement());
		node = node->NextSibling();
	}
	printf("%d levels found\n", (int) levels_.size());
}


void LevelManager::ParseLevel(TiXmlElement* elem)
{
	ClearWaitingLine();
	last_insert_time_ = 0.f;
	elem = elem->FirstChildElement();
	while (elem)
	{
		ParseEntity(elem);
		elem = elem->NextSiblingElement();
	}
}


void LevelManager::ParseEntity(TiXmlElement* elem)
{
	EntityManager& entity_mgr = EntityManager::GetInstance();
	const char* tag_name = elem->Value();
	if (strcmp(tag_name, "loop") == 0)
	{
		int count = 0;
		elem->QueryIntAttribute("count", &count);
		while (count > 0)
		{
			TiXmlElement* child = elem->FirstChildElement();
			while (child != NULL)
			{
				ParseEntity(child);
				child = child->NextSiblingElement();
			}
			--count;
		}
	}
	else if (strcmp(tag_name, "call") == 0)
	{
		const char* func_name = elem->Attribute("func");
		if (func_name != NULL)
		{
			std::map<std::string, TiXmlElement*>::iterator it = functions_.find(func_name);
			if (it != functions_.end())
			{
				TiXmlElement* child	= it->second->FirstChildElement();
				while (child != NULL)
				{
					ParseEntity(child);
					child = child->NextSiblingElement();
				}
			}
			else
			{
				std::cerr << "[levels] invalid function '" << func_name << "' in 'call' tag" << std::endl;
			}
		}
		else
		{
			std::cerr << "[levels] 'func' attribute missing in 'call' tag" << std::endl;
		}
	}
	else if (strcmp(tag_name, "wait") == 0)
	{
		float t = 0.f;
		elem->QueryFloatAttribute("t", &t);
		last_insert_time_ += t;
	}
	else
	{
		sf::Vector2f position;
		position.x = 640; // default: screen right side
		float time = 0.f; // default: instant
		elem->QueryFloatAttribute("x", &position.x);
		elem->QueryFloatAttribute("y", &position.y);
		elem->QueryFloatAttribute("t", &time);
		Entity* entity = NULL;
		if (strcmp(tag_name, "ship") == 0)
		{
			int id = 0;
			elem->QueryIntAttribute("id", &id);
			entity = entity_mgr.CreateSpaceShip(id, position.x, position.y);
		}
		else if (strcmp(tag_name, "asteroid") == 0)
		{
			entity = new Asteroid(position, Asteroid::BIG);
		}
		else if (strcmp(tag_name, "evilboss") == 0)
		{
			entity = new EvilBoss(position);
		}
		else if (strcmp(tag_name, "boss_tentacles") == 0)
		{
			entity = new BossTentacles(position);
		}
		else
		{
			std::cerr << "[levels] unsupported tag '" << tag_name << "' ignored" << std::endl;
		}
		AppendToWaitingLine(entity, time);
	}
}


void LevelManager::AppendToWaitingLine(Entity* entity, float time)
{
	EntitySlot slot;
	if (IsHardcoreEnabled())
	{
		entity->SetHP(entity->GetHP() * 2);
	}
	slot.entity = entity;
	last_insert_time_ += time;
	slot.spawntime = last_insert_time_;
	waiting_line_.push(slot);
}


void LevelManager::ClearWaitingLine()
{
	// delete remaining entities in the waiting line
	while (!waiting_line_.empty())
	{
		delete waiting_line_.front().entity;
		waiting_line_.pop();
	}
}


TiXmlElement* LevelManager::GetLevelElement(int level) const
{
	if (level > (int) levels_.size())
		level -= levels_.size();
	--level; // index starts at 0
	if (level < 0 || level >= (int) levels_.size())
	{
		std::cerr << "[levels] index " << level << " is not a valid level, using index 0" << std::endl;
		level = 0;
	}
	return levels_[level];
}


sf::Color LevelManager::HexaToColor(const std::string& hexcolor)
{
	sf::Color color = sf::Color::Black;
	if (hexcolor.size() == 7)
	{
		color.r = strtoul(hexcolor.substr(1, 2).c_str(), NULL, 16);
		color.g = strtoul(hexcolor.substr(3, 2).c_str(), NULL, 16);
		color.b = strtoul(hexcolor.substr(5, 2).c_str(), NULL, 16);
	}
	return color;
}
