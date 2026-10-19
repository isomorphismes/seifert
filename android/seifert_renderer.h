#ifndef SEIFERT_RENDERER_H
#define SEIFERT_RENDERER_H

#include "../native/seifert.h"

#ifdef __cplusplus
extern "C" {
#endif

/* EGL/context ownership remains with NativeActivity; geometry is native C. */
int seifert_renderer_start(int width, int height, int gles_major);
void seifert_renderer_resize(int width, int height);
void seifert_renderer_stop(void);
void seifert_renderer_draw(void);

int seifert_renderer_turn_block(unsigned block_index, float radians);
int seifert_renderer_set_angles(const float angles[SEIFERT_BLOCK_COUNT]);
void seifert_renderer_get_angles(float angles[SEIFERT_BLOCK_COUNT]);

/* Pointer positions use Android top-left pixel coordinates; -1 means miss. */
int seifert_renderer_pick_block(float pixel_x, float pixel_y);

#ifdef __cplusplus
}
#endif

#endif
