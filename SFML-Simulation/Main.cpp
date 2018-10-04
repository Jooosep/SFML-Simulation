#pragma once
#include <SFML/graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>
#include <fstream>
#include <vector>
#include <math.h>
#include <map>
#include <iostream>
#include <random>
#include <algorithm>
#include <memory>

#define FLAME_MAX 2800
typedef std::vector<std::vector<sf::Vector2f>> multiVector;
typedef std::mt19937 rng_type;
std::uniform_int_distribution<rng_type::result_type> udist(0, 20);

rng_type rng;

struct Fuel
{

	double material;
	double ignition;
	double spread;
	sf::Vector2f pos;
	sf::Vector2f dir;
	double force;
	double forceMultiplier;
	Fuel(sf::Vector2f pos, sf::Vector2f dir, double material = 1.0, double spread = 0.0, double ignition = 0.0, double force = 1.0, double forceMultiplier = 0.9995)
		: pos(pos), dir(dir), material(material), spread(spread), ignition(ignition), force(force), forceMultiplier(forceMultiplier)
	{

	}
};

struct Smoke
{

	sf::Vector2f pos;
	float radius;
	Smoke(sf::Vector2f pos)
		: pos(pos), radius(3.0)
	{
	}
};

bool hasNoMaterial(struct Fuel flame)
{
	return(flame.material < 0);
}


bool radiusBelowZero(struct Smoke smoke)
{
	return(smoke.radius < 0);
}

int numberOfNeighbors(sf::Vector2f * center, std::vector<struct Fuel> * neighbors)
{
	int n = 0;
	for (auto& v : *neighbors)
	{
		if (sqrt(pow(abs(v.pos.x - center->x), 2) + pow(abs(v.pos.y - center->y), 2)) < 10.f)
			n++;
	}
	return n;
}




