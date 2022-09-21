#pragma once
#include "globals.hpp"
#include <glm/vec2.hpp>
#include "ui.hpp"

enum Scene {
    MAIN_MENU,
    IN_GAME,
};

const i32 MAX_MENU_OPTIONS  = 2;
enum MenuOption {
    START_GAME = 0,
    QUIT_GAME,
};

struct Paddle { f32 y; };
struct Ball   { f32 x; f32 y; glm::vec2 direction; };
struct GameState {
    Paddle player;
    Paddle cpu;
    Ball   ball;
    u32    playerScore;
    u32    cpuScore;
    bool   scored;
};

struct PlayerInput {
    bool up;
    bool down;
    bool enter;
};

class Pong {
public:
    Pong();
    void UpdateMenu(const PlayerInput& input);
    void UpdateGame( DeltaTime ts, const PlayerInput& input );
    const GameState& GetGameState() { return m_gameState; }
    Scene CurrentScene() { return m_currentScene; }
    MenuOption GetSelectedMenuOption() { return m_selectedMenuOption; }
private:
    GameState m_gameState;
    static f32 MovePaddle(const f32&, const f32&);
    static f32 PlayerDY(const PlayerInput&);
    f32 CpuDY();
    void MoveBallX(const f32&);
    void MoveBallY(const f32&);
    void BallCollision();
    void ResetBall();

    Scene m_currentScene;
    MenuOption m_selectedMenuOption = MenuOption::START_GAME;

    f32 m_scoreTimer = 0.0f;
    bool m_lastUp   = false;
    bool m_lastDown = false;
};

const glm::vec3 SELECT_COLOR   = glm::vec3(1.0f);
const glm::vec3 DESELECT_COLOR = glm::vec3(0.5f);

const UITextElement& GetTitleText();
UITextElement& GetStartGameText();
UITextElement& GetQuitGameText();
const UITextElement& GetControlsText0();
const UITextElement& GetControlsText1();
const UITextElement& GetControlsText2();
