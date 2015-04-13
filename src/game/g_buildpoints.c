/*
===========================================================================
Copyright (C) 2015 Paweł Redman <pawel.redman@gmail.com>
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

#include "g_local.h"

static int build_points[ 2 ];
static int build_points_initial[ 2 ];
static int last_decay;

void G_InitBuildPoints( void )
{
  build_points[ 0 ] = build_points_initial[ 0 ] = g_alienBuildPoints.integer;
  build_points[ 1 ] = build_points_initial[ 1 ] = g_humanBuildPoints.integer;
}

void G_AddBuildPoints( const vec3_t point, team_t team, int value )
{
  if( !value )
    return;

  build_points[ team == TEAM_HUMANS ] += value;
}

void G_RecoverBuildPoints( gentity_t *ent )
{
  int value;
  float frac;
  const buildableAttributes_t *battr = BG_Buildable( ent->s.modelindex );

  frac = MAX( (float)ent->health / battr->health, 0 );

  value = ( 1.0f - ( 1.0f - frac ) *
      ( 1.0f - g_buildPointsRecovery.value ) ) *
      battr->buildPoints;

  G_AddBuildPoints( ent->s.origin, ent->buildableTeam, value );
}

int G_GetBuildPoints( const vec3_t point, team_t team )
{
  return build_points[ team == TEAM_HUMANS ];
}

void G_RunBuildPoints( void )
{
  trap_SetConfigstring( CS_BUILD_POOLS, va( "%d %d %d %d %d %d", 
    build_points[ 0 ], build_points_initial[ 0 ], level.alienBPFlashTime,
    build_points[ 1 ], build_points_initial[ 1 ], level.alienBPFlashTime ) );

  if( g_buildPointsDecay.integer &&
    ( level.time - level.startTime ) / 60000 >= g_buildPointsDecayStart.integer &&
    level.time - last_decay >= g_buildPointsDecay.integer )
  {
    build_points[ 0 ]--;
    build_points[ 1 ]--;
    last_decay = level.time;
  }
}
