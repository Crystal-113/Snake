#pragma once
#include "Config.h"
#include "Renderer.h"
#include "Texture.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <sstream>
#include <vector>

enum SnakeDirections
{
	SNAKE_UP,
	SNAKE_DOWN,
	SNAKE_LEFT,
	SNAKE_RIGHT
};

enum GameStatus
{
	GAME_STATUS_WAITING,
	GAME_STATUS_ACTIVE,
	GAME_STATUS_PAUSED,
	GAME_STATUS_LOST
};

enum QuitStatus
{
	QUIT_STATUS_QUIT_GAME,
	QUIT_STATUS_RETURN_TO_MENU
};

class DefaultGamemode
{
public:
	DefaultGamemode(Renderer& renderer);
	~DefaultGamemode();
	bool load_media();
	void cleanup();
	int main_loop();

private:
	Renderer& mRenderer;
	TTF_Font* mFont36;
	TTF_Font* mFont18;
	Texture mHead, mBody, mApple;
	Texture mLengthTextTexture, mWaitingScreenTextTexture, mPausedScreenTextTexture, mLostScreenTextTexture0, mLostScreenTextTexture1;
	SnakeDirections mSnakeDirection;
	std::vector<SDL_Point> mSnakePositionGrid;
	SDL_Point mApplePositionGrid;
	GameStatus mGameStatus;
	QuitStatus mQuitStatus;
	int mLength;

	void _check_keydown_events(bool& running);
	void _update();
	void _change_apple_position();
	bool _apple_collide_with_snake();
	void _render_waiting();
	void _render_active();
	void _render_paused();
	void _render_lost();
	void _prep();
	std::string _prep_length_text();
};