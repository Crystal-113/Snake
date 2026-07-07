#pragma once

#include <SDL.h>

class Renderer
{
public:
	Renderer();
	~Renderer();
	void free();
	bool init(SDL_Window* window);
	void clear() const;
	void present() const;
	SDL_Renderer* get_renderer() const;

	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

private:
	SDL_Renderer* mRenderer;
};