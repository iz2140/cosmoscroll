#include "EvilBoss.hpp"
#include "MediaManager.hpp"
#include "Math.hpp"
#include "Game.hpp"

#include <iostream>

#define SPEED 25
#define CHILDS 8

#define L_EYE_OFFSET        sf::Vector2f(131,  67)
#define R_EYE_OFFSET	    sf::Vector2f(233, 70)

#define L_MOUTH_X_OFFSET 85
#define R_MOUTH_X_OFFSET 201
#define MOUTH_Y_OFFSET 128


EvilBoss::EvilBoss(const sf::Vector2f& offset, Entity* target) :
	Ennemy(offset, GET_IMG("evil_boss"), 500, target),
	eye_left_(Weapon::PLASMACANNON, this),
	eye_right_(Weapon::PLASMACANNON, this),
	canon_(Weapon::DEVILSEYES, this),
	game_(Game::GetInstance())
{
	left_ = true;
}


void EvilBoss::Move(float frametime)
{
	frametime_ = frametime;
	bool left = true;
	static bool direction_ = false;
	static float mover_ = 10.0;
	float moving_ = SPEED * frametime;
	mover_ -= moving_;
	if (mover_ < 1 && mover_ > -1)
	{
		mover_ = 30.0;
		direction_ ^= 1;
	}
	if (direction_)
	{
		sprite_.Move(0, -SPEED * frametime);
	}
	else
	{
		sprite_.Move(0, SPEED * frametime);
	}

	if (sprite_.GetPosition().x < target_->GetPosition().x)
    {
    	left = false;
    }

	if (left != left_)
	{
	    sprite_.FlipX(!left);
	    left_ = left;
	}
	eye_left_.Update(frametime);
	eye_right_.Update(frametime);
	canon_.Update(frametime);
}


void EvilBoss::Collide(Entity& ent)
{
	(void) ent;
}


void EvilBoss::Action()
{
	/*static float cpt = 5.0;
	cpt -= frametime_;
	int mode = sf::Randomizer::Random(0, 1);
	if (cpt <= 0)
	{
		cpt = 5.0;
		mode = 2;
	}*/
	float radians_L = ANGLE(target_->GetPosition(), sprite_.GetPosition() + L_EYE_OFFSET);
	float radians_R = ANGLE(target_->GetPosition(), sprite_.GetPosition() + R_EYE_OFFSET);
	//if (mode == 1 || mode == 2)
	//{
	
		eye_left_.Shoot(sprite_.GetPosition() + L_EYE_OFFSET, radians_L);
		eye_right_.Shoot(sprite_.GetPosition() + R_EYE_OFFSET, radians_R);
	//}
	//if (mode == 2)
	//{
		sf::Vector2f randV2f, my = sprite_.GetPosition();
		randV2f.x = my.x + sf::Randomizer::Random(L_MOUTH_X_OFFSET, R_MOUTH_X_OFFSET);
		randV2f.y = my.y + MOUTH_Y_OFFSET;
		canon_.Shoot(randV2f, ANGLE(target_->GetPosition(), randV2f));
		//for (int i = 0; i < CHILDS; ++i)
		//{

		//}
	//}*/
}
