#include "sprites_ai/covern.h"
#include "macros.h"
#include "event.h"
#include "sprite_util.h"

#include "constants/audio.h"
#include "constants/sprite_util.h"
#include "constants/particle.h"

#include "data/sprites/covern.h"

#include "structs/sprite.h"



#define EVENT_COVERN_INACTIVE 67

#define COVERN_SPAWNER_POSE_IDLE 1

#define COVERN_POSE_IDLE 1
#define COVERN_POSE_ATTACK_WARNING 2
#define COVERN_POSE_ATTACKING 3

#define PSPRITE_COVERN_SPAWNER 67
#define SSPRITE_COVERN 69

#define COVERN_ATTACK_DELAY work0

/*
void CovernSpawnerInit(void)
{
	gCurrentSprite.status |= SPRITE_STATUS_NOT_DRAWN | SPRITE_STATUS_IGNORE_PROJECTILES;

	gCurrentSprite.hitboxTop = -PIXEL_SIZE;
    gCurrentSprite.hitboxBottom = PIXEL_SIZE;
    gCurrentSprite.hitboxLeft = -PIXEL_SIZE;
    gCurrentSprite.hitboxRight = PIXEL_SIZE;

	gCurrentSprite.health = 1;

	gCurrentSprite.pose = COVERN_SPAWNER_POSE_IDLE;

	gCurrentSprite.samusCollision = SSC_NONE;
	
	gCurrentSprite.properties = SP_IMMUNE_TO_PROJECTILES;
}
*/
/*
void CovernSpawnerIdle(void)
{
	// Spawn a covern if none exist
	if (SpriteUtilCountChildSecondarySprites(SSPRITE_COVERN) == 0)
	{

	}
}
*/
/*
void CovernSpawner(void)
{
	if (CHECK_EVENT(EVENT_COVERN_INACTIVE))
	{
		gCurrentSprite.status = 0;
		return;
	}

	switch (gCurrentSprite.pose)
    {
	case SPRITE_POSE_UNINITIALIZED:
		CovernSpawnerInit();
		break;

	case COVERN_SPAWNER_POSE_IDLE:
		CovernSpawnerIdle();
		break;

	default:
		gCurrentSprite.status = 0;
		break;
	}
}
*/



static void CovernInit(void)
{
	gCurrentSprite.status |= SPRITE_STATUS_NOT_DRAWN | SPRITE_STATUS_IGNORE_PROJECTILES;
	gCurrentSprite.hitboxTop = -BLOCK_SIZE;
    gCurrentSprite.hitboxBottom = BLOCK_SIZE;
    gCurrentSprite.hitboxLeft = -BLOCK_SIZE;
    gCurrentSprite.hitboxRight = BLOCK_SIZE;
	gCurrentSprite.health = GET_PSPRITE_HEALTH(gCurrentSprite.spriteId);
	gCurrentSprite.pOam = sCovernOam_Idle;
	gCurrentSprite.animationDurationCounter = 0;
    gCurrentSprite.currentAnimationFrame = 0;
	gCurrentSprite.pose = COVERN_POSE_IDLE;
	gCurrentSprite.samusCollision = SSC_NONE;
	gCurrentSprite.drawDistanceTop = SUB_PIXEL_TO_PIXEL(-BLOCK_SIZE);
	gCurrentSprite.drawDistanceBottom = SUB_PIXEL_TO_PIXEL(BLOCK_SIZE);
	gCurrentSprite.drawDistanceHorizontal = SUB_PIXEL_TO_PIXEL(BLOCK_SIZE);
	gCurrentSprite.work0 = CONVERT_SECONDS(3);
}

static void CovernIdle(void)
{
	gCurrentSprite.work0--;
	if (gCurrentSprite.work0 == 0)
	{
		gCurrentSprite.status &= ~(SPRITE_STATUS_NOT_DRAWN | SPRITE_STATUS_IGNORE_PROJECTILES);
		gCurrentSprite.yPosition = gSamusData.yPosition;
		gCurrentSprite.xPosition = gSamusData.xPosition;
		gCurrentSprite.pose = COVERN_POSE_ATTACK_WARNING;
		gCurrentSprite.work0 = CONVERT_SECONDS(.5f);
	}
}

static void CovernAttackWarning(void)
{
	gCurrentSprite.work0--;
	if (gCurrentSprite.work0 == 0)
	{
		gCurrentSprite.pose = COVERN_POSE_ATTACKING;
		gCurrentSprite.samusCollision = SSC_HURTS_SAMUS;
		gCurrentSprite.work0 = CONVERT_SECONDS(3);
	}
}

static void CovernAttacking(void)
{
	gCurrentSprite.work0--;
	if (gCurrentSprite.work0 == 0)
	{
		gCurrentSprite.status |= SPRITE_STATUS_NOT_DRAWN | SPRITE_STATUS_IGNORE_PROJECTILES;
		gCurrentSprite.health = GET_PSPRITE_HEALTH(gCurrentSprite.spriteId);
		gCurrentSprite.pose = COVERN_POSE_IDLE;
		gCurrentSprite.samusCollision = SSC_NONE;
		gCurrentSprite.work0 = CONVERT_SECONDS(3);
	}
}

void Covern(void)
{
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
		CovernInit();
		break;

	case COVERN_POSE_IDLE:
		CovernIdle();
		break;

	case COVERN_POSE_ATTACK_WARNING:
		CovernAttackWarning();
		break;

	case COVERN_POSE_ATTACKING:
		CovernAttacking();
		break;

	default:
		SpriteUtilSpriteDeath(DEATH_RESPAWNING, gCurrentSprite.yPosition, gCurrentSprite.xPosition, TRUE, PE_SPRITE_EXPLOSION_MEDIUM);
	}
}



