#include "Engine.h"

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

public:
	Flying_Object(const Vec2& Pos, const Vec2& Speed, const int Size, const float Angle) :
		m_Pos(Pos), m_Speed(Speed), m_Size(Size), m_Angle(Angle) {}

	Flying_Object() : m_Pos(Vec2()), m_Speed(Vec2()), m_Size(1), m_Angle(0.0f) {}

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
};


class Asteroid : public Flying_Object 
{
public:
	Asteroid(const Vec2& Pos, const Vec2& Speed, const int Size, const float Angle) :
		Flying_Object(Pos, Speed, Size, Angle) {}

};


class Shuttle : public Flying_Object
{
private:
	std::vector<Vec2> Model = { Vec2(-12.0f, 8.0f), Vec2(0.0f, -20.0f), Vec2(12.0f, 8.0f) };
	float m_Acceleration;
	float m_SpeedLimit;

public:
	Shuttle() : Flying_Object() {}

	Shuttle(const Vec2& Pos, const Vec2& Speed, const int Size, const float Angle, const float Acceleration):
		Flying_Object(Pos, Speed, Size, Angle), m_Acceleration(Acceleration), m_SpeedLimit(200) {}

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

	std::vector<Vec2> GetModel()
	{
		return Model;
	}
};


class Gamefield
{
private:
	uint32_t* const m_Board;
	uint32_t const m_Obstacle = 0xFF0000FF;
	uint32_t const m_Player_Color = 0x0000FFFF;

public:
	Gamefield(uint32_t* board) : m_Board(board)
	{}

	void DrawAsteroids(std::vector<Asteroid>& Asteroids)
	{
		for (std::vector<Asteroid>::iterator it = Asteroids.begin(); it != Asteroids.end(); ++it)
		{
			int x = int(it->GetPosition().x);
			int y = int(it->GetPosition().y);

			for (int i = 0; i < it->GetSize(); i++)
			{
				for (int j = 0; j < it->GetSize(); j++)
				{
					*(m_Board + int(y + i)*SCREEN_WIDTH + int(x + j)) = m_Obstacle;
				}
			}
		}
	}

	bool DrawPlayer(Shuttle player)
	{
		std::vector<Vec2> BoardModel = player.GetModel();
		for (int i = 0; i < BoardModel.size(); i++)
		{
			float x = BoardModel[i].x;
			float y = BoardModel[i].y;
			float CosF = cosf(player.GetAngle()*PI / 180);
			float SinF = sinf(player.GetAngle()*PI / 180);
			BoardModel[i].x = x * CosF - y * SinF;
			BoardModel[i].y = x * SinF + y * CosF;
			BoardModel[i] += player.GetPosition();
		}

		return DrawPolygon(&BoardModel, m_Player_Color);
	}

	bool DrawPolygon(const std::vector<Vec2>* Model, uint32_t Color)
	{
		if (Model->size() <= 0)
			return false;

		if (Model->size() == 1)
		{
			DrawPoint((*Model)[0].x, (*Model)[0].y, Color);
			return true;
		}

		for (int i = 0; i < Model->size(); i++)
		{
			if (i == Model->size()-1)		
				DrawLine((*Model)[i].x, (*Model)[i].y, (*Model)[0].x, (*Model)[0].y, Color);
			else
				DrawLine((*Model)[i].x, (*Model)[i].y, (*Model)[i + 1].x, (*Model)[i + 1].y, Color);
		}
		return true;
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
			for (int i = x0; i < x1; i++)
			{
				float y = round(k*i + b);
				if (y > last_y)
					for(float yy = last_y; yy < y+1; yy++)
						DrawPoint(i, yy, Color);
				else
					for (float yy = y; yy < last_y+1; yy++)
						DrawPoint(i, yy, Color);
				last_y = y;
			}
		}
			
		else 
		{
			last_y = y1;
			for (int i = x1; i < x0; i++)
			{
				float y = round(k*i + b);
				if (y > last_y)
					for (float yy = last_y; yy < y+1; yy++)
						DrawPoint(i, yy, Color);
				else
					for (float yy = y; yy < last_y+1; yy++)
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

private:
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
	Shuttle m_Player;
	std::vector<Asteroid> m_Asteroids;
	Vec2 SpawnPoint;

public:
	GameManager(uint32_t* board) : m_GameBoard(Gamefield(board))
	{
		m_Player = Shuttle(Vec2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2), Vec2(), 5, 0.0f, 50);
		SpawnPoint = Vec2(100, 100);
		SpawnAsteroid();
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
	}

	void SpawnAsteroid()
	{
		Asteroid Rock = Asteroid(SpawnPoint, Vec2(10, 10), 10, 0);
		m_Asteroids.push_back(Rock);
	}

	void UpdateGame(float dt)
	{
		ReadInputs(dt);

		UpdatePlayerPosition(dt);
		UpdateAsteroidPositions(dt);
	}

	void DrawGame() 
	{
		m_GameBoard.DrawAsteroids(m_Asteroids);
		m_GameBoard.DrawPlayer(m_Player);
	}

private:
	void UpdatePlayerPosition(float dt) 
	{
		float x, y;
		Vec2 newPos = m_Player.UpdatePosition(dt);
		LoopCoordinates(newPos.x, newPos.y, x, y);
		m_Player.SetPosition(Vec2(x, y));
	}

	void UpdateAsteroidPositions(float dt)
	{
		for (std::vector<Asteroid>::iterator it = m_Asteroids.begin(); it != m_Asteroids.end(); it++)
		{
			float x, y;
			Vec2 newPos = it->UpdatePosition(dt);
			LoopCoordinates(newPos.x, newPos.y, x, y);
			it->SetPosition(Vec2(x, y));
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

}

