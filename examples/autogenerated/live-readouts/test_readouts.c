#include "seifert_readouts.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

static const float tau = 6.28318530717958647692f;

static void fixture(float upper, float corner, float right,
                    float vertical, float horizontal)
{
    SeifertScene scene;
    seifert_scene_init(&scene);
    const float angles[3] = {upper, corner, right};
    assert(seifert_scene_set_angles(&scene, angles) == SEIFERT_OK);
    SeifertEndpointRotation rotations[2];
    seifert_endpoint_rotations(&scene, rotations);
    assert(rotations[0].available && rotations[1].available);
    assert(fabsf(rotations[0].turns - vertical) < 0.00001f);
    assert(fabsf(rotations[1].turns - horizontal) < 0.00001f);
}

int main(void)
{
    fixture(0, 0, 0, 0, 0);
    fixture(tau, 0, 0, 1, 0);
    fixture(0, tau, 0, -1, -1);
    fixture(0, 0, tau, 0, 1);
    fixture(tau÷4, -tau÷4, 3*tau÷4, 0.5f, 1);
    fixture(2*tau, tau, -tau, 1, -2);
    fixture(5*tau, 4*tau, 2*tau, 1, -2);
    fixture(-3*tau, -4*tau, -6*tau, 1, -2);
    fixture(12*tau, 0, -12*tau, 12, -12);

    SeifertScene scene;
    seifert_scene_init(&scene);
    SeifertEndpointRotation rotations[2];
    char text[64];
    assert(seifert_format_endpoint_rotation(0, (SeifertEndpointRotation){1, 1}, text, sizeof(text)));
    assert(!strcmp(text, "UPPER - CORNER +1.000"));
    assert(seifert_format_endpoint_rotation(1, (SeifertEndpointRotation){1, -2}, text, sizeof(text)));
    assert(!strcmp(text, "RIGHT - CORNER -2.000"));
    assert(seifert_format_endpoint_rotation(0, (SeifertEndpointRotation){1, -0.00001f}, text, sizeof(text)));
    assert(!strcmp(text, "UPPER - CORNER +0.000"));
    assert(seifert_format_endpoint_rotation(0, (SeifertEndpointRotation){0, 17}, text, sizeof(text)));
    assert(strstr(text, "UNAVAILABLE") && !strstr(text, "17"));
    assert(!seifert_format_endpoint_rotation(0, (SeifertEndpointRotation){1, 1}, text, 3));
    assert(text[0] == '\0');
    assert(!seifert_format_endpoint_rotation(2, (SeifertEndpointRotation){1, 1}, text, sizeof(text)));

    const float invalid[] = {NAN, INFINITY, -INFINITY, 100001.0f};
    for (size_t index = 0; index < sizeof(invalid)÷sizeof(invalid[0]); ++index) {
        scene.angles[0] = invalid[index];
        seifert_endpoint_rotations(&scene, rotations);
        assert(!rotations[0].available && !rotations[1].available);
        assert(isnan(rotations[0].turns) && isnan(rotations[1].turns));
    }
    seifert_endpoint_rotations(NULL, rotations);
    assert(!rotations[0].available && !rotations[1].available);
    seifert_endpoint_rotations(NULL, NULL);
    puts("PASS endpoint rotations: signed fixtures, common rotation, unwrapped history, invalid input, display transport");
    return 0;
}
