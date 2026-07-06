#pragma execution_character_set("utf-8")

#include "DefaultGamemode.h"
#include "Config.h"
#include "Renderer.h"
#include "Texture.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>
#include <random>
#include <chrono>

DefaultGamemode::DefaultGamemode(Renderer& renderer)
	: mRenderer(renderer)
{
	mFont36 = NULL;
	mFont18 = NULL;
}

DefaultGamemode::~DefaultGamemode()
{
	cleanup();
}

bool DefaultGamemode::load_media()
{
	if (!mHead.load_from_file(mRenderer.get_renderer(), "assets/head.png"))
	{
		printf("[Warning] Failed to load head image! Trying to continue...\n");
	}

	if (!mBody.load_from_file(mRenderer.get_renderer(), "assets/body.png"))
	{
		printf("[Warning] Failed to load body image! Trying to continue...\n");
	}

	if (!mApple.load_from_file(mRenderer.get_renderer(), "assets/apple.png"))
	{
		printf("[Warning] Failed to load apple image! Trying to continue...\n");
	}

	mFont36 = TTF_OpenFont("assets/fzht.ttf", 36);
	if (mFont36 == NULL)
	{
		printf("[Error] Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
		return false;
	}

	mFont18 = TTF_OpenFont("assets/fzht.ttf", 18);
	if (mFont18 == NULL)
	{
		printf("[Error] Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
		return false;
	}

	if (!mWaitingScreenTextTexture.load_from_text(mRenderer.get_renderer(), mFont36, "按空格键开始", TEXT_COLOR))
	{
		return false;
	}

	if (!mPausedScreenTextTexture.load_from_text(mRenderer.get_renderer(), mFont36, "暂 停", TEXT_COLOR))
	{
		return false;
	}

	if (!mLostScreenTextTexture0.load_from_text(mRenderer.get_renderer(), mFont36, "游戏结束", TEXT_COLOR))
	{
		return false;
	}

	if (!mLostScreenTextTexture1.load_from_text(mRenderer.get_renderer(), mFont36, "按空格键重玩", TEXT_COLOR))
	{
		return false;
	}
	
	return true;
}

void DefaultGamemode::cleanup()
{
	mHead.free();
	mBody.free();
	mApple.free();
	mLengthTextTexture.free();
	mWaitingScreenTextTexture.free();
	mPausedScreenTextTexture.free();
	mLostScreenTextTexture0.free();
	mLostScreenTextTexture1.free();

	TTF_CloseFont(mFont36);
	mFont36 = NULL;
	TTF_CloseFont(mFont18);
	mFont18 = NULL;
}

void DefaultGamemode::_check_keydown_events(bool &running)
{
	SDL_Event e;

	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT)
		{
			running = 0;
		}
		if (e.type == SDL_KEYDOWN)
		{
			if (mGameStatus == GAME_STATUS_ACTIVE)
			{
				if (e.key.keysym.sym == SDLK_w || e.key.keysym.sym == SDLK_UP)
				{
					mSnakeDirection = SNAKE_UP;
				}
				else if (e.key.keysym.sym == SDLK_s || e.key.keysym.sym == SDLK_DOWN)
				{
					mSnakeDirection = SNAKE_DOWN;
				}
				else if (e.key.keysym.sym == SDLK_a || e.key.keysym.sym == SDLK_LEFT)
				{
					mSnakeDirection = SNAKE_LEFT;
				}
				else if (e.key.keysym.sym == SDLK_d || e.key.keysym.sym == SDLK_RIGHT)
				{
					mSnakeDirection = SNAKE_RIGHT;
				}
				else if (e.key.keysym.sym == SDLK_SPACE)
				{
					mGameStatus = GAME_STATUS_PAUSED;
				}
			}
			else if (mGameStatus == GAME_STATUS_WAITING || mGameStatus == GAME_STATUS_PAUSED)
			{
				if (e.key.keysym.sym == SDLK_SPACE)
				{
					mGameStatus = GAME_STATUS_ACTIVE;
				}
			}
			else if (mGameStatus == GAME_STATUS_LOST)
			{
				if (e.key.keysym.sym == SDLK_SPACE)
				{
					_prep();
					mGameStatus = GAME_STATUS_ACTIVE;
				}
			}
		}
	}
}

bool DefaultGamemode::_apple_collide_with_snake()
{
	for (int i = 0; i < mLength; i++)
	{
		if (mApplePositionGrid.x == mSnakePositionGrid[i].x && mApplePositionGrid.y == mSnakePositionGrid[i].y)
		{
			return true;
		}
	}
	return false;
}

