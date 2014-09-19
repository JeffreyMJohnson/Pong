#include "AIE.h"
#include <iostream>
#include <time.h>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;


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
const float SERVING_TIME_DELAY = 2.0f;

const char* GAME_TITLE = "Pong Dong";
const char* PLAYER_IMAGE_LOCATION = "./images/pong_paddle.png";
const char* FONT_PATH = "./fonts/invaders.fnt";
const char* PLAYER1_SCORE = "0";
const char* PLAYER2_SCORE = "0";
const char* MENU_TEXT_NEW_GAME = "(S)tart a New Game";
const char* MENU_TEXT_HIGH_SCORE = "(D)isplay High Scores";
const char* MENU_TEXT_QUIT = "(Q)uit Game";


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

void DrawGame();

void InitializePlayers();

void DrawGameUI();

void drawCenterLine();

void InitializeBall();

void ServeBall(float &a_timer);

void DrawMenu();

void HandleMenuInput();

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
		//std::cout << a_result << std::endl;
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

struct HighScore
{
	int score;
	string name;
};

bool CompareTest(HighScore a_score1, HighScore a_score2);

class HighScores
{
public:
	//constructor: will init mHighScores from file if any
	HighScores();

	bool IsHighScore(HighScore &a_HighScore);

	void AddScore(HighScore &a_HighScore);

	void SaveScores();

	string ToString();


private:
	//	const string LOG_FILE_PATH = "Highscores.log";
	const string LOG_FILE_PATH = "foo.txt"; //TODO: THIS IS FOR DEBUG, REMOVE BEFORE RELEASE
	//needs to be static for some reason?
	const static int MAX_CHAR_COUNT = 30;

	const int HIGH_SCORE_COUNT = 5;

	vector<HighScore> mHighScores;

	//sort vector by HighScore.score with highest score in first index
	void SortList();

	//sort helper function for sorting function in algorithm library
	//bool SortFunction(HighScore a_score1, HighScore a_score2);

	//load mHighScores vector with data from file if any.
	void ParseFile();


};



