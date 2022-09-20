#pragma once
#include "defines.hpp"

inline bool g_RUNNING = true;

const i32 MAX_MENU_OPTIONS  = 2;
enum MenuOption {
    START_GAME = 0,
    QUIT_GAME,
};

const f32 TEXT_SCALE = 2.0f;
const f32 BALL_SIZE  = 0.05f;

const f32 PADDLE_W = BALL_SIZE;
const f32 PADDLE_H = BALL_SIZE * 5.0f;

const f32 SCREEN_W = 1280.0f;
const f32 SCREEN_H = 720.0f;

const f32 ASPECT   = SCREEN_W / SCREEN_H;

const f32 FIELD_W = ASPECT;
const f32 FIELD_H = 1.0f;

const f32 PADDLE_X_POS = FIELD_W * 0.8f;
