#pragma once
#include "./core/app.hpp"
#include "./core/font.hpp"
#include <glm/mat4x4.hpp>
#include <map>
#include <string>

struct Character {
    u32 texture;
    i32 w; i32 h;
    i32 bearing_x;
    i32 bearing_y;
    u32 advance;
};

enum TextStyle {
    NORMAL,
    REVERSE
};

class OpenGLRenderer {
public:
    OpenGLRenderer();
    void RenderMenu(MenuOption);
    void RenderControls();
    void RenderGame(const GameState&);
    void LoadFont(const Font&);
    void RenderText(std::string, f32, f32, f32, TextStyle, const glm::vec3&);
    void RenderScore(u32,u32);
    void ClearScreen();
private:
    u32 m_vao, m_vbo, m_ebo;
    u32 m_shader;
    i32 m_transformLoc;

    glm::mat4 m_paddleScale;
    glm::mat4 m_ballScale;

    u32 m_fontShader;
    u32 m_fontVao;
    u32 m_fontVbo;

    i32 m_fontColorLoc;
    glm::vec3 m_lastFontColor = glm::vec3(0.0f);

    std::map<char, Character> m_characters;
    bool m_fontLoaded = false;

    void RenderCharacter(const Character&, f32, f32, f32);
};
