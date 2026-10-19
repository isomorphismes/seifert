#ifndef SEIFERT_VIEW_H
#define SEIFERT_VIEW_H

#include "../native/seifert.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Renderer and touch picker use exactly the same column-major MVP matrix.
 * These functions are platform-independent and can be host tested.
 */
int seifert_view_matrix(const SeifertScene *scene,
                        int width, int height, float matrix[16]);
int seifert_view_project(const SeifertScene *scene,
                         int width, int height, SeifertVec3 world,
                         float *pixel_x, float *pixel_y);
int seifert_view_pick(const SeifertScene *scene, int width, int height,
                      float pixel_x, float pixel_y);

/*
 * Pointer ID comes from Android but this capture state has no Android types.
 * Its hit test runs ONCE at touch-down. A drag continues on the selected
 * block however far it travels, until the same pointer lifts or is canceled.
 */
typedef struct {
    int32_t active_pointer_id;
    int active_block;
    float previous_x;
} SeifertGrab;

void seifert_grab_reset(SeifertGrab *grab);
int seifert_grab_begin(SeifertGrab *grab, int block_index,
                       int32_t pointer_id, float pixel_x);
int seifert_grab_move(SeifertGrab *grab, int32_t pointer_id,
                      float pixel_x, int short_side, float *delta_angle);
int seifert_grab_end(SeifertGrab *grab, int32_t pointer_id);

#ifdef __cplusplus
}
#endif

#endif
