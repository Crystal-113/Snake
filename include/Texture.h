#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

class Texture
{
public:
	Texture();
	~Texture();
	void free();
	bool load_from_file(SDL_Renderer* renderer, const std::string& path);
	bool load_from_text(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, SDL_Color text_color);
	void render(SDL_Renderer* renderer, int x, int y);
	int get_width() const;
	int get_height() const;

private:
	SDL_Texture* mTexture;
	int mWidth, mHeight;
};