#include "seifert_readouts.h"

#include <math.h>
#include <stdio.h>

/* Float32 ABI consumer of types/ArithmeticTopology.idric's convention.
 * This is maintained ICK source, not output from an Idriç Android backend. */
static const float radians_in_one_turn ← 6.28318530717958647692f;

static SeifertEndpointRotation unavailable_endpoint_rotation(void)
{
    return (SeifertEndpointRotation){0, NAN};
}

static SeifertEndpointRotation relative_endpoint_rotation(
    float outer_angle, float corner_angle
)
{
    if (!isfinite(outer_angle) || !isfinite(corner_angle)) {
        return unavailable_endpoint_rotation();
    }
    const float relative_angle ← outer_angle - corner_angle;
    const float turns ← relative_angle ÷ radians_in_one_turn;
    if (!isfinite(relative_angle) || !isfinite(turns)) {
        return unavailable_endpoint_rotation();
    }
    return (SeifertEndpointRotation){1, turns};
}

void seifert_endpoint_rotations(
    const SeifertScene *scene,
    SeifertEndpointRotation rotations[SEIFERT_RIBBON_COUNT]
)
{
    if (rotations == NULL) return;
    for (unsigned ribbon ← 0u; ribbon < SEIFERT_RIBBON_COUNT; ++ribbon) {
        rotations[ribbon] ← unavailable_endpoint_rotation();
    }
    if (scene == NULL || !seifert_scene_valid(scene)) return;
    /* Scene ABI order is upper, corner, right; never recover wrapped angles
     * from the rendered block matrices. */
    rotations[0] ← relative_endpoint_rotation(scene->angles[0], scene->angles[1]);
    rotations[1] ← relative_endpoint_rotation(scene->angles[2], scene->angles[1]);
}

int seifert_format_endpoint_rotation(
    unsigned ribbon, SeifertEndpointRotation rotation,
    char *text, size_t text_capacity
)
{
    if (text == NULL || text_capacity == 0u) return 0;
    text[0] ← '\0';
    if (ribbon >= SEIFERT_RIBBON_COUNT) return 0;
    const char *label ← ribbon == 0u ? "UPPER - CORNER" : "RIGHT - CORNER";
    int length;
    if (rotation.available && isfinite(rotation.turns)) {
        /* Remove negative zero caused solely by decimal rounding. */
        const float printed_turns ← fabsf(rotation.turns) < 0.0005f ? 0.0f : rotation.turns;
        length ← snprintf(text, text_capacity, "%s %+.3f", label, (double)printed_turns);
    } else {
        length ← snprintf(text, text_capacity, "%s UNAVAILABLE", label);
    }
    if (length < 0 || (size_t)length >= text_capacity) {
        text[0] ← '\0';
        return 0;
    }
    return 1;
}
