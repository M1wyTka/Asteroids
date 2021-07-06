#include "Engine.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <time.h>
#include <cmath>
#include <vector>
#include <map>
#include <algorithm>
#include <string>


//
//  You are free to modify this file
//

//  is_key_pressed(int button_vk_code) - check if a key is pressed,
//                                       use keycodes (VK_SPACE, VK_RIGHT, VK_LEFT, VK_UP, VK_DOWN, 'A', 'B')
//
//  get_cursor_x(), get_cursor_y() - get mouse cursor position
//  is_mouse_button_pressed(int button) - check if mouse button is pressed (0 - left button, 1 - right button)
//  clear_buffer() - set all pixels in buffer to 'black'
//  is_window_active() - returns true if window is active
//  schedule_quit_game() - quit game after act()

#define PI 3.14159265

struct Vec2 
{
	float x, y;

	Vec2() : x(0), y(0) {}

	Vec2(float x, float y) : x(x), y(y) {}

	float Length() const
	{
		return sqrt(x*x + y*y);
	}

	Vec2 operator+(const Vec2& other) const
	{
		return Vec2(x + other.x, y + other.y);
	}

	Vec2 operator-(const Vec2& other) const
	{
		return Vec2(x - other.x, y - other.y);
	}

	Vec2 operator*(const float& other) const
	{
		return Vec2(x*other, y*other);
	}

	Vec2& operator+=(const Vec2& other)
	{
		this->x += other.x;
		this->y += other.y;
		return *this;
	}

};

class Flying_Object
{
protected:
	Vec2 m_Pos;
	Vec2 m_Speed;
	int m_Size;
	float m_Angle;
	std::vector<Vec2> m_Model;

public: 
	Flying_Object(const Vec2& Pos, const Vec2& Speed, const int Size, const float Angle, const std::vector<Vec2>& Model) :
		m_Pos(Pos), m_Speed(Speed), m_Size(Size), m_Angle(Angle), m_Model(Model) {}

	Flying_Object() : m_Pos(Vec2()), m_Speed(Vec2()), m_Size(1), m_Angle(0.0f), m_Model({ Vec2(0.0f, 0.0f) }) {}

	Flying_Object(const Vec2& Pos, const Vec2& Speed, const int Size, const float Angle) : m_Pos(Pos), m_Speed(Speed), m_Size(Size), m_Angle(Angle), m_Model({ Vec2(0.0f, 0.0f) }) {}

	const Vec2 GetPosition() const
	{
		return m_Pos;
	}

	void SetPosition(const Vec2& newPos)
	{
		m_Pos = newPos;
	}

	int GetSize() const
	{
		return m_Size;
	}

	void SetSpeed(const Vec2& Speed)
	{
		m_Speed = Speed;
	}

	Vec2 GetSpeed() const 
	{
		return m_Speed;
	}

	void AddAngle(float Angle)
	{
		m_Angle += Angle;
	}

	float GetAngle() const
	{
		return m_Angle;
	}

	Vec2 UpdatePosition(float dt) 
	{	
		Vec2 newPos = m_Pos + m_Speed * dt;
		m_Pos = newPos;

		return newPos;
	}

	std::vector<Vec2> GetModel()
	{
		return m_Model;
	}
};


class Shuttle : public Flying_Object
{
private:
	float m_Acceleration;
	float m_SpeedLimit;

public:
	Shuttle() : Flying_Object(), m_Acceleration(50.0f), m_SpeedLimit(200) {}

	Shuttle(const Vec2& Pos, const Vec2& Speed, const int Size, const float Angle, const float Acceleration):
		Flying_Object(Pos, Speed, Size, Angle, { Vec2(-1.0f, 1.0f), Vec2(0.0f, -1.0f), Vec2(1.0f, 1.0f) }), m_Acceleration(Acceleration), m_SpeedLimit(100) {}

	void SetAcceleration(const float Acceleration)
	{
		m_Acceleration = Acceleration;
	}

	float GetAcceleration() const 
	{
		return m_Acceleration;
	}

	void UpdateSpeed(float dt)
	{
		AddSpeed(Vec2(sinf(m_Angle*PI/180), -cosf(m_Angle*PI / 180)) * m_Acceleration * dt);
	}

	void AddSpeed(const Vec2& Speed) 
	{	
		Vec2 newSpeed = m_Speed + Speed;

		if (newSpeed.Length() > m_SpeedLimit)
			return;

		m_Speed = newSpeed;
	}
};


