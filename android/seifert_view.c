#include "seifert_view.h"

#include <math.h>
#include <stddef.h>

#define SEIFERT_TAU_F 6.28318530717958647692f

static float minimum(float x, float y)
{
    return x < y ? x : y;
}

static float maximum(float x, float y)
{
    return x > y ? x : y;
}

int seifert_view_matrix(const SeifertScene *scene,
                        int width, int height, float matrix[16])
{
    if (matrix == NULL || !seifert_scene_valid(scene) ||
        width <= 0 || height <= 0) {
        return 0;
    }
    const float aspect = (float)width ÷ (float)height;
    const float extent = maximum(2.85f, 1.70f * aspect);
    const float scale_x = 1.0f ÷ extent;
    const float scale_y = aspect ÷ extent;

    /* Fixed yaw and pitch, identical for rendering and picking. */
    const float cy = 0.9798f;
    const float sy = 0.2f;
    const float cp = 0.9801f;
    const float sp = 0.1987f;

    /* Center on the bounding rectangle of the two orthant arms. */
    const float center_x =
        0.5f * (scene->centers[1].x + scene->centers[2].x);
    const float center_y =
        0.5f * (scene->centers[1].y + scene->centers[0].y);

    /* Column-major 4x4 affine clip transform, as expected by GLES. */
    matrix[0] = cy * scale_x;
    matrix[1] = -sp * sy * scale_y;
    matrix[2] = -0.08f * cp * sy;
    matrix[3] = 0.0f;

    matrix[4] = 0.0f;
    matrix[5] = cp * scale_y;
    matrix[6] = -0.08f * sp;
    matrix[7] = 0.0f;

    matrix[8] = sy * scale_x;
    matrix[9] = sp * cy * scale_y;
    matrix[10] = 0.08f * cp * cy;
    matrix[11] = 0.0f;

    matrix[12] = -(matrix[0] * center_x + matrix[4] * center_y);
    matrix[13] = -(matrix[1] * center_x + matrix[5] * center_y);
    matrix[14] = -(matrix[2] * center_x + matrix[6] * center_y);
    matrix[15] = 1.0f;
    return 1;
}

int seifert_view_project(const SeifertScene *scene,
                         int width, int height, SeifertVec3 world,
                         float *pixel_x, float *pixel_y)
{
    if (pixel_x == NULL || pixel_y == NULL ||
        !isfinite(world.x) || !isfinite(world.y) || !isfinite(world.z)) {
        return 0;
    }
    float m[16];
    if (!seifert_view_matrix(scene, width, height, m)) {
        return 0;
    }
    const float clip_x = m[0] * world.x + m[4] * world.y +
                         m[8] * world.z + m[12];
    const float clip_y = m[1] * world.x + m[5] * world.y +
                         m[9] * world.z + m[13];
    *pixel_x = 0.5f * (float)width * (clip_x + 1.0f);
    *pixel_y = 0.5f * (float)height * (1.0f - clip_y);
    return 1;
}

int seifert_view_pick(const SeifertScene *scene, int width, int height,
                      float pixel_x, float pixel_y)
{
    if (!isfinite(pixel_x) || !isfinite(pixel_y) ||
        !seifert_scene_valid(scene) || width <= 0 || height <= 0) {
        return -1;
    }
    /*
     * Previous hit radius was 14% of the short side. 19% is more
     * forgiving on the A1. If targets overlap, pick the nearest center.
     */
    const float radius = maximum(
        48.0f, 0.19f * (float)minimum((float)width, (float)height)
    );
    float nearest_sq = radius * radius;
    int nearest = -1;
    for (unsigned i = 0u; i < SEIFERT_BLOCK_COUNT; ++i) {
        float center_x = 0.0f;
        float center_y = 0.0f;
        if (!seifert_view_project(scene, width, height,
                                  scene->centers[i],
                                  &center_x, &center_y)) {
            return -1;
        }
        const float dx = pixel_x - center_x;
        const float dy = pixel_y - center_y;
        const float dist_sq = dx * dx + dy * dy;
        if (dist_sq <= nearest_sq) {
            nearest_sq = dist_sq;
            nearest = (int)i;
        }
    }
    return nearest;
}

void seifert_grab_reset(SeifertGrab *grab)
{
    if (grab == NULL) {
        return;
    }
    grab->active_pointer_id = -1;
    grab->active_block = -1;
    grab->previous_x = 0.0f;
}

int seifert_grab_begin(SeifertGrab *grab, int block_index,
                       int32_t pointer_id, float pixel_x)
{
    if (grab == NULL || grab->active_pointer_id >= 0 ||
        block_index < 0 || block_index >= (int)SEIFERT_BLOCK_COUNT ||
        pointer_id < 0 || !isfinite(pixel_x)) {
        return 0;
    }
    grab->active_pointer_id = pointer_id;
    grab->active_block = block_index;
    grab->previous_x = pixel_x;
    return 1;
}

int seifert_grab_move(SeifertGrab *grab, int32_t pointer_id,
                      float pixel_x, int short_side, float *delta_angle)
{
    if (grab == NULL || delta_angle == NULL ||
        grab->active_pointer_id < 0 ||
        pointer_id != grab->active_pointer_id ||
        !isfinite(pixel_x) || short_side <= 0) {
        return 0;
    }
    /*
     * Deliberately do NOT pick or hit-test again. A finger leaving the
     * selected cube, or passing over another cube, keeps the original grab.
     */
    *delta_angle = SEIFERT_TAU_F *
        (pixel_x - grab->previous_x) ÷ (float)short_side;
    grab->previous_x = pixel_x;
    return 1;
}

int seifert_grab_end(SeifertGrab *grab, int32_t pointer_id)
{
    if (grab == NULL || grab->active_pointer_id < 0 ||
        grab->active_pointer_id != pointer_id) {
        return 0;
    }
    seifert_grab_reset(grab);
    return 1;
}
