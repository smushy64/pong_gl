#pragma once
#include "./core/font.hpp"
#include "./core/app.hpp"
#include <glm/vec3.hpp>
#include <string>
#include "./core/ui.hpp"

#ifdef OPENGL
typedef void* (*LoadFunctionGL)(const char*);
bool InitializeGL(LoadFunctionGL);
#endif

bool InitializeRenderer();
void RenderMenu(const MenuOption& currentMenuOption);

void ClearScreen();
void RenderGame(const GameState& gameState);
void RendererLoadFont(const Font& font);
void RenderText(std::string text, f32 x, f32 y, f32 scale, UITextStyle textStyle, const glm::vec3& color);

void RenderText(std::string text, f32 x, f32 y, f32 scale, UITextStyle textStyle);
void RenderText(std::string text, f32 x, f32 y, f32 scale);
void RenderText(const UITextElement& textElement);