class Gamefield
{
public:
	uint32_t const m_Obstacle_Color = 0xFF0000FF;
	uint32_t const m_Player_Color = 0x0000FFFF;
	uint32_t const m_Invincibility_Color = 0xFFFF00FF;
	uint32_t const m_Bullet_Color = 0x00FF00FF;
	uint32_t const m_Text_Color = 0x0FFFF0FF;

private:
	uint32_t* const m_Board;
	std::map<char, std::vector<Vec2>> PixelNumbers = { {'0', { Vec2(0.0f, 0.0f), Vec2(1.0f, 0.0f), Vec2(1.0f, 2.0f), Vec2(0.0f, 2.0f), Vec2(0.0f, 0.0f) } },
													   {'1', { Vec2(1.0f, 0.0f), Vec2(1.0f, 2.0f) } },
													   {'2', { Vec2(0.0f, 0.0f), Vec2(1.0f, 0.0f), Vec2(1.0f, 1.0f), Vec2(0.0f, 2.0f), Vec2(1.0f, 2.0f) } },
													   {'3', { Vec2(0.0f, 0.0f), Vec2(1.0f, 0.0f), Vec2(1.0f, 1.0f), Vec2(0.0f, 1.0f), Vec2(1.0f, 1.0f), Vec2(1.0f, 2.0f), Vec2(0.0f, 2.0f) } },
													   {'4', { Vec2(0.0f, 0.0f), Vec2(0.0f, 1.0f), Vec2(1.0f, 1.0f), Vec2(1.0f, 0.0f), Vec2(1.0f, 2.0f) } },
													   {'5', { Vec2(1.0f, 0.0f), Vec2(0.0f, 0.0f), Vec2(0.0f, 1.0f), Vec2(1.0f, 1.0f), Vec2(1.0f, 2.0f), Vec2(0.0f, 2.0f) } },
													   {'6', { Vec2(0.0f, 0.0f), Vec2(0.0f, 2.0f), Vec2(1.0f, 2.0f), Vec2(1.0f, 1.0f), Vec2(0.0f, 1.0f) } },
													   {'7', { Vec2(0.0f, 0.0f), Vec2(1.0f, 0.0f), Vec2(0.0f, 2.0f) } },
													   {'8', { Vec2(0.0f, 0.0f), Vec2(0.0f, 2.0f), Vec2(1.0f, 2.0f), Vec2(1.0f, 0.0f), Vec2(0.0f, 0.0f), Vec2(0.0f, 1.0f), Vec2(1.0f, 1.0f) } },
													   {'9', { Vec2(0.0f, 2.0f), Vec2(1.0f, 2.0f), Vec2(1.0f, 0.0f), Vec2(0.0f, 0.0f), Vec2(0.0f, 1.0f), Vec2(1.0f, 1.0f) } } };

public:
	Gamefield(uint32_t* board) : m_Board(board)
	{}

	void DrawText(std::string Text, const Vec2& offset) 
	{
		Vec2 step_offset = Vec2(15.0f, 0.0f);

		for (int i = 0; i < Text.length(); i++)
		{
			std::vector<Vec2> OffsetModel = PixelNumbers[Text[i]];
			for (int j = 0; j < OffsetModel.size(); j++)
			{
				OffsetModel[j] = OffsetModel[j] * 10;
				OffsetModel[j] += step_offset * i + offset;
			}

			DrawFigure(OffsetModel, m_Text_Color);
		}
	}

	void DrawFlyingObjects(std::vector<Flying_Object>& F_Objs, uint32_t Color)
	{
		for (std::vector<Flying_Object>::iterator it = F_Objs.begin(); it != F_Objs.end(); ++it)
		{
			DrawFlyingObject(*it, Color);
		}
	}

	void DrawFlyingObject(Flying_Object& F_Obj, uint32_t Color)
	{
		std::vector<Vec2> BoardModel = F_Obj.GetModel();
		for (int i = 0; i < BoardModel.size(); i++)
		{
			AdjustByDimensions(BoardModel[i], F_Obj.GetPosition(), F_Obj.GetAngle(), F_Obj.GetSize());
		}

		DrawPolygon(&BoardModel, Color);
	}

	void Clear() 
	{
		memset(buffer, 0, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(uint32_t));
	}

private:
	void AdjustByDimensions(Vec2& vec, const Vec2& offset, float angle, int size)
	{
		float x = vec.x * size;
		float y = vec.y * size;
		float CosF = cosf(angle*PI / 180);
		float SinF = sinf(angle*PI / 180);
		vec.x = x * CosF - y * SinF;
		vec.y = x * SinF + y * CosF;
		vec += offset;
	}

