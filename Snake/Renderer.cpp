#include "Renderer.h"
#include <SDL.h>
#include <stdio.h>

Renderer::Renderer()
{
	mRenderer = NULL;
}

Renderer::~Renderer()
{
	free();
}

void Renderer::free()
{
	if (mRenderer != NULL)
	{
		SDL_DestroyRenderer(mRenderer);
		mRenderer = NULL;
	}
}

bool Renderer::init(SDL_Window* window)
{
	free();

	mRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (mRenderer == NULL)
	{
		printf("[Error] Failed to enable GPU accleration! SDL Error: %s\n", SDL_GetError());
		mRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
		if (mRenderer == NULL)
		{
			printf("[Error] Failed to create a renderer! SDL Error: %s\n", SDL_GetError());
			return false;
		}
		else
		{
			printf("[Warning] Using software acceleration instead!\n");
		}
	}
	SDL_SetRenderDrawColor(mRenderer, 0x00, 0x00, 0x00, 0xff);

	return true;
}

void Renderer::clear() const
{
	SDL_RenderClear(mRenderer);
}

void Renderer::present() const
{
	SDL_RenderPresent(mRenderer);
}

SDL_Renderer* Renderer::get_renderer() const
{
	return mRenderer;
}
