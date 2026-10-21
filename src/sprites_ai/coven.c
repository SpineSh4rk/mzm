#include "sprites_ai/coven.h"
#include "macros.h"
#include "event.h"
#include "sprite_util.h"
#include "color_effects.h"

#include "constants/audio.h"
#include "constants/sprite_util.h"
#include "constants/particle.h"

#include "data/sprites/coven.h"
#include "data/sprite_data.h"

#include "gba/display.h"

#include "structs/sprite.h"

/* 
TODO:
- sounds
- frozen palette
- ?
*/

#define EVENT_COVEN_INACTIVE 67

#define COVEN_SPAWNER_POSE_IDLE 1

#define COVEN_POSE_IDLE 1
#define COVEN_POSE_ATTACK_WARNING 2
#define COVEN_POSE_ATTACKING 3
#define COVEN_POSE_DISAPPEARING 4

#define PSPRITE_COVEN_SPAWNER 67
#define SSPRITE_COVEN 69

#define COVEN_ATTACK_DELAY work0
#define COVEN_LOCK_ON_TIMER work1
#define COVEN_DIRECTED_MOVEMENT_TIMER work2
#define COVEN_SAMUS_DIRECTION work3

#define COVEN_X_DIR_LEFT 1
#define COVEN_X_DIR_RIGHT 2
#define COVEN_Y_DIR_DOWN 0x10
#define COVEN_Y_DIR_UP 0x20


static void CovenInit(void)
{
	gCurrentSprite.status |= (SPRITE_STATUS_NOT_DRAWN | SPRITE_STATUS_IGNORE_PROJECTILES);
	gCurrentSprite.hitboxTop = -BLOCK_SIZE;
    gCurrentSprite.hitboxBottom = BLOCK_SIZE;
    gCurrentSprite.hitboxLeft = -BLOCK_SIZE;
    gCurrentSprite.hitboxRight = BLOCK_SIZE;
	gCurrentSprite.health = GET_PSPRITE_HEALTH(gCurrentSprite.spriteId);
	gCurrentSprite.pOam = sCovenOam_Idle;
	gCurrentSprite.animationDurationCounter = 0;
    gCurrentSprite.currentAnimationFrame = 0;
	gCurrentSprite.bgPriority = BGCNT_HIGH_PRIORITY;
	gCurrentSprite.pose = COVEN_POSE_IDLE;
	gCurrentSprite.samusCollision = SSC_NONE;
	gCurrentSprite.drawDistanceTop = SUB_PIXEL_TO_PIXEL(-BLOCK_SIZE);
	gCurrentSprite.drawDistanceBottom = SUB_PIXEL_TO_PIXEL(BLOCK_SIZE);
	gCurrentSprite.drawDistanceHorizontal = SUB_PIXEL_TO_PIXEL(BLOCK_SIZE);
	gCurrentSprite.work0 = CONVERT_SECONDS(2);
}

static void CovenIdle(void)
{	
	u32 direction;
	u32 i;

	// Cooldown before starting attack behavior
	if (gCurrentSprite.work0 > 0)
	{
		gCurrentSprite.work0--;
		return;
	}

	direction = 0;

	// Materialise if Samus has moved at most 1px/frame for 40h frames (tracked by the "lock-on timer" and "zone" variables)
	// or has moved more than 1px/frame in the same direction 10h times (tracked by the "directed movement timer" and "previous Samus" variables)
	
	if (gSamusData.xPosition > gPreviousXPosition + 1)
	{
		direction = COVEN_X_DIR_RIGHT;
	}
	else if (gSamusData.xPosition < gPreviousXPosition - 1)
	{
		direction = COVEN_X_DIR_LEFT;
	}

	if (gSamusData.yPosition > gPreviousYPosition + 1)
	{
		direction |= COVEN_Y_DIR_DOWN;
	}
	else if (gSamusData.yPosition < gPreviousYPosition - 1)
	{
		direction |= COVEN_Y_DIR_UP;
	}

	if (!direction)
	{
		gCurrentSprite.COVEN_LOCK_ON_TIMER++;
		gCurrentSprite.COVEN_DIRECTED_MOVEMENT_TIMER = 0;
	}
	else
	{
		gCurrentSprite.COVEN_LOCK_ON_TIMER = 0;

		if (direction == gCurrentSprite.COVEN_SAMUS_DIRECTION)
		{
			gCurrentSprite.COVEN_DIRECTED_MOVEMENT_TIMER++;
		}
		else
		{
			gCurrentSprite.COVEN_DIRECTED_MOVEMENT_TIMER = 0;
		}
	}

	gCurrentSprite.COVEN_SAMUS_DIRECTION = direction;

	if (gCurrentSprite.COVEN_LOCK_ON_TIMER >= 64)
	{
		gCurrentSprite.COVEN_LOCK_ON_TIMER = 0;
		gCurrentSprite.COVEN_DIRECTED_MOVEMENT_TIMER = 0;

		gCurrentSprite.status &= ~(SPRITE_STATUS_NOT_DRAWN | SPRITE_STATUS_IGNORE_PROJECTILES);
		gCurrentSprite.yPosition = gSamusData.yPosition - BLOCK_TO_SUB_PIXEL(1);
		gCurrentSprite.xPosition = gSamusData.xPosition;
		gCurrentSprite.pose = COVEN_POSE_ATTACK_WARNING;
		//gCurrentSprite.work0 = CONVERT_SECONDS(1); // Need to count up instead
	}
	else if (gCurrentSprite.COVEN_DIRECTED_MOVEMENT_TIMER >= 16)
	{
		gCurrentSprite.COVEN_LOCK_ON_TIMER = 0;
		gCurrentSprite.COVEN_DIRECTED_MOVEMENT_TIMER = 0;

		// Map the direction to an index from [0, 8]
		i = ((direction & 0xF0) >> 4) * 3 + (direction & 0xF);
		if (i > 8) // Just to be safe
			i = 0;

		gCurrentSprite.status &= ~(SPRITE_STATUS_NOT_DRAWN | SPRITE_STATUS_IGNORE_PROJECTILES);
		gCurrentSprite.yPosition = gSamusData.yPosition - BLOCK_TO_SUB_PIXEL(1) + sCovenDirectedMovementOffsets[i][1];
		gCurrentSprite.xPosition = gSamusData.xPosition + sCovenDirectedMovementOffsets[i][0];
		gCurrentSprite.pose = COVEN_POSE_ATTACK_WARNING;
		//gCurrentSprite.work0 = CONVERT_SECONDS(1); // Need to count up instead
	}
}

static void CovenAttackWarning(void)
{
	u16* dstPal;
	//gCurrentSprite.work0--;
	//if (gCurrentSprite.work0 == 0)
	if (gCurrentSprite.work0 >= 32)
	{
		gCurrentSprite.pose = COVEN_POSE_ATTACKING;
		gCurrentSprite.samusCollision = SSC_HURTS_SAMUS;
		//gCurrentSprite.work0 = CONVERT_SECONDS(2); // Need to count up instead
		gCurrentSprite.work0 = 0;
	}
	else
	{
		if (gCurrentSprite.work0 <= 18)
		{
			dstPal = PALRAM_OBJ + 0x100 + gCurrentSprite.paletteRow * 0x20;
			ApplySmoothPaletteTransition((u16*)sCovenWhitePal, (u16*)sCovenPal, dstPal, gCurrentSprite.work0 + 14);
		}

		gCurrentSprite.work0++;
	}
}

static void CovenAttacking(void)
{
	//gCurrentSprite.work0--;
	//if (gCurrentSprite.work0 == 0)
	if (gCurrentSprite.work0 >= 120)
	{
		gCurrentSprite.pose = COVEN_POSE_DISAPPEARING;
		//gCurrentSprite.work0 = CONVERT_SECONDS(.5f); // Need to count up instead
		gCurrentSprite.work0 = 0;
	}
	else
	{
		gCurrentSprite.yPosition += sCovenHoverDeltaY[gCurrentSprite.work0];
		gCurrentSprite.work0++;
	}
}

static void CovenDisappearing(void)
{
	u16* dstPal;

	//gCurrentSprite.work0--;
	//if (gCurrentSprite.work0 == 0)
	if (gCurrentSprite.work0 >= 32)
	{
		gCurrentSprite.status |= (SPRITE_STATUS_NOT_DRAWN | SPRITE_STATUS_IGNORE_PROJECTILES);
		gCurrentSprite.health = GET_PSPRITE_HEALTH(gCurrentSprite.spriteId);
		gCurrentSprite.pose = COVEN_POSE_IDLE;
		gCurrentSprite.samusCollision = SSC_NONE;
		gCurrentSprite.work0 = CONVERT_SECONDS(2);
	}
	else
	{
		dstPal = PALRAM_OBJ + 0x100 + gCurrentSprite.paletteRow * 0x20;
		ApplySmoothPaletteTransition((u16*)sCovenPal, (u16*)sCovenWhitePal, dstPal, gCurrentSprite.work0);
		gCurrentSprite.work0++;
	}
}

static void CovenRespawn(void)
{
	gCurrentSprite.status |= (SPRITE_STATUS_NOT_DRAWN | SPRITE_STATUS_IGNORE_PROJECTILES);
	gCurrentSprite.yPosition = 1;
	gCurrentSprite.xPosition = 1;
	gCurrentSprite.health = GET_PSPRITE_HEALTH(gCurrentSprite.spriteId);
	gCurrentSprite.pose = COVEN_POSE_IDLE;
	gCurrentSprite.samusCollision = SSC_NONE;
	gCurrentSprite.ignoreSamusCollisionTimer = DELTA_TIME;
	gCurrentSprite.invincibilityStunFlashTimer = 0;
	gCurrentSprite.work0 = CONVERT_SECONDS(2);
	gCurrentSprite.freezeTimer = 0;
	gCurrentSprite.standingOnSprite = SAMUS_STANDING_ON_SPRITE_OFF;
}

void Coven(void)
{
	if (CHECK_EVENT(EVENT_ZIPLINES_ACTIVATED))
	{
		gCurrentSprite.status = 0;
		return;
	}

	if (gCurrentSprite.properties & SP_DAMAGED)
    {
        gCurrentSprite.properties &= ~SP_DAMAGED;
        if (gCurrentSprite.status & SPRITE_STATUS_ONSCREEN)
            SoundPlayNotAlreadyPlaying(SOUND_RIPPER_DAMAGED);
    }

    if (gCurrentSprite.freezeTimer != 0)
    {
        SpriteUtilUpdateFreezeTimer();
        return;
    }

	switch (gCurrentSprite.pose)
	{
	case SPRITE_POSE_UNINITIALIZED:
		CovenInit();
		break;

	case COVEN_POSE_IDLE:
		CovenIdle();
		break;

	case COVEN_POSE_ATTACK_WARNING:
		CovenAttackWarning();
		break;

	case COVEN_POSE_ATTACKING:
		CovenAttacking();
		break;

	case COVEN_POSE_DISAPPEARING:
		CovenDisappearing();
		break;

	default:
		SpriteUtilSpriteDeath(DEATH_RESPAWNING, gCurrentSprite.yPosition, gCurrentSprite.xPosition, TRUE, PE_SPRITE_EXPLOSION_MEDIUM);
		CovenRespawn();
		break;
	}
}