	void DrawFigure(const std::vector<Vec2>& Model, uint32_t Color)
	{
		if (Model.size() <= 0)
			return;

		if (Model.size() == 1)
			DrawPoint(Model[0].x, Model[0].y, Color);

		for (int i = 0; i < Model.size() - 1; i++)
		{
			DrawLine(Model[i].x, Model[i].y, Model[i + 1].x, Model[i + 1].y, Color);
		}
	}

	void DrawPolygon(const std::vector<Vec2>* Model, uint32_t Color)
	{
		if (Model->size() <= 0)
			return;

		if (Model->size() == 1)
			DrawPoint((*Model)[0].x, (*Model)[0].y, Color);

		for (int i = 0; i < Model->size(); i++)
		{
			if (i == Model->size() - 1)
				DrawLine((*Model)[i].x, (*Model)[i].y, (*Model)[0].x, (*Model)[0].y, Color);
			else
				DrawLine((*Model)[i].x, (*Model)[i].y, (*Model)[i + 1].x, (*Model)[i + 1].y, Color);
		}
	}

	void DrawLine(int x0, int y0, int x1, int y1, uint32_t Color)
	{
		if (x0 == x1)
		{
			if (y1 >= y0)
				for (int i = y0; i < y1; i++)
					DrawPoint(x0, i, Color);
			else
				for (int i = y0; i > y1; i--)
					DrawPoint(x0, i, Color);
			return;
		}

		float k = float(y1 - y0) / (x1 - x0);
		float b = y1 - x1 * k;
		float last_y;
		if (x1 > x0)
		{
			last_y = y0;
			for (int i = x0; i < x1 + 1; i++)
			{
				float y = round(k*i + b);
				if (y > last_y)
					for (float yy = last_y; yy < y + 1; yy++)
						DrawPoint(i, yy, Color);
				else
					for (float yy = y; yy < last_y + 1; yy++)
						DrawPoint(i, yy, Color);
				last_y = y;
			}
		}

		else
		{
			last_y = y1;
			for (int i = x1; i < x0 + 1; i++)
			{
				float y = round(k*i + b);
				if (y > last_y)
					for (float yy = last_y; yy < y + 1; yy++)
						DrawPoint(i, yy, Color);
				else
					for (float yy = y; yy < last_y + 1; yy++)
						DrawPoint(i, yy, Color);
				last_y = y;
			}
		}
	}

	void DrawPoint(int x, int y, uint32_t Color)
	{
		float fx, fy;
		LoopCoordinates(x, y, fx, fy);
		*(m_Board + int(fy * SCREEN_WIDTH) + int(fx)) = Color;
	}

	void LoopCoordinates(float in_x, float in_y, float& out_x, float& out_y)
	{
		out_x = in_x;
		out_y = in_y;
		if (in_x < 0) out_x = in_x + SCREEN_WIDTH;
		if (in_x >= SCREEN_WIDTH) out_x = in_x - SCREEN_WIDTH;
		if (in_y < 0) out_y = in_y + SCREEN_HEIGHT;
		if (in_y >= SCREEN_HEIGHT) out_y = in_y - SCREEN_HEIGHT;
	}
};


class GameManager 
{
private:
	Gamefield m_GameBoard;
	int Score;
	
	Shuttle m_Player;
	int Health;

	float ShootCD;
	float ShootTimer;

	float InvicibilityTimeOnHit;
	bool isInvincible;
	float InvincibilityTimer;

	std::vector<Flying_Object> m_Asteroids;
	std::vector<Flying_Object> m_Bullets;

	const std::vector<Vec2> m_Asteroid_Model = { Vec2(-1.0f, -1.0f), Vec2(-1.0f, 1.0f), Vec2(1.0f, 1.0f), Vec2(1.0f, -1.0f) };
	const std::vector<Vec2> m_Bullet_Model = { Vec2(-1.0f, -1.0f), Vec2(-1.0f, 1.0f), Vec2(1.0f, 1.0f), Vec2(1.0f, -1.0f) };

public:
	GameManager(uint32_t* board) : m_GameBoard(Gamefield(board))
	{
		Score = 0;
		
		m_Player = Shuttle(Vec2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2), Vec2(), 10, 0.0f, 50);
		Health = 5;
		isInvincible = false;

		ShootCD = 1.0f;
		InvicibilityTimeOnHit = 3.0f;
		ShootTimer = 0.0f;
		
		srand(time(0));

		SpawnAsteroidField();
	}

	void ReadInputs(float dt) 
	{
		if (is_key_pressed(VK_LEFT))
			m_Player.AddAngle(-90 * dt);

		if (is_key_pressed(VK_RIGHT))
			m_Player.AddAngle(90 * dt);

		if (is_key_pressed(VK_UP))
		{
			m_Player.UpdateSpeed(dt);
		}

		if (is_key_pressed(VK_SPACE) && ShootTimer > ShootCD) 
		{
			SpawnBullet();
			ShootTimer = 0;
		}
		ShootTimer += dt;
	}

	void UpdateGame(float dt)
	{
		ReadInputs(dt);

		UpdatePlayerPosition(dt);
		UpdateBulletPositions(dt);
		UpdateAsteroidPositions(dt);

		CheckInteractions(dt);
	}

	void DrawGame() 
	{
		if(isInvincible)
			m_GameBoard.DrawFlyingObject(m_Player, m_GameBoard.m_Invincibility_Color);
		else
			m_GameBoard.DrawFlyingObject(m_Player, m_GameBoard.m_Player_Color);

		m_GameBoard.DrawFlyingObjects(m_Bullets, m_GameBoard.m_Bullet_Color);
		m_GameBoard.DrawFlyingObjects(m_Asteroids, m_GameBoard.m_Obstacle_Color);

		m_GameBoard.DrawText(std::to_string(Score), Vec2(5.0f, 5.0f));
		m_GameBoard.DrawText(std::to_string(Health), Vec2(5.0f, 30.0f));
	}

private:
	void SpawnBullet()
	{
		Flying_Object bullet = Flying_Object(m_Player.GetPosition(), Vec2(sinf(m_Player.GetAngle()*PI / 180), -cosf(m_Player.GetAngle()*PI / 180))*200.0f, 1, 0, m_Bullet_Model);
		m_Bullets.push_back(bullet);
	}

	void UpdateObjectPosition(Flying_Object& F_Obj, float dt)
	{
		float x, y;
		Vec2 newPos = F_Obj.UpdatePosition(dt);
		LoopCoordinates(newPos.x, newPos.y, x, y);
		F_Obj.SetPosition(Vec2(x, y));
	}

	void UpdatePlayerPosition(float dt) 
	{
		UpdateObjectPosition(m_Player, dt);
	}

	void UpdateBulletPositions(float dt) 
	{
		for (std::vector<Flying_Object>::iterator it = m_Bullets.begin(); it != m_Bullets.end(); it++)
		{
			UpdateObjectPosition(*it, dt);
		}

		m_Bullets.erase(std::remove_if(m_Bullets.begin(), m_Bullets.end(), [](const Flying_Object& b) {return b.GetPosition().x <= 0 || b.GetPosition().y <= 0 || b.GetPosition().x >= SCREEN_WIDTH-1 || b.GetPosition().y >= SCREEN_HEIGHT-1; }), m_Bullets.end());
	}

	void UpdateAsteroidPositions(float dt)
	{
		for (std::vector<Flying_Object>::iterator asteroid = m_Asteroids.begin(); asteroid != m_Asteroids.end(); asteroid++)
		{
			UpdateObjectPosition(*asteroid, dt);
		}
	}

	Flying_Object CreateAsteroid(const Vec2& Spawn, int Size)
	{
		float randRadAngle = (float(rand()) / float(RAND_MAX)) * 2 * PI;
		float randSpeedAmp = 1.0f + (float(rand()) / float(RAND_MAX / (1.0f - 75.0f)));

		Vec2 Speed = Vec2(sinf(randRadAngle), cosf(randRadAngle)) * randSpeedAmp;
		Flying_Object Rock = Flying_Object(Spawn, Speed, Size, randRadAngle, m_Asteroid_Model);
		return Rock;
	}

	void CheckInteractions(float dt)
	{
		std::vector<Flying_Object>::iterator bullet = m_Bullets.begin();

		while (bullet != m_Bullets.end())
		{
			if (CheckBulletAsteroidCollision(*bullet)) {
				bullet = m_Bullets.erase(bullet);
				Score += 50;
			}
			else
				++bullet;
		}
		
		CheckPlayerAsteroidCollision(dt);

		if (m_Asteroids.empty()) 
		{
			SpawnAsteroidField();
		}
	}

	bool CheckBulletAsteroidCollision(Flying_Object& bullet) 
	{
		bool flag = false;

		std::vector<Flying_Object> newAsteroids;
		std::vector<Flying_Object>::iterator asteroid = m_Asteroids.begin();
		while (asteroid != m_Asteroids.end())
		{
			bool isColiding = CheckCollision(bullet.GetPosition(), asteroid->GetPosition(), (bullet.GetSize() + asteroid->GetSize()));
			flag = flag || isColiding;
			if (isColiding) {
				if (asteroid->GetSize() > 5) 
				{
					newAsteroids.push_back(CreateAsteroid(asteroid->GetPosition(), asteroid->GetSize()/2));
					newAsteroids.push_back(CreateAsteroid(asteroid->GetPosition(), asteroid->GetSize()/2));
				}
				asteroid = m_Asteroids.erase(asteroid);
				break;
			}
			else
				++asteroid;
		}

		for (std::vector<Flying_Object>::iterator asteroid = newAsteroids.begin(); asteroid != newAsteroids.end(); asteroid++)
		{
			m_Asteroids.push_back(*asteroid);
		}

		return flag;
	}

	bool CheckCollision(const Vec2& obj1, const Vec2& obj2, float limit) 
	{
		return (obj1 - obj2).Length() < limit;
	}

	bool CheckPlayerAsteroidCollision(float dt) 
	{
		bool isColliding = std::any_of(m_Asteroids.begin(), m_Asteroids.end(), [&](const Flying_Object& a) {return CheckCollision(a.GetPosition(), m_Player.GetPosition(), (a.GetSize() + m_Player.GetSize())); });
		if (isColliding)
		{
			if (!isInvincible)
			{
				Health--;
				if (Health < 1)
					Loose();
				else
				{
					isInvincible = true;
					InvincibilityTimer = 0;
				}
			}
		}
		if (isInvincible)
		{
			InvincibilityTimer += dt;
			if (InvincibilityTimer > InvicibilityTimeOnHit)
			{
				InvincibilityTimer = 0;
				isInvincible = false;
			}
		}
		return isColliding;
	}

	void Loose() 
	{
		m_GameBoard.Clear();

		m_Player = Shuttle(Vec2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2), Vec2(), 10, 0.0f, 50);

		Score = 0;
		Health = 5;
		InvincibilityTimer = 0;
		isInvincible = false;

		m_Asteroids.clear();
		m_Bullets.clear();
	}

	void SpawnAsteroidField() 
	{
		for (int i = 0; i < 5; i++) 
		{
			int randPoint = int((float(rand()) / float(RAND_MAX / float(SCREEN_WIDTH * 2 + SCREEN_HEIGHT * 2))));
			if (randPoint < SCREEN_WIDTH)
				m_Asteroids.push_back(CreateAsteroid(Vec2(randPoint, 0.0f), 20));
			else if (randPoint < SCREEN_WIDTH + SCREEN_HEIGHT)
				m_Asteroids.push_back(CreateAsteroid(Vec2(SCREEN_WIDTH - 1, randPoint - SCREEN_WIDTH), 20));
			else if (randPoint < 2 * SCREEN_WIDTH + SCREEN_HEIGHT)
				m_Asteroids.push_back(CreateAsteroid(Vec2(randPoint - SCREEN_WIDTH - SCREEN_HEIGHT, SCREEN_HEIGHT - 1), 20));
			else if (randPoint < 2 * SCREEN_WIDTH + 2 * SCREEN_HEIGHT)
				m_Asteroids.push_back(CreateAsteroid(Vec2(0.0f, randPoint - 2 * SCREEN_WIDTH - SCREEN_HEIGHT), 20));
		}
	}

	void LoopCoordinates(float in_x, float in_y, float& out_x, float& out_y)
	{
		out_x = in_x;
		out_y = in_y;
		if (in_x < 0) out_x = in_x + SCREEN_WIDTH;
		if (in_x >= SCREEN_WIDTH) out_x = in_x - SCREEN_WIDTH;
		if (in_y < 0) out_y = in_y + SCREEN_HEIGHT;
		if (in_y >= SCREEN_HEIGHT) out_y = in_y - SCREEN_HEIGHT;
	}
};

GameManager* gm;

// initialize game data in this function
void initialize()
{
	gm = new GameManager(*buffer);
}

// this function is called to update game data,
// dt - time elapsed since the previous update (in seconds)
void act(float dt)
{
	if (is_key_pressed(VK_ESCAPE))
		schedule_quit_game();
	
	gm->UpdateGame(dt);
}

// fill buffer in this function
// uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH] - is an array of 32-bit colors (8 bits per R, G, B)
void draw()
{
	// clear backbuffer
	memset(buffer, 0, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(uint32_t));
	gm->DrawGame();
}

// free game data in this function
void finalize()
{
	delete gm;
}

