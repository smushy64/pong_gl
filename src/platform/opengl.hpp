#pragma once
#include "glad/glad.h"
#include "./core/app.hpp"
#include <glm/mat4x4.hpp>
#include <map>
#include <string>

struct Character {
    u32 texture;
    i32 w; i32 h;
    i32 bearing_x; i32 bearing_y;
    u32 advance;
};

class OpenGLRenderer {
public:
    OpenGLRenderer();
    void RenderMenu(u32);
    void RenderGame(const GameState&);
    void LoadFont();
    void RenderText(std::string, f32, f32, f32, bool, const glm::vec3&);
    void RenderScore(u32,u32);
private:
    GLuint    m_vao, m_vbo, m_ebo;
    GLuint    m_shader;
    GLint     m_transformLoc;

    glm::mat4 m_paddleScale;
    glm::mat4 m_ballScale;

    GLuint    m_fontShader;
    GLuint    m_fontVao;
    GLuint    m_fontVbo;
    GLint     m_fontColorLoc;

    std::map<char, Character> m_characters;
    bool m_fontLoaded = false;

    void RenderCharacter(const Character&, f32, f32, f32);
};
