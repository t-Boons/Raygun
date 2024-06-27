#include "precomp.h"
#include "renderer/renderer.h"
#include "gameplay/levels/gameplayLevel.h"
#include "gameplay/levels/mainMenuLevel.h"

namespace Gameplay
{
	void Game::Init()
	{
		instance = this;

		renderer = new Renderer();
		renderer->Init();
		renderer->SetDrawSurface(screen);
		LoadLevel(new MainMenuLevel());
	}

	void Game::Tick(float deltaTime)
	{
		renderer->Tick(deltaTime);
		level->Tick(deltaTime);
	}

	void Game::UI()
	{
		level->TickUI();
	}

	void Game::LoadLevel(Level* lvl)
	{

		// Reset render level.
		renderer->GetScene()->Reset();

		delete level;
		level = lvl;

		level->Init();
		level->Start();
	}
}