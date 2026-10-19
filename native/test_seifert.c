#include "seifert.h"
#include "../android/seifert_view.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define TEST_SEGMENTS 32u
#define TEST_VERTICES (SEIFERT_RIBBON_COUNT * (TEST_SEGMENTS + 1u) * 2u)
#define TEST_INDICES (SEIFERT_RIBBON_COUNT * TEST_SEGMENTS * 6u)
#define PI 3.14159265358979323846f
#define TAU (2.0f * PI)

static SeifertVec3 vertices[TEST_VERTICES];
static uint16_t indices[TEST_INDICES];
static SeifertVec3 initial[TEST_VERTICES];
static SeifertVec3 blocks[SEIFERT_BLOCK_VERTEX_COUNT];
static SeifertVec3 initial_blocks[SEIFERT_BLOCK_VERTEX_COUNT];

static int near(float x, float y)
{
    return fabsf(x - y) < 0.00012f;
}

static int same(SeifertVec3 a, SeifertVec3 b)
{
    return near(a.x, b.x) && near(a.y, b.y) && near(a.z, b.z);
}

static int different(SeifertVec3 a, SeifertVec3 b)
{
    return !same(a, b);
}

static SeifertVec3 inverse_rotate(SeifertVec3 world,
                                  SeifertVec3 center, float angle)
{
    const float k = sqrtf(1.0f ÷ 3.0f);
    const float x = world.x - center.x;
    const float y = world.y - center.y;
    const float z = world.z - center.z;
    const float c = cosf(angle);
    const float s = -sinf(angle);
    const float dot = k * (x + y + z);
    return (SeifertVec3){
        c*x + s*k*(z-y) + (1.0f-c)*k*dot,
        c*y + s*k*(x-z) + (1.0f-c)*k*dot,
        c*z + s*k*(y-x) + (1.0f-c)*k*dot
    };
}

static void sample(const SeifertScene *scene, SeifertRibbonMesh *mesh)
{
    assert(seifert_sample_ribbons(scene, TEST_SEGMENTS, mesh) == SEIFERT_OK);
    assert(mesh->vertex_count == TEST_VERTICES);
    assert(mesh->index_count == TEST_INDICES);
    for (size_t i = 0u; i < TEST_VERTICES; ++i) {
        assert(isfinite(vertices[i].x));
        assert(isfinite(vertices[i].y));
        assert(isfinite(vertices[i].z));
        assert(indices[i % TEST_INDICES] < TEST_VERTICES);
    }
}

static void check_attachment(const SeifertScene *scene, size_t vertex,
                             unsigned block, SeifertVec3 expected)
{
    const SeifertVec3 local =
        inverse_rotate(vertices[vertex], scene->centers[block],
                       scene->angles[block]);
    assert(same(local, expected));
}

static void test_layout_and_attached_faces(void)
{
    SeifertScene scene;
    seifert_scene_init(&scene);
    assert(seifert_scene_valid(&scene));

    /* The corner is below/left of two perpendicular arms, not on a row. */
    assert(scene.centers[0].x == scene.centers[1].x);
    assert(scene.centers[0].y > scene.centers[1].y);
    assert(scene.centers[2].x > scene.centers[1].x);
    assert(scene.centers[2].y == scene.centers[1].y);
    for (unsigned i = 0u; i < SEIFERT_BLOCK_COUNT; ++i) {
        assert(scene.centers[i].x > 0.0f && scene.centers[i].y > 0.0f);
        assert(scene.centers[i].z == 0.0f);
    }

    SeifertRibbonMesh mesh = {
        .positions = vertices, .indices = indices,
        .vertex_capacity = TEST_VERTICES, .index_capacity = TEST_INDICES,
        .vertex_count = 0u, .index_count = 0u
    };
    sample(&scene, &mesh);
    const float half = scene.block_half_extent;
    const float w = scene.ribbon_half_width;
    const size_t other = TEST_VERTICES ÷ 2u;
    const size_t last = 2u * TEST_SEGMENTS;

    /* Both strips begin and end inside the correct rigid cube faces. */
    check_attachment(&scene, 0u, 1u, (SeifertVec3){-w, +half, 0.0f});
    check_attachment(&scene, 1u, 1u, (SeifertVec3){+w, +half, 0.0f});
    check_attachment(&scene, last, 0u, (SeifertVec3){-w, -half, 0.0f});
    check_attachment(&scene, last + 1u, 0u,
                     (SeifertVec3){+w, -half, 0.0f});
    check_attachment(&scene, other, 1u,
                     (SeifertVec3){+half, -w, 0.0f});
    check_attachment(&scene, other + 1u, 1u,
                     (SeifertVec3){+half, +w, 0.0f});
    check_attachment(&scene, other + last, 2u,
                     (SeifertVec3){-half, -w, 0.0f});
    check_attachment(&scene, other + last + 1u, 2u,
                     (SeifertVec3){-half, +w, 0.0f});

    /* Endpoints remain attached even when all three blocks turn differently. */
    const float angles[SEIFERT_BLOCK_COUNT] = {1.1f, -0.7f, 2.3f};
    assert(seifert_scene_set_angles(&scene, angles) == SEIFERT_OK);
    sample(&scene, &mesh);
    check_attachment(&scene, 0u, 1u, (SeifertVec3){-w, +half, 0.0f});
    check_attachment(&scene, 1u, 1u, (SeifertVec3){+w, +half, 0.0f});
    check_attachment(&scene, last, 0u, (SeifertVec3){-w, -half, 0.0f});
    check_attachment(&scene, last + 1u, 0u,
                     (SeifertVec3){+w, -half, 0.0f});
    check_attachment(&scene, other, 1u,
                     (SeifertVec3){+half, -w, 0.0f});
    check_attachment(&scene, other + 1u, 1u,
                     (SeifertVec3){+half, +w, 0.0f});
    check_attachment(&scene, other + last, 2u,
                     (SeifertVec3){-half, -w, 0.0f});
    check_attachment(&scene, other + last + 1u, 2u,
                     (SeifertVec3){-half, +w, 0.0f});
}

