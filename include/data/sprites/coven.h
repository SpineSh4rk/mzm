#ifndef COVEN_DATA_H
#define COVEN_DATA_H

#include "types.h"
#include "oam.h"

#include "gba/memory.h"

extern const u32 sCovenGfx[201];
extern const u16 sCovenPal[1 * PAL_ROW];
extern const struct FrameData sCovenOam_Idle[4];
extern const s16 sCovenDirectedMovementOffsets[9][2];

#endif /* COVEN_DATA_H */
