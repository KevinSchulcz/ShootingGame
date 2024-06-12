#include "constants.h"
#include "game.h"
#include <iostream>
#include <fstream>
#include <time.h>

game::game(const std::string& config)
{
	init(config);
}

void game::init(const std::string& path)
{
	// temp var
	std::string tmpStr      = "";

	// input configs into structs
	std::ifstream fin(path);
	// read in window line 
	fin >> tmpStr >> m_wWidthInput >> m_wHeightInput >> m_frameRateLimitInput;
	// read in player line
	fin >> tmpStr >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S
		>> m_playerConfig.FR >> m_playerConfig.FG >> m_playerConfig.FB
		>> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB
		>> m_playerConfig.OT >> m_playerConfig.V;
	// read in enemy line
	fin >> tmpStr >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SMIN
		>> m_enemyConfig.SMAX >> m_enemyConfig.OR >> m_enemyConfig.OG
		>> m_enemyConfig.OB >> m_enemyConfig.OT >> m_enemyConfig.VMIN
		>> m_enemyConfig.VMAX >> m_enemyConfig.L >> m_enemyConfig.SI;
	// read in bullet line
	fin >> tmpStr >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.S
		>> m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB
		>> m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB
		>> m_bulletConfig.OT >> m_bulletConfig.V >> m_bulletConfig.L;

	// window setup
	m_window.create(sf::VideoMode(m_wWidthInput, m_wHeightInput), "Shooting Game");
	m_window.setFramerateLimit(m_frameRateLimitInput);

	// text aint working
	// draw() occurs in sRender()
/*
	// text setup
	sf::Font myFont;
	if (!myFont.loadFromFile("font.ttf"))
	{
		std::cerr << "Couldn't load font\n";
		exit(-1);
	}
	m_text.setString("sCORE:\nHIGHSCORE");
	m_text.setFont(myFont);
	m_text.setCharacterSize(30);
	m_text.setPosition(0, 0 + (float)m_text.getCharacterSize()* 2);
	m_text.setFillColor(sf::Color::White);
*/
	spawnPlayer();
}

void game::run()
{
	while (m_running)
	{
		m_entities.update();

		if (!m_paused)
		{
			if (!m_player->isAlive())
			{
				spawnPlayer();
			}
			sEnemySpawner();
			sMovement();
			sCollision();
			sLifespan();

			m_currentFrame++;
		}
		sUserInput();
		sRender();
	}

	m_window.close();
}

void game::setPaused()
{
	m_paused = !m_paused;
}

void game::spawnPlayer()
{
	// Temp solution
	std::cout << "Score: " << m_score << "\n";
	std::cout << "Highscore: " << m_highScore << "\n";
	//

	// have the EM create player
	auto entity = m_entities.addEntity(TAG_PLAYER);

	// spawn in the middle, with no velocity and angle 0
	entity->cTransform = std::make_shared<cTransform>(
		vec2(m_window.getSize().x / 2.0f, m_window.getSize().y / 2.0f),
		vec2(0.0f, 0.0f), 0.0f);

	// set shape from structs
	entity->cShape = std::make_shared<cShape>(m_playerConfig.SR, m_playerConfig.V
		, sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB)
		, sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB)
		, m_playerConfig.OT);

	entity->cCollision = std::make_shared<cCollision>(m_playerConfig.CR);

	// allows this entity (player) to get the inputs
	entity->cInput = std::make_shared<cInput>();

	// little against the EM architecture but worth for this game
	m_player = entity;
}

void game::spawnEnemy()
{
	srand(time(NULL));

	auto entity = m_entities.addEntity(TAG_ENEMY);

	// get random point for enemy to spawn 
	float x = (rand() % (m_window.getSize().x - 2 * m_enemyConfig.SR)) + (2 * m_enemyConfig.SR);
	float y = (rand() % (m_window.getSize().y - 2 * m_enemyConfig.SR)) + (2 * m_enemyConfig.SR);
	// get random speed for enemy 
	float vx = (rand() % (int)m_enemyConfig.SMAX) + m_enemyConfig.SMIN;
	float vy = (rand() % (int)m_enemyConfig.SMAX) + m_enemyConfig.SMIN;
	// get random verticies for enemy
	int vertices = (rand() % m_enemyConfig.VMAX) + m_enemyConfig.VMIN;

	// spawn at random place, w random speed and angle 0
	entity->cTransform = std::make_shared<cTransform>(vec2(x, y), vec2(vx, vy), 0.0f);

	// set shape from structs
	entity->cShape = std::make_shared<cShape>(m_enemyConfig.SR, vertices
		, sf::Color::Black
		, sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB)
		, m_enemyConfig.OT);

	entity->cCollision = std::make_shared<cCollision>(m_enemyConfig.CR);

	m_lastEnemySpawnTime = m_currentFrame;
}