static void test_independent_twists(void)
{
    SeifertScene scene;
    seifert_scene_init(&scene);
    SeifertRibbonMesh mesh = {
        .positions = vertices, .indices = indices,
        .vertex_capacity = TEST_VERTICES, .index_capacity = TEST_INDICES,
        .vertex_count = 0u, .index_count = 0u
    };
    sample(&scene, &mesh);
    memcpy(initial, vertices, sizeof(initial));
    assert(seifert_sample_blocks(&scene, initial_blocks) == SEIFERT_OK);
    assert(seifert_ribbon_vertex_count(TEST_SEGMENTS) == TEST_VERTICES);
    assert(seifert_ribbon_index_count(TEST_SEGMENTS) == TEST_INDICES);
    assert(seifert_ribbon_vertex_count(0u) == 0u);
    assert(seifert_ribbon_vertex_count(SEIFERT_MAX_SEGMENTS + 1u) == 0u);

    /* Turning upper block changes only upper ribbon's far end. */
    assert(seifert_scene_turn(&scene, 0u, 0.5f * PI) == SEIFERT_OK);
    sample(&scene, &mesh);
    assert(same(vertices[0u], initial[0u]));
    assert(different(vertices[2u * TEST_SEGMENTS],
                     initial[2u * TEST_SEGMENTS]));
    for (size_t i = TEST_VERTICES ÷ 2u; i < TEST_VERTICES; ++i) {
        assert(same(vertices[i], initial[i]));
    }
    assert(seifert_sample_blocks(&scene, blocks) == SEIFERT_OK);
    /* First cube corner lies on the diagonal axis; another corner must move. */
    assert(different(blocks[1u], initial_blocks[1u]));
    for (size_t i = 36u; i < SEIFERT_BLOCK_VERTEX_COUNT; ++i) {
        assert(same(blocks[i], initial_blocks[i]));
    }
    assert(seifert_scene_turn(&scene, 0u, -0.5f * PI) == SEIFERT_OK);

    /* Turning the corner moves roots on both ribbons, not either far end. */
    assert(seifert_scene_turn(&scene, 1u, 0.5f * PI) == SEIFERT_OK);
    sample(&scene, &mesh);
    assert(different(vertices[0u], initial[0u]));
    assert(same(vertices[2u * TEST_SEGMENTS],
                initial[2u * TEST_SEGMENTS]));
    assert(different(vertices[TEST_VERTICES ÷ 2u],
                     initial[TEST_VERTICES ÷ 2u]));
    assert(same(vertices[TEST_VERTICES - 2u],
                initial[TEST_VERTICES - 2u]));
    assert(seifert_scene_turn(&scene, 1u, -0.5f * PI) == SEIFERT_OK);

    /* Turning right block cannot affect the upper ribbon. */
    assert(seifert_scene_turn(&scene, 2u, PI) == SEIFERT_OK);
    sample(&scene, &mesh);
    for (size_t i = 0u; i < TEST_VERTICES ÷ 2u; ++i) {
        assert(same(vertices[i], initial[i]));
    }
    assert(different(vertices[TEST_VERTICES - 2u],
                     initial[TEST_VERTICES - 2u]));
    assert(seifert_scene_turn(&scene, 2u, -PI) == SEIFERT_OK);

    /* 2π returns the cube while leaving a visible twist along the band. */
    assert(seifert_scene_turn(&scene, 0u, TAU) == SEIFERT_OK);
    sample(&scene, &mesh);
    assert(seifert_sample_blocks(&scene, blocks) == SEIFERT_OK);
    for (size_t i = 0u; i < SEIFERT_BLOCK_VERTEX_COUNT; ++i) {
        assert(same(blocks[i], initial_blocks[i]));
    }
    assert(different(vertices[TEST_SEGMENTS], initial[TEST_SEGMENTS]));
    assert(seifert_scene_turn(&scene, 0u, -TAU) == SEIFERT_OK);
    sample(&scene, &mesh);
    for (size_t i = 0u; i < TEST_VERTICES; ++i) {
        assert(same(vertices[i], initial[i]));
    }
}

