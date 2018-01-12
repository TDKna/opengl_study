//
// Created by NAOKI TSUZUKI on 2018/01/11.
//

#ifndef OPENGL_STUDY_RENDERER_H
#define OPENGL_STUDY_RENDERER_H

bool on_init();
void on_dispose();
bool on_surface_changed(int width, int height);
void on_update();
void on_draw();
void on_pause();
void on_resume();
void move_camera(int x, int y);
void rotate_camera(int rotation);

#endif //OPENGL_STUDY_RENDERER_H