void gotoxy(int x, int y)
{
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

float jump(int time, float v0, float scale)
{
	float t = time / 1000.f;
	float yPos0 = 0.f;
	float a = -9.8;
	float v = v0 + a * t;
	float y = v0 * t + 0.5f * a * pow(t, 2.f);
	return y * scale;
}

int rndm()
{
	// seed rng first:
	rng_type::result_type const seedval = rand(); // get this from somewhere
	rng.seed(seedval);

	rng_type::result_type random_number = udist(rng);

	return random_number;
}

int main()
{

	int width = 1800;
	int height = 1000;

	srand(time(NULL));

	float maxTimePerFrame = 0;

	// let's define a view
	sf::View view(sf::FloatRect(0, 0, width, height));
	sf::RenderWindow window(sf::VideoMode(width, height), "vertex");
	sf::RenderStates states = sf::RenderStates(sf::BlendMode::BlendMode());

	sf::Time t1 = sf::milliseconds(0);

	//Lets define moveable character and set the initial pos
	float characterHeight = 40.f;
	float characterWidth = 40.f;
	float initialX = width / 2;
	float initialY = height - 40;
	float jumpVelocity = 7.f;
	sf::Texture particleTexture;
	particleTexture.loadFromFile("resources/Images/particleBlu.png");
	sf::RectangleShape character(sf::Vector2f(characterWidth, characterHeight));
	character.setTexture(&particleTexture);
	character.setPosition(sf::Vector2f(initialX, initialY));
	sf::Vector2f curPos;
	int centerX;
	int centerY;
	float characterAngle = 0.f;
	sf::Transform characterTransform;


	//Lets create a lightning effect

	sf::Texture particleTexture2;
	particleTexture2.loadFromFile("resources/Images/lightning3.png");
	float lSize = 50.0;
	sf::RectangleShape bigLightning(sf::Vector2f(lSize, lSize));
	bigLightning.setTexture(&particleTexture2);
	multiVector lightningVector;
	std::vector<sf::Vector2f> v;
	for (int i = 0; i < 10; i++)
	{
		lightningVector.push_back(v);
	}

	sf::VertexArray points(sf::Points, 1000);

	//Lets create a fire effect
	float fOriginalSize = 7.f;
	sf::Texture particleTexture3;
	particleTexture3.loadFromFile("resources/Images/burn4.png");
	sf::RectangleShape fireParticle(sf::Vector2f(fOriginalSize, fOriginalSize));
	fireParticle.setTexture(&particleTexture3);

	sf::CircleShape fuelParticle;
	fuelParticle.setFillColor(sf::Color::Cyan);
	fuelParticle.setRadius(1.f);
	fuelParticle.setOrigin(sf::Vector2f(1.f, 1.f));
	std::vector<struct Fuel> fuelVector;
	const auto f_ptr = &fuelVector;
	fuelVector.reserve(FLAME_MAX);
	std::vector<struct Fuel> fuelSpreadVector;
	fuelSpreadVector.reserve(FLAME_MAX / 5);
	std::vector<struct Smoke> smokeVector;
	smokeVector.reserve(FLAME_MAX);

	float rotate = 0;
	float multiplier = 0.999;
	float fGravity = 1.0;

	bool LMB = false; // is left mouse button hit ?
	bool Left = false;
	bool Right = false;
	bool Up = false;
	bool Down = false;
	bool Z = false;
	bool ZUp = false;
	bool S = false;
	bool Space = false;

	bool JumpInitiated = false;
	bool LightningLaunched = false;
	bool FlameLaunched = false;
	int shootingTime;
	float scale = 800.f / 3.f;

	sf::Clock jumpTimer;
	sf::Clock lightningTimer;
	sf::Clock flameTimer;
	sf::Clock flameEvaporateTimer;
	flameEvaporateTimer.restart();
	sf::Clock lightningSizeTimer;
	lightningSizeTimer.restart();
	sf::Clock clock;


	sf::Clock flameDelay;
	sf::Clock deltaTime; // SFML clock to keep track of our framerate
	float dt = 0; // the time a frame requires to be drawn (=1/framerate). I set it at 0.0000001 to avoid getting undefined behaviors during the very first frame

	while (window.isOpen())
	{

		sf::Event event;
		while (window.pollEvent(event))
		{

			// Request for closing the window
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::KeyPressed)
			{

				if (event.key.code == sf::Keyboard::Space)
				{
				}
			}
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				LMB = true;
			}
			else
			{
				LMB = false;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			{
				Right = true;
			}
			else
			{
				Right = false;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			{
				Down = true;
			}
			else
			{
				Down = false;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			{
				Left = true;
			}
			else
			{
				Left = false;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			{
				Up = true;
			}
			else
			{
				Up = false;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
			{
				Z = true;
			}
			else
			{
				Z = false;
				ZUp = true;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			{
				S = true;
			}
			else
			{
				S = false;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
			{
				Space = true;
			}
			else
			{
				Space = false;
			}
		}
		curPos = character.getPosition();
		centerX = curPos.x + characterWidth / 2;
		centerY = curPos.y + characterHeight / 2;


		if (Right)
		{
			if (!Space)
			{
				if (character.getPosition().x < width - characterWidth)
				{

					sf::Vector2f newPos = sf::Vector2f(curPos.x + 1.f, curPos.y);
					character.setPosition(newPos);
				}
			}
			else
			{
				characterAngle += 0.01;
			}
		}
		if (Left)
		{
			if (!Space)
			{
				if (character.getPosition().x > 0)
				{
					sf::Vector2f newPos = sf::Vector2f(curPos.x - 1.f, curPos.y);
					character.setPosition(newPos);
				}
			}
			else
			{
				characterAngle -= 0.01;
			}
		}
		if (!Space)
		{
			if (Up && !JumpInitiated)
			{
				JumpInitiated = true;
				jumpTimer.restart();
			}
		}


		if (JumpInitiated)
		{
			float t = jumpTimer.getElapsedTime().asMilliseconds();
			float jumpHeight = jump(t, jumpVelocity, scale);
			sf::Vector2f curPos = character.getPosition();
			if (jumpHeight < 0 && t > 0)
			{
				JumpInitiated = false;
			}
			else
			{
				character.setPosition(curPos.x, initialY - jumpHeight);
			}
		}
		if (Z)
		{
			LightningLaunched = true;
			lightningTimer.restart();
			if (ZUp)
			{
				for (auto& v : lightningVector)
				{

					if (v.empty())
					{
						ZUp = false;
						v.push_back(sf::Vector2f(curPos.x + characterWidth / 2.f, curPos.y + characterHeight / 2.f));
						break;
					}
				}
			}
		}
		if (S)
		{
			FlameLaunched = true;
			flameTimer.restart();

			int x = 0;
			int y = 0;
			if (Left)
				x -= 5;
			if (Right)
				x += 5;
			if (Up)
				y -= 5;
			if (Down)
				y += 5;

			y += rand() % 2 - 1;
			x += rand() % 2 - 1;
			if (flameDelay.getElapsedTime().asMicroseconds() > 2)
			{
				flameDelay.restart();
				for (int i = 0; i < 3; i++)
				{
					sf::Vector2f direction = sf::Vector2f(std::cos(characterAngle + 0.1 - i * 0.1) * 5, std::sin(characterAngle + 0.1 - i * 0.1) * 5);
					fuelVector.emplace_back(sf::Vector2f(centerX, centerY) + direction, direction);
				}
			}
		}

		if (LightningLaunched)
		{
			for (auto& v : lightningVector)
			{
				if (!v.empty())
				{
					sf::Vector2f addition = v.back();
					int x = rndm() - 10;
					int y = rndm() - 10;
					if (Left)
						x -= 5;
					if (Right)
						x += 5;
					if (Up)
						y -= 5;
					if (Down)
						y += 5;
					addition += sf::Vector2f(x, y);

					v.push_back(addition);

					if (addition.x > width || addition.x < 0 || addition.y > height || addition.y < 0)
					{
						shootingTime = lightningTimer.getElapsedTime().asMilliseconds();
						v.clear();
					}
					if (v.size() > 70)
					{
						v.erase(v.begin());
					}
				}
			}
		}

		window.clear();

		if (LightningLaunched)
		{

			for (auto& v : lightningVector)
			{
				if (!v.empty())
				{
					if ((Left || Right || Up || Down) && lightningTimer.getElapsedTime().asMilliseconds() > 50)
					{
						lightningSizeTimer.restart();
						lSize = rand() % 120;
					}
					else if (lightningSizeTimer.getElapsedTime().asMilliseconds() > 60)
					{
						lightningSizeTimer.restart();
						lSize = rand() % 80 + 20;
					}
					bigLightning.setSize(sf::Vector2f(lSize, lSize));
					for (auto& posVect : v)
					{
						bigLightning.setPosition(sf::Vector2f(float(posVect.x) - lSize / 2.f, float(posVect.y) - lSize / 2.f));
						window.draw(bigLightning);
					}
				}
			}
		}

		for (auto& f : fuelVector)
		{
			if (f.ignition < 1.0)
			{
				sf::Transform t;
				t.translate(f.pos.x, f.pos.y);

				f.pos += sf::Vector2f(f.dir.x * f.force * 0.5, f.dir.y * f.force * 0.5);

				f.ignition += 0.1;
				f.force *= f.forceMultiplier;
				f.forceMultiplier *= 0.9999;
				fuelParticle.setFillColor(sf::Color(209, 233, 255, 150));
				t.scale(2, 2);
				window.draw(fuelParticle, t);
			}
			else
			{
				sf::Transform t;
				f.spread += 0.03;
				if (f.spread > 1.0)
				{
					f.spread = 0.0;
					float offset = (rand() % 3 - 1) * 0.1;
					fuelSpreadVector.emplace_back(f.pos, sf::Vector2f(std::cos(std::acos(f.dir.x) + 10.1), std::sin(std::asin(f.dir.y) + 10.1)), 0.5, 0.0, 1.0, f.force, f.forceMultiplier);
				}
				t.translate(f.pos.x, f.pos.y);
				f.pos += sf::Vector2f(f.dir.x * f.force * 0.5, f.dir.y * f.force * 0.5);
				f.force *= f.forceMultiplier;
				f.forceMultiplier *= 0.9999;
				f.material -= 0.01;
				auto n = numberOfNeighbors(&f.pos, &fuelVector) - 1;
				fuelParticle.setFillColor(sf::Color(242, 200 - (20 * n), 0));
				float r = n * f.material / 2.f;
				t.scale(3, 3);
				window.draw(fuelParticle, t);
			}
		}
		fuelVector.insert(fuelVector.end(), fuelSpreadVector.begin(), fuelSpreadVector.end());
		fuelSpreadVector.clear();

		for (auto& s : smokeVector)
		{
			sf::Transform t;
			t.translate(s.pos.x, s.pos.y);
			s.pos += sf::Vector2f(0.f, -0.25f);
			s.radius -= 0.01;
			fuelParticle.setFillColor(sf::Color(50, 50, 50, 200));
			t.scale(s.radius, s.radius);
			window.draw(fuelParticle, t);
		}


		fuelVector.erase(
			std::remove_if(fuelVector.begin(), fuelVector.end(), hasNoMaterial),
			fuelVector.end());

		smokeVector.erase(
			std::remove_if(smokeVector.begin(), smokeVector.end(), radiusBelowZero),
			smokeVector.end());


		window.draw(character);

		window.display();


		dt = deltaTime.restart().asMilliseconds(); // measure the time it took to complete all the calculations for this particular frame (dt=1/framerate) 
	}
	return 0;
}