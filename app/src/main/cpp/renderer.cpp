//
// Created by NAOKI TSUZUKI on 2018/01/11.
//
#include "renderer.h"
#include <stdlib.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <android/log.h>

#define STRINGIFY(A)  #A
#define LOG_TAG "OpenGLTest"
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define ARRAY_LENGTH(array) (sizeof(array) / sizeof(array[0]))

// シェーダプログラム
GLuint g_program;
// 頂点のシェーダ内アドレス情報
GLuint g_positionLocation;
// 頂点カラーのシェーダ内アドレス情報
GLuint g_colorLocation;
// 変換用行列のシェーダ内アドレス情報
GLuint g_projViewModel;

// プロジェクション行列
glm::mat4 g_matProj;
// ビュー行列
glm::mat4 g_matView;
// 回転角度
float g_rotation = 30.0f;

// 頂点シェーダ
const char *g_VertexShader = STRINGIFY(
    attribute vec4 position;
    attribute vec4 color;
    uniform mat4 projViewModel;

    varying vec4 v_fragmentColor;

    void main() {
        v_fragmentColor = color;
        gl_Position = projViewModel * position;
    }
);

// フラグメントシェーダ
const char *g_FragmentShader = STRINGIFY(
    precision mediump float;
    varying vec4 v_fragmentColor;
    void main() {
        gl_FragColor = v_fragmentColor;
    }
);

// 頂点情報
const GLfloat g_vertexData[][3] = {
    { -1.0f, -1.0f, -1.0f }, // 0: 左下奥
    {  1.0f, -1.0f, -1.0f }, // 1: 右下奥
    {  1.0f,  1.0f, -1.0f }, // 2: 右上奥
    { -1.0f,  1.0f, -1.0f }, // 3: 左上奥
    { -1.0f, -1.0f,  1.0f }, // 4: 左下手前
    {  1.0f, -1.0f,  1.0f }, // 5: 右下手前
    {  1.0f,  1.0f,  1.0f }, // 6: 右上手前
    { -1.0f,  1.0f,  1.0f }, // 7: 左上手前
};

// カラー情報
const GLfloat g_colorData[][4] = {
    /*
    { 0.0f, 0.0f, 0.0f, 1.0f }, // 0: 左下奥
    { 1.0f, 0.0f, 0.0f, 1.0f }, // 1: 右下奥
    { 0.0f, 1.0f, 0.0f, 1.0f }, // 2: 右上奥
    { 0.0f, 0.0f, 1.0f, 1.0f }, // 3: 左上奥
    { 1.0f, 1.0f, 0.0f, 1.0f }, // 4: 左下手前
    { 0.0f, 1.0f, 1.0f, 1.0f }, // 5: 右下手前
    { 1.0f, 0.0f, 1.0f, 1.0f }, // 6: 右上手前
    { 1.0f, 1.0f, 1.0f, 1.0f }, // 7: 左上手前
    */
    { 1.0f, 0.0f, 0.0f, 1.0f }, // 0: 左下奥
    { 1.0f, 0.0f, 0.0f, 1.0f }, // 1: 右下奥
    { 1.0f, 0.0f, 0.0f, 1.0f }, // 2: 右上奥
    { 1.0f, 0.0f, 0.0f, 1.0f }, // 3: 左上奥
    { 1.0f, 0.0f, 0.0f, 1.0f }, // 4: 左下手前
    { 1.0f, 0.0f, 0.0f, 1.0f }, // 5: 右下手前
    { 1.0f, 0.0f, 0.0f, 1.0f }, // 6: 右上手前
    { 1.0f, 0.0f, 0.0f, 1.0f }, // 7: 左上手前
};

// 面インデックス
const unsigned short g_indexData[][3] = {
    { 0, 3, 1 }, // 奥1
    { 1, 3, 2 }, // 奥2
    { 0, 3, 4 }, // 左1
    { 4, 3, 7 }, // 左2
    { 1, 5, 2 }, // 右1
    { 2, 5, 6 }, // 右2
    { 1, 0, 5 }, // 下1
    { 5, 0, 4 }, // 下2
    { 3, 2, 7 }, // 上1
    { 7, 2, 6 }, // 上2
    { 5, 4, 6 }, // 手前1
    { 6, 4, 7 }, // 手前2
};

void LOGEGL() {
    GLint logLength = 0;
    glGetProgramiv(g_program, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength) {
        char *logBuf = (char*)malloc(logLength);
        if (logBuf) {
            glGetProgramInfoLog(g_program, logLength, NULL, logBuf);
            LOGE("OpenGL情報：%s", logBuf);
            free(logBuf);
        }
    }
}

GLuint loadShader(GLenum shaderType, const char *shaderCode) {
    LOGD("%d シェーダコンパイル：開始", shaderType);
    GLuint shader = glCreateShader(shaderType);
    if (shader == NULL) {
        LOGD("%d シェーダコンパイル：");
        return NULL;
    }

    glShaderSource(shader, 1, &shaderCode, NULL);
    glCompileShader(shader);
    LOGD("%d シェーダコンパイル：終了", shaderType);

    LOGD("%d シェーダコンパイル確認：開始", shaderType);
    GLint compiled = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_FALSE) {
        LOGE("シェーダコンパイルエラー: shaderType=%d", shaderType);
        LOGEGL();
        glDeleteShader(shader);
        shader = NULL;
    }
    LOGD("%d シェーダコンパイル確認：終了", shaderType);
    return shader;
}

bool on_init() {
    // 頂点シェーダを作成
    LOGD("頂点シェーダ作成：開始");
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, g_VertexShader);
    if (!vertexShader) {
        LOGE("頂点シェーダの作成に失敗しました！");
        return false;
    }
    LOGD("頂点シェーダ作成：終了");

    // フラグメントシェーダを作成
    LOGD("フラグメントシェーダ作成：開始");
    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, g_FragmentShader);
    if (!fragmentShader) {
        LOGE("フラグメントシェーダの作成に失敗しました！");
        glDeleteShader(vertexShader);
        return false;
    }
    LOGD("フラグメントシェーダ作成：終了");

    // シェーダプログラムを作成
    LOGD("シェーダプログラム作成：開始");
    g_program = glCreateProgram();
    if (!g_program) {
        LOGE("シェーダプログラムの作成に失敗しました！");
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }
    LOGD("シェーダプログラム作成：終了");

    // シェーダープログラムをリンク
    LOGD("シェーダプログラムリンク：開始");
    glAttachShader(g_program, vertexShader);
    glAttachShader(g_program, fragmentShader);
    glLinkProgram(g_program);
    LOGD("シェーダプログラムリンク：終了");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    LOGD("シェーダリンク確認：開始");
    GLint linkStatus = GL_FALSE;
    glGetProgramiv(g_program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE) {
        LOGE("シェーダプログラムのリンクに失敗しました！");
        LOGEGL();
        glDeleteProgram(g_program);
        return false;
    }
    glUseProgram(g_program);
    LOGD("シェーダリンク確認：終了");

    LOGD("パラメータ確保：開始");
    // 頂点座標と頂点カラーを格納する場所をそれぞれ確保
    g_positionLocation = glGetAttribLocation(g_program, "position");
    glEnableVertexAttribArray(g_positionLocation);
    g_colorLocation = glGetAttribLocation(g_program, "color");
    glEnableVertexAttribArray(g_colorLocation);
    g_projViewModel = glGetUniformLocation(g_program, "projViewModel");
    LOGD("パラメータ確保：終了");
    return true;
}

void on_dispose() {
    if (g_program) {
        glUseProgram(NULL);
        glDeleteProgram(g_program);
        g_program = NULL;
    }
}

bool on_surface_changed(int width, int height) {
    // 変換行列の更新
    g_matProj = glm::perspective(glm::radians(60.0f), (float)width / (float)height, 0.1f, 10.0f);
    g_matView = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    return true;
}

void on_update() {
    g_rotation += 0.02f;
}

void on_draw() {
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glUseProgram(g_program);

    glm::mat4 matModel = glm::rotate(glm::mat4(), g_rotation, glm::vec3(0.0f, 1.0f, 1.0f));
    glm::mat4 matProjView = g_matProj * g_matView * matModel;

    glUniformMatrix4fv(g_projViewModel, 1, 0, &(matProjView[0][0]));

    glVertexAttribPointer(g_positionLocation, 3, GL_FLOAT, false, 0, g_vertexData);
    glVertexAttribPointer(g_colorLocation, 4, GL_FLOAT, false, 0, g_colorData);

    glDrawElements(GL_TRIANGLES, sizeof(g_indexData) / sizeof(unsigned short), GL_UNSIGNED_SHORT, g_indexData);
}

void on_pause() {
}

void on_resume() {
}