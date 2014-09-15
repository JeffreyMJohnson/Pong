#include "AIE.h"
#include <iostream>
#include <time.h>
#include <string>

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;
const float PLAYER_SPEED = 1500.0f;
const float PLAYER_HEIGHT = 100.0f;
const float PLAYER_WIDTH = 20.0f;
const float PLAYER1_XPOS = 100.0F;
const float PLAYER1_YPOS = SCREEN_HEIGHT / 2;
const float PLAYER2_XPOS = SCREEN_WIDTH - 100;
const float PLAYER2_YPOS = SCREEN_HEIGHT / 2;
const float PLAYER1_SCORE_POSX = SCREEN_WIDTH / 2 - 100.0f;
const float PLAYER1_SCORE_POSY = SCREEN_HEIGHT - 30.0f;
const float PLAYER2_SCORE_POSX = SCREEN_WIDTH / 2 + 100.0f;
const float PLAYER2_SCORE_POSY = PLAYER1_SCORE_POSY;
const int CENTER_LINE_WIDTH = 5;
const float BALL_SPEED = 100.0f;
const float BALL_WIDTH = PLAYER_WIDTH;
const float BALL_HEIGHT = 25;

const char* GAME_TITLE = "Pong Dong";
const char* PLAYER_IMAGE_LOCATION = "./images/pong_paddle.png";
//const char* CENTER_LINE_IMAGE_LOCATION = "./images/crate_sideup.png";
const char* PLAYER1_SCORE = "0";
const char* PLAYER2_SCORE = "0";

enum DIRECTION
{
	LEFT,
	RIGHT,
	UP,
	DOWN
};

enum GAMESTATE
{
	MENU,
	PLAY,
	HIGH_SCORE,
	WIN,
	QUIT
};

GAMESTATE currentGameState;

enum PLAY_STATE
{
	SERVE,
	ROUND
};

PLAY_STATE currentPlayState;

void InitializeGame();

void Update();

void Draw();

void InitializePlayers();

void DrawUI();

void drawCenterLine();

void InitializeBall();



DIRECTION GetRandXDirection();

DIRECTION GetRandomYDirection();

struct Player
{
	unsigned int spriteId;
	float speed;
	unsigned int score = 0;

	float width;
	float height;
	void SetSize(float a_width, float a_height)
	{
		width = a_width;
		height = a_height;

	}


	float x;
	float y;
	void SetPosition(float a_x, float a_y)
	{
		x = a_x;
		y = a_y;
	}


	unsigned int moveUpKey;
	unsigned int moveDownKey;

	void SetMoveKeys(unsigned int a_MoveUpKey, unsigned int a_MoveDownKey)
	{
		moveUpKey = a_MoveUpKey;
		moveDownKey = a_MoveDownKey;
	}

	void Move(float a_deltaTime)
	{
		if (IsKeyDown(moveUpKey))
		{
			y += a_deltaTime * speed;
			if (y > SCREEN_HEIGHT - (height / 2))
			{
				y = SCREEN_HEIGHT - (height / 2);
			}
		}

		if (IsKeyDown(moveDownKey))
		{
			y -= a_deltaTime * speed;
			if (y < height / 2)
			{
				y = height / 2;
			}
		}
		MoveSprite(spriteId, x, y);
	}

	void AddScore(unsigned int a_score)
	{
		score += a_score;
	}

	void GetScore(char* a_result)
	{
		//char result[3];
		itoa(score, a_result, 10);
		std::cout << a_result << std::endl;
		//return result;
	}
};

Player player1;
Player player2;



struct Ball
{
	unsigned int spriteID;

	float width;
	float height;
	void SetSize(float a_width, float a_height)
	{
		width = a_width;
		height = a_width;
	}

	float x;
	float y;
	void SetPosition(float a_x, float a_y)
	{
		x = a_x;
		y = a_y;
	}

	DIRECTION xDirection;
	DIRECTION yDirection;
	void SetDirection(DIRECTION a_xDirection, DIRECTION a_yDirection)
	{
		xDirection = a_xDirection;
		yDirection = a_yDirection;
	}

	void SwapDirection(DIRECTION a_direction)
	{
		switch (a_direction)
		{
		case RIGHT:
			xDirection = LEFT;
			break;
		case LEFT:
			xDirection = RIGHT;
			break;
		case UP:
			yDirection = DOWN;
			break;
		case DOWN:
			yDirection = UP;
			break;
		}
	}

	bool IsPlayer1Collision()
	{
		if (x - width / 2 <= player1.x + player1.width / 2 &&
			x - width / 2 >= player1.x - player1.width / 2 &&
			y - height <= player1.y + player1.height / 2 &&
			y + height >= player1.y - player1.height / 2)
		{
			return true;
		}
		return false;
	}

	bool IsPlayer2Collision()
	{
		if (x + width / 2 >= player2.x - player2.width / 2 &&
			x - width / 2 <= player2.x + player2.width / 2 &&
			y <= player2.y + player2.height / 2 &&
			y >= player2.y - player2.height / 2)
		{
			return true;
		}
		return false;
	}

	//move the ball position checking for collision with the players paddles
	void Move(float a_deltaTime)
	{
		switch (xDirection)
		{
		case LEFT:
			x -= a_deltaTime * BALL_SPEED;
			if (IsPlayer1Collision())
			{
				xDirection = RIGHT;
				if (x < player1.x + player1.width / 2)
				{
					x = player1.x + player1.width / 2;
				}
			}
			break;
		case RIGHT:
			x += a_deltaTime * BALL_SPEED;
			if (IsPlayer2Collision())
			{
				xDirection = LEFT;
				if (x > player2.x - player2.width / 2)
				{
					x = player2.x - player2.width / 2;
				}
			}
			break;
		}

		switch (yDirection)
		{
		case UP:
			y += a_deltaTime * BALL_SPEED;
			if (y > SCREEN_HEIGHT)
			{
				y = SCREEN_HEIGHT;
				yDirection = DOWN;
			}
			break;
		case DOWN:
			y -= a_deltaTime * BALL_SPEED;
			if (y < 0)
			{
				y = 0;
				yDirection = UP;
			}
		}
	}

	/*call to update the ball's position, direction and score if needed.
	*/
	void Update(float a_deltaTime)
	{
		Move(a_deltaTime);
		MoveSprite(spriteID, x, y);
		if (xDirection == LEFT)
		{
			if (x <= 0)
			{
				player2.AddScore(1);
				currentPlayState = SERVE;
			}
		}
		else
		{
			if (x >= SCREEN_WIDTH)
			{
				player1.AddScore(1);
				currentPlayState = SERVE;
			}
		}
	}

};

Ball ball;

int main(int argc, char* argv[])
{
	Initialise(SCREEN_WIDTH, SCREEN_HEIGHT, false, GAME_TITLE);

	SetBackgroundColour(SColour(0, 0, 0, 255));

	InitializeGame();

	//Game Loop
	do
	{
		ClearScreen();

		//implement gamestates
		switch (currentGameState)
		{
		case MENU:
			break;
		case PLAY:
			//implement play states
			switch (currentPlayState)
			{
			case SERVE:
				currentPlayState = ROUND;
				break;
			case ROUND:
				Update();
				Draw();
				break;
			}


			break;
		case HIGH_SCORE:
			break;
		case QUIT:
			break;
		}



	} while (!FrameworkUpdate());

	Shutdown();

	return 0;
}


void InitializeGame()
{
	currentGameState = PLAY;
	currentPlayState = SERVE;

	InitializePlayers();

	InitializeBall();
}

void Draw()
{
	DrawSprite(player1.spriteId);
	DrawSprite(player2.spriteId);
	DrawSprite(ball.spriteID);
	DrawUI();
}

void Update()
{
	float deltaTime = GetDeltaTime();

	player1.Move(deltaTime);
	player2.Move(deltaTime);

	ball.Update(deltaTime);
}

void DrawUI()
{
	drawCenterLine();
	char player1Score[3];
	player1.GetScore(player1Score);
	DrawString(player1Score , PLAYER1_SCORE_POSX, PLAYER1_SCORE_POSY);
	DrawString("0", PLAYER2_SCORE_POSX, PLAYER2_SCORE_POSY);
}

void drawCenterLine()
{
	DrawLine(SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH / 2, 0);
	for (int i = 0; i < CENTER_LINE_WIDTH; i++)
	{
		DrawLine(SCREEN_WIDTH / 2 - i, SCREEN_HEIGHT, SCREEN_WIDTH / 2 - i, 0);
		DrawLine(SCREEN_WIDTH / 2 + i, SCREEN_HEIGHT, SCREEN_WIDTH / 2 + i, 0);
	}

}

void InitializePlayers()
{
	player1.SetSize(PLAYER_WIDTH, PLAYER_HEIGHT);
	player1.speed = PLAYER_SPEED;
	player1.SetMoveKeys('W', 'X');
	player1.spriteId = CreateSprite(PLAYER_IMAGE_LOCATION, player1.width, player1.height, true);
	player1.SetPosition(PLAYER1_XPOS, PLAYER1_YPOS);
	MoveSprite(player1.spriteId, player1.x, player1.y);

	player2.SetSize(PLAYER_WIDTH, PLAYER_HEIGHT);
	player1.speed = PLAYER_SPEED;


	//set up to numpad 8 (328)
	//set down to numpad 2 (322)
	player2.SetMoveKeys(328, 322);
	player2.speed = PLAYER_SPEED;
	player2.spriteId = CreateSprite(PLAYER_IMAGE_LOCATION, player2.width, player2.height, true);
	player2.SetPosition(PLAYER2_XPOS, PLAYER2_YPOS);
	MoveSprite(player2.spriteId, player2.x, player2.y);
}

void InitializeBall()
{
	ball.SetSize(BALL_WIDTH, BALL_HEIGHT);
	ball.spriteID = CreateSprite(PLAYER_IMAGE_LOCATION, ball.width, ball.height, true);
	const unsigned int x = SCREEN_WIDTH / 2;

	//get random y value for serving the ball
	srand(time(nullptr));
	int y = rand() % SCREEN_HEIGHT;
	//see if ball is above or below the horizontal half line, this is to check if the ball is out of bounds to start with
	if (y < SCREEN_HEIGHT / 2)
	{
		if (y < ball.height)
		{
			y = ball.height;
		}
	}
	else
	{
		if (y > SCREEN_HEIGHT - ball.height)
		{
			y = SCREEN_HEIGHT - ball.height;
		}
	}
	ball.SetPosition(x, y);
	//std::cout << "x: " << x << "\ny: " << y << std::endl;

	//get random directions

	DIRECTION xDir = GetRandXDirection();
	DIRECTION yDir = GetRandomYDirection();

	ball.SetDirection(xDir, yDir);
	MoveSprite(ball.spriteID, ball.x, ball.y);
}

DIRECTION GetRandXDirection()
{
	srand(time(nullptr));
	int dir = rand() % 2;
	switch (dir)
	{
	case 0:
		return LEFT;
	case 1:
		return RIGHT;
	}
}

DIRECTION GetRandomYDirection()
{
	srand(time(nullptr));
	int dir = rand() % 2 + 2;
	std::cout << "dir: " << dir << std::endl;
	switch (dir)
	{
	case 2:
		return UP;
	case 3:
		return DOWN;
	}
}