void game::spawnSmallEnemies(std::shared_ptr<entity> e)
{
	float currAngle;
	float angleInterval = 360.0f / e->cShape->circle.getPointCount();

	// NOTE: not intended spread of angles, but cool so kept
	for (int i = 0; i < e->cShape->circle.getPointCount(); i++)
	{
		currAngle = angleInterval * i;
		auto smalle = m_entities.addEntity(TAG_SMALLENEMY);
		smalle->cTransform = std::make_shared<cTransform>(
			vec2(e->cTransform->position.x, e->cTransform->position.y)
			, vec2(e->cTransform->velocity.x * cos(currAngle), e->cTransform->velocity.y * sin(currAngle))
			, currAngle);
		smalle->cShape = std::make_shared<cShape>(
			e->cShape->circle.getRadius() / 2, e->cShape->circle.getPointCount()
			, sf::Color::Black, e->cShape->circle.getOutlineColor()
			, e->cShape->circle.getOutlineThickness());
		smalle->cCollision = std::make_shared<cCollision>(e->cCollision->radius);
		smalle->cLifespan = std::make_shared<cLifespan>(60);
	}
}

void game::spawnBullet(std::shared_ptr<entity> e, const vec2& target)
{
	// create bullet
	auto bullet = m_entities.addEntity(TAG_BULLET);

	// calculate bullet velocity vector
	vec2 differenceVector = target - m_player->cTransform->position;
	float differenceVectorLength = vec2(0.0f, 0.0f).dist(differenceVector);
	vec2 normalizedVector = differenceVector / differenceVectorLength;

	// initalize components
	bullet->cTransform = std::make_shared<cTransform>(m_player->cTransform->position
		, (normalizedVector * m_bulletConfig.S), 0.0f);
	bullet->cShape = std::make_shared<cShape>(m_bulletConfig.SR, m_bulletConfig.V
		, sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB)
		, sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB)
		, m_bulletConfig.OT);
	bullet->cCollision = std::make_shared<cCollision>(m_bulletConfig.CR);
	bullet->cLifespan = std::make_shared<cLifespan>(m_bulletConfig.L);
}

void game::sLifespan()
{
	// go over both entity types that have lifespans
	//	check if lifespan is over plus reduce opacity
	//	NOTE: missing functionality of fading away

	for (auto& e : m_entities.getEntities(TAG_SMALLENEMY))
	{
		if (e->cLifespan->remaining <= 0)
			e->destroy();
		e->cLifespan->remaining--;
		
	}
	for (auto& e : m_entities.getEntities(TAG_BULLET))
	{
		if (e->cLifespan->remaining <= 0)
			e->destroy();
		e->cLifespan->remaining--;
	}
}

void game::sCollision()
{
	// check collisions for bullets - enemies
	for (auto b : m_entities.getEntities(TAG_BULLET))
	{
		for (auto enemy : m_entities.getEntities(TAG_ENEMY))
		{
			if (b->cTransform->position.distSqrd(enemy->cTransform->position) <=
				(b->cCollision->radius + enemy->cCollision->radius) * (b->cCollision->radius + enemy->cCollision->radius))
			{
				m_score += enemy->cShape->circle.getPointCount();
				spawnSmallEnemies(enemy);
				b->destroy();
				enemy->destroy();
			}
		}
	}
	// check for bullets - smallEnemies
	for (auto b : m_entities.getEntities(TAG_BULLET))
	{
		for (auto enemy : m_entities.getEntities(TAG_SMALLENEMY))
		{
			if (b->cTransform->position.distSqrd(enemy->cTransform->position) <=
				(b->cCollision->radius + enemy->cCollision->radius) * (b->cCollision->radius + enemy->cCollision->radius))
			{
				m_score += enemy->cShape->circle.getPointCount();
				b->destroy();
				enemy->destroy();
			}
		}
	}
	// check for player - enemies (small enemies don't kill player)
	for (auto enemy : m_entities.getEntities(TAG_ENEMY))
	{
		if (m_player->cTransform->position.distSqrd(enemy->cTransform->position) <=
			(m_player->cCollision->radius + enemy->cCollision->radius) * (m_player->cCollision->radius + enemy->cCollision->radius))
		{
			if (m_score > m_highScore)
				m_highScore = m_score;
			m_player->destroy();
			enemy->destroy();
		}
	}
}

void game::sEnemySpawner()
{
	if (m_currentFrame - m_lastEnemySpawnTime >= m_enemyConfig.SI)
		spawnEnemy();
}

void game::sMovement()
{
	// set velocity to 0,0 (reset if nothing pressed)
	m_player->cTransform->velocity = { 0, 0 };

	if (m_player->cInput->up)
		m_player->cTransform->velocity.y = -m_playerConfig.S;
	if (m_player->cInput->down)
		m_player->cTransform->velocity.y = m_playerConfig.S;
	if (m_player->cInput->left)
		m_player->cTransform->velocity.x = -m_playerConfig.S;
	if (m_player->cInput->right)
		m_player->cTransform->velocity.x = m_playerConfig.S;

	// update movement for player
	if (m_player->cTransform->position.x + m_player->cTransform->velocity.x <= 0 + m_player->cShape->circle.getRadius() ||
		m_player->cTransform->position.x + m_player->cTransform->velocity.x >= m_wWidthInput - m_player->cShape->circle.getRadius())
	{}
	else
		m_player->cTransform->position.x += m_player->cTransform->velocity.x;
	if (m_player->cTransform->position.y + m_player->cTransform->velocity.y <= 0 + m_player->cShape->circle.getRadius() ||
		m_player->cTransform->position.y + m_player->cTransform->velocity.y >= m_wHeightInput - m_player->cShape->circle.getRadius())
	{}
	else
		m_player->cTransform->position.y += m_player->cTransform->velocity.y;

	// update enemy movement
	//	BUG: if enemy gets past boundary by enough
	//		will end up loop switching directions 
	//		every frame and get stuck
	for (auto e : m_entities.getEntities(TAG_ENEMY))
	{
		if (e->cTransform->position.x + e->cTransform->velocity.x <= 0 + e->cShape->circle.getRadius() ||
			e->cTransform->position.x + e->cTransform->velocity.x >= m_wWidthInput - e->cShape->circle.getRadius())
		{
			e->cTransform->velocity.x = -(e->cTransform->velocity.x);
		}
		if (e->cTransform->position.y + e->cTransform->velocity.y <= 0 + e->cShape->circle.getRadius() ||
			e->cTransform->position.y + e->cTransform->velocity.y >= m_wHeightInput - e->cShape->circle.getRadius())
		{
			e->cTransform->velocity.y = -(e->cTransform->velocity.y);
		}

		e->cTransform->position.x += e->cTransform->velocity.x;
		e->cTransform->position.y += e->cTransform->velocity.y;
	}

	// update bullet and small enemy movement
	for (auto e : m_entities.getEntities(TAG_BULLET))
	{
		e->cTransform->position.x += e->cTransform->velocity.x;
		e->cTransform->position.y += e->cTransform->velocity.y;
	}
	for (auto e : m_entities.getEntities(TAG_SMALLENEMY))
	{
		e->cTransform->position.x += e->cTransform->velocity.x;
		e->cTransform->position.y += e->cTransform->velocity.y;
	}
}

void game::sRender()
{
	m_window.clear();

	for (auto e : m_entities.getEntities())
	{
		// set to draw in new position
		e->cShape->circle.setPosition(e->cTransform->position.x, e->cTransform->position.y);
		// set new angle (for rotate effect)
		e->cTransform->angle += 1.0f;
		e->cShape->circle.setRotation(e->cTransform->angle);
		// draw entity
		m_window.draw(e->cShape->circle);
	}

	// draw text (aint workin)
	// m_window.draw(m_text);

	m_window.display();
}

void game::sUserInput()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			m_running = false;

		// key pressesed
		if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				std::cout << "W key pressed\n";
				m_player->cInput->up = true;
				break;
			case sf::Keyboard::A:
				std::cout << "A key pressed\n";
				m_player->cInput->left = true;
				break;
			case sf::Keyboard::S:
				std::cout << "S key pressed\n";
				m_player->cInput->down = true;
				break;
			case sf::Keyboard::D:
				std::cout << "D key pressed\n";
				m_player->cInput->right = true;
				break;
			case sf::Keyboard::Escape:
				std::cout << "Esc key pressed\n";
				setPaused();
				break;
			default:
				break;
			}
		}

		// key released
		if (event.type == sf::Event::KeyReleased)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				std::cout << "W key released\n";
				m_player->cInput->up = false;
				break;
			case sf::Keyboard::A:
				std::cout << "A key released\n";
				m_player->cInput->left = false;
				break;
			case sf::Keyboard::S:
				std::cout << "S key released\n";
				m_player->cInput->down = false;
				break;
			case sf::Keyboard::D:
				std::cout << "D key released\n";
				m_player->cInput->right = false;
				break;
			default:
				break;
			}
		}

		// mouse pressed
		if (event.type == sf::Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				std::cout << "Left mouse pressed at (" << event.mouseButton.x 
					<< "," << event.mouseButton.y << ")\n";
				spawnBullet(m_player, vec2(event.mouseButton.x, event.mouseButton.y));
			}
		}
	}
}