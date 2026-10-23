#include "macros.h"

#include "gba/memory.h"

#include "data/sprites/coven.h"

const u32 sCovenGfx[201] = {
	#include "coven_new.gfx.lz.inc"
};

const u16 sCovenPal[1 * PAL_ROW] = {
	#include "coven_new.pal.inc"
};

const u16 sCovenWhitePal[1 * PAL_ROW] = {
	COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, 
	COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE
};

static const u16 sCovenOam_Idle_Frame0[OAM_DATA_SIZE(2)] = {
	2,
	OAM_ENTRY(-15, -16, OAM_DIMS_32x16, OAM_NO_FLIP, 512, 8, 0),
	OAM_ENTRY(-15, 0, OAM_DIMS_32x16, OAM_NO_FLIP, 516, 8, 0),
};

static const u16 sCovenOam_Idle_Frame1[OAM_DATA_SIZE(2)] = {
	2,
	OAM_ENTRY(-15, -16, OAM_DIMS_32x16, OAM_NO_FLIP, 512, 8, 0),
	OAM_ENTRY(-15, 0, OAM_DIMS_32x16, OAM_NO_FLIP, 520, 8, 0),
};

static const u16 sCovenOam_Idle_Frame2[OAM_DATA_SIZE(2)] = {
	2,
	OAM_ENTRY(-15, -16, OAM_DIMS_32x16, OAM_NO_FLIP, 512, 8, 0),
	OAM_ENTRY(-15, 0, OAM_DIMS_32x16, OAM_NO_FLIP, 524, 8, 0),
};

const struct FrameData sCovenOam_Idle[4] = {
	[0] = {
		.pFrame = sCovenOam_Idle_Frame0,
		.timer = 16
	},
	[1] = {
		.pFrame = sCovenOam_Idle_Frame1,
		.timer = 16
	},
	[2] = {
		.pFrame = sCovenOam_Idle_Frame2,
		.timer = 16
	},
	[3] = FRAME_DATA_TERMINATOR
};

// X and Y offsets from Samus position for the directed attack
const s16 sCovenDirectedMovementOffsets[9][2] = {
	{0, 0}, // no direction (dummy)
	{-4 * BLOCK_SIZE, 0}, // left
	{4 * BLOCK_SIZE, 0}, // right
	{0, 4 * BLOCK_SIZE}, // down
	{-4 * BLOCK_SIZE, 4 * BLOCK_SIZE}, // down left
	{4 * BLOCK_SIZE, 4 * BLOCK_SIZE}, // down right
	{0, -4 * BLOCK_SIZE}, // up
	{-4 * BLOCK_SIZE, -4 * BLOCK_SIZE}, // up left
	{4 * BLOCK_SIZE, -4 * BLOCK_SIZE}  // up right
};

// Coven Y position changes while hovering
const s8 sCovenHoverDeltaY[120] = {
	4, 4, 4, 4, 0, 4, 0, 4, 0, 0, 
	0, 0, 0, 0, -4, 0, -4, 0, -4, -4, 
	0, -4, -8, 0, -4, -4, -4, 0, -4, 0, 
	-4, 0, 0, 0, 0, 0, 0, 4, 0, 4, 
	0, 4, 4, 4, 4, 4, 4, 0, 4, 0, 
	4, 0, 0, 4, 0, 0, 0, -4, 0, 0, 
	-4, -4, 0, -4, -4, -4, -4, 0, -4, -4, 
	0, -4, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 4, 0, 4, 4, 4, 0, 4, 4, 4, 
	4, 4, 0, 4, 0, 0, 0, 0, 0, 0, 
	0, 0, -4, 0, -4, -4, 0, -4, -4, -4, 
	-4, -4, -4, -4, 0, -4, 0, 0, -4, 0
};
