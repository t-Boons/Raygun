#pragma once
#include "renderer/renderer.h"
#include "gameplay/levels/level.h"

namespace Gameplay
{
	class Game : public TheApp
	{
	public:
		void Init();

		void MouseMove(int x, int y) { mousePos.x = x, mousePos.y = y; }
		const int2& GetMousePos() const { return mousePos; }

		void Tick(float deltaTime);
		void UI() override;

		void LoadLevel(Level* level);

		static Game* Get() { return instance; }
		Renderer* GetRenderer() { return renderer; }

	private:
		Renderer* renderer;
		RenderLevel* scene;
		Level* level;
		int2 mousePos;

		inline static Game* instance;
	};
}