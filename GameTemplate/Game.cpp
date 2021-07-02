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

public:
	Flying_Object(const Vec2& Pos, const Vec2& Speed):
		m_Pos(Pos), m_Speed(Speed) {}

	Flying_Object(): m_Pos(Vec2()), m_Speed(Vec2()) {}

	const Vec2 GetPosition() const 
	{
		return m_Pos;
	}

	void SetSpeed(const Vec2& Speed) 
	{
		m_Speed = Speed;
	}

	void UpdatePosition(float dt) 
	{	
		Vec2 newPos = m_Pos + m_Speed * dt;
		if (newPos.x >= SCREEN_WIDTH) 
			newPos.x = 0;

		if (newPos.x < 0)
			newPos.x = SCREEN_WIDTH - 1;

		if (newPos.y >= SCREEN_HEIGHT)
			newPos.y = 0;

		if (newPos.y < 0)
			newPos.y = SCREEN_HEIGHT - 1;

		m_Pos = newPos;
	}
};


class Shuttle : public Flying_Object
{
private:
	Vec2 m_Acceleration;
	float m_SpeedLimit;

public:
	Shuttle() : Flying_Object() {}

	Shuttle(const Vec2& Pos, const Vec2& Speed, const Vec2& Acceleration):
		Flying_Object(Pos, Speed), m_Acceleration(Vec2()), m_SpeedLimit(100) {}

	void SetAcceleration(const Vec2& Acceleration)
	{
		m_Acceleration = Acceleration;
	}

	Vec2 GetAcceleration() const 
	{
		return m_Acceleration;
	}

	void UpdateSpeed(float dt)
	{
		Vec2 newSpeed = m_Speed + m_Acceleration * dt;

		if (newSpeed.Length() > m_SpeedLimit)
			return;

		m_Speed = newSpeed;
	}

	void AddSpeed(const Vec2& Speed) 
	{	
		Vec2 newSpeed = m_Speed + Speed;

		if (newSpeed.Length() > m_SpeedLimit)
			return;

		m_Speed = newSpeed;
	}
};


Shuttle Player;

// initialize game data in this function
void initialize()
{
	Player = Shuttle(Vec2(SCREEN_WIDTH/2, SCREEN_HEIGHT/2), Vec2(), Vec2());
}

#define min(a,b)            (((a) < (b)) ? (a) : (b))
#define max(a,b)            (((a) > (b)) ? (a) : (b))
int clamp(int n, int lower, int upper) {
	return max(lower, min(n, upper));
}


// this function is called to update game data,
// dt - time elapsed since the previous update (in seconds)
void act(float dt)
{
	if (is_key_pressed(VK_ESCAPE))
		schedule_quit_game();
	
	//if (is_key_pressed(VK_SPACE))
	//	//Player.AddSpeed(Vec2(1, 0));

	Vec2 GuideSpeed = Vec2();

	if (is_key_pressed(VK_LEFT))
		GuideSpeed += Vec2(-10, 0);

	if (is_key_pressed(VK_RIGHT))
		GuideSpeed += Vec2(10, 0);

	if (is_key_pressed(VK_UP))
		GuideSpeed += Vec2(0, -10);

	if (is_key_pressed(VK_DOWN))
		GuideSpeed += Vec2(0, 10);

	
	Player.AddSpeed(GuideSpeed*dt);
	Player.UpdatePosition(dt);
}

// fill buffer in this function
// uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH] - is an array of 32-bit colors (8 bits per R, G, B)
void draw()
{
	// clear backbuffer
	memset(buffer, 0, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(uint32_t));

	Vec2 Pos = Player.GetPosition();

	for(int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			buffer[int(Pos.y)+i][int(Pos.x)+j] = 0x0000FFFF;
		}
	}
}

// free game data in this function
void finalize()
{

}