int main(int argc, char* argv[])
{
	

	bool quitGame = false;
	Initialise(SCREEN_WIDTH, SCREEN_HEIGHT, false, GAME_TITLE);

	SetBackgroundColour(SColour(0, 0, 0, 255));

	AddFont(FONT_PATH);


	//variable for pausing the serve screen
	float serveTimer = 0.0f;

	InitializeGame();

	//Game Loop
	do
	{
		ClearScreen();
		SetFont(FONT_PATH);
		string winner = "YOU WIN ";
		//implement gamestates
		switch (currentGameState)
		{
		case MENU:
			DrawMenu();
			HandleMenuInput();
			break;
		case PLAY:
			//implement play states
			switch (currentPlayState)
			{
			case SERVE:
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
			break;
		case WIN:
			if (player1.score > player2.score)
			{
				winner += " PLAYER 1 !";
			}
			else
			{
				winner += " PLAYER 2 !";
			}
			DrawGameUI();
			DrawString(winner.c_str(), SCREEN_WIDTH / 2 - 100.0f, SCREEN_HEIGHT / 2);
			DrawString("<ESC> to return to menu", SCREEN_WIDTH / 2 - 150, 50);

			if (IsKeyDown(ESC_KEYCODE))
			{
				currentGameState = MENU;
				std::cout << "here\n"; 
			}
			break;
		case QUIT:
			cout << "quit" << endl;
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
	if (player1.score > 10 || player2.score > 10)
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

HighScores::HighScores()
{
	ParseFile();
}

void HighScores::ParseFile()
{
	fstream file;
	file.open(LOG_FILE_PATH, ios::in);

	//buffer for line of data to parse
	char buffer[MAX_CHAR_COUNT];

	//set loop for max num of highscores allowed, will break if less than that present;
	//CHANGE::maybe a while loop would be better here, using isFileEnd flag;
	for (int i = 0; i < HIGH_SCORE_COUNT; i++)
	{
		file.getline(buffer, MAX_CHAR_COUNT);
		HighScore score;
		//convert to string for functionality
		string line = string(buffer);

		//cout << "line: " << line << endl;

		if (line.size() != 0)
		{

			int delimPos = line.find(',');
			//not sure if second param is inclusive or exclusive. might have to subtract 1 if stoi() exceptions
			score.score = stoi(line.substr(0, delimPos));

			//assuming starting char is inclusive so added 1.
			score.name = line.substr(delimPos + 1, string::npos);

			mHighScores.push_back(score);
		}
		else
		{
			/*cout << "end of file reached\n";
			cout << "size: " << line.size() << endl;*/
			break;
		}
	}

	file.close();

}

string HighScores::ToString()
{
	string result;
	//cout << "Score List\n";
	//cout << "SCORE\tNAME\n";

	result.append("***********************************\n");
	result.append("\tScore List\n");
	result.append("***********************************\n");
	result.append("SCORE\tNAME\n\n");

	for (int i = 0; i < mHighScores.size(); i++)
	{
		int score = mHighScores[i].score;
		string name = mHighScores[i].name;

		//result.append("Score: ");
		result.append(to_string(mHighScores[i].score));
		//result.append("\nName: ");
		result.append("\t");
		result.append(mHighScores[i].name);
		result.append("\n\n");
	}
	result.append("***********************************\n");
	return result;
}

//mut be global function for some unknown reason to myself
bool CompareTest(HighScore a_score1, HighScore a_score2)
{
	return a_score1.score > a_score2.score;
}


//return true if first HighScore.score is greater than second HighScore.score. Returns false otherwise. 
//This sorts HighScore list in descending order.
//bool HighScores::SortFunction(HighScore a_score1, HighScore a_score2)
//{
//	return a_score1.score > a_score2.score;
//}

void HighScores::SortList()
{
	cout << ToString();
	cout << "sort list\n";
	sort(mHighScores.begin(), mHighScores.end(), CompareTest);
	cout << ToString();
}

//adds score if a_HighScore param score value is higher than any score value in current list.  If so, will insert a_HighScore into list
//in appropriate place (sorted descending) moving the rest one step towards list end and removing the last entry which is the smallest value
void HighScores::AddScore(HighScore &a_HighScore)
{
	if (mHighScores.size() == 0)
	{
		//know it's highest score because it's first entry so add it and return
		mHighScores.push_back(a_HighScore);
		return;
	}

	//at least one entry in list so check to find insert pos
	//need to be iterator because insert() requires.
	vector<HighScore>::iterator insertLocation = mHighScores.end();

	//search for highscore less than current score.
	for (vector<HighScore>::iterator it = mHighScores.begin(); it != mHighScores.end(); it++)
	{
		//if the list is less than max and the value is equal to a value than that's the insert location
		if (a_HighScore.score > it->score || (a_HighScore.score == it->score && mHighScores.size() < HIGH_SCORE_COUNT))
		{
			//found one so note place in vector
			insertLocation = it;
			//must break out of loop. once a smaller value is found, all others will be as well.
			break;
		}
		//the list size still isn't filled and the value is lower than others so will be inserted last pos
		else if (mHighScores.size() < HIGH_SCORE_COUNT)
		{
			mHighScores.push_back(a_HighScore);
			return;
		}
	}

	//might not be a valid highscore so check if still end() value;
	if (insertLocation != mHighScores.end())
	{
		//insert current score into highscores vector in appropriate place
		mHighScores.insert(insertLocation, a_HighScore);

		//vector might not be filled with high scores yet.
		if (mHighScores.size() > HIGH_SCORE_COUNT){
			//insert() moves everything down a place in vector, so now has too many entries. need to remove final entry
			mHighScores.erase(mHighScores.end() - 1);
		}
	}
	else //TODO: DEBUG ONLY REMOVE THIS AND THE NEXT LINE FOR RELEASE
		cout << "Score not high enough for HighScores list\n";
}

void HighScores::SaveScores()
{
	fstream file;
	file.open(LOG_FILE_PATH, ios::out);
	for (int i = 0; i < mHighScores.size(); i++)
	{
		file << mHighScores[i].score << ',' << mHighScores[i].name << endl;
	}
	//put empty line at end to help parsing
	file << '\n';
	file.flush();
	file.close();

}