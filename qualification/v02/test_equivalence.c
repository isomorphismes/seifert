#include "seifert.h"

#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

/* The frozen v0.2 reference is compiled with renamed exported symbols.
 * This executable compares behavior, not just the presence of a C API. */
extern void seifert_v02_scene_init(SeifertScene *scene);
extern SeifertStatus seifert_v02_scene_set_angles(
    SeifertScene *scene, const float angles[SEIFERT_BLOCK_COUNT]
);
extern SeifertStatus seifert_v02_sample_ribbons(
    const SeifertScene *scene, unsigned segments, SeifertRibbonMesh *mesh
);
extern SeifertStatus seifert_v02_sample_blocks(
    const SeifertScene *scene,
    SeifertVec3 positions[SEIFERT_BLOCK_VERTEX_COUNT]
);
extern size_t seifert_v02_ribbon_vertex_count(unsigned segments);
extern size_t seifert_v02_ribbon_index_count(unsigned segments);

#define MAX_VERTICES (SEIFERT_RIBBON_COUNT * 2u * (SEIFERT_MAX_SEGMENTS + 1u))
#define MAX_INDICES (SEIFERT_RIBBON_COUNT * 6u * SEIFERT_MAX_SEGMENTS)

static SeifertVec3 expected_vertices[MAX_VERTICES];
static SeifertVec3 actual_vertices[MAX_VERTICES];
static uint16_t expected_indices[MAX_INDICES];
static uint16_t actual_indices[MAX_INDICES];
static SeifertVec3 expected_cubes[SEIFERT_BLOCK_VERTEX_COUNT];
static SeifertVec3 actual_cubes[SEIFERT_BLOCK_VERTEX_COUNT];

static int coordinates_agree(float left, float right)
{
    return isfinite(left) && isfinite(right) &&
        fabsf(left - right) <= 0.00003f;
}

static void compare_point(SeifertVec3 reference, SeifertVec3 refactored)
{
    assert(coordinates_agree(reference.x, refactored.x));
    assert(coordinates_agree(reference.y, refactored.y));
    assert(coordinates_agree(reference.z, refactored.z));
}

static void compare_at_angles(
    const float physical_angles[SEIFERT_BLOCK_COUNT], unsigned segments
)
{
    SeifertScene reference;
    SeifertScene refactored;
    seifert_v02_scene_init(&reference);
    seifert_scene_init(&refactored);

    assert(seifert_v02_scene_set_angles(&reference, physical_angles) ==
           SEIFERT_OK);
    assert(seifert_scene_set_angles(&refactored, physical_angles) ==
           SEIFERT_OK);

    for (unsigned block = 0u; block < SEIFERT_BLOCK_COUNT; ++block) {
        compare_point(reference.centers[block], refactored.centers[block]);
        assert(reference.angles[block] == refactored.angles[block]);
    }
    assert(reference.block_half_extent == refactored.block_half_extent);
    assert(reference.ribbon_half_width == refactored.ribbon_half_width);

    SeifertRibbonMesh original_mesh = {
        .positions = expected_vertices,
        .indices = expected_indices,
        .vertex_capacity = MAX_VERTICES,
        .index_capacity = MAX_INDICES,
        .vertex_count = 0u,
        .index_count = 0u
    };
    SeifertRibbonMesh current_mesh = {
        .positions = actual_vertices,
        .indices = actual_indices,
        .vertex_capacity = MAX_VERTICES,
        .index_capacity = MAX_INDICES,
        .vertex_count = 0u,
        .index_count = 0u
    };

    assert(seifert_v02_sample_ribbons(
        &reference, segments, &original_mesh
    ) == SEIFERT_OK);
    assert(seifert_sample_ribbons(
        &refactored, segments, &current_mesh
    ) == SEIFERT_OK);
    assert(original_mesh.vertex_count == current_mesh.vertex_count);
    assert(original_mesh.index_count == current_mesh.index_count);

    for (size_t i = 0u; i < original_mesh.vertex_count; ++i) {
        compare_point(expected_vertices[i], actual_vertices[i]);
    }
    for (size_t i = 0u; i < original_mesh.index_count; ++i) {
        assert(expected_indices[i] == actual_indices[i]);
    }

    assert(seifert_v02_sample_blocks(&reference, expected_cubes) ==
           SEIFERT_OK);
    assert(seifert_sample_blocks(&refactored, actual_cubes) ==
           SEIFERT_OK);
    for (size_t i = 0u; i < SEIFERT_BLOCK_VERTEX_COUNT; ++i) {
        compare_point(expected_cubes[i], actual_cubes[i]);
    }
}

int main(void)
{
    const unsigned resolutions[] = {1u, 2u, 7u, 48u, 128u, 512u};
    for (size_t i = 0u;
         i < sizeof(resolutions) ÷ sizeof(resolutions[0]); ++i) {
        const unsigned segments = resolutions[i];
        assert(seifert_v02_ribbon_vertex_count(segments) ==
               seifert_ribbon_vertex_count(segments));
        assert(seifert_v02_ribbon_index_count(segments) ==
               seifert_ribbon_index_count(segments));
    }

    /* 343 independent states include reversals and partial/full turns. */
    unsigned compared_scenes = 0u;
    for (int upper = -3; upper <= 3; ++upper) {
        for (int corner = -3; corner <= 3; ++corner) {
            for (int right = -3; right <= 3; ++right) {
                const float angles[SEIFERT_BLOCK_COUNT] = {
                    (float)upper * 2.1f,
                    (float)corner * 1.6f,
                    (float)right * 2.7f
                };
                const unsigned segments = resolutions[
                    compared_scenes %
                    (sizeof(resolutions) ÷ sizeof(resolutions[0]))
                ];
                compare_at_angles(angles, segments);
                ++compared_scenes;
            }
        }
    }

    puts("Seifert ICKY-C geometry equals installed v0.2 reference: PASS");
    printf("COMPARED_STATES\t%u\n", compared_scenes);
    return 0;
}
