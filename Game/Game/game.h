#pragma once

#include "entity.h"
#include "entityManager.h"
#include <SFML/Graphics.hpp>

struct playerConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float S; };
struct enemyConfig  { int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI; float SMIN, SMAX; };
struct bulletConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float S; };

// player SR CR S FR FG FB OR OG OB OT V
// enemy SR CR SMIN SMAX OR OG OB OT VMIN VMAX L SI
// bullet SR CR S FR FG FB OR OG OB OT V L

class game
{
	sf::RenderWindow        m_window;
	entityManager           m_entities;
	sf::Font                m_font;
	sf::Text                m_text;
	sf::Text                m_scoreText;
	sf::Text                m_highScoreText;
	playerConfig            m_playerConfig;
	enemyConfig             m_enemyConfig;
	bulletConfig            m_bulletConfig;
	int                     m_score = 0;
	int                     m_highScore = 0;
	int                     m_currentFrame = 0;
	int                     m_lastEnemySpawnTime = 0;
	bool                    m_paused = false;
	bool                    m_running = true;
	std::shared_ptr<entity> m_player;
	int                     m_wWidthInput = 0;
	int                     m_wHeightInput = 0;
	int                     m_frameRateLimitInput = 0;

	// util functions
	void init(const std::string& config);
	void setPaused();

	// systems
	void sMovement();
	void sUserInput();
	void sLifespan();
	void sRender();
	void sEnemySpawner();
	void sCollision();

	// spawning
	void spawnPlayer();
	void spawnEnemy();
	void spawnSmallEnemies(std::shared_ptr<entity> entity);
	void spawnBullet(std::shared_ptr<entity> entity, const vec2& mousePos);

public:
	game(const std::string& config);
	void run();
};