#include "AIE.h"
#include <iostream>
#include <time.h>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

//magic numbers to adjust for debug / assesment 
const float SERVING_TIME_DELAY = 2.0f;
const int MIN_SCORE_FOR_WIN = 10;


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
const float BALL_SPEED = 200.0f;
const float BALL_WIDTH = PLAYER_WIDTH;
const float BALL_HEIGHT = 25;
const int ESC_KEYCODE = 256;
const int MAX_CHAR_COUNT = 30;


const char* PLAYER1_SCORE = "0";
const char* PLAYER2_SCORE = "0";
const char* GAME_TITLE = "Pong Dong";
const char* PLAYER_IMAGE_LOCATION = "./images/pong_paddle.png";
const char* FONT_PATH = "./fonts/invaders.fnt";
const string LOG_FILE_PATH = "Highscore.data";

const char* MENU_TEXT_NEW_GAME = "(S)tart a New Game";
const char* MENU_TEXT_HIGH_SCORE = "(D)isplay High Scores";
const char* MENU_TEXT_QUIT = "(Q)uit Game";

//used for ball updeate
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

//general initialization of game objects.
void InitializeGame();

//update all game objects per frame cycle
void Update();

//draw all game objects per frame cycle
void DrawGame();

//initialize the 2 player objects
void InitializePlayers();

//draw the scores, and center line of the game court per every frame
void DrawGameUI();

//refactored function because the line is a qty of lines next to eachother so done programtically here
void drawCenterLine();

//initialize the ball object 
void InitializeBall();

//check if game is won and if not compute ball location and direction to start the round and change playstate to round. If the game winning criteria is 
//true changes state to win.
void ServeBall(float &a_timer);

//draw the game menu per frame 
void DrawMenu();

//function to handle the input keys for the menu game state
void HandleMenuInput();

//initialize the highscore variable with value from text file. this is done during game initialization
void LoadHighScore();

//output value of highscore variable to text file. This is done during cleanup before quitting app.
void WriteHighScore();

//compute and return random direction enum on x-axis (e.g. RIGHT, LEFT)
DIRECTION GetRandXDirection();

//compute and return random direction enum on y-axis (e.g. UP, DOWN)
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
		itoa(score, a_result, 10);
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

int mHighScore;

int main(int argc, char* argv[])
{
	//flag to leave loop when want to quit app
	bool quitGame = false;

	//used as flag to keep writing high score text during loop	in WIN gamestate
	bool isHighScore = false;

	Initialise(SCREEN_WIDTH, SCREEN_HEIGHT, false, GAME_TITLE);

	SetBackgroundColour(SColour(0, 0, 0, 255));

	//using 8-bit style font from space invaders exercise
	AddFont(FONT_PATH);

	//variable for pausing the serve screen
	float serveTimer = 0.0f;

	//setup players, ball and variables
	InitializeGame();

	//Game Loop
	do
	{
		ClearScreen();
		SetFont(FONT_PATH);
		string winner = "YOU WIN ";
		
		
		switch (currentGameState)
		{
		case MENU:
			DrawMenu();
			HandleMenuInput();
			break;
		case PLAY:
			switch (currentPlayState)
			{
			case SERVE:
				//reset the flag because new game is starting
				isHighScore = false;
				ServeBall(serveTimer);
				break;
			case ROUND:
				serveTimer = 0.0f;
				Update();
				DrawGame();
				break;
			}
			break;
		case HIGH_SCORE:
			
			DrawString("HIGH SCORE", SCREEN_WIDTH / 2 - 100.0f, SCREEN_HEIGHT * 0.75f);
			char buff[30];//itoa requires a char buffer for param but it's not used because itoa returns char* as well
			DrawString(itoa(mHighScore, buff, 10), SCREEN_WIDTH/2 - 25, SCREEN_HEIGHT *0.66f);	
			DrawString("<ESC> to return to menu", SCREEN_WIDTH / 2 - 150, 50);

			if (IsKeyDown(ESC_KEYCODE))
			{
				currentGameState = MENU;
			}
			break;
		case WIN:
			if (player1.score > player2.score)
			{
				winner += " PLAYER 1 !";
				//the isHighScore flag is used to keep the string drawn each frame after setting the highscore variable
				if (isHighScore || player1.score > mHighScore)
				{
					DrawString("You are the  new High Score!", SCREEN_WIDTH / 2 - 155.f, SCREEN_HEIGHT * 0.66f);
					mHighScore = player1.score;
					isHighScore = true;
				}
			}
			else
			{
				winner += " PLAYER 2 !";
				//the isHighScore flag is used to keep the string drawn each frame after setting the highscore variable
				if (isHighScore || player2.score > mHighScore)
				{
					DrawString("You are the  new High Score!", SCREEN_WIDTH / 2 - 155.f, SCREEN_HEIGHT * 0.66f);
					mHighScore = player2.score;
					isHighScore = true;
				}
			}
			//keep the game ui at last state drawing each frame
			DrawGameUI();

			//draw the concatenated win string
			DrawString(winner.c_str(), SCREEN_WIDTH / 2 - 100.0f, SCREEN_HEIGHT / 2);

			//using key handler to pause the screen in this state until user decides
			DrawString("<ESC> to return to menu", SCREEN_WIDTH / 2 - 150, 50);

			if (IsKeyDown(ESC_KEYCODE))
			{
				currentGameState = MENU;
				std::cout << "here\n";
			}
			break;
		case QUIT:
			WriteHighScore();
			//this flag necessary to break out of loop
			quitGame = true;
			break;
		}

	} while (!FrameworkUpdate() && !quitGame);

	Shutdown();

	return 0;
}


void InitializeGame()
{
	currentGameState = MENU;
	currentPlayState = SERVE;

	InitializePlayers();

	InitializeBall();

	LoadHighScore();
}

void DrawGame()
{
	DrawSprite(player1.spriteId);
	DrawSprite(player2.spriteId);
	DrawSprite(ball.spriteID);
	DrawGameUI();
}

void Update()
{
	float deltaTime = GetDeltaTime();

	player1.Move(deltaTime);
	player2.Move(deltaTime);

	ball.Update(deltaTime);
}

void DrawGameUI()
{
	drawCenterLine();
	char player1Score[3];
	char player2Score[3];
	player1.GetScore(player1Score);
	player2.GetScore(player2Score);
	DrawString(player1Score, PLAYER1_SCORE_POSX, PLAYER1_SCORE_POSY);
	DrawString(player2Score, PLAYER2_SCORE_POSX, PLAYER2_SCORE_POSY);
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
	//player2.SetMoveKeys(328, 322);
	player2.SetMoveKeys('[', '/'); //changed keys so can use on keyboard w/no numpad
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

	switch (dir)
	{
	case 2:
		return UP;
	case 3:
		return DOWN;
	}
}

void ServeBall(float &a_timer)
{
	//implemented player msut have winning point at least 2 more than opponent to make high score be sensical
	if ((player1.score > MIN_SCORE_FOR_WIN && player1.score > player2.score + 1) ||
		(player2.score > MIN_SCORE_FOR_WIN && player2.score > player1.score + 1))
	{
		currentGameState = WIN;
	}
	else
	{
		DrawString("Ready!", SCREEN_WIDTH / 2 - 50.0f, SCREEN_HEIGHT / 2);
		DrawGameUI();
		if (a_timer > SERVING_TIME_DELAY)
		{
			//set balls xDirection
			if (player1.score == player2.score)
			{
				ball.xDirection = GetRandXDirection();
			}
			else //flip ball xdirection
			{
				ball.SwapDirection(ball.xDirection);
			}

			//set ball yDirection to random
			ball.yDirection = GetRandomYDirection();

			//move ball to center
			int x = SCREEN_WIDTH / 2;
			int y = rand() % SCREEN_HEIGHT;
			//see if ball is above or below the horizontal half line, this is to check if the ball is out of bounds 
			//to start with
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
			MoveSprite(ball.spriteID, ball.x, ball.y);
			currentPlayState = ROUND;
		}
		else
		{
			a_timer += GetDeltaTime();
		}

	}
}

void DrawMenu()
{
	DrawString(GAME_TITLE, SCREEN_WIDTH / 2 - 100.0f, SCREEN_HEIGHT - 100);
	DrawString(MENU_TEXT_NEW_GAME, SCREEN_WIDTH / 2 - 150.0f, SCREEN_HEIGHT * .66f);
	DrawString(MENU_TEXT_HIGH_SCORE, SCREEN_WIDTH / 2 - 150.0f, SCREEN_HEIGHT *.66f - 100);
	DrawString(MENU_TEXT_QUIT, SCREEN_WIDTH / 2 - 150.0f, SCREEN_HEIGHT *.66f - 200);
}

void HandleMenuInput()
{
	if (IsKeyDown('S'))
	{
		currentGameState = PLAY;
		currentPlayState = SERVE;
	}
	if (IsKeyDown('D'))
	{
		currentGameState = HIGH_SCORE;
		//change savestate here when implemented
	}
	if (IsKeyDown('Q'))
	{
		currentGameState = QUIT;
	}
}


void LoadHighScore()
{
	fstream file;
	file.open(LOG_FILE_PATH, ios::in);

	//buffer for line of data to parse
	char buffer[MAX_CHAR_COUNT];
	file.getline(buffer, MAX_CHAR_COUNT);
	mHighScore = atoi(buffer);
	file.close();
}

void WriteHighScore()
{
	fstream file;
	file.open(LOG_FILE_PATH, ios::out);
	file << mHighScore;
	file.close();
}



