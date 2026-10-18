#include "macros.h"

#include "gba/memory.h"

#include "data/sprites/covern.h"

const u32 sCovernGfx[201] = {
	#include "covern.gfx.lz.inc"
};

const u16 sCovernPal[1 * PAL_ROW] = {
	#include "covern.pal.inc"
};

static const u16 sCovernOam_Idle_Frame0[OAM_DATA_SIZE(2)] = {
	2,
	OAM_ENTRY(-15, -16, OAM_DIMS_32x16, OAM_NO_FLIP, 512, 8, 0),
	OAM_ENTRY(-15, 0, OAM_DIMS_32x16, OAM_NO_FLIP, 516, 8, 0),
};

static const u16 sCovernOam_Idle_Frame1[OAM_DATA_SIZE(2)] = {
	2,
	OAM_ENTRY(-15, -16, OAM_DIMS_32x16, OAM_NO_FLIP, 512, 8, 0),
	OAM_ENTRY(-15, 0, OAM_DIMS_32x16, OAM_NO_FLIP, 520, 8, 0),
};

static const u16 sCovernOam_Idle_Frame2[OAM_DATA_SIZE(2)] = {
	2,
	OAM_ENTRY(-15, -16, OAM_DIMS_32x16, OAM_NO_FLIP, 512, 8, 0),
	OAM_ENTRY(-15, 0, OAM_DIMS_32x16, OAM_NO_FLIP, 524, 8, 0),
};

const struct FrameData sCovernOam_Idle[4] = {
	[0] = {
		.pFrame = sCovernOam_Idle_Frame0,
		.timer = 16
	},
	[1] = {
		.pFrame = sCovernOam_Idle_Frame1,
		.timer = 16
	},
	[2] = {
		.pFrame = sCovernOam_Idle_Frame2,
		.timer = 16
	},
	[3] = FRAME_DATA_TERMINATOR
};


