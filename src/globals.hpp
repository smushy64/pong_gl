#pragma once
#include "define.hpp"

inline bool g_RUNNING = true;

const f32 BALL_SIZE  = 0.05f;
const f32 BALL_HALF_SIZE = BALL_SIZE / 2.0f;
const f32 BALL_SPEED = 0.6f;

const f32 PADDLE_W = BALL_SIZE;
const f32 PADDLE_H = BALL_SIZE * 5.0f;
const f32 PADDLE_HALF_W = PADDLE_W / 2.0f;
const f32 PADDLE_HALF_H = PADDLE_H / 2.0f;
const f32 PADDLE_THIRD_H = PADDLE_H / 3.0f;
const f32 PADDLE_SPEED  = 0.5f;

const f32 BOUNCE_MAX = 0.6f;

const f32 SCREEN_W = 1280.0f;
const f32 SCREEN_H = 720.0f;

const i32 SCREEN_W_I = 1280;
const i32 SCREEN_H_I = 720;

const f32 ASPECT   = SCREEN_W / SCREEN_H;

const f32 FIELD_W = ASPECT;
const f32 FIELD_H = 1.0f;

const f32 PADDLE_X_POS = FIELD_W * 0.8f;
