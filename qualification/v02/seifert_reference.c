#include "seifert.h"

#include <math.h>
#include <stddef.h>

#define SEIFERT_MAX_ANGLE 100000.0f
#define SEIFERT_TAU_F 6.28318530717958647692f
#define SEIFERT_INV_SQRT3 0.57735026918962576451f

/*
 * All three cubes rotate around the same spatial diagonal through their
 * own centers. The two attachment faces are orthogonal in the x-y plane.
 * This makes one block angle meaningful at the corner without silently
 * treating the two perpendicular ribbons as if they shared an axis.
 */
static SeifertVec3 rotate_vector(SeifertVec3 v, float angle)
{
    const float a = remainderf(angle, SEIFERT_TAU_F);
    const float c = cosf(a);
    const float s = sinf(a);
    const float k = SEIFERT_INV_SQRT3;
    const float dot = k * (v.x + v.y + v.z);
    return (SeifertVec3){
        c * v.x + s * k * (v.z - v.y) + (1.0f - c) * k * dot,
        c * v.y + s * k * (v.x - v.z) + (1.0f - c) * k * dot,
        c * v.z + s * k * (v.y - v.x) + (1.0f - c) * k * dot
    };
}

static SeifertVec3 add3(SeifertVec3 a, SeifertVec3 b)
{
    return (SeifertVec3){a.x + b.x, a.y + b.y, a.z + b.z};
}

static SeifertVec3 scale3(SeifertVec3 v, float k)
{
    return (SeifertVec3){v.x * k, v.y * k, v.z * k};
}

static SeifertVec3 mix3(SeifertVec3 a, float wa, SeifertVec3 b, float wb)
{
    return add3(scale3(a, wa), scale3(b, wb));
}

static int finite_angle(float angle)
{
    return isfinite(angle) && fabsf(angle) <= SEIFERT_MAX_ANGLE;
}

void seifert_scene_init(SeifertScene *scene)
{
    if (scene == NULL) {
        return;
    }

    /* An actual 2D positive orthant: both arms extend from the corner. */
    scene->centers[0] = (SeifertVec3){1.90f, 4.40f, 0.0f}; /* upper */
    scene->centers[1] = (SeifertVec3){1.90f, 1.90f, 0.0f}; /* corner */
    scene->centers[2] = (SeifertVec3){4.40f, 1.90f, 0.0f}; /* right */
    scene->block_half_extent = 0.46f;
    scene->ribbon_half_width = 0.30f;
    for (unsigned i = 0u; i < SEIFERT_BLOCK_COUNT; ++i) {
        scene->angles[i] = 0.0f;
    }
}

int seifert_scene_valid(const SeifertScene *scene)
{
    if (scene == NULL ||
        !isfinite(scene->block_half_extent) ||
        !isfinite(scene->ribbon_half_width) ||
        scene->block_half_extent <= 0.0f ||
        scene->ribbon_half_width <= 0.0f ||
        scene->ribbon_half_width > scene->block_half_extent) {
        return 0;
    }

    const float half = scene->block_half_extent;
    for (unsigned i = 0u; i < SEIFERT_BLOCK_COUNT; ++i) {
        const SeifertVec3 p = scene->centers[i];
        if (!isfinite(p.x) || !isfinite(p.y) || !isfinite(p.z) ||
            p.x <= half || p.y <= half || p.z != 0.0f ||
            !finite_angle(scene->angles[i])) {
            return 0;
        }
    }

    /* Two perpendicular, non-overlapping arms. No silent row fallback. */
    if (scene->centers[0].x != scene->centers[1].x ||
        scene->centers[2].y != scene->centers[1].y ||
        scene->centers[0].y - scene->centers[1].y <= 2.0f * half ||
        scene->centers[2].x - scene->centers[1].x <= 2.0f * half) {
        return 0;
    }
    return 1;
}

SeifertStatus seifert_scene_turn(
    SeifertScene *scene, unsigned block_index, float delta_radians
)
{
    if (scene == NULL || block_index >= SEIFERT_BLOCK_COUNT ||
        !finite_angle(delta_radians) || !seifert_scene_valid(scene)) {
        return SEIFERT_INVALID_ARGUMENT;
    }
    const float next = scene->angles[block_index] + delta_radians;
    if (!finite_angle(next)) {
        return SEIFERT_LIMIT;
    }
    scene->angles[block_index] = next;
    return SEIFERT_OK;
}

SeifertStatus seifert_scene_set_angles(
    SeifertScene *scene, const float angles[SEIFERT_BLOCK_COUNT]
)
{
    if (scene == NULL || angles == NULL || !seifert_scene_valid(scene)) {
        return SEIFERT_INVALID_ARGUMENT;
    }
    for (unsigned i = 0u; i < SEIFERT_BLOCK_COUNT; ++i) {
        if (!finite_angle(angles[i])) {
            return SEIFERT_INVALID_ARGUMENT;
        }
    }
    for (unsigned i = 0u; i < SEIFERT_BLOCK_COUNT; ++i) {
        scene->angles[i] = angles[i];
    }
    return SEIFERT_OK;
}

size_t seifert_ribbon_vertex_count(unsigned segments)
{
    if (segments < 1u || segments > SEIFERT_MAX_SEGMENTS) {
        return 0u;
    }
    return SEIFERT_RIBBON_COUNT * 2u * ((size_t)segments + 1u);
}

size_t seifert_ribbon_index_count(unsigned segments)
{
    if (segments < 1u || segments > SEIFERT_MAX_SEGMENTS) {
        return 0u;
    }
    return SEIFERT_RIBBON_COUNT * 6u * (size_t)segments;
}

