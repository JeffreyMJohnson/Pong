﻿#include "AIE.h"
#include <iostream>

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;
const float PLAYER_SPEED = 1500.0f;
const float PLAYER_HEIGHT = 100.0f;
const float PLAYER_WIDTH = 20.0f;
const float PLAYER1_XPOS = 100.0F;
const float PLAYER1_YPOS = SCREEN_HEIGHT / 2;
const float PLAYER2_XPOS = SCREEN_WIDTH - 100;
const float PLAYER2_YPOS = SCREEN_HEIGHT / 2;
const float PLAYER1_SCORE_POSX = SCREEN_WIDTH / 2 - 10.0f;
const float PLAYER1_SCORE_POSY = SCREEN_HEIGHT - 50.0f;
const float PLAYER2_SCORE_POSX = SCREEN_WIDTH / 2 + 10.0f;
const float PLAYER2_SCORE_POSY = PLAYER1_SCORE_POSY;




const char* GAME_TITLE = "Pong Dong";
const char* PLAYER_IMAGE_LOCATION = "./images/pong_paddle.png";
const char* PLAYER1_SCORE = "0";
const char* PLAYER2_SCORE = "0";


void InitializePlayers();

void DrawUI();

struct Player
{
	unsigned int spriteId;
	float speed;
		
	float width;
	float height;
	void setSize(float a_width, float a_height)
	{
		width = a_width;
		height = a_height;

	}


	float x;
	float y;
	void setPosition(float a_x, float a_y)
	{
		x = a_x;
		y = a_y;
	}
	
	
	unsigned int moveUpKey;
	unsigned int moveDownKey;

	void setMoveKeys(unsigned int a_MoveUpKey, unsigned int a_MoveDownKey)
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
};

Player player1;
Player player2;

int main( int argc, char* argv[] )
{	
    Initialise(SCREEN_WIDTH, SCREEN_HEIGHT, false, GAME_TITLE);
    
    SetBackgroundColour(SColour(0, 0, 0, 255));

	InitializePlayers();

//float xPos = 100;
//    float yPos = 100;
//    int myTextureHandle = CreateSprite("./images/crate_sideup.png", xPos, yPos, false);
//	int direction = 1;

    //Game Loop
    do
	{
		ClearScreen();
		float deltaTime = GetDeltaTime();
		

		player1.Move(deltaTime);
		player2.Move(deltaTime);
		DrawSprite(player1.spriteId);
		DrawSprite(player2.spriteId);

		DrawUI();

        

    } while(!FrameworkUpdate());

    Shutdown();

    return 0;
}

void InitializePlayers()
{
	player1.setSize(PLAYER_WIDTH, PLAYER_HEIGHT);
	player1.speed = PLAYER_SPEED;
	player1.setMoveKeys('W', 'X');
	player1.spriteId = CreateSprite(PLAYER_IMAGE_LOCATION, player1.width, player1.height, true);
	player1.setPosition(PLAYER1_XPOS, PLAYER1_YPOS);
	MoveSprite(player1.spriteId, player1.x, player1.y);

	player2.setSize(PLAYER_WIDTH, PLAYER_HEIGHT);
	player1.speed = PLAYER_SPEED;
	
	
	//set up to numpad 8 (328)
	//set down to numpad 2 (322)
	player2.setMoveKeys(328, 322);
	player2.speed = PLAYER_SPEED;
	player2.spriteId = CreateSprite(PLAYER_IMAGE_LOCATION, player2.width, player2.height, true);
	player2.setPosition(PLAYER2_XPOS, PLAYER2_YPOS);
	MoveSprite(player2.spriteId, player2.x, player2.y);
}


void DrawUI()
{
	DrawLine(SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH / 2, 0);
	DrawLine(SCREEN_WIDTH / 2 - 1, SCREEN_HEIGHT, SCREEN_WIDTH / 2 - 1, 0);
	DrawLine(SCREEN_WIDTH / 2 + 1, SCREEN_HEIGHT, SCREEN_WIDTH / 2 + 1, 0);

	DrawString(PLAYER1_SCORE, PLAYER1_SCORE_POSX, PLAYER1_SCORE_POSY);
}