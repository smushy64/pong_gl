#include "opengl.hpp"
#include "glad/glad.h"
#include "globals.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "./core/font.hpp"

#include <iostream>

void OpenGLRenderer::ClearScreen() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderer::RenderGame(const GameState& state) {

    if( state.scored ) {
        RenderScore(state.playerScore, state.cpuScore);
    }
    
    glUseProgram(m_shader);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

    if(!state.scored) {
        glm::mat4 ballTransform =
            glm::translate( glm::mat4(1.0f), glm::vec3( state.ball.x, state.ball.y, 0.0f ) ) *
            m_ballScale;
        glUniformMatrix4fv(m_transformLoc, 1, GL_FALSE, glm::value_ptr(ballTransform));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }

    glm::mat4 paddlePlayerTransform =
        glm::translate( glm::mat4(1.0f), glm::vec3( -PADDLE_X_POS, state.player.y, 0.0f ) ) *
        m_paddleScale;
    glUniformMatrix4fv(m_transformLoc, 1, GL_FALSE, glm::value_ptr(paddlePlayerTransform));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    glm::mat4 paddleCPUTransform =
        glm::translate( glm::mat4(1.0f), glm::vec3( PADDLE_X_POS, state.cpu.y, 0.0f ) ) *
        m_paddleScale;
    glUniformMatrix4fv(m_transformLoc, 1, GL_FALSE, glm::value_ptr(paddleCPUTransform));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void OpenGLRenderer::RenderMenu(MenuOption selectedOption) {
    glm::vec3 startColor = glm::vec3(0.5f);
    glm::vec3 quitColor  = glm::vec3(0.5f);
    switch(selectedOption) {
        case MenuOption::START_GAME: {
            startColor = glm::vec3(1.0f);
        } break;
        case MenuOption::QUIT_GAME: {
            quitColor = glm::vec3(1.0f);
        } break;
    }
    RenderText("PongGL",
        (SCREEN_W / 2.0f) - 150.0f,
        (SCREEN_H / 2.0f) + (SCREEN_H / 4.0f),
        TEXT_SCALE,
        TextStyle::NORMAL,
        glm::vec3(1.0f)
    );
    RenderText("Start Game",
        (SCREEN_W / 2.0f) - 250.0f,
        SCREEN_H / 2.0f,
        TEXT_SCALE * 0.9f,
        TextStyle::NORMAL,
        startColor
    );
    RenderText("Quit  Game",
        (SCREEN_W / 2.0f) - 250.0f,
        SCREEN_H / 3.0f,
        TEXT_SCALE * 0.9f,
        TextStyle::NORMAL,
        quitColor
    );
    RenderControls();
}

void OpenGLRenderer::RenderControls() {
    f32 scale = 0.3f;
    f32 vSpacing = 30.0f;
    RenderText( "Move Cursor [ Arrow Up | Arrow Down | W | S ]",
        10.0f, SCREEN_H - ( 10.0f + (vSpacing * 2.0f) ), TEXT_SCALE * scale, TextStyle::NORMAL, glm::vec3(1.0f)
    );
    RenderText( "Confirm     [ Enter | Space ]",
        10.0f, SCREEN_H - ( 10.0f + vSpacing ), TEXT_SCALE * scale, TextStyle::NORMAL, glm::vec3(1.0f)
    );
    RenderText( "Exit        [ Escape ]",
        10.0f, SCREEN_H - 10.0f, TEXT_SCALE * scale, TextStyle::NORMAL, glm::vec3(1.0f)
    );
}

void OpenGLRenderer::RenderScore(u32 playerScore, u32 cpuScore) {
    f32 textX = 200.0f;
    f32 textY = SCREEN_H - 125.0f;
    RenderText(std::to_string(playerScore), textX, textY, TEXT_SCALE, TextStyle::NORMAL, glm::vec3(1.0f));
    RenderText(std::to_string(cpuScore), SCREEN_W - textX, textY, TEXT_SCALE, TextStyle::REVERSE, glm::vec3(1.0f));
}

void OpenGLRenderer::RenderText(std::string text, f32 x, f32 y, f32 scale, TextStyle textStyle, const glm::vec3& color) {
    if(!m_fontLoaded) {
        std::cout << "ERROR: FONT NOT LOADED!" << std::endl;
        return;
    }

    glUseProgram(m_fontShader);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if( m_lastFontColor != color ) {
        glUniform3fv(m_fontColorLoc, 1, glm::value_ptr(color));
        m_lastFontColor = color;
    }

    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(m_fontVao);

    switch(textStyle) {
        case TextStyle::NORMAL: {
            std::string::const_iterator c;
            for( c = text.begin(); c != text.end(); ++c ) {
                Character character = m_characters[*c];
                RenderCharacter(character, x, y, scale);
                x += (character.advance >> 6) * scale;
            }
        } break;
        case TextStyle::REVERSE: {
            std::string::const_reverse_iterator c;
            for( c = text.rbegin(); c != text.rend(); ++c ) {
                Character character = m_characters[*c];
                RenderCharacter(character, x - (character.w + character.bearing_x) * scale, y, scale);
                x -= (character.advance >> 6) * scale;
            }
        } break;
    }
}

void OpenGLRenderer::RenderCharacter(const Character& character, f32 x, f32 y, f32 scale) {
    f32 xpos = x + character.bearing_x * scale;
    f32 ypos = y - (character.h - character.bearing_y) * scale;

    f32 w = character.w * scale;
    f32 h = character.h * scale;

    f32 vertices[6][4] = {
        { xpos,     ypos + h,   0.0f, 0.0f },
        { xpos,     ypos,       0.0f, 1.0f },
        { xpos + w, ypos,       1.0f, 1.0f },

        { xpos,     ypos + h,   0.0f, 0.0f },
        { xpos + w, ypos,       1.0f, 1.0f },
        { xpos + w, ypos + h,   1.0f, 0.0f }
    };

    glBindTexture(GL_TEXTURE_2D, character.texture);
    glBindBuffer(GL_ARRAY_BUFFER, m_fontVbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void OpenGLRenderer::LoadFont(const Font& font) {    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for( u8 c = 0; c < 128; c++ ) {
        Glyph glyph = font.glyphs.at(c);
        Character character = {};
        glGenTextures(1, &character.texture);
        glBindTexture(GL_TEXTURE_2D, character.texture);
        glTexImage2D(
            GL_TEXTURE_2D, 0,
            GL_RED,
            glyph.width,
            glyph.height,
            0, GL_RED, GL_UNSIGNED_BYTE,
            glyph.bitmap
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        character.w         = glyph.width;
        character.h         = glyph.height;
        character.bearing_x = 0;
        character.bearing_y = 0;
        character.advance   = glyph.advanceWidth + 400;

        m_characters.insert(std::pair<char, Character>( (char)c, character ));
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    m_fontLoaded = true;
}

#ifdef DEBUG

const i32 IGNORE_IDS_LEN = 1;
const i32 IGNORE_IDS[] = { 131185 };

void GLMessageCallback(
    GLuint, GLenum,
    GLint id, GLenum,
    GLint, const GLchar* msg,
    void*
) {
    for( i32 i = 0; i < IGNORE_IDS_LEN; i++ ) {
        if(id == IGNORE_IDS[i]) {
            return;
        }
    }
    std::cout << "OpenGL [" << id << "]: " << msg << "\n\n";
}
#endif

OpenGLRenderer::OpenGLRenderer() {

    #ifdef DEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(
            (GLDEBUGPROC)GLMessageCallback,
            nullptr
        );
    #endif

    m_paddleScale = glm::scale( glm::mat4(1.0f), glm::vec3(PADDLE_W, PADDLE_H, 1.0f) );
    m_ballScale   = glm::scale( glm::mat4(1.0f), glm::vec3(BALL_SIZE, BALL_SIZE, 1.0f) );

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    f32 vertices[] = {
        -0.5f,  0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
    };
    u32 indices[] = {
        0, 1, 2,
        1, 2, 3
    };

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(f32) * 3 * 4,
        &vertices,
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0, 3,
        GL_FLOAT, GL_FALSE,
        12,
        0
    );
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(u32) * 3 * 3,
        &indices,
        GL_STATIC_DRAW
    );

    const char* vert_src = R"(
#version 460 core
layout(location = 0) in vec3 v_pos;

uniform mat4 u_projection;
uniform mat4 u_transform;

void main() {
    gl_Position = u_projection * u_transform * vec4(v_pos, 1.0);
}
)";

    const char* frag_src = R"(
#version 460 core
out vec4 FRAG_COLOR;
void main() {
    FRAG_COLOR = vec4(1.0);
}
)";

    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(
        vert, 1,
        &vert_src, nullptr
    );
    glCompileShader(vert);

    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(
        frag, 1,
        &frag_src, nullptr
    );
    glCompileShader(frag);

    m_shader = glCreateProgram();
    glAttachShader(m_shader, vert);
    glAttachShader(m_shader, frag);

    glLinkProgram(m_shader);

    glDetachShader(m_shader, frag);
    glDetachShader(m_shader, vert);
    glDeleteShader(vert);
    glDeleteShader(frag);

    glUseProgram(m_shader);

    f32 size = 1.0f;
    f32 hor  = ASPECT * size;

    glm::mat4 projection = glm::ortho(
        -hor, hor,
        -size, size,
        -10.0f, 10.0f
    );

    GLint projectionLoc = glGetUniformLocation(m_shader, "u_projection");
    glUniformMatrix4fv(
        projectionLoc,
        1, GL_FALSE,
        glm::value_ptr(projection)
    );

    m_transformLoc = glGetUniformLocation(m_shader, "u_transform");

    const char* font_vert_src = R"(
#version 460 core
layout(location = 0) in vec4 v_vertex;
out vec2 v2f_uv;

uniform mat4 u_projection;

void main() {
    gl_Position = u_projection * vec4(v_vertex.xy, 0.0, 1.0);
    v2f_uv = v_vertex.zw;
}
)";

    const char* font_frag_src = R"(
#version 460 core
in vec2 v2f_uv;
out vec4 FRAG_COLOR;

uniform sampler2D u_glyph;
uniform vec3 u_textColor;

void main() {
    float result = texture(u_glyph, v2f_uv).r;
    FRAG_COLOR = vec4( u_textColor.rgb, result );
}
)";

    GLuint font_vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(
        font_vert, 1,
        &font_vert_src, nullptr
    );
    glCompileShader(font_vert);

    GLint success;
    glGetShaderiv(font_vert, GL_COMPILE_STATUS, &success);
    if(success == GL_FALSE) {
        std::cout << "Failed to compile font vertex shader!" << std::endl;
    }

    GLuint font_frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(
        font_frag, 1,
        &font_frag_src, nullptr
    );
    glCompileShader(font_frag);

    glGetShaderiv(font_frag, GL_COMPILE_STATUS, &success);
    if(success == GL_FALSE) {
        std::cout << "Failed to compile font fragment shader!" << std::endl;
    }

    m_fontShader = glCreateProgram();
    glAttachShader(m_fontShader, font_vert);
    glAttachShader(m_fontShader, font_frag);

    glLinkProgram(m_fontShader);

    glGetProgramiv(m_fontShader, GL_LINK_STATUS, &success);
    if(success == GL_FALSE) {
        std::cout << "Failed to link shaders!" << std::endl;
    }

    glDetachShader(m_fontShader, font_vert);
    glDetachShader(m_fontShader, font_frag);
    glDeleteShader(font_vert);
    glDeleteShader(font_frag);

    glUseProgram(m_fontShader);

    GLint fontProjectionLoc  = glGetUniformLocation(m_fontShader, "u_projection");
    glm::mat4 fontProjection = glm::ortho(0.0f, SCREEN_W, 0.0f, SCREEN_H, -1.0f, 1.0f);
    glUniformMatrix4fv(
        fontProjectionLoc, 1,
        GL_FALSE, glm::value_ptr(fontProjection)
    );

    GLint fontSamplerLoc = glGetUniformLocation(m_fontShader, "u_glyph");
    glUniform1i(fontSamplerLoc, 0);

    m_fontColorLoc = glGetUniformLocation(m_fontShader, "u_textColor");

    glGenVertexArrays(1, &m_fontVao);
    glBindVertexArray(m_fontVao);

    glGenBuffers(1, &m_fontVbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_fontVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(f32) * 6 * 4, nullptr, GL_DYNAMIC_DRAW );

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(f32) * 4, 0);
    glEnableVertexAttribArray(0);
}

