#pragma once
#include "globals.hpp"
#include <glm/vec2.hpp>

const f32 DELAY_BETWEEN_ROUNDS = 1.5f;
const f32 BALL_SPEED   = 1.25f;
const f32 PADDLE_SPEED = 1.15f;
const f32 BALL_HALF_SIZE = BALL_SIZE / 2.0f;
const f32 PADDLE_THIRD_H = PADDLE_H / 3.0f;
const f32 PADDLE_HALF_W = PADDLE_W / 2.0f;
const f32 PADDLE_HALF_H = PADDLE_H / 2.0f;
const f32 BOUNCE_MAX = 0.6f;

enum AppState {
    START,
    GAME,
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
    void Update( DeltaTime, const PlayerInput& );
    const GameState& GetState() { return m_gameState; }
    AppState GetAppState() { return m_state; }
    MenuOption SelectedMenuOption() { return m_selectedOption; }
private:
    GameState m_gameState;
    static f32 MovePaddle(const f32&, const f32&);
    static f32 PlayerDY(const PlayerInput&);
    f32 CpuDY();
    void MoveBallX(const f32&);
    void MoveBallY(const f32&);
    void BallCollision();
    void ResetBall();

    AppState   m_state;
    MenuOption m_selectedOption = MenuOption::START_GAME;

    f32 m_scoreTimer = 0.0f;
    bool m_lastUp   = false;
    bool m_lastDown = false;
};
