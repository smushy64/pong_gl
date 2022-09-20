#pragma once
#include "define.hpp"

#define Timestep f32

enum AppState {
    START,
    GAME,
};

struct Paddle { f32 y; };
struct Ball   { f32 x; f32 y; f32 dx; f32 dy; };
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

const f32 DELAY_BETWEEN_ROUNDS = 5.0f;

class Pong {
public:
    Pong();
    void Update( Timestep, const PlayerInput& );
    const GameState& GetState() { return m_gameState; }
    AppState GetAppState() { return m_state; }
    u32 SelectedMenuOption() { return m_selectedOption; }
private:
    GameState m_gameState;
    static f32 MovePaddle(const f32&, const f32&);
    static f32 PlayerDY(const PlayerInput&);
    f32 CpuDY();
    void MoveBallX(const f32&);
    void MoveBallY(const f32&);
    void BallCollision();
    void ResetBall();

    AppState m_state;
    u32 m_selectedOption = 0;

    f32 m_scoreTimer = 0.0f;
    bool m_lastUp   = false;
    bool m_lastDown = false;
};
