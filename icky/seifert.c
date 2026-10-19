#include "seifert.h"

#include <math.h>
#include <stddef.h>

/*
 * Seifert: mathematical and topological meaning lives here.
 *
 * The public C ABI is deliberately the already-installed v0.2 ABI.
 * Assignment uses the qualified ICKY C ← token. Host and Android builds
 * compile this file directly with ICK; the NDK assembles and links the
 * Android output. No compatibility translation selects another frontend.
 *
 * The experimental deformation formula is unchanged. No new elasticity,
 * smoother frame, automatic untwisting or surface-collision rule is implied.
 */
#define SEIFERT_MAX_ANGLE 100000.0f
#define SEIFERT_TAU_F 6.28318530717958647692f
#define SEIFERT_INV_SQRT3 0.57735026918962576451f

typedef enum {
    UPPER_BLOCK,
    CORNER_BLOCK,
    RIGHT_BLOCK
} SeifertBlockId;

typedef struct {
    SeifertBlockId outer_block;
    SeifertVec3 direction_from_corner;
    SeifertVec3 transverse_direction;
} SeifertRibbonConnection;

/*
 * This is the actual two-edge attachment diagram, not a generic cloth
 * network. Each entry runs FROM the corner cube TO one outer cube.
 */
static const SeifertRibbonConnection orthant_connections[SEIFERT_RIBBON_COUNT] ← {
    {
        .outer_block ← UPPER_BLOCK,
        .direction_from_corner ← {0.0f, 1.0f, 0.0f},
        .transverse_direction ← {1.0f, 0.0f, 0.0f}
    },
    {
        .outer_block ← RIGHT_BLOCK,
        .direction_from_corner ← {1.0f, 0.0f, 0.0f},
        .transverse_direction ← {0.0f, 1.0f, 0.0f}
    }
};

typedef struct {
    SeifertVec3 corner_root;
    SeifertVec3 outer_root;
    SeifertVec3 corner_tangent;
    SeifertVec3 outer_tangent;
    float corner_angle;
    float outer_angle;
} SeifertRibbonAttachments;

typedef struct {
    float start_position_weight;
    float start_tangent_weight;
    float end_position_weight;
    float end_tangent_weight;
} SeifertHermiteWeights;

static SeifertVec3 add_vectors(SeifertVec3 left, SeifertVec3 right)
{
    return (SeifertVec3){
        left.x + right.x,
        left.y + right.y,
        left.z + right.z
    };
}

static SeifertVec3 scale_vector(SeifertVec3 vector, float scalar)
{
    return (SeifertVec3){
        vector.x * scalar,
        vector.y * scalar,
        vector.z * scalar
    };
}

static SeifertVec3 weighted_sum(
    SeifertVec3 first, float first_weight,
    SeifertVec3 second, float second_weight
)
{
    return add_vectors(
        scale_vector(first, first_weight),
        scale_vector(second, second_weight)
    );
}

/* Rodrigues rotation about the same (1,1,1)/√3 diagonal as v0.2. */
static SeifertVec3 rotate_around_block_diagonal(
    SeifertVec3 vector, float physical_angle
)
{
    const float wrapped_angle ← remainderf(physical_angle, SEIFERT_TAU_F);
    const float cosine ← cosf(wrapped_angle);
    const float sine ← sinf(wrapped_angle);
    const float inverse_root_three ← SEIFERT_INV_SQRT3;
    const float axis_projection ← inverse_root_three *
        (vector.x + vector.y + vector.z);

    return (SeifertVec3){
        cosine * vector.x
          + sine * inverse_root_three * (vector.z - vector.y)
          + (1.0f - cosine) * inverse_root_three * axis_projection,
        cosine * vector.y
          + sine * inverse_root_three * (vector.x - vector.z)
          + (1.0f - cosine) * inverse_root_three * axis_projection,
        cosine * vector.z
          + sine * inverse_root_three * (vector.y - vector.x)
          + (1.0f - cosine) * inverse_root_three * axis_projection
    };
}

static int is_admissible_physical_angle(float physical_angle)
{
    return isfinite(physical_angle) &&
        fabsf(physical_angle) <= SEIFERT_MAX_ANGLE;
}

void seifert_scene_init(SeifertScene *scene)
{
    if (scene == NULL) {
        return;
    }

    scene->centers[UPPER_BLOCK] ← (SeifertVec3){1.90f, 4.40f, 0.0f};
    scene->centers[CORNER_BLOCK] ← (SeifertVec3){1.90f, 1.90f, 0.0f};
    scene->centers[RIGHT_BLOCK] ← (SeifertVec3){4.40f, 1.90f, 0.0f};
    scene->block_half_extent ← 0.46f;
    scene->ribbon_half_width ← 0.30f;

    for (unsigned block_id ← 0u;
         block_id < SEIFERT_BLOCK_COUNT;
         ++block_id) {
        scene->angles[block_id] ← 0.0f;
    }
}

static int all_block_centers_are_admissible(const SeifertScene *scene)
{
    const float half_extent ← scene->block_half_extent;
    for (unsigned block_id ← 0u;
         block_id < SEIFERT_BLOCK_COUNT;
         ++block_id) {
        const SeifertVec3 center ← scene->centers[block_id];
        if (!isfinite(center.x) || !isfinite(center.y) ||
            !isfinite(center.z) || center.x <= half_extent ||
            center.y <= half_extent || center.z != 0.0f ||
            !is_admissible_physical_angle(scene->angles[block_id])) {
            return 0;
        }
    }
    return 1;
}

static int diagram_is_positive_orthant(const SeifertScene *scene)
{
    const SeifertVec3 upper ← scene->centers[UPPER_BLOCK];
    const SeifertVec3 corner ← scene->centers[CORNER_BLOCK];
    const SeifertVec3 right ← scene->centers[RIGHT_BLOCK];
    const float minimum_separation ← 2.0f * scene->block_half_extent;

    return upper.x == corner.x &&
        right.y == corner.y &&
        upper.y - corner.y > minimum_separation &&
        right.x - corner.x > minimum_separation;
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

    return all_block_centers_are_admissible(scene) &&
           diagram_is_positive_orthant(scene);
}

SeifertStatus seifert_scene_turn(
    SeifertScene *scene, unsigned block_id, float delta_radians
)
{
    if (scene == NULL || block_id >= SEIFERT_BLOCK_COUNT ||
        !is_admissible_physical_angle(delta_radians) ||
        !seifert_scene_valid(scene)) {
        return SEIFERT_INVALID_ARGUMENT;
    }

    const float turned_angle ← scene->angles[block_id] + delta_radians;
    if (!is_admissible_physical_angle(turned_angle)) {
        return SEIFERT_LIMIT;
    }

    scene->angles[block_id] ← turned_angle;
    return SEIFERT_OK;
}

SeifertStatus seifert_scene_set_angles(
    SeifertScene *scene, const float angles[SEIFERT_BLOCK_COUNT]
)
{
    if (scene == NULL || angles == NULL || !seifert_scene_valid(scene)) {
        return SEIFERT_INVALID_ARGUMENT;
    }

    for (unsigned block_id ← 0u;
         block_id < SEIFERT_BLOCK_COUNT;
         ++block_id) {
        if (!is_admissible_physical_angle(angles[block_id])) {
            return SEIFERT_INVALID_ARGUMENT;
        }
    }

    for (unsigned block_id ← 0u;
         block_id < SEIFERT_BLOCK_COUNT;
         ++block_id) {
        scene->angles[block_id] ← angles[block_id];
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

static float smooth_ribbon_parameter(float fraction)
{
    return fraction * fraction * (3.0f - 2.0f * fraction);
}

static SeifertHermiteWeights cubic_hermite_weights(float fraction)
{
    const float squared_fraction ← fraction * fraction;
    const float cubed_fraction ← squared_fraction * fraction;
    const SeifertHermiteWeights weights ← {
        .start_position_weight ← 2.0f * cubed_fraction
            - 3.0f * squared_fraction + 1.0f,
        .start_tangent_weight ← cubed_fraction
            - 2.0f * squared_fraction + fraction,
        .end_position_weight ← -2.0f * cubed_fraction
            + 3.0f * squared_fraction,
        .end_tangent_weight ← cubed_fraction - squared_fraction
    };
    return weights;
}

static SeifertVec3 rotated_attachment_root(
    SeifertVec3 block_center, SeifertVec3 face_normal,
    float half_extent, float angle
)
{
    return add_vectors(
        block_center,
        scale_vector(
            rotate_around_block_diagonal(face_normal, angle),
            half_extent
        )
    );
}

static float ribbon_center_separation(
    const SeifertScene *scene, SeifertRibbonConnection connection
)
{
    const SeifertVec3 corner ← scene->centers[CORNER_BLOCK];
    const SeifertVec3 outer ← scene->centers[connection.outer_block];
    return connection.outer_block == UPPER_BLOCK
        ? outer.y - corner.y
        : outer.x - corner.x;
}

static SeifertRibbonAttachments attachment_pair(
    const SeifertScene *scene, SeifertRibbonConnection connection
)
{
    const SeifertBlockId outer_block ← connection.outer_block;
    const float corner_angle ← scene->angles[CORNER_BLOCK];
    const float outer_angle ← scene->angles[outer_block];
    const float half_extent ← scene->block_half_extent;
    const float span_length ← ribbon_center_separation(scene, connection)
        - 2.0f * half_extent;
    const SeifertVec3 outward ← connection.direction_from_corner;

    const SeifertRibbonAttachments attachments ← {
        .corner_root ← rotated_attachment_root(
            scene->centers[CORNER_BLOCK], outward,
            half_extent, corner_angle
        ),
        .outer_root ← rotated_attachment_root(
            scene->centers[outer_block], outward,
            -half_extent, outer_angle
        ),
        .corner_tangent ← scale_vector(
            rotate_around_block_diagonal(outward, corner_angle), span_length
        ),
        .outer_tangent ← scale_vector(
            rotate_around_block_diagonal(outward, outer_angle), span_length
        ),
        .corner_angle ← corner_angle,
        .outer_angle ← outer_angle
    };
    return attachments;
}

static SeifertVec3 centerline_at(
    SeifertRibbonAttachments attachments, float fraction
)
{
    const SeifertHermiteWeights weights ←
        cubic_hermite_weights(fraction);
    const SeifertVec3 endpoint_positions ← weighted_sum(
        attachments.corner_root, weights.start_position_weight,
        attachments.outer_root, weights.end_position_weight
    );
    const SeifertVec3 endpoint_tangents ← weighted_sum(
        attachments.corner_tangent, weights.start_tangent_weight,
        attachments.outer_tangent, weights.end_tangent_weight
    );
    return add_vectors(endpoint_positions, endpoint_tangents);
}

static float ribbon_rotation_at(
    SeifertRibbonAttachments attachments, float fraction
)
{
    const float eased_fraction ← smooth_ribbon_parameter(fraction);
    return (1.0f - eased_fraction) * attachments.corner_angle
        + eased_fraction * attachments.outer_angle;
}

static void sample_ribbon_cross_section(
    const SeifertScene *scene,
    SeifertRibbonConnection connection,
    SeifertRibbonAttachments attachments,
    float fraction,
    SeifertVec3 out_edge_vertices[2]
)
{
    const SeifertVec3 centerline ← centerline_at(attachments, fraction);
    const SeifertVec3 transverse_offset ← scale_vector(
        rotate_around_block_diagonal(
            connection.transverse_direction,
            ribbon_rotation_at(attachments, fraction)
        ),
        scene->ribbon_half_width
    );

    out_edge_vertices[0] ← add_vectors(
        centerline, scale_vector(transverse_offset, -1.0f)
    );
    out_edge_vertices[1] ← add_vectors(centerline, transverse_offset);
}

static void write_ribbon_triangles(
    SeifertRibbonMesh *mesh, unsigned ribbon_id,
    unsigned segments, size_t first_vertex
)
{
    for (unsigned segment ← 0u; segment < segments; ++segment) {
        const size_t position ←
            ((size_t)ribbon_id * segments + segment) * 6u;
        const uint16_t lower_left ← (uint16_t)(
            first_vertex + 2u * segment
        );
        const uint16_t upper_left ← (uint16_t)(lower_left + 1u);
        const uint16_t lower_right ← (uint16_t)(lower_left + 2u);
        const uint16_t upper_right ← (uint16_t)(lower_left + 3u);

        mesh->indices[position] ← lower_left;
        mesh->indices[position + 1u] ← lower_right;
        mesh->indices[position + 2u] ← upper_left;
        mesh->indices[position + 3u] ← upper_left;
        mesh->indices[position + 4u] ← lower_right;
        mesh->indices[position + 5u] ← upper_right;
    }
}

static void sample_one_ribbon(
    const SeifertScene *scene,
    unsigned ribbon_id,
    unsigned segments,
    SeifertRibbonMesh *mesh
)
{
    const SeifertRibbonConnection connection ←
        orthant_connections[ribbon_id];
    const SeifertRibbonAttachments attachments ←
        attachment_pair(scene, connection);
    const size_t first_vertex ←
        (size_t)ribbon_id * 2u * ((size_t)segments + 1u);

    for (unsigned step ← 0u; step <= segments; ++step) {
        const float fraction ← (float)step ÷ (float)segments;
        const size_t vertex_offset ← first_vertex + 2u * (size_t)step;

        sample_ribbon_cross_section(
            scene, connection, attachments, fraction,
            &mesh->positions[vertex_offset]
        );
    }

    write_ribbon_triangles(mesh, ribbon_id, segments, first_vertex);
}

static SeifertStatus validate_ribbon_output(
    const SeifertScene *scene,
    unsigned segments,
    const SeifertRibbonMesh *mesh
)
{
    if (!seifert_scene_valid(scene) || mesh == NULL ||
        mesh->positions == NULL || mesh->indices == NULL) {
        return SEIFERT_INVALID_ARGUMENT;
    }

    const size_t vertices ← seifert_ribbon_vertex_count(segments);
    const size_t indices ← seifert_ribbon_index_count(segments);
    if (vertices == 0u) {
        return SEIFERT_LIMIT;
    }
    if (mesh->vertex_capacity < vertices ||
        mesh->index_capacity < indices) {
        return SEIFERT_BUFFER_TOO_SMALL;
    }
    return SEIFERT_OK;
}

SeifertStatus seifert_sample_ribbons(
    const SeifertScene *scene, unsigned segments, SeifertRibbonMesh *mesh
)
{
    const SeifertStatus output_status ←
        validate_ribbon_output(scene, segments, mesh);
    if (output_status != SEIFERT_OK) {
        return output_status;
    }

    for (unsigned ribbon_id ← 0u;
         ribbon_id < SEIFERT_RIBBON_COUNT;
         ++ribbon_id) {
        sample_one_ribbon(scene, ribbon_id, segments, mesh);
    }

    mesh->vertex_count ← seifert_ribbon_vertex_count(segments);
    mesh->index_count ← seifert_ribbon_index_count(segments);
    return SEIFERT_OK;
}

static SeifertVec3 cube_corner_in_world(
    const SeifertScene *scene,
    unsigned block_id,
    SeifertVec3 local_corner
)
{
    return add_vectors(
        scene->centers[block_id],
        rotate_around_block_diagonal(
            local_corner, scene->angles[block_id]
        )
    );
}

SeifertStatus seifert_sample_blocks(
    const SeifertScene *scene,
    SeifertVec3 positions[SEIFERT_BLOCK_VERTEX_COUNT]
)
{
    if (!seifert_scene_valid(scene) || positions == NULL) {
        return SEIFERT_INVALID_ARGUMENT;
    }

    static const unsigned cube_triangle_indices[36] ← {
        0, 1, 2, 0, 2, 3,
        4, 6, 5, 4, 7, 6,
        0, 4, 5, 0, 5, 1,
        3, 2, 6, 3, 6, 7,
        0, 3, 7, 0, 7, 4,
        1, 5, 6, 1, 6, 2
    };
    static const int8_t corner_signs[8][3] ← {
        {-1, -1, -1}, { 1, -1, -1}, { 1, 1, -1}, {-1, 1, -1},
        {-1, -1,  1}, { 1, -1,  1}, { 1, 1,  1}, {-1, 1,  1}
    };

    for (unsigned block_id ← 0u;
         block_id < SEIFERT_BLOCK_COUNT;
         ++block_id) {
        const float half_extent ← scene->block_half_extent;
        for (unsigned vertex_id ← 0u; vertex_id < 36u; ++vertex_id) {
            const unsigned corner_id ← cube_triangle_indices[vertex_id];
            const SeifertVec3 local_corner ← {
                half_extent * (float)corner_signs[corner_id][0],
                half_extent * (float)corner_signs[corner_id][1],
                half_extent * (float)corner_signs[corner_id][2]
            };
            positions[block_id * 36u + vertex_id] ←
                cube_corner_in_world(scene, block_id, local_corner);
        }
    }
    return SEIFERT_OK;
}
