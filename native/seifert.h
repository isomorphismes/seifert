#ifndef SEIFERT_H
#define SEIFERT_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SEIFERT_BLOCK_COUNT 3u
#define SEIFERT_RIBBON_COUNT 2u
#define SEIFERT_MAX_SEGMENTS 512u
#define SEIFERT_BLOCK_VERTEX_COUNT (SEIFERT_BLOCK_COUNT * 36u)

typedef struct {
    float x;
    float y;
    float z;
} SeifertVec3;

typedef struct {
    /* Centers lie on a positive-x, positive-y line, with z = 0. */
    SeifertVec3 centers[SEIFERT_BLOCK_COUNT];
    float block_half_extent;
    float ribbon_half_width;
    float angles[SEIFERT_BLOCK_COUNT]; /* signed and unwrapped */
} SeifertScene;

typedef enum {
    SEIFERT_OK = 0,
    SEIFERT_INVALID_ARGUMENT = 1,
    SEIFERT_BUFFER_TOO_SMALL = 2,
    SEIFERT_LIMIT = 3
} SeifertStatus;

typedef struct {
    SeifertVec3 *positions;
    uint16_t *indices; /* compatible with GLES2 */
    size_t vertex_capacity;
    size_t index_capacity;
    size_t vertex_count;
    size_t index_count;
} SeifertRibbonMesh;

void seifert_scene_init(SeifertScene *scene);
SeifertStatus seifert_scene_turn(
    SeifertScene *scene, unsigned block_index, float delta_radians
);
SeifertStatus seifert_scene_set_angles(
    SeifertScene *scene, const float angles[SEIFERT_BLOCK_COUNT]
);
int seifert_scene_valid(const SeifertScene *scene);

size_t seifert_ribbon_vertex_count(unsigned segments);
size_t seifert_ribbon_index_count(unsigned segments);

/* Neither function allocates; all geometry and rotation lives below Android. */
SeifertStatus seifert_sample_ribbons(
    const SeifertScene *scene, unsigned segments, SeifertRibbonMesh *mesh
);
SeifertStatus seifert_sample_blocks(
    const SeifertScene *scene,
    SeifertVec3 positions[SEIFERT_BLOCK_VERTEX_COUNT]
);

#ifdef __cplusplus
}
#endif

#endif