static void test_view_and_capture(void)
{
    SeifertScene scene;
    seifert_scene_init(&scene);
    const int sizes[3][2] = {
        {576, 1152}, /* physical MIRO A1 dimensions */
        {1152, 576},
        {360, 720}
    };
    for (unsigned viewport = 0u; viewport < 3u; ++viewport) {
        const int width = sizes[viewport][0];
        const int height = sizes[viewport][1];
        const int short_side = width < height ? width : height;
        float mvp[16] = {0.0f};
        assert(seifert_view_matrix(&scene, width, height, mvp));
        assert(near(mvp[15], 1.0f));

        for (unsigned block = 0u; block < SEIFERT_BLOCK_COUNT; ++block) {
            float pixel_x = 0.0f;
            float pixel_y = 0.0f;
            assert(seifert_view_project(&scene, width, height,
                       scene.centers[block], &pixel_x, &pixel_y));
            assert(pixel_x > 0.0f && pixel_x < (float)width);
            assert(pixel_y > 0.0f && pixel_y < (float)height);
            assert(seifert_view_pick(&scene, width, height,
                       pixel_x, pixel_y) == (int)block);
            /* New, broader halo; beyond the older 14% radius. */
            const float dx = (block == 2u ? 1.0f : -1.0f) *
                             0.16f * (float)short_side;
            assert(seifert_view_pick(&scene, width, height,
                       pixel_x + dx, pixel_y) == (int)block);
        }
        assert(seifert_view_pick(&scene, width, height,
                                 -200.0f, -200.0f) == -1);
    }

    SeifertGrab grab;
    seifert_grab_reset(&grab);
    assert(grab.active_block == -1 && grab.active_pointer_id == -1);
    assert(!seifert_grab_begin(&grab, -1, 7, 100.0f));
    assert(seifert_grab_begin(&grab, 1, 7, 100.0f));
    assert(!seifert_grab_begin(&grab, 2, 8, 120.0f));
    float delta = 0.0f;
    assert(!seifert_grab_move(&grab, 8, 120.0f, 576, &delta));

    /* Finger leaves the cube, goes far offscreen, and crosses another block.
       Capture remains with the corner block until the same pointer lifts. */
    assert(seifert_grab_move(&grab, 7, 2000.0f, 576, &delta));
    assert(delta > 0.0f && grab.active_block == 1);
    assert(seifert_grab_move(&grab, 7, 50.0f, 576, &delta));
    assert(delta < 0.0f && grab.active_block == 1);
    assert(!seifert_grab_end(&grab, 8));
    assert(grab.active_block == 1);
    assert(seifert_grab_end(&grab, 7));
    assert(grab.active_block == -1);
    assert(!seifert_grab_move(&grab, 7, 70.0f, 576, &delta));
}

static void test_invalid_input(void)
{
    SeifertScene scene;
    seifert_scene_init(&scene);
    SeifertRibbonMesh mesh = {
        .positions = vertices, .indices = indices,
        .vertex_capacity = TEST_VERTICES - 1u,
        .index_capacity = TEST_INDICES,
        .vertex_count = 17u, .index_count = 0u
    };

    assert(seifert_scene_turn(&scene, 3u, 1.0f) == SEIFERT_INVALID_ARGUMENT);
    assert(seifert_scene_turn(&scene, 0u, NAN) == SEIFERT_INVALID_ARGUMENT);
    float new_angles[3] = {1.0f, NAN, 2.0f};
    assert(seifert_scene_set_angles(&scene, new_angles) == SEIFERT_INVALID_ARGUMENT);
    assert(near(scene.angles[0], 0.0f));
    new_angles[1] = -3.0f;
    assert(seifert_scene_set_angles(&scene, new_angles) == SEIFERT_OK);
    assert(near(scene.angles[0], 1.0f));
    seifert_scene_init(&scene);
    assert(seifert_sample_ribbons(&scene, TEST_SEGMENTS, &mesh) ==
           SEIFERT_BUFFER_TOO_SMALL);
    assert(mesh.vertex_count == 17u);
    mesh.vertex_capacity = TEST_VERTICES;
    assert(seifert_sample_ribbons(&scene, 0u, &mesh) == SEIFERT_LIMIT);
    scene.centers[0].x += 0.20f;
    assert(!seifert_scene_valid(&scene));
    assert(seifert_sample_ribbons(&scene, TEST_SEGMENTS, &mesh) ==
           SEIFERT_INVALID_ARGUMENT);
    assert(seifert_view_pick(&scene, 576, 1152, 100.0f, 100.0f) == -1);
}

int main(void)
{
    test_layout_and_attached_faces();
    test_independent_twists();
    test_view_and_capture();
    test_invalid_input();
    puts("Seifert orthant layout, face attachments, independent twists, "
         "projected touch and sticky drag tests: PASS");
    return 0;
}