void DefaultGamemode::_change_apple_position()
{
	// 使用时间 + random_device 混合种子
	std::random_device rd;
	std::mt19937 gen(rd() ^ static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count()));
	std::uniform_int_distribution<int> dist_x(0, SCREEN_WIDTH / GRID_SIZE - 1);
	std::uniform_int_distribution<int> dist_y(0, SCREEN_HEIGHT / GRID_SIZE - 1);
	do
	{
		mApplePositionGrid.x = dist_x(gen);
		mApplePositionGrid.y = dist_y(gen);
	} while (_apple_collide_with_snake());
}

void DefaultGamemode::_update()
{
	SDL_Point head = mSnakePositionGrid.front();
	if (mSnakeDirection == SNAKE_RIGHT)
	{
		if (head.x + 1 >= SCREEN_WIDTH / GRID_SIZE)
		{
			mGameStatus = GAME_STATUS_LOST;
			return;
		}
		else if(head.x + 1 == mApplePositionGrid.x && head.y == mApplePositionGrid.y)
		{
			mSnakePositionGrid.insert(mSnakePositionGrid.begin(), mApplePositionGrid);
			mLength++;
			_change_apple_position();
		}
		else
		{
			SDL_Point a = mSnakePositionGrid.back();
			a.x = head.x + 1;
			a.y = head.y;
			for (int i = 0; i < mLength - 1; i++)
			{
				if (mSnakePositionGrid[i].x == a.x && mSnakePositionGrid[i].y == a.y)
				{
					mGameStatus = GAME_STATUS_LOST;
					return;
				}
			}
			mSnakePositionGrid.pop_back();
			mSnakePositionGrid.insert(mSnakePositionGrid.begin(), a);
		}
	}
	else if (mSnakeDirection == SNAKE_LEFT)
	{
		if (head.x - 1 < 0)
		{
			mGameStatus = GAME_STATUS_LOST;
			return;
		}
		else if (head.x - 1 == mApplePositionGrid.x && head.y == mApplePositionGrid.y)
		{
			mSnakePositionGrid.insert(mSnakePositionGrid.begin(), mApplePositionGrid);
			mLength++;
			_change_apple_position();
		}
		else
		{
			SDL_Point a = mSnakePositionGrid.back();
			a.x = head.x - 1;
			a.y = head.y;
			for (int i = 0; i < mLength - 1; i++)
			{
				if (mSnakePositionGrid[i].x == a.x && mSnakePositionGrid[i].y == a.y)
				{
					mGameStatus = GAME_STATUS_LOST;
					return;
				}
			}
			mSnakePositionGrid.pop_back();
			mSnakePositionGrid.insert(mSnakePositionGrid.begin(), a);
		}
	}
	else if (mSnakeDirection == SNAKE_DOWN)
	{
		if (head.y + 1 >= SCREEN_HEIGHT / GRID_SIZE)
		{
			mGameStatus = GAME_STATUS_LOST;
			return;
		}
		else if (head.x == mApplePositionGrid.x && head.y + 1 == mApplePositionGrid.y)
		{
			mSnakePositionGrid.insert(mSnakePositionGrid.begin(), mApplePositionGrid);
			mLength++;
			_change_apple_position();
		}
		else
		{
			SDL_Point a = mSnakePositionGrid.back();
			a.x = head.x;
			a.y = head.y + 1;
			for (int i = 0; i < mLength - 1; i++)
			{
				if (mSnakePositionGrid[i].x == a.x && mSnakePositionGrid[i].y == a.y)
				{
					mGameStatus = GAME_STATUS_LOST;
					return;
				}
			}
			mSnakePositionGrid.pop_back();
			mSnakePositionGrid.insert(mSnakePositionGrid.begin(), a);
		}
	}
	else// if (mSnakeDirection == SNAKE_UP)
	{
		if (head.y - 1 < 0)
		{
			mGameStatus = GAME_STATUS_LOST;
			return;
		}
		else if (head.x == mApplePositionGrid.x && head.y - 1 == mApplePositionGrid.y)
		{
			mSnakePositionGrid.insert(mSnakePositionGrid.begin(), mApplePositionGrid);
			mLength++;
			_change_apple_position();
		}
		else
		{
			SDL_Point a = mSnakePositionGrid.back();
			a.x = head.x;
			a.y = head.y - 1;
			for (int i = 0; i < mLength - 1; i++)
			{
				if (mSnakePositionGrid[i].x == a.x && mSnakePositionGrid[i].y == a.y)
				{
					mGameStatus = GAME_STATUS_LOST;
					return;
				}
			}
			mSnakePositionGrid.pop_back();
			mSnakePositionGrid.insert(mSnakePositionGrid.begin(), a);
		}
	}
}

