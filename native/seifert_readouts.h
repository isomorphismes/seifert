#ifndef SEIFERT_READOUTS_H
#define SEIFERT_READOUTS_H

#include "seifert.h"

typedef struct {
    int available;
    float turns; /* meaningful only when available */
} SeifertEndpointRotation;

/* Oriented from corner to upper/right; signed, unwrapped, binary32. */
void seifert_endpoint_rotations(
    const SeifertScene *scene,
    SeifertEndpointRotation rotations[SEIFERT_RIBBON_COUNT]
);

/* No geometric twist/linking/genus claim: these are commanded rotations. */
int seifert_format_endpoint_rotation(
    unsigned ribbon, SeifertEndpointRotation rotation,
    char *text, size_t text_capacity
);

#endif
