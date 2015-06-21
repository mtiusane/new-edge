/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2000-2009 Darklegion Development

This file is part of Tremulous.

Tremulous is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Tremulous is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Tremulous; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
/*
===========================================================================
TREMULOUS EDGE MOD SRC FILE
===========================================================================
*/
// g_weapon.c
// perform the server side effects of a weapon firing

#include "g_local.h"

static  vec3_t  forward, right, up;
static  vec3_t  muzzle;

/*
================
G_ForceWeaponChange
================
*/
void G_ForceWeaponChange( gentity_t *ent, weapon_t weapon )
{
  playerState_t *ps = &ent->client->ps;

  // stop a reload in progress
  if( ps->weaponstate == WEAPON_RELOADING )
  {
    ps->torsoAnim = ( ( ps->torsoAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | TORSO_RAISE;
    ps->weaponTime = 250;
    ps->weaponstate = WEAPON_READY;
  }
  
  if( weapon == WP_NONE ||
      !BG_InventoryContainsWeapon( weapon, ps->stats ) )
  {
    // switch to the first non blaster weapon
    ps->persistant[ PERS_NEWWEAPON ] =
      BG_PrimaryWeapon( ent->client->ps.stats );
  }
  else
    ps->persistant[ PERS_NEWWEAPON ] = weapon;

  // force this here to prevent flamer effect from continuing
  ps->generic1 = WPM_NOTFIRING;

  // The PMove will do an animated drop, raise, and set the new weapon
  ps->pm_flags |= PMF_WEAPON_SWITCH;
}

/*
=================
G_GiveClientMaxAmmo
=================
*/
void G_GiveClientMaxAmmo( gentity_t *ent, qboolean buyingEnergyAmmo )
{
  int i, maxAmmo, maxClips;
  qboolean restoredAmmo = qfalse, restoredEnergy = qfalse;

  for( i = WP_NONE + 1; i < WP_NUM_WEAPONS; i++ )
  {
    qboolean energyWeapon;
  
    energyWeapon = BG_Weapon( i )->usesEnergy;
    if( !BG_InventoryContainsWeapon( i, ent->client->ps.stats ) ||
        BG_Weapon( i )->infiniteAmmo ||
        BG_WeaponIsFull( i, ent->client->ps.stats,
                         ent->client->ps.ammo, ent->client->ps.clips ) ||
        ( buyingEnergyAmmo && !energyWeapon ) )
      continue;
      
    maxAmmo = BG_Weapon( i )->maxAmmo;
    maxClips = BG_Weapon( i )->maxClips;
    
    // Apply battery pack modifier
    if( energyWeapon &&
        BG_InventoryContainsUpgrade( UP_BATTPACK, ent->client->ps.stats ) )
    {
      maxAmmo *= BATTPACK_MODIFIER;
      restoredEnergy = qtrue;
    }

    ent->client->ps.ammo = maxAmmo;
    ent->client->ps.clips = maxClips;

    restoredAmmo = qtrue;
  }

  if( restoredAmmo )
    G_ForceWeaponChange( ent, ent->client->ps.weapon );

  if( restoredEnergy )
    G_AddEvent( ent, EV_RPTUSE_SOUND, 0 );
}

/*
================
G_BounceProjectile
================
*/
void G_BounceProjectile( vec3_t start, vec3_t impact, vec3_t dir, vec3_t endout )
{
  vec3_t v, newv;
  float dot;

  VectorSubtract( impact, start, v );
  dot = DotProduct( v, dir );
  VectorMA( v, -2 * dot, dir, newv );

  VectorNormalize(newv);
  VectorMA(impact, 8192, newv, endout);
}

/*
================
G_WideTrace

Trace a bounding box against entities, but not the world
Also check there is a line of sight between the start and end point
================
*/
static void G_WideTrace( trace_t *tr, gentity_t *ent, float range,
                         float width, float height, gentity_t **target )
{
  vec3_t    mins, maxs;
  vec3_t    end;

  VectorSet( mins, -width, -width, -height );
  VectorSet( maxs, width, width, width );

  *target = NULL;

  if( !ent->client )
    return;

  G_UnlaggedOn( ent, muzzle, range + width );

  VectorMA( muzzle, range, forward, end );

  // Trace against entities
  trap_Trace( tr, muzzle, mins, maxs, end, ent->s.number, CONTENTS_BODY );
  if( tr->entityNum != ENTITYNUM_NONE )
    *target = &g_entities[ tr->entityNum ];

  // Set range to the trace length plus the width, so that the end of the
  // LOS trace is close to the exterior of the target's bounding box
  range = Distance( muzzle, tr->endpos ) + width;
  VectorMA( muzzle, range, forward, end );

  // Trace for line of sight against the world
  trap_Trace( tr, muzzle, NULL, NULL, end, ent->s.number, CONTENTS_SOLID );
  if( tr->entityNum != ENTITYNUM_NONE )
    *target = &g_entities[ tr->entityNum ];

  G_UnlaggedOff( );
}

/*
======================
SnapVectorTowards
SnapVectorNormal

Round a vector to integers for more efficient network
transmission, but make sure that it rounds towards a given point
rather than blindly truncating.  This prevents it from truncating
into a wall.
======================
*/
void SnapVectorTowards( vec3_t v, vec3_t to )
{
  int   i;

  for( i = 0 ; i < 3 ; i++ )
  {
    if( v[ i ] >= 0 )
      v[ i ] = (int)( v[ i ] + ( to[ i ] <= v[ i ] ? 0 : 1 ) );
    else
      v[ i ] = (int)( v[ i ] + ( to[ i ] <= v[ i ] ? -1 : 0 ) );
  }
}

void SnapVectorNormal( vec3_t v, vec3_t normal )
{
  int i;

  for( i = 0 ; i < 3 ; i++ )
  {
    if( v[ i ] >= 0 )
      v[ i ] = (int)( v[ i ] + ( normal[ i ] <= 0 ? 0 : 1 ) );
    else
      v[ i ] = (int)( v[ i ] + ( normal[ i ] <= 0 ? -1 : 0 ) );
  }
}

/*
===============
BloodSpurt

Generates a blood spurt event for traces with accurate end points
===============
*/
static void BloodSpurt( gentity_t *attacker, gentity_t *victim, trace_t *tr )
{
  gentity_t *tent;

  if( !attacker->client )
    return;

  if( victim->health <= 0 )
    return;

  tent = G_TempEntity( tr->endpos, EV_MISSILE_HIT );
  tent->s.otherEntityNum = victim->s.number;
  tent->s.eventParm = DirToByte( tr->plane.normal );
  tent->s.weapon = attacker->s.weapon;
  tent->s.generic1 = attacker->s.generic1; // weaponMode
}

/*
===============
WideBloodSpurt

Calculates the position of a blood spurt for wide traces and generates an event
===============
*/
static void WideBloodSpurt( gentity_t *attacker, gentity_t *victim, trace_t *tr, int mod )
{
  gentity_t *tent;
  vec3_t normal, origin;
  float mag, radius;

  if( !attacker->client )
    return;

  if( victim->health <= 0 )
    return;

  if( tr )
    VectorSubtract( tr->endpos, victim->s.origin, normal );
  else
    VectorSubtract( attacker->client->ps.origin,
                    victim->s.origin, normal );

  // Normalize the horizontal components of the vector difference to the
  // "radius" of the bounding box
  mag = sqrt( normal[ 0 ] * normal[ 0 ] + normal[ 1 ] * normal[ 1 ] );
  radius = victim->r.maxs[ 0 ] * 1.21f;
  if( mag > radius )
  {
    normal[ 0 ] = normal[ 0 ] / mag * radius;
    normal[ 1 ] = normal[ 1 ] / mag * radius;
  }

  // Clamp origin to be within bounding box vertically
  if( normal[ 2 ] > victim->r.maxs[ 2 ] )
    normal[ 2 ] = victim->r.maxs[ 2 ];
  if( normal[ 2 ] < victim->r.mins[ 2 ] )
    normal[ 2 ] = victim->r.mins[ 2 ];

  VectorAdd( victim->s.origin, normal, origin );
  VectorNegate( normal, normal );
  VectorNormalize( normal );

  // Create the blood spurt effect entity
  tent = G_TempEntity( origin, EV_MISSILE_HIT );
  tent->s.eventParm = DirToByte( normal );
  tent->s.otherEntityNum = victim->s.number;
  tent->s.weapon = attacker->s.weapon;

  if( mod == MOD_LEVEL3_CLAW )
  {
    tent->s.generic1 = WPM_PRIMARY;
  }
  else if( mod == MOD_LEVEL3_POUNCE )
  {
    tent->s.generic1 = WPM_SECONDARY;
  }
  else
  {
    tent->s.generic1 = attacker->s.generic1;
  }
}

/*
===============
meleeAttack
===============
*/
void meleeAttack( gentity_t *ent, float range, float width, float height,
                  int damage, meansOfDeath_t mod )
{
  trace_t   tr;
  gentity_t *traceEnt;

  if( mod == MOD_LEVEL2_CLAW && damage == LEVEL2_CLAW_UPG_DMG )
    G_CombatStats_Fire( ent, CSW_LEVEL2_UPG, damage );
  else
    G_CombatStats_FireMOD( ent, mod, damage );

  G_WideTrace( &tr, ent, range, width, height, &traceEnt );
  if( traceEnt == NULL || !traceEnt->takedamage )
    return;

  WideBloodSpurt( ent, traceEnt, &tr, mod );
  G_Damage( traceEnt, ent, ent, forward, tr.endpos, damage, DAMAGE_NO_KNOCKBACK, mod );
}

/*
======================================================================

MACHINEGUN

======================================================================
*/

void bulletFire( gentity_t *ent, float spread, int damage, int mod )
{
  trace_t   tr;
  vec3_t    end;
  float   r;
  float   u;
  gentity_t *tent;
  gentity_t *traceEnt;

  G_CombatStats_FireMOD( ent, mod, damage );

  r = random( ) * M_PI * 2.0f;
  u = sin( r ) * crandom( ) * spread * 16;
  r = cos( r ) * crandom( ) * spread * 16;
  VectorMA( muzzle, 8192 * 16, forward, end );
  VectorMA( end, r, right, end );
  VectorMA( end, u, up, end );

  // don't use unlagged if this is not a client (e.g. turret)
  if( ent->client )
  {
    G_UnlaggedOn( ent, muzzle, 8192 * 16 );
    trap_Trace( &tr, muzzle, NULL, NULL, end, ent->s.number, MASK_SHOT );
    G_UnlaggedOff( );
  }
  else
    trap_Trace( &tr, muzzle, NULL, NULL, end, ent->s.number, MASK_SHOT );

  if( tr.surfaceFlags & SURF_NOIMPACT )
    return;

  traceEnt = &g_entities[ tr.entityNum ];

  // snap the endpos to integers, but nudged towards the line
  SnapVectorTowards( tr.endpos, muzzle );

  // send bullet impact
  if( traceEnt->takedamage &&
      (traceEnt->s.eType == ET_PLAYER ||
       traceEnt->s.eType == ET_BUILDABLE ) )
  {
    tent = G_TempEntity( tr.endpos, EV_BULLET_HIT_FLESH );
    tent->s.eventParm = traceEnt->s.number;
  }
  else
  {
    tent = G_TempEntity( tr.endpos, EV_BULLET_HIT_WALL );
    tent->s.eventParm = DirToByte( tr.plane.normal );
  }
  tent->s.otherEntityNum = ent->s.number;

  if( traceEnt->takedamage )
  {
    G_Damage( traceEnt, ent, ent, forward, tr.endpos,
      damage, 0, mod );
  }
}



/*
======================================================================

SHOTGUN

======================================================================
*/

// this should match CG_ShotgunPattern
void ShotgunPattern( vec3_t origin, vec3_t origin2, int seed, gentity_t *ent )
{
  int        i;
  float      r, u;
  vec3_t    end;
  vec3_t    forward, right, up;
  trace_t    tr;
  gentity_t  *traceEnt;

  // derive the right and up vectors from the forward vector, because
  // the client won't have any other information
  VectorNormalize2( origin2, forward );
  PerpendicularVector( right, forward );
  CrossProduct( forward, right, up );

  // generate the "random" spread pattern
  for( i = 0; i < SHOTGUN_PELLETS; i++ )
  {
    r = Q_crandom( &seed ) * SHOTGUN_SPREAD * 16;
    u = Q_crandom( &seed ) * SHOTGUN_SPREAD * 16;
    VectorMA( origin, SHOTGUN_RANGE, forward, end );
    VectorMA( end, r, right, end );
    VectorMA( end, u, up, end );

    trap_Trace( &tr, origin, NULL, NULL, end, ent->s.number, MASK_SHOT );
    traceEnt = &g_entities[ tr.entityNum ];

    // send bullet impact
    if( !( tr.surfaceFlags & SURF_NOIMPACT ) )
    {
      if( traceEnt->takedamage )
        G_Damage( traceEnt, ent, ent, forward, tr.endpos,  SHOTGUN_DMG, 0, MOD_SHOTGUN );
    }
  }
}


void shotgunFire( gentity_t *ent )
{
  gentity_t    *tent;

  G_CombatStats_Fire( ent, CSW_SHOTGUN, SHOTGUN_DMG * SHOTGUN_PELLETS );

  // send shotgun blast
  tent = G_TempEntity( muzzle, EV_SHOTGUN );
  VectorScale( forward, 4096, tent->s.origin2 );
  SnapVector( tent->s.origin2 );
  tent->s.eventParm = rand() / ( RAND_MAX / 0x100 + 1 );    // seed for spread pattern
  tent->s.otherEntityNum = ent->s.number;
  G_UnlaggedOn( ent, muzzle, SHOTGUN_RANGE );
  ShotgunPattern( tent->s.pos.trBase, tent->s.origin2, tent->s.eventParm, ent );
  G_UnlaggedOff();
}

/*
======================================================================

MASS DRIVER

======================================================================
*/

void massDriverFire( gentity_t *ent )
{
  trace_t   tr;
  vec3_t    end;
  gentity_t *tent;
  gentity_t *traceEnt;

  G_CombatStats_Fire( ent, CSW_MDRIVER, MDRIVER_DMG );

  VectorMA( muzzle, 8192.0f * 16.0f, forward, end );

  G_UnlaggedOn( ent, muzzle, 8192.0f * 16.0f );
  trap_Trace( &tr, muzzle, NULL, NULL, end, ent->s.number, MASK_SHOT );
  G_UnlaggedOff( );

  if( tr.surfaceFlags & SURF_NOIMPACT )
    return;

  traceEnt = &g_entities[ tr.entityNum ];

  // snap the endpos to integers, but nudged towards the line
  SnapVectorTowards( tr.endpos, muzzle );

  // send impact
  if( traceEnt->takedamage && 
      (traceEnt->s.eType == ET_BUILDABLE || 
       traceEnt->s.eType == ET_PLAYER ) )
  {
    BloodSpurt( ent, traceEnt, &tr );
  }
  else
  {
    tent = G_TempEntity( tr.endpos, EV_MISSILE_MISS );
    tent->s.eventParm = DirToByte( tr.plane.normal );
    tent->s.weapon = ent->s.weapon;
    tent->s.generic1 = ent->s.generic1; //weaponMode
  }

  if( traceEnt->takedamage )
  {
    G_Damage( traceEnt, ent, ent, forward, tr.endpos,
      MDRIVER_DMG, 0, MOD_MDRIVER );
  }
}

/*
======================================================================

MD Third mode

======================================================================
*/

void massDriverFire2( gentity_t *ent )
{
  vec3_t origin;

  VectorMA( muzzle, ent->r.maxs[ 2 ], ent->s.origin2, origin );
  
  fire_md2( ent, origin, forward );
}


/*
======================================================================

LOCKBLOB

======================================================================
*/

void lockBlobLauncherFire( gentity_t *ent )
{
  fire_lockblob( ent, muzzle, forward );
}

/*
======================================================================

HIVE

======================================================================
*/

void hiveFire( gentity_t *ent )
{
  vec3_t origin;

  // Fire from the hive tip, not the center
  VectorMA( muzzle, ent->r.maxs[ 2 ], ent->s.origin2, origin );
  
  fire_hive( ent, origin, forward );
}

/*
======================================================================

BLASTER PISTOL

======================================================================
*/

void blasterFire( gentity_t *ent )
{
  G_CombatStats_Fire( ent, CSW_BLASTER, BLASTER_DMG );
  fire_blaster( ent, muzzle, forward );
}

/*
======================================================================

PULSE RIFLE

======================================================================
*/

void pulseRifleFire( gentity_t *ent )
{
  G_CombatStats_Fire( ent, CSW_MDRIVER, PRIFLE_DMG );
  fire_pulseRifle( ent, muzzle, forward );
}

/*
======================================================================

PULSE RIFLE STASIS

======================================================================
*/

void prifleStasisFire( gentity_t *ent )
{
  fire_prifle_stasis( ent, muzzle, forward );
}


/*
===============
FLAME THROWER
Napalm Charge
===============
*/
void NapalmFire( gentity_t *ent, qboolean secondary )
{
  int damage;

  damage = ent->client->ps.stats[ STAT_MISC ] *
           LCANNON_DAMAGE / LCANNON_CHARGE_TIME_MAX;

  G_CombatStats_Fire( ent, CSW_FLAMER, damage );

  NapalmChargeFire( ent, muzzle, forward,
                    damage, LCANNON_RADIUS, LCANNON_SPEED );

  NapalmChargeImp( ent, muzzle, forward,
                   damage, LCANNON_RADIUS, LCANNON_SPEED );

  ent->client->ps.stats[ STAT_MISC ] = 0;
}

/*
===============
FLAME THROWER
Normal
===============
*/
 void FlamerNormal( gentity_t *ent )
{
  vec3_t origin;

  G_CombatStats_Fire( ent, CSW_FLAMER, FLAMER_DMG );

  // Correct muzzle so that the missile does not start in the ceiling 
  VectorMA( muzzle, -7.0f, up, origin );

  // Correct muzzle so that the missile fires from the player's hand
  VectorMA( origin, 4.5f, right, origin );

  FlamerNormalFire( ent, origin, forward );
}

/*
===============
FireBreath Tyrant
===============
*/
void FireBreath_tyrant( gentity_t *ent )
{
  G_CombatStats_Fire( ent, CSW_LEVEL4_ALT, LEVEL4_FIREBREATHDMG );
  FireBreath_fire( ent, muzzle, forward, 0, LCANNON_RADIUS, LCANNON_SPEED );
}

/*
======================================================================
FlameTurret
======================================================================
*/

void FlameTurretFire( gentity_t *ent )
{
  muzzle[2] += 8.0f;
  FlameTurretFireNormal( ent, muzzle, forward );
}

/*
======================================================================
GRENADE
======================================================================
*/

void throwGrenade( gentity_t *ent )
{
  G_CombatStats_Fire( ent, CSW_GRENADE, GRENADE_DAMAGE );

  launch_grenade( ent, muzzle, forward );
  launch_grenade_flames( ent, muzzle, forward );
}

/*
======================================================================
MINE
======================================================================
*/

void throwMine( gentity_t *ent )
{
  launch_mine( ent, muzzle, forward );
}

/*
======================================================================
LAS GUN
======================================================================
*/

void lasGunFire( gentity_t *ent )
{
  trace_t   tr;
  vec3_t    end;
  gentity_t *tent;
  gentity_t *traceEnt;

  G_CombatStats_Fire( ent, CSW_LASGUN, LASGUN_DAMAGE );

  VectorMA( muzzle, 8192 * 16, forward, end );

  G_UnlaggedOn( ent, muzzle, 8192 * 16 );
  trap_Trace( &tr, muzzle, NULL, NULL, end, ent->s.number, MASK_SHOT );
  G_UnlaggedOff( );

  if( tr.surfaceFlags & SURF_NOIMPACT )
  return;

  traceEnt = &g_entities[ tr.entityNum ];

  // snap the endpos to integers, but nudged towards the line
  SnapVectorTowards( tr.endpos, muzzle );

  // send impact
  if( traceEnt->takedamage && 
      (traceEnt->s.eType == ET_BUILDABLE || 
       traceEnt->s.eType == ET_PLAYER ) )
  {
    BloodSpurt( ent, traceEnt, &tr );
  }
  else
  {
    tent = G_TempEntity( tr.endpos, EV_MISSILE_MISS );
    tent->s.eventParm = DirToByte( tr.plane.normal );
    tent->s.weapon = ent->s.weapon;
    tent->s.generic1 = ent->s.generic1; //weaponMode
  }

  if( traceEnt->takedamage )
    G_Damage( traceEnt, ent, ent, forward, tr.endpos, LASGUN_DAMAGE, 0, MOD_LASGUN );
}

// Lasgun 2ndary (shield)

void lasGunFire2( gentity_t *ent )
{
  launch_shield( ent, muzzle, forward );
}

/*
======================================================================
PAIN SAW
======================================================================
*/
void painSawFire( gentity_t *ent )
{
  trace_t   tr;
  vec3_t    temp;
  gentity_t *tent, *traceEnt;

  G_CombatStats_Fire( ent, CSW_PAINSAW, PAINSAW_DAMAGE );

  G_WideTrace( &tr, ent, PAINSAW_RANGE, PAINSAW_WIDTH, PAINSAW_HEIGHT,
               &traceEnt );
  if( !traceEnt || !traceEnt->takedamage )
    return;

  // hack to line up particle system with weapon model
  tr.endpos[ 2 ] -= 5.0f;

  // send blood impact
  if( traceEnt->s.eType == ET_PLAYER || traceEnt->s.eType == ET_BUILDABLE )
  {
      BloodSpurt( ent, traceEnt, &tr );
  }
  else
  {
    VectorCopy( tr.endpos, temp );
    tent = G_TempEntity( temp, EV_MISSILE_MISS );
    tent->s.eventParm = DirToByte( tr.plane.normal );
    tent->s.weapon = ent->s.weapon;
    tent->s.generic1 = ent->s.generic1; //weaponMode
  }

  G_Damage( traceEnt, ent, ent, forward, tr.endpos, PAINSAW_DAMAGE, DAMAGE_NO_KNOCKBACK, MOD_PAINSAW );
}

/*
======================================================================
PSAW BLADES
======================================================================
*/
void painSawFire2( gentity_t *ent )
{
  G_CombatStats_Fire( ent, CSW_PAINSAW_ALT, PAINSAW_DAMAGE2 );
  launch_saw( ent, muzzle, forward );
}

/*
======================================================================

LUCIFER CANNON

======================================================================
*/

void LCChargeFire( gentity_t *ent, qboolean secondary )
{
  if( secondary && ent->client->ps.stats[ STAT_MISC ] <= 0 )
  {
    G_CombatStats_Fire( ent, CSW_LCANNON, LCANNON_SECONDARY_DAMAGE );

    fire_luciferCannon( ent, muzzle, forward, LCANNON_SECONDARY_DAMAGE,
                            LCANNON_SECONDARY_RADIUS, LCANNON_SECONDARY_SPEED );
  }
  else
  {
    int damage;

    damage = ent->client->ps.stats[ STAT_MISC ] *
             LCANNON_DAMAGE / LCANNON_CHARGE_TIME_MAX;

    G_CombatStats_Fire( ent, CSW_LCANNON, damage );

    fire_luciferCannon( ent, muzzle, forward, damage,
                        LCANNON_RADIUS, LCANNON_SPEED );
  }

  ent->client->ps.stats[ STAT_MISC ] = 0;
}

/*
======================================================================
ROCKET LAUNCHER
======================================================================
*/

void rocketLauncherFire( gentity_t *ent )
{
  G_CombatStats_Fire( ent, CSW_ROCKETL, ROCKETL_DAMAGE );
  fire_rocket( ent, muzzle, forward );
}


/*
======================================================================
LIGHTNING GUN
======================================================================
*/

float G_LightningAccuracy( const vec3_t ws_origin, const vec3_t ws_dir,
                           const vec3_t mins, const vec3_t maxs, const vec3_t ws_target )
{
        int i/*, damage*/;
	vec3_t origin, dir, boxcenter, boxdelta;
	float dist, chord;

	VectorSubtract( ws_origin, ws_target, origin );
	VectorSubtract( maxs, mins, boxdelta );

	for( i = 0; i < 3; i++ )
		boxcenter[ i ] = ( mins[ i ] + maxs[ i ] ) / 2.0f;

	VectorSubtract( origin, boxcenter, origin );

	for( i = 0; i < 3; i++ )
		origin[ i ] *= 2.0f / boxdelta[ i ],
		dir[ i ] = ws_dir[ i ] * 1.0f / boxdelta[ i ];

	VectorNormalize( dir );

	dist = VectorLength( origin );

	if( dist <= 1.0f ) // inside the sphere (shouldn't happen)
		return 1.0f;

	chord = pow( DotProduct( origin, dir ), 2 ) - pow( dist, 2 ) + 1;

	if( chord <= 0.0f )
		return 0.0f;

	chord = pow( chord, LIGHTNING_DIFFICULTY * 0.5f );

	return chord;
}


void lightningGunFire( gentity_t *ent )
{
        vec3_t /*start, */end, mins, maxs, target_origin;
	trace_t tr;
	gentity_t *target;
	int damage;

	damage = LIGHTNING_DAMAGE;
	G_CombatStats_Fire( ent, CSW_LIGHTNING, damage );

	VectorMA( muzzle, LIGHTNING_RANGE, forward, end );

	G_UnlaggedOn( ent, muzzle, LIGHTNING_RANGE );

	trap_Trace( &tr, muzzle, NULL, NULL, end, ent->s.number, MASK_SHOT );

	if( tr.fraction == 1.0f ||
	    tr.entityNum == ENTITYNUM_NONE ||
	    ( tr.surfaceFlags & SURF_NOIMPACT ) )
	{
		G_UnlaggedOff( );
		return;
	}

	target = g_entities + tr.entityNum;


	if( target->s.eType == ET_PLAYER || target->s.eType == ET_BUILDABLE )
	{
		float acc;

		VectorCopy( target->r.mins, mins );
		VectorCopy( target->r.maxs, maxs );
		VectorCopy( target->r.currentOrigin, target_origin );
		BloodSpurt( ent, target, &tr );

		acc = G_LightningAccuracy( muzzle, forward, mins, maxs, target_origin );
		damage = MAX( round( (float)damage * acc ), 1 );
	}
	else
	{
		gentity_t *tent;

		tent = G_TempEntity( tr.endpos, EV_MISSILE_MISS );
		tent->s.eventParm = DirToByte( tr.plane.normal );
		tent->s.weapon = ent->s.weapon;
		tent->s.generic1 = ent->s.generic1;
	}

	G_UnlaggedOff( );
	G_Damage( target, ent, ent, forward, tr.endpos, damage, DAMAGE_NO_KNOCKBACK, MOD_LIGHTNING );
}


/*
======================================================================
TESLA GENERATOR
======================================================================
*/
void teslaFire( gentity_t *self )
{
  trace_t tr;
  vec3_t origin, target;
  gentity_t *tent;

  if( !self->enemy )
    return;

  // Move the muzzle from the entity origin up a bit to fire over turrets
  VectorMA( muzzle, self->r.maxs[ 2 ], self->s.origin2, origin );

  // Don't aim for the center, aim at the top of the bounding box
  VectorCopy( self->enemy->s.origin, target );
  target[ 2 ] += self->enemy->r.maxs[ 2 ];

  // Trace to the target entity
  trap_Trace( &tr, origin, NULL, NULL, target, self->s.number, MASK_SHOT );
  if( tr.entityNum != self->enemy->s.number )
    return;

  // Client side firing effect
  self->s.eFlags |= EF_FIRING;

  // Deal damage
  if( self->enemy->takedamage )
  {
    vec3_t dir;

    VectorSubtract( target, origin, dir );
    G_Damage( self->enemy, self, self, dir, tr.endpos,
              TESLAGEN_DMG, 0, MOD_TESLAGEN );
  }

  // Send tesla zap trail
  tent = G_TempEntity( tr.endpos, EV_TESLATRAIL );
  tent->s.generic1 = self->s.number; // src
  tent->s.clientNum = self->enemy->s.number; // dest
}


/*
======================================================================
BUILD GUN
======================================================================
*/
void CheckCkitRepair( gentity_t *ent )
{
  vec3_t      viewOrigin, forward, end;
  trace_t     tr;
  gentity_t   *traceEnt;
  int         bHealth;

  if( ent->client->ps.weaponTime > 0 ||
      ent->client->ps.stats[ STAT_MISC ] > 0 )
    return;

  BG_GetClientViewOrigin( &ent->client->ps, viewOrigin );
  AngleVectors( ent->client->ps.viewangles, forward, NULL, NULL );
  VectorMA( viewOrigin, 100, forward, end );

  trap_Trace( &tr, viewOrigin, NULL, NULL, end, ent->s.number, MASK_PLAYERSOLID );
  traceEnt = &g_entities[ tr.entityNum ];

  if( tr.fraction < 1.0f && traceEnt->spawned && traceEnt->health > 0 &&
      traceEnt->s.eType == ET_BUILDABLE && traceEnt->buildableTeam == TEAM_HUMANS )
  {
    bHealth = BG_Buildable( traceEnt->s.modelindex )->health;
    if( traceEnt->health < bHealth )
    {
      traceEnt->health += HBUILD_HEALRATE;
      if( traceEnt->health >= bHealth )
      {
        traceEnt->health = bHealth;
        G_AddEvent( ent, EV_BUILD_REPAIRED, 0 );
      }
      else
        G_AddEvent( ent, EV_BUILD_REPAIR, 0 );

      ent->client->ps.weaponTime += BG_Weapon( ent->client->ps.weapon )->repeatRate1;
    }
  }
}

/*
===============
cancelBuildFire
===============
*/
void cancelBuildFire( gentity_t *ent )
{
  // Cancel ghost buildable
  if( ent->client->ps.stats[ STAT_BUILDABLE ] != BA_NONE )
  {
    ent->client->ps.stats[ STAT_BUILDABLE ] = BA_NONE;
    return;
  }
  if( ent->client->ps.weapon == WP_ABUILD )
    meleeAttack( ent, ABUILDER_CLAW_RANGE, ABUILDER_CLAW_WIDTH,
                 ABUILDER_CLAW_WIDTH, ABUILDER_CLAW_DMG, MOD_ABUILDER_CLAW );
}

/*
===============
buildFire
===============
*/
void buildFire( gentity_t *ent, dynMenu_t menu )
{
  buildable_t buildable = ( ent->client->ps.stats[ STAT_BUILDABLE ]
                            & ~SB_VALID_TOGGLEBIT );

  if( buildable > BA_NONE )
  {
    if( ent->client->ps.stats[ STAT_MISC ] > 0 )
    {
      G_AddEvent( ent, EV_BUILD_DELAY, ent->client->ps.clientNum );
      return;
    }

    if( G_BuildIfValid( ent, buildable ) )
    {
      if( !g_cheats.integer )
      {
        ent->client->ps.stats[ STAT_MISC ] +=
          BG_Buildable( buildable )->buildTime;
      }

      ent->client->ps.stats[ STAT_BUILDABLE ] = BA_NONE;
    }

    return;
  }

  G_TriggerMenu( ent->client->ps.clientNum, menu );
}

void slowBlobFire( gentity_t *ent )
{
  G_CombatStats_Fire( ent, CSW_ABUILDER_ALT, ABUILDER_BLOB_DMG );
  fire_slowBlob( ent, muzzle, forward );
}


/*
======================================================================
LEVEL0
======================================================================
*/
/*
===============
CheckVenomAttack
===============
*/
qboolean CheckVenomAttack( gentity_t *ent )
{
  trace_t   tr;
  gentity_t *traceEnt;
  int       damage = LEVEL0_BITE_DMG;

  if( ent->client->ps.weaponTime )
	return qfalse;

  // Calculate muzzle point
  AngleVectors( ent->client->ps.viewangles, forward, right, up );
  CalcMuzzlePoint( ent, forward, right, up, muzzle );

  G_WideTrace( &tr, ent, LEVEL0_BITE_RANGE, LEVEL0_BITE_WIDTH,
               LEVEL0_BITE_WIDTH, &traceEnt );

  if( traceEnt == NULL )
    return qfalse;

  if( !traceEnt->takedamage )
    return qfalse;

  if( traceEnt->health <= 0 )
      return qfalse;

  // only allow bites to work against buildings as they are constructing
  if( traceEnt->s.eType == ET_BUILDABLE )
  {
    if( traceEnt->buildableTeam == TEAM_ALIENS )
      return qfalse;

    if ( !( traceEnt->s.modelindex == BA_H_MGTURRET || traceEnt->s.modelindex == BA_H_MGTURRET2 || traceEnt->s.modelindex == BA_H_TESLAGEN || !traceEnt->spawned ) )
      damage = (int)(damage * g_DretchBuildingDamage.value);
    else
      damage = (int)(damage * g_DretchTurretDamage.value);

    if (damage <= 0)
      return qfalse;
  }

  if( traceEnt->client )
  {
    if( traceEnt->client->ps.stats[ STAT_TEAM ] == TEAM_ALIENS )
      return qfalse;
    if( traceEnt->client->ps.stats[ STAT_HEALTH ] <= 0 )
      return qfalse;
  }

  // send blood impact
  WideBloodSpurt( ent, traceEnt, &tr, MOD_LEVEL0_BITE );

  G_Damage( traceEnt, ent, ent, forward, tr.endpos, damage, DAMAGE_NO_KNOCKBACK, MOD_LEVEL0_BITE );
  ent->client->ps.weaponTime += LEVEL0_BITE_REPEAT;
  return qtrue;
}

/*
======================================================================
LEVEL2
======================================================================
*/
#define MAX_ZAPS MAX_CLIENTS

static zap_t zaps[ MAX_ZAPS ];
/*
===============
G_FindZapChainTargets
===============
*/
static void G_FindZapChainTargets( zap_t *zap )
{
  gentity_t *ent = zap->targets[ 0 ]; // the source
  int       entityList[ MAX_GENTITIES ];
  vec3_t    range = { LEVEL2_AREAZAP_CHAIN_RANGE,
                      LEVEL2_AREAZAP_CHAIN_RANGE,
                      LEVEL2_AREAZAP_CHAIN_RANGE };
  vec3_t    mins, maxs;
  int       i, num;
  gentity_t *enemy;
  trace_t   tr;
  float     distance;

  VectorAdd( ent->s.origin, range, maxs );
  VectorSubtract( ent->s.origin, range, mins );

  num = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );

  for( i = 0; i < num; i++ )
  {
    enemy = &g_entities[ entityList[ i ] ];
    // don't chain to self; noclippers can be listed, don't chain to them either
    if( enemy == ent || ( enemy->client && enemy->client->noclip ) )
      continue;

    distance = Distance( ent->s.origin, enemy->s.origin );

    if( ( ( enemy->client &&
            enemy->client->ps.stats[ STAT_TEAM ] == TEAM_HUMANS ) ||
          ( enemy->s.eType == ET_BUILDABLE &&
            BG_Buildable( enemy->s.modelindex )->team == TEAM_HUMANS ) ||
	  ( enemy->s.eType == ET_MISSILE &&
	    enemy->s.weapon == WP_MINE ) ||
	  ( enemy->client &&
	    enemy->client->ps.stats[ STAT_TEAM ] == TEAM_ALIENS &&
	    enemy->client->ps.stats[ STAT_CLASS ] == PCL_ALIEN_LEVEL2_UPG ) ) &&
        enemy->health > 0 && // only chain to living targets
        distance <= LEVEL2_AREAZAP_CHAIN_RANGE )
    {
      // world-LOS check: trace against the world, ignoring other BODY entities
      trap_Trace( &tr, ent->s.origin, NULL, NULL,
         enemy->s.origin, ent->s.number, CONTENTS_SOLID );

      if( tr.entityNum == ENTITYNUM_NONE )
      {
        zap->targets[ zap->numTargets ] = enemy;
        zap->distances[ zap->numTargets ] = distance;
        if( ++zap->numTargets >= LEVEL2_AREAZAP_MAX_TARGETS )
          return;
      }
    }
  }
}

/*
===============
G_UpdateZapEffect
===============
*/
static void G_UpdateZapEffect( zap_t *zap )
{
  int i;
  int entityNums[ LEVEL2_AREAZAP_MAX_TARGETS + 1 ];

  entityNums[ 0 ] = zap->creator->s.number;

  for( i = 0; i < zap->numTargets; i++ )
    entityNums[ i + 1 ] = zap->targets[ i ]->s.number;

  BG_PackEntityNumbers( &zap->effectChannel->s,
                        entityNums, zap->numTargets + 1 );

  VectorCopy( zap->creator->s.origin, zap->effectChannel->r.currentOrigin );
  trap_LinkEntity( zap->effectChannel );
}

/*
===============
G_CreateNewZap
===============
*/
static void G_CreateNewZap( gentity_t *creator, gentity_t *target )
{
  int   i;
  zap_t *zap;

  for( i = 0; i < MAX_ZAPS; i++ )
  {
    zap = &zaps[ i ];
    if( zap->used )
      continue;

    zap->used = qtrue;
    zap->timeToLive = LEVEL2_AREAZAP_TIME;

    zap->creator = creator;
    zap->targets[ 0 ] = target;
    zap->numTargets = 1;

    // the zap chains only through living entities
    if( target->health > 0 )
    {
      if( !target->client || target->client->ps.stats[ STAT_TEAM ] != TEAM_ALIENS )
        G_Damage( target, creator, creator, forward, target->s.origin, LEVEL2_AREAZAP_DMG,
                  DAMAGE_NO_KNOCKBACK | DAMAGE_NO_LOCDAMAGE, MOD_LEVEL2_ZAP );

      G_FindZapChainTargets( zap );

      for( i = 1; i < zap->numTargets; i++ )
      {
        if( !zap->targets[ i ]->client ||
            zap->targets[ i ]->client->ps.stats[ STAT_TEAM ] != TEAM_ALIENS )
          G_Damage( zap->targets[ i ], target, zap->creator, forward, zap->targets[ i ]->s.origin,
                    LEVEL2_AREAZAP_DMG * ( 1 - pow( (zap->distances[ i ] /
                    LEVEL2_AREAZAP_CHAIN_RANGE ), LEVEL2_AREAZAP_CHAIN_FALLOFF ) ) + 1,
                    DAMAGE_NO_KNOCKBACK | DAMAGE_NO_LOCDAMAGE,
                    MOD_LEVEL2_ZAP );
      }
    }

    zap->effectChannel = G_Spawn( );
    zap->effectChannel->s.eType = ET_LEV2_ZAP_CHAIN;
    zap->effectChannel->classname = "lev2zapchain";
    G_UpdateZapEffect( zap );

    return;
  }
}

/*
===============
G_UpdateZaps
===============
*/
void G_UpdateZaps( int msec )
{
  int   i, j;
  zap_t *zap;

  for( i = 0; i < MAX_ZAPS; i++ )
  {
    zap = &zaps[ i ];
    if( !zap->used )
      continue;

    zap->timeToLive -= msec;

    // first, the disappearance of players is handled immediately in G_ClearPlayerZapEffects()
    // the deconstruction or gibbing of a directly targeted buildable destroys the whole zap effect
    if( zap->timeToLive <= 0 || !zap->targets[ 0 ]->inuse )
    {
      G_FreeEntity( zap->effectChannel );
      zap->used = qfalse;
      continue;
    }

    // the deconstruction or gibbing of chained buildables destroy the appropriate beams
    for( j = 1; j < zap->numTargets; j++ )
    {
      if( !zap->targets[ j ]->inuse )
        zap->targets[ j-- ] = zap->targets[ --zap->numTargets ];
    }

    G_UpdateZapEffect( zap );
  }
}

/*
===============
G_ClearPlayerZapEffects

called from G_LeaveTeam() and TeleportPlayer()
===============
*/
void G_ClearPlayerZapEffects( gentity_t *player )
{
  int i, j;
  zap_t *zap;

  for( i = 0; i < MAX_ZAPS; i++ )
  {
    zap = &zaps[ i ];
    if( !zap->used )
      continue;

    // the disappearance of the creator or the first target destroys the whole zap effect
    if( zap->creator == player || zap->targets[ 0 ] == player )
    {
      G_FreeEntity( zap->effectChannel );
      zap->used = qfalse;
      continue;
    }

    // the disappearance of chained players destroy the appropriate beams
    for( j = 1; j < zap->numTargets; j++ )
    {
      if( zap->targets[ j ] == player )
        zap->targets[ j-- ] = zap->targets[ --zap->numTargets ];
    }
  }
}


/*
===============
G_MedkitTarget

Look for a possible healing target (a client) in the front.
===============
*/
gentity_t *G_MedkitTarget( gentity_t *ent )
{
  trace_t   tr;
  gentity_t *targetEnt = NULL;

  if( g_humanMedkitRange.value <= 0 ||
      g_humanMedkitWidth.value <= 0 )
    return NULL;

  // Calculate muzzle point
  AngleVectors( ent->client->ps.viewangles, forward, right, up );
  CalcMuzzlePoint( ent, forward, right, up, muzzle );

  G_WideTrace( &tr, ent, g_humanMedkitRange.value,
      g_humanMedkitWidth.value, g_humanMedkitWidth.value, &targetEnt );

  if( ( targetEnt != NULL ) &&
      ( targetEnt->client != NULL ) )
    return targetEnt;
  else
    return NULL;
}

/*
===============
areaZapFire
===============
*/
void areaZapFire( gentity_t *ent )
{
  trace_t   tr;
  gentity_t *traceEnt;

  G_CombatStats_Fire( ent, CSW_LEVEL2_ALT, LEVEL2_AREAZAP_DMG );

  G_WideTrace( &tr, ent, LEVEL2_AREAZAP_RANGE, LEVEL2_AREAZAP_WIDTH, LEVEL2_AREAZAP_WIDTH, &traceEnt );

  if( traceEnt == NULL )
    return;

  if( ( traceEnt->client && traceEnt->client->ps.stats[ STAT_TEAM ] == TEAM_HUMANS ) ||
      ( traceEnt->s.eType == ET_BUILDABLE &&
        BG_Buildable( traceEnt->s.modelindex )->team == TEAM_HUMANS ) ||
      ( traceEnt->s.eType == ET_MISSILE &&
	traceEnt->s.weapon == WP_MINE ) ||
      ( traceEnt->client && traceEnt->client->ps.stats[ STAT_TEAM ] == TEAM_ALIENS &&
	traceEnt->client->ps.stats[ STAT_CLASS ] == PCL_ALIEN_LEVEL2_UPG ) )
  {
    G_CreateNewZap( ent, traceEnt );
  }
}


/*
======================================================================
LEVEL3 / Hummel
======================================================================
*/
/*
===============
CheckPounceAttack
===============
*/
qboolean CheckPounceAttack( gentity_t *ent )
{
  trace_t tr;
  gentity_t *traceEnt;
  int damage, timeMax, pounceRange, pounceWidth, payload;

  if( ent->client->pmext.pouncePayload <= 0 )
    return qfalse;

  // In case the goon lands on his target, he get's one shot after landing
  payload = ent->client->pmext.pouncePayload;
  if( !( ent->client->ps.pm_flags & PMF_CHARGE/* || ent->client->ps.weapon == WP_ALEVEL5 */) )
    ent->client->pmext.pouncePayload = 0;

  // Calculate muzzle point
  AngleVectors( ent->client->ps.viewangles, forward, right, up );
  CalcMuzzlePoint( ent, forward, right, up, muzzle );

  // Trace from muzzle to see what we hit
  if( ent->client->ps.weapon == WP_ALEVEL5)
  {
      pounceRange = LEVEL5_POUNCE_RANGE;
      pounceWidth = LEVEL5_POUNCE_WIDTH;
  }
  else
  {
      pounceRange = ent->client->ps.weapon == WP_ALEVEL3 ? LEVEL3_POUNCE_RANGE : LEVEL3_POUNCE_UPG_RANGE;
      pounceWidth = LEVEL3_POUNCE_WIDTH;
  }
  G_WideTrace( &tr, ent, pounceRange, pounceWidth, pounceWidth, &traceEnt );
  if( traceEnt == NULL )
    return qfalse;

  // Send blood impact
  if( traceEnt->takedamage )
    WideBloodSpurt( ent, traceEnt, &tr, MOD_LEVEL3_POUNCE );

  if( !traceEnt->takedamage )
    return qfalse;
    
  // Deal damage
  if( ent->client->ps.weapon == WP_ALEVEL5)
  {
    timeMax = LEVEL5_POUNCE_TIME;
    damage = payload * LEVEL5_POUNCE_DMG / timeMax;
    ent->client->pmext.pouncePayload = 0;
    G_Damage( traceEnt, ent, ent, forward, tr.endpos, damage,
	      DAMAGE_NO_LOCDAMAGE, MOD_LEVEL5_POUNCE );
  }
  else
  {
    timeMax = ent->client->ps.weapon == WP_ALEVEL3 ? LEVEL3_POUNCE_TIME : LEVEL3_POUNCE_TIME_UPG;
    damage = payload * LEVEL3_POUNCE_DMG / timeMax;
    ent->client->pmext.pouncePayload = 0;
    G_Damage( traceEnt, ent, ent, forward, tr.endpos, damage,
	      DAMAGE_NO_LOCDAMAGE, MOD_LEVEL3_POUNCE );
  }
  return qtrue;
}

void bounceBallFire( gentity_t *ent )
{
  G_CombatStats_Fire( ent, CSW_LEVEL3_ALT, LEVEL3_BOUNCEBALL_DMG );
  fire_bounceBall( ent, muzzle, forward );
}


void bounceBallFire_level2( gentity_t *ent )
{
  fire_bounceBall2( ent, muzzle, forward,
		    WP_ALEVEL2_UPG, LEVEL2_BOUNCEBALL_DMG,
		    MOD_LEVEL2_BOUNCEBALL, LEVEL2_BOUNCEBALL_SPEED,
		    LEVEL2_BOUNCEBALL_RADIUS );
}

/*
======================================================================
LEVEL4
======================================================================
*/

/*
===============
G_ChargeAttack
===============
*/
void G_ChargeAttack( gentity_t *ent, gentity_t *victim )
{
  int       damage;
  int       i;
  vec3_t    forward;

  if( ent->client->ps.stats[ STAT_MISC ] <= 0 ||
      !( ent->client->ps.stats[ STAT_STATE ] & SS_CHARGING ) ||
      ent->client->ps.weaponTime )
    return;

  VectorSubtract( victim->s.origin, ent->s.origin, forward );
  VectorNormalize( forward );

  if( !victim->takedamage )
    return;

  // For buildables, track the last MAX_TRAMPLE_BUILDABLES_TRACKED buildables
  //  hit, and do not do damage if the current buildable is in that list
  //  in order to prevent dancing over stuff to kill it very quickly
  if( !victim->client )
  {
    for( i = 0; i < MAX_TRAMPLE_BUILDABLES_TRACKED; i++ )
    {
      if( ent->client->trampleBuildablesHit[ i ] == victim - g_entities )
        return;
    }

    ent->client->trampleBuildablesHit[
      ent->client->trampleBuildablesHitPos++ % MAX_TRAMPLE_BUILDABLES_TRACKED ] =
      victim - g_entities;
  }

  WideBloodSpurt( ent, victim, NULL, MOD_LEVEL4_TRAMPLE );

  damage = LEVEL4_TRAMPLE_DMG * ent->client->ps.stats[ STAT_MISC ] /
           LEVEL4_TRAMPLE_DURATION;

  G_Damage( victim, ent, ent, forward, victim->s.origin, damage,
            DAMAGE_NO_LOCDAMAGE, MOD_LEVEL4_TRAMPLE );

  ent->client->ps.weaponTime += LEVEL4_TRAMPLE_REPEAT;
}

/*
===============
G_CrushAttack
Should only be called if there was an impact between a tyrant and another player
===============
*/
void G_CrushAttack( gentity_t *ent, gentity_t *victim )
{
  vec3_t dir;
  float jump;
  int damage;

  if( !victim->takedamage ||
      ent->client->ps.origin[ 2 ] + ent->r.mins[ 2 ] <
      victim->s.origin[ 2 ] + victim->r.maxs[ 2 ] ||
      ( victim->client &&
        victim->client->ps.groundEntityNum == ENTITYNUM_NONE ) )
    return;

  // Deal velocity based damage to target
  jump = BG_Class( ent->client->ps.stats[ STAT_CLASS ] )->jumpMagnitude;
  damage = ( ent->client->pmext.fallVelocity + jump ) *
           -LEVEL4_CRUSH_DAMAGE_PER_V;

  if( damage < 0 )
    damage = 0;
    
  // Players also get damaged periodically
  if( victim->client &&
      ent->client->lastCrushTime + LEVEL4_CRUSH_REPEAT < level.time )
  {
    ent->client->lastCrushTime = level.time;
    damage += LEVEL4_CRUSH_DAMAGE;
  }
  
  if( damage < 1 )
    return;

  // Crush the victim over a period of time
  VectorSubtract( victim->s.origin, ent->client->ps.origin, dir );
  G_Damage( victim, ent, ent, dir, victim->s.origin, damage,
            DAMAGE_NO_LOCDAMAGE, MOD_LEVEL4_CRUSH );
}

//======================================================================

/*
===============
CalcMuzzlePoint
set muzzle location relative to pivoting eye
===============
*/
void CalcMuzzlePoint( gentity_t *ent, vec3_t forward, vec3_t right, vec3_t up, vec3_t muzzlePoint )
{
  vec3_t normal;

  VectorCopy( ent->client->ps.origin, muzzlePoint );
  BG_GetClientNormal( &ent->client->ps, normal );
  VectorMA( muzzlePoint, ent->client->ps.viewheight, normal, muzzlePoint );
  VectorMA( muzzlePoint, 1, forward, muzzlePoint );
  // snap to integer coordinates for more efficient network bandwidth usage
  SnapVector( muzzlePoint );
}

/*
======================================================================
LEVEL5

======================================================================
*/

void Prickles( gentity_t *ent )
{
  vec3_t  dir;
  float   r;
  float   u;

  G_CombatStats_Fire( ent, CSW_LEVEL5_ALT, LEVEL5_PRICKLES_DMG );

  r = random( ) * M_PI * 2.0f;
  u = sin( r ) * crandom( ) * LEVEL5_PRICKLES_SPREAD * 16;
  r = cos( r ) * crandom( ) * LEVEL5_PRICKLES_SPREAD * 16;

  VectorScale( forward, 8192 * 16, dir );
  VectorMA( dir, r, right, dir );
  VectorMA( dir, u, up, dir );
  VectorNormalize( dir );

  Prickles_Fire( ent, muzzle, dir );
}


////////////////////////////////////
//FIRE WEAPONS//////////////////////
/*
===============
FireWeapon3
===============
*/
void FireWeapon3( gentity_t *ent )
{
  if( ent->client )
  {
    // set aiming directions
    AngleVectors( ent->client->ps.viewangles, forward, right, up );
    CalcMuzzlePoint( ent, forward, right, up, muzzle );
  }
  else
  {
    AngleVectors( ent->s.angles2, forward, right, up );
    VectorCopy( ent->s.pos.trBase, muzzle );
  }

  // fire the specific weapon
  switch( ent->s.weapon )
  {
    case WP_ALEVEL3_UPG:
      bounceBallFire( ent );
      break;

    case WP_ABUILD:
      slowBlobFire( ent );
      break;

   case WP_ALEVEL2_UPG:
      bounceBallFire_level2( ent );
      break;

  case WP_ALEVEL5:
      Prickles( ent );
      break;
	  
    case WP_ALEVEL4:
      // FireBreath_tyrant( ent );
      break;

    case WP_MASS_DRIVER:
      if(g_humanStage.integer == S5 && BG_InventoryContainsUpgrade( UP_BATTPACK, ent->client->ps.stats ))
      {
	  massDriverFire2( ent );
      }
      break;

    default:
      break;
  }
}

/*
===============
FireWeapon2
===============
*/
void FireWeapon2( gentity_t *ent )
{
  if( ent->client )
  {
    // set aiming directions
    AngleVectors( ent->client->ps.viewangles, forward, right, up );
    CalcMuzzlePoint( ent, forward, right, up, muzzle );
  }
  else
  {
    AngleVectors( ent->s.angles2, forward, right, up );
    VectorCopy( ent->s.pos.trBase, muzzle );
  }

  // fire the specific weapon
  switch( ent->s.weapon )
  {
  
    case WP_MACHINEGUN:
      bulletFire( ent, RIFLE_SPREAD2, RIFLE_DMG2, MOD_MACHINEGUN );
      break;

    case WP_ALEVEL2_UPG:
      areaZapFire( ent );
      break;

    case WP_PAIN_SAW:
      painSawFire2( ent );
      break; 
	  
    case WP_LUCIFER_CANNON:
      LCChargeFire( ent, qtrue );
      break;
	  
    case WP_CHAINGUN:
      bulletFire( ent, CHAINGUN_SPREAD2, CHAINGUN_DMG2, MOD_CHAINGUN );
      break;
	  
    case WP_FLAMER:
      FlamerNormal( ent );
      break;

    case WP_LAS_GUN:
      lasGunFire2( ent );
      break;
	  
    case WP_PULSE_RIFLE:
      prifleStasisFire( ent );
      break;
	  
    case WP_ABUILD:
    case WP_HBUILD:
      cancelBuildFire( ent );
      break;
	  
    default:
      break;
  }
}

/*
===============
FireWeapon
===============
*/
void FireWeapon( gentity_t *ent )
{
  if( ent->client )
  {
    // set aiming directions
    AngleVectors( ent->client->ps.viewangles, forward, right, up );
    CalcMuzzlePoint( ent, forward, right, up, muzzle );    
  }
  else
  {
    AngleVectors( ent->turretAim, forward, right, up );
    VectorCopy( ent->s.pos.trBase, muzzle );
  }

  // fire the specific weapon
  switch( ent->s.weapon )
  {
    case WP_ALEVEL3:
      meleeAttack( ent, LEVEL3_CLAW_RANGE, LEVEL3_CLAW_WIDTH, LEVEL3_CLAW_WIDTH,
                   LEVEL3_CLAW_DMG, MOD_LEVEL3_CLAW );
      break;
    case WP_ALEVEL3_UPG:
      meleeAttack( ent, LEVEL3_CLAW_UPG_RANGE, LEVEL3_CLAW_WIDTH,
                   LEVEL3_CLAW_WIDTH, LEVEL3_CLAW_DMG, MOD_LEVEL3_CLAW );
      break;
    case WP_ALEVEL2:
      meleeAttack( ent, LEVEL2_CLAW_RANGE, LEVEL2_CLAW_WIDTH_UPG, LEVEL2_CLAW_WIDTH_UPG,
                   LEVEL2_CLAW_DMG, MOD_LEVEL2_CLAW );
      break;
    case WP_ALEVEL2_UPG:
      meleeAttack( ent, LEVEL2_CLAW_U_RANGE_UPG, LEVEL2_CLAW_WIDTH, LEVEL2_CLAW_WIDTH,
                   LEVEL2_CLAW_UPG_DMG, MOD_LEVEL2_CLAW_UPG );
      break;
      
    case WP_ALEVEL5:
      meleeAttack( ent, LEVEL5_CLAW_U_RANGE, LEVEL5_CLAW_WIDTH, LEVEL5_CLAW_WIDTH,
                   LEVEL5_CLAW_DMG, MOD_LEVEL5_CLAW );			   
      break;
	  
    case WP_ALEVEL4:
      meleeAttack( ent, LEVEL4_CLAW_RANGE, LEVEL4_CLAW_WIDTH,
                   LEVEL4_CLAW_HEIGHT, LEVEL4_CLAW_DMG, MOD_LEVEL4_CLAW );
      break;

    case WP_BLASTER:
      blasterFire( ent );
      break;
    case WP_MACHINEGUN:
      bulletFire( ent, RIFLE_SPREAD, RIFLE_DMG, MOD_MACHINEGUN );
      break;
    case WP_SHOTGUN:
      shotgunFire( ent );
      break;
    case WP_CHAINGUN:
      bulletFire( ent, CHAINGUN_SPREAD, CHAINGUN_DMG, MOD_CHAINGUN );
      break;
    case WP_FLAMER:
      NapalmFire( ent, qfalse );
      break;
    case WP_PULSE_RIFLE:
      pulseRifleFire( ent );
      break;      
    case WP_MASS_DRIVER:
      massDriverFire( ent );
      break;
    case WP_LUCIFER_CANNON:
      LCChargeFire( ent, qfalse );
      break;
    case WP_LAS_GUN:
      lasGunFire( ent );
      break;
    case WP_PAIN_SAW:
      painSawFire( ent );
      break;
    case WP_ROCKET_LAUNCHER:
      rocketLauncherFire( ent );
      break;
    case WP_LIGHTNING_GUN:
      lightningGunFire( ent );
      break;
    case WP_GRENADE:
      throwGrenade( ent );
      break;
    case WP_MINE:
      throwMine( ent );
      break;
    case WP_LOCKBLOB_LAUNCHER:
      lockBlobLauncherFire( ent );
      break;
    case WP_HIVE:
      hiveFire( ent );
      break;
    case WP_TESLAGEN:
      teslaFire( ent );
      break;
    case WP_MGTURRET:
	  bulletFire( ent, MGTURRET_SPREAD, MGTURRET_DMG, MOD_MGTURRET ); 
      break;  
	  case WP_MGTURRET2:
	   FlameTurretFire( ent );	   
      break;	  	  
    case WP_ABUILD:
      buildFire( ent, MN_A_BUILD );
      break;
    case WP_HBUILD:
      buildFire( ent, MN_H_BUILD );
      break;
	  
    default:
      break;
  }
}

