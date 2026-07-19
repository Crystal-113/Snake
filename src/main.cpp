#pragma execution_character_set("utf-8")

#include "Config.h"
#include "Renderer.h"
#include "Texture.h"
#include "DefaultGamemode.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>

SDL_Window* gWindow = NULL;
Renderer gRenderer;
TTF_Font* gFont = NULL;
DefaultGamemode default_mode(gRenderer);
Texture gTitleTextTexture, gButtonTextTexture, gVersionTexture;

bool init()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("[Error] Failed to initialize SDL! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
	{
		printf("[Warning] Failed to enable linear texture filtering!\n");
	}

	gWindow = SDL_CreateWindow("Snake", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (gWindow == NULL)
	{
		printf("[Error] Failed to create a window! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	if (!gRenderer.init(gWindow))
	{
		return false;
	}

	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
	{
		printf("[Error] Failed to initialize SDL_image! SDL_image Error: %s\n", IMG_GetError());
		return false;
	}

	if (TTF_Init() == -1)
	{
		printf("[Error] Failed to initialize SDL_ttf! SDL_ttf Error: %s\n", TTF_GetError());
		return false;
	}

	return true;
}

bool load_media()
{
	gFont = TTF_OpenFont("assets/fzht.ttf", 24);
	if (gFont == NULL)
	{
		printf("[Error] Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
		return false;
	}

	return true;
}

void cleanup()
{
	gTitleTextTexture.free();
	gButtonTextTexture.free();
	gVersionTexture.free();

	if(gFont != NULL)
	{
		TTF_CloseFont(gFont);
		gFont = NULL;
	}

	gRenderer.free();

	if(gWindow != NULL)
	{
		SDL_DestroyWindow(gWindow);
		gWindow = NULL;
	}

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

int main(int argc, char* args[])
{
	if (!init())
	{
		printf("[Error] Failed to initialize!\n");
		cleanup();
		return 1;
	}

	if (!load_media())
	{
		printf("[Error] Failed to load media!\n");
		cleanup();
		return 1;
	}

	if (!default_mode.load_media())
	{
		printf("[Error] Failed to create game instance!\n");
		default_mode.cleanup();
		cleanup();
		return 1;
	}

	if (default_mode.main_loop() == QUIT_STATUS_QUIT_GAME)
	{
		default_mode.cleanup();
		cleanup();
		return 0;
	}

	cleanup();
	return 0;
}