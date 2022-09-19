#pragma once
#include "define.hpp"

#define Timestep f32

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
};

const f32 DELAY_BETWEEN_ROUNDS = 5.0f;

class Pong {
public:
    Pong();
    void Update( Timestep, const PlayerInput& );
    const GameState& GetState() { return m_gameState; }
private:
    GameState m_gameState;
    static f32 MovePaddle(const f32&, const f32&);
    static f32 PlayerDY(const PlayerInput&);
    f32 CpuDY();
    void MoveBallX(const f32&);
    void MoveBallY(const f32&);
    void BallCollision();
    void ResetBall();

    f32 m_scoreTimer = 0.0f;
};
