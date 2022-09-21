#pragma once
#include "defines.hpp"
#include "glm/vec3.hpp"
enum UITextStyle {
    NORMAL,
    REVERSE
};

struct UITextElement {
public:
    UITextElement();
    UITextElement(const char* text) : text(text) {}
    UITextElement(const char* text, f32 x, f32 y) : text(text), xPos(x), yPos(y) {}
    UITextElement(const char* text, f32 x, f32 y, f32 scale) : text(text), xPos(x), yPos(y), scale(scale) {}
    UITextElement(const char* text, f32 x, f32 y, f32 scale, glm::vec3 color)
        : text(text), xPos(x), yPos(y), scale(scale), color(color) {}
    const char* text  = "";
    f32 xPos          = 0.0f;
    f32 yPos          = 0.0f;
    f32 scale         = 1.0f;
    UITextStyle style = UITextStyle::NORMAL;
    glm::vec3 color   = glm::vec3(1.0f);
};
