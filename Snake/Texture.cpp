#include "Texture.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>
#include <stdio.h>

Texture::Texture()
{
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

Texture::~Texture()
{
	free();
}

void Texture::free()
{
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

bool Texture::load_from_file(SDL_Renderer* renderer, const std::string& path)
{
	free();

	SDL_Surface* temp_surface = NULL;
	
	temp_surface = IMG_Load(path.c_str());
	if (temp_surface == NULL)
	{
		printf("[Error] Failed to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
		return false;
	}

	mTexture = SDL_CreateTextureFromSurface(renderer, temp_surface);
	if (mTexture == NULL)
	{
		printf("[Error] Failed to create texture from image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
	}
	else
	{
		mWidth = temp_surface->w;
		mHeight = temp_surface->h;
	}

	SDL_FreeSurface(temp_surface);
	temp_surface = NULL;

	return true;
}

bool Texture::load_from_text(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, SDL_Color text_color)
{
	free();

	SDL_Surface* temp_surface = NULL;

	temp_surface = TTF_RenderUTF8_Blended(font, text.c_str(), text_color);
	if (temp_surface == NULL)
	{
		printf("[Error] Failed to render text! SDL_ttf Error: %s\n", TTF_GetError());
		return false;
	}

	mTexture = SDL_CreateTextureFromSurface(renderer, temp_surface);
	if (mTexture == NULL)
	{
		printf("[Error] Failed to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
	}
	else
	{
		mWidth = temp_surface->w;
		mHeight = temp_surface->h;
	}

	SDL_FreeSurface(temp_surface);
	temp_surface = NULL;

	return true;
}

void Texture::render(SDL_Renderer* renderer, int x, int y)
{
	if (mTexture == NULL) return;
	SDL_Rect render_quad = { x, y, mWidth, mHeight };
	SDL_RenderCopy(renderer, mTexture, NULL, &render_quad);
}

int Texture::get_width() const
{
	return mWidth;
}

int Texture::get_height() const
{
	return mHeight;
}