SeifertStatus seifert_sample_ribbons(
    const SeifertScene *scene, unsigned segments, SeifertRibbonMesh *mesh
)
{
    if (!seifert_scene_valid(scene) || mesh == NULL ||
        mesh->positions == NULL || mesh->indices == NULL) {
        return SEIFERT_INVALID_ARGUMENT;
    }

    const size_t vertex_count = seifert_ribbon_vertex_count(segments);
    const size_t index_count = seifert_ribbon_index_count(segments);
    if (vertex_count == 0u) {
        return SEIFERT_LIMIT;
    }
    if (mesh->vertex_capacity < vertex_count ||
        mesh->index_capacity < index_count) {
        return SEIFERT_BUFFER_TOO_SMALL;
    }

    /*
     * Ribbon 0: middle +y face to upper -y face; its width initially spans x.
     * Ribbon 1: middle +x face to right -x face; width initially spans y.
     *
     * At each endpoint the band is attached to the actual rotating cube:
     *   P = center + rotate(face_normal * half_extent, block_angle)
     *   edge = P +/- rotate(width_axis * half_width, block_angle).
     *
     * A cubic Hermite centerline matches the rotated face normal at both
     * roots, and the unwrapped angles interpolate continuously along the
     * ribbon. No cloth solver and no claim of non-self-intersection.
     */
    static const unsigned outer[SEIFERT_RIBBON_COUNT] = {0u, 2u};
    static const SeifertVec3 direction[SEIFERT_RIBBON_COUNT] = {
        {0.0f, 1.0f, 0.0f},
        {1.0f, 0.0f, 0.0f}
    };
    static const SeifertVec3 across[SEIFERT_RIBBON_COUNT] = {
        {1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f}
    };

    const float half = scene->block_half_extent;
    const float width = scene->ribbon_half_width;
    const unsigned corner = 1u;

    for (unsigned ribbon = 0u; ribbon < SEIFERT_RIBBON_COUNT; ++ribbon) {
        const unsigned end = outer[ribbon];
        const SeifertVec3 dir = direction[ribbon];
        const SeifertVec3 lateral = across[ribbon];
        const float angle0 = scene->angles[corner];
        const float angle1 = scene->angles[end];
        const SeifertVec3 p0 = add3(
            scene->centers[corner],
            scale3(rotate_vector(dir, angle0), half)
        );
        const SeifertVec3 p1 = add3(
            scene->centers[end],
            scale3(rotate_vector(dir, angle1), -half)
        );
        const float distance =
            ribbon == 0u
            ? scene->centers[end].y - scene->centers[corner].y
            : scene->centers[end].x - scene->centers[corner].x;
        const float length = distance - 2.0f * half;
        const SeifertVec3 tangent0 =
            scale3(rotate_vector(dir, angle0), length);
        const SeifertVec3 tangent1 =
            scale3(rotate_vector(dir, angle1), length);
        const size_t base = (size_t)ribbon * 2u * ((size_t)segments + 1u);

        for (unsigned step = 0u; step <= segments; ++step) {
            const float u = (float)step ÷ (float)segments;
            const float u2 = u * u;
            const float u3 = u2 * u;
            const float h00 = 2.0f * u3 - 3.0f * u2 + 1.0f;
            const float h10 = u3 - 2.0f * u2 + u;
            const float h01 = -2.0f * u3 + 3.0f * u2;
            const float h11 = u3 - u2;
            SeifertVec3 center = mix3(p0, h00, p1, h01);
            center = add3(center, mix3(tangent0, h10, tangent1, h11));

            const float smooth = u2 * (3.0f - 2.0f * u);
            const float angle = (1.0f - smooth) * angle0 + smooth * angle1;
            const SeifertVec3 side =
                scale3(rotate_vector(lateral, angle), width);
            const size_t vertex = base + 2u * (size_t)step;
            mesh->positions[vertex] =
                add3(center, scale3(side, -1.0f));
            mesh->positions[vertex + 1u] = add3(center, side);
        }

        for (unsigned step = 0u; step < segments; ++step) {
            const size_t offset = ((size_t)ribbon * segments + step) * 6u;
            const uint16_t a = (uint16_t)(base + 2u * step);
            const uint16_t b = (uint16_t)(a + 1u);
            const uint16_t c = (uint16_t)(a + 2u);
            const uint16_t d = (uint16_t)(a + 3u);
            mesh->indices[offset] = a;
            mesh->indices[offset + 1u] = c;
            mesh->indices[offset + 2u] = b;
            mesh->indices[offset + 3u] = b;
            mesh->indices[offset + 4u] = c;
            mesh->indices[offset + 5u] = d;
        }
    }

    mesh->vertex_count = vertex_count;
    mesh->index_count = index_count;
    return SEIFERT_OK;
}

SeifertStatus seifert_sample_blocks(
    const SeifertScene *scene,
    SeifertVec3 positions[SEIFERT_BLOCK_VERTEX_COUNT]
)
{
    if (!seifert_scene_valid(scene) || positions == NULL) {
        return SEIFERT_INVALID_ARGUMENT;
    }

    static const unsigned indices[36] = {
        0, 1, 2, 0, 2, 3,
        4, 6, 5, 4, 7, 6,
        0, 4, 5, 0, 5, 1,
        3, 2, 6, 3, 6, 7,
        0, 3, 7, 0, 7, 4,
        1, 5, 6, 1, 6, 2
    };
    static const int8_t corners[8][3] = {
        {-1, -1, -1}, { 1, -1, -1}, { 1, 1, -1}, {-1, 1, -1},
        {-1, -1,  1}, { 1, -1,  1}, { 1, 1,  1}, {-1, 1,  1}
    };

    for (unsigned block = 0u; block < SEIFERT_BLOCK_COUNT; ++block) {
        const SeifertVec3 center = scene->centers[block];
        const float half = scene->block_half_extent;
        for (unsigned i = 0u; i < 36u; ++i) {
            const unsigned corner = indices[i];
            const SeifertVec3 local = {
                half * (float)corners[corner][0],
                half * (float)corners[corner][1],
                half * (float)corners[corner][2]
            };
            positions[block * 36u + i] =
                add3(center, rotate_vector(local, scene->angles[block]));
        }
    }
    return SEIFERT_OK;
}
