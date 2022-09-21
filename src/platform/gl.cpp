#ifdef OPENGL

#include "renderer.hpp"
#include "platform.hpp"
#include "glad/glad.h"
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>

GLuint shader;
GLuint vao, vbo, ebo;
GLint transformLoc;
glm::mat4 ballScale, paddleScale;

void RenderScore(u32 playerScore, u32 cpuScore);

void RenderGame(const GameState& gameState) {
    if( gameState.scored ) {
        RenderScore(gameState.playerScore, gameState.cpuScore);
    }
    
    glUseProgram(shader);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    if(!gameState.scored) {
        glm::mat4 ballTransform =
            glm::translate( glm::mat4(1.0f), glm::vec3( gameState.ball.x, gameState.ball.y, 0.0f ) ) *
            ballScale;
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(ballTransform));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }

    glm::mat4 paddlePlayerTransform =
        glm::translate( glm::mat4(1.0f), glm::vec3( -PADDLE_X_POS, gameState.player.y, 0.0f ) ) *
        paddleScale;
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(paddlePlayerTransform));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    glm::mat4 paddleCPUTransform =
        glm::translate( glm::mat4(1.0f), glm::vec3( PADDLE_X_POS, gameState.cpu.y, 0.0f ) ) *
        paddleScale;
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(paddleCPUTransform));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void RenderScore(u32 playerScore, u32 cpuScore) {
    f32 textX = 200.0f;
    f32 textY = SCREEN_H - 125.0f;
    RenderText(std::to_string(playerScore), textX, textY, TEXT_SCALE);
    RenderText(std::to_string(cpuScore), SCREEN_W - textX, textY, TEXT_SCALE, UITextStyle::REVERSE, glm::vec3(1.0f));
}

void RenderMenu(const MenuOption& currentMenuOption) {
    switch(currentMenuOption) {
        case MenuOption::START_GAME: {
            GetStartGameText().color = SELECT_COLOR;
            GetQuitGameText().color  = DESELECT_COLOR;
        } break;
        case MenuOption::QUIT_GAME: {
            GetStartGameText().color = DESELECT_COLOR;
            GetQuitGameText().color  = SELECT_COLOR;
        } break;
    }
    RenderText( GetTitleText() );
    RenderText( GetStartGameText() );
    RenderText( GetQuitGameText() );
    RenderText( GetControlsText0() );
    RenderText( GetControlsText1() );
    RenderText( GetControlsText2() );
}

#ifdef DEBUG

void GLMessageCallback(
    GLuint, GLenum,
    GLint id, GLenum severity,
    GLint, const GLchar* msg,
    void*
) {
    if(severity == GL_DEBUG_SEVERITY_HIGH) {
        ErrorBox("OpenGL [" + std::to_string(id) + "]: " + std::string(msg));
    }
}
#endif

GLuint fontVao, fontVbo;
GLuint fontShader;
GLint fontColorLoc;

bool InitializeGL( LoadFunctionGL loadFunc ) {
    return gladLoadGLLoader((GLADloadproc)loadFunc) != 0;
}

bool InitializeRenderer() {
#ifdef DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(
        (GLDEBUGPROC)GLMessageCallback,
        nullptr
    );