void DefaultGamemode::_render_waiting()
{
	mRenderer.clear();
	mWaitingScreenTextTexture.render(mRenderer.get_renderer(), (SCREEN_WIDTH - mWaitingScreenTextTexture.get_width()) / 2, (SCREEN_HEIGHT - mWaitingScreenTextTexture.get_height()) / 2);
}

void DefaultGamemode::_render_active()
{
	mRenderer.clear();
	mApple.render(mRenderer.get_renderer(), mApplePositionGrid.x * GRID_SIZE, mApplePositionGrid.y * GRID_SIZE);
	mHead.render(mRenderer.get_renderer(), mSnakePositionGrid[0].x * GRID_SIZE, mSnakePositionGrid[0].y * GRID_SIZE);
	for (int i = 1; i < mLength; i++)
	{
		mBody.render(mRenderer.get_renderer(), mSnakePositionGrid[i].x * GRID_SIZE, mSnakePositionGrid[i].y * GRID_SIZE);
	}
	mLengthTextTexture.load_from_text(mRenderer.get_renderer(), mFont18, _prep_length_text(), TEXT_COLOR);
	mLengthTextTexture.render(mRenderer.get_renderer(), SCREEN_WIDTH - mLengthTextTexture.get_width(), 0);
}

void DefaultGamemode::_render_paused()
{
	_render_active();
	mPausedScreenTextTexture.render(mRenderer.get_renderer(), (SCREEN_WIDTH - mPausedScreenTextTexture.get_width()) / 2, (SCREEN_HEIGHT - mPausedScreenTextTexture.get_height()) / 2);
}

void DefaultGamemode::_render_lost()
{
	_render_active();
	mLostScreenTextTexture0.render(mRenderer.get_renderer(), (SCREEN_WIDTH - mLostScreenTextTexture0.get_width()) / 2, (SCREEN_HEIGHT - mLostScreenTextTexture0.get_height() - mLostScreenTextTexture1.get_height()) / 2);
	mLostScreenTextTexture1.render(mRenderer.get_renderer(), (SCREEN_WIDTH - mLostScreenTextTexture1.get_width()) / 2, (SCREEN_HEIGHT + mLostScreenTextTexture0.get_height() - mLostScreenTextTexture1.get_height()) / 2);
}

void DefaultGamemode::_prep()
{
	mSnakePositionGrid.clear();
	for (int i = 0; i < SNAKE_START_LENGTH; i++)
	{
		SDL_Point new_body = { i, SCREEN_HEIGHT / GRID_SIZE - 1 };
		mSnakePositionGrid.insert(mSnakePositionGrid.begin(), new_body);
	}
	mLength = SNAKE_START_LENGTH;
	mSnakeDirection = SNAKE_RIGHT;
	_change_apple_position();
}

std::string DefaultGamemode::_prep_length_text()
{
	std::stringstream stream;
	stream.str("");
	stream.clear();
	stream << "长度：" << mLength;
	std::string length_text = stream.str();
	return length_text;
}

int DefaultGamemode::main_loop()
{
	bool running = true;
	Uint32 last_update = SDL_GetTicks();
	mGameStatus = GAME_STATUS_WAITING;
	mQuitStatus = QUIT_STATUS_QUIT_GAME;
	_prep();

	while (running)
	{
		_check_keydown_events(running);
		if (mGameStatus == GAME_STATUS_WAITING)
		{
			_render_waiting();
		}
		else if (mGameStatus == GAME_STATUS_ACTIVE)
		{
			Uint32 now = SDL_GetTicks();
			if (now - last_update >= 100)
			{
				_update();
				last_update = now;
			}
			_render_active();
		}
		else if (mGameStatus == GAME_STATUS_PAUSED)
		{
			_render_paused();
		}
		else// if (mGameStatus == GAME_STATUS_LOST)
		{
			_render_lost();
		}
		mRenderer.present();
		SDL_Delay(1000 / MAX_FPS);
	}

	return mQuitStatus;
}