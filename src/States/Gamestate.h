#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "Basestate.h"

#include "../Player.h"
#include "../Application.h"
#include "../../Engine/Engine.h"

class GameState : public Basestate
{
public:
    GameState(Application& app);

    bool input() override;
    void update(float dt) override;
	void lateUpdate(Camera* cam) override;
	void render(Renderer* renderer) override;


    ~GameState() override;
protected:


private:
	std::vector<vn::GameObject> m_gameObjects;
	
	Input::Inputs vInput;

	Player m_player;
};

#endif // GAMESTATE_H
