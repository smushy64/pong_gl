#include "app.hpp"
#include <cmath>
#include <stdlib.h>
#include <glm/geometric.hpp>

Pong::Pong() {
    m_state = AppState::START;
    m_gameState = {};
    m_gameState.scored  = true;
    m_gameState.ball.direction = glm::vec2(-1.0f, 0.0f);
}

void Pong::Update( DeltaTime ts, const PlayerInput& input ) {
    switch(m_state) {
        case AppState::START:{
            if(input.enter) {
                switch(m_selectedOption) {
                    case MenuOption::START_GAME: {
                        m_state = AppState::GAME;
                    } return;
                    case MenuOption::QUIT_GAME: {
                        g_RUNNING = false;
                    } return;
                }
            }
            if(input.up != m_lastUp && input.up) {
                i32 selectedOption = (i32)m_selectedOption;

                if(selectedOption > 0) { selectedOption -= 1; }
                else { selectedOption = MAX_MENU_OPTIONS - 1; }

                m_selectedOption = (MenuOption)selectedOption;
            } else if(input.down != m_lastDown && input.down) {
                i32 selectedOption = (i32)m_selectedOption;

                selectedOption += 1;
                if(selectedOption >= MAX_MENU_OPTIONS) { selectedOption = 0; }

                m_selectedOption = (MenuOption)selectedOption;
            }
            m_lastUp   = input.up;
            m_lastDown = input.down;
        }break;
        case AppState::GAME:{
            if( !m_gameState.scored ) {
                MoveBallX(m_gameState.ball.direction.x * ts * BALL_SPEED);
                MoveBallY(m_gameState.ball.direction.y * ts * BALL_SPEED);
                BallCollision();
            } else {
                m_scoreTimer += ts;
                if(m_scoreTimer >= DELAY_BETWEEN_ROUNDS) {
                    m_scoreTimer = 0.0f;
                    m_gameState.scored = false;
                    ResetBall();
                }
            }

            m_gameState.player.y = MovePaddle( m_gameState.player.y, PADDLE_SPEED * PlayerDY(input) * ts );
            m_gameState.cpu.y    = MovePaddle( m_gameState.cpu.y,    CpuDY() * ts );
        }break;
    }
}

void Pong::BallCollision() {
    f32 left   = m_gameState.ball.x - BALL_HALF_SIZE;
    f32 right  = m_gameState.ball.x + BALL_HALF_SIZE;
    f32 top    = m_gameState.ball.y + BALL_HALF_SIZE;
    f32 bottom = m_gameState.ball.y - BALL_HALF_SIZE;

    f32 player_left   = -PADDLE_X_POS - PADDLE_HALF_W;
    f32 player_right  = -PADDLE_X_POS + PADDLE_HALF_W;
    f32 player_top    = m_gameState.player.y + PADDLE_HALF_H;
    f32 player_bottom = m_gameState.player.y - PADDLE_HALF_H;

    f32 cpu_left   = PADDLE_X_POS - PADDLE_HALF_W;
    f32 cpu_right  = PADDLE_X_POS + PADDLE_HALF_W;
    f32 cpu_top    = m_gameState.cpu.y + PADDLE_HALF_H;
    f32 cpu_bottom = m_gameState.cpu.y - PADDLE_HALF_H;

    bool bounce = false;
    bool top_third    = false;
    bool bottom_third = false;

    // within player vertically
    if( bottom <= player_top && top >= player_bottom ) {
        // within player horizontally
        if( left <= player_right && right >= player_left ) {
            m_gameState.ball.direction.x = 1.0f;
            top_third    = m_gameState.ball.y > m_gameState.player.y + PADDLE_THIRD_H / 2.0f;
            bottom_third = m_gameState.ball.y < m_gameState.player.y - PADDLE_THIRD_H / 2.0f;
            bounce = true;
        }
    }
    // within cpu vertically
    if( bottom <= cpu_top && top >= cpu_bottom ) {
        // within cpu horizontally
        if( left <= cpu_right && right >= cpu_left ) {
            m_gameState.ball.direction.x = -1.0f;
            top_third    = m_gameState.ball.y > m_gameState.cpu.y + PADDLE_THIRD_H / 2.0f;
            bottom_third = m_gameState.ball.y < m_gameState.cpu.y - PADDLE_THIRD_H / 2.0f;
            bounce = true;
        }
    }

    if(bounce) {
        // ball is in the top third of the paddle
        if(top_third) {
            m_gameState.ball.direction.y = BOUNCE_MAX;
        // ball is in the bottom third of the paddle
        } else if(bottom_third) {
            m_gameState.ball.direction.y = -BOUNCE_MAX;
        // ball is in the middle of the paddle
        } else {
            m_gameState.ball.direction.y = rand() % 2 == 0 ? 0.05f : -0.05f;
        }

        m_gameState.ball.direction = glm::normalize(m_gameState.ball.direction);
    }
}
void Pong::MoveBallX(const f32& delta) {
    f32 result = m_gameState.ball.x + delta;
    f32 test_left  = result - BALL_HALF_SIZE;
    f32 test_right = result + BALL_HALF_SIZE;

    bool out_left  = test_right <= -FIELD_W;
    bool out_right = test_left >= FIELD_W;

    // out of bounds
    if( out_left || out_right ) {

        m_gameState.scored = true;

        if(out_left) {
            m_gameState.cpuScore++;
        } else if(out_right) {
            m_gameState.playerScore++;
        }
        return;
    }

    m_gameState.ball.x = result;
}

void Pong::ResetBall() {
    m_gameState.ball.x  = 0.0f;
    m_gameState.ball.y  = 0.0f;
    m_gameState.ball.direction.x = -m_gameState.ball.direction.x;
    m_gameState.ball.direction.y = 0.0f;
}

void Pong::MoveBallY(const f32& delta) {
    f32 result = m_gameState.ball.y + delta;
    f32 test_top    = result + BALL_HALF_SIZE;
    f32 test_bottom = result - BALL_HALF_SIZE;

    // collision with ceiling/floor
    if( test_top >= FIELD_H ) {
        m_gameState.ball.direction.y = fabs(m_gameState.ball.direction.y) * -1.0f;
        return;
    } else if( test_bottom <= -FIELD_H ) {
        m_gameState.ball.direction.y = fabs(m_gameState.ball.direction.y);
        return;
    }
    
    m_gameState.ball.y = result;
}

f32 Pong::PlayerDY(const PlayerInput& input) {
    if(input.up) { return 1.0f; }
    else if(input.down) { return -1.0f; }
    return 0.0f;
}

f32 Pong::MovePaddle( const f32& paddleY, const f32& delta ) {
    f32 result = paddleY + delta;
    f32 test_top    = result + PADDLE_HALF_H;
    f32 test_bottom = result - PADDLE_HALF_H;
    if( test_top >= FIELD_H ) {
        result = paddleY;
    } else if( test_bottom <= -FIELD_H ) {
        result = paddleY;
    }
    return result;
}

f32 Pong::CpuDY() {
    // if ball is too far, don't do anything
    if( m_gameState.ball.x < 0.65f ) { return 0.0f; }
    // if it's above, move up, else move down
    if( m_gameState.ball.y > m_gameState.cpu.y + PADDLE_HALF_H ) {
        return PADDLE_SPEED;
    } else if( m_gameState.ball.y < m_gameState.cpu.y - PADDLE_HALF_H ) {
        return -PADDLE_SPEED;
    } else { return 0.0f; }
}