#endif

    paddleScale = glm::scale( glm::mat4(1.0f), glm::vec3(PADDLE_W, PADDLE_H, 1.0f) );
    ballScale   = glm::scale( glm::mat4(1.0f), glm::vec3(BALL_SIZE, BALL_SIZE, 1.0f) );

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

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

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
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

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
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

    shader = glCreateProgram();
    glAttachShader(shader, vert);
    glAttachShader(shader, frag);

    glLinkProgram(shader);

    glDetachShader(shader, frag);
    glDetachShader(shader, vert);
    glDeleteShader(vert);
    glDeleteShader(frag);

    glUseProgram(shader);

    f32 size = 1.0f;
    f32 hor  = ASPECT * size;

    glm::mat4 projection = glm::ortho(
        -hor, hor,
        -size, size,
        -10.0f, 10.0f
    );

    GLint projectionLoc = glGetUniformLocation(shader, "u_projection");
    glUniformMatrix4fv(
        projectionLoc,
        1, GL_FALSE,
        glm::value_ptr(projection)
    );

    transformLoc = glGetUniformLocation(shader, "u_transform");

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

    GLuint font_frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(
        font_frag, 1,
        &font_frag_src, nullptr
    );
    glCompileShader(font_frag);

    fontShader = glCreateProgram();
    glAttachShader(fontShader, font_vert);
    glAttachShader(fontShader, font_frag);

    glLinkProgram(fontShader);

    glDetachShader(fontShader, font_vert);
    glDetachShader(fontShader, font_frag);
    glDeleteShader(font_vert);
    glDeleteShader(font_frag);

    glUseProgram(fontShader);

    GLint fontProjectionLoc  = glGetUniformLocation(fontShader, "u_projection");
    glm::mat4 fontProjection = glm::ortho(0.0f, SCREEN_W, 0.0f, SCREEN_H, -1.0f, 1.0f);
    glUniformMatrix4fv(
        fontProjectionLoc, 1,
        GL_FALSE, glm::value_ptr(fontProjection)
    );

    GLint fontSamplerLoc = glGetUniformLocation(fontShader, "u_glyph");
    glUniform1i(fontSamplerLoc, 0);

    fontColorLoc = glGetUniformLocation(fontShader, "u_textColor");

    glGenVertexArrays(1, &fontVao);
    glBindVertexArray(fontVao);

    glGenBuffers(1, &fontVbo);
    glBindBuffer(GL_ARRAY_BUFFER, fontVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(f32) * 6 * 4, nullptr, GL_DYNAMIC_DRAW );

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(f32) * 4, 0);
    glEnableVertexAttribArray(0);

    return true;
}

// TODO: Rewrite text renderer
struct CharacterGL {
    u32 texture;
    i32 w; i32 h;
    i32 bearing_x;
    i32 bearing_y;
    u32 advance;
};
void RenderCharacter(const CharacterGL& character, f32 x, f32 y, f32 scale);
std::map<char, CharacterGL> characterMap;
bool fontLoaded = false;

void RendererLoadFont(const Font& font) {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for( u8 c = 0; c < 128; c++ ) {
        Glyph glyph = font.glyphs.at(c);
        CharacterGL character = {};
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

        characterMap.insert(std::pair<char, CharacterGL>( (char)c, character ));
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    fontLoaded = true;
}
glm::vec3 lastFontColor = glm::vec3(0.0f);
void RenderText(std::string text, f32 x, f32 y, f32 scale, UITextStyle textStyle, const glm::vec3& color) {
    if(!fontLoaded) { return; }
    glUseProgram(fontShader);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if( lastFontColor != color ) {
        glUniform3fv(fontColorLoc, 1, glm::value_ptr(color));
        lastFontColor = color;
    }

    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(fontVao);

    switch(textStyle) {
        case UITextStyle::NORMAL: {
            std::string::const_iterator c;
            for( c = text.begin(); c != text.end(); ++c ) {
                CharacterGL character = characterMap[*c];
                RenderCharacter(character, x, y, scale);
                x += (character.advance >> 6) * scale;
            }
        } break;
        case UITextStyle::REVERSE: {
            std::string::const_reverse_iterator c;
            for( c = text.rbegin(); c != text.rend(); ++c ) {
                CharacterGL character = characterMap[*c];
                RenderCharacter(character, x - (character.w + character.bearing_x) * scale, y, scale);
                x -= (character.advance >> 6) * scale;
            }
        } break;
    }
}
void RenderCharacter(const CharacterGL& character, f32 x, f32 y, f32 scale) {
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
    glBindBuffer(GL_ARRAY_BUFFER, fontVbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}
void RenderText(std::string text, f32 x, f32 y, f32 scale, UITextStyle textStyle) {
    RenderText(text, x, y, scale, textStyle, glm::vec3(1.0f));
}
void RenderText(std::string text, f32 x, f32 y, f32 scale) {
    RenderText(text, x, y, scale, UITextStyle::NORMAL);
}
void RenderText(const UITextElement& textElement) {
    RenderText(
        textElement.text,
        textElement.xPos,
        textElement.yPos,
        textElement.scale,
        textElement.style,
        textElement.color
    );
}

void ClearScreen() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

#endif