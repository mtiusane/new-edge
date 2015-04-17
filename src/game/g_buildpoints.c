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

#define malloc BG_Alloc
#define free BG_Free

typedef int veci_t[ 3 ];

typedef struct bpfield_point_s bpfield_point_t;
struct bpfield_point_s
{
  veci_t origin;
  float value;
  bpfield_point_t *next;
};

#define BPFIELD_HASH_BITS 8
#define BPFIELD_HASH_MAX ( ( 1 << BPFIELD_HASH_BITS ) - 1 )

int bpfield_hash( const veci_t coords )
{
  int r;
  r = ( coords[0] + 119 ) ^ ( coords[1] + 61 ) ^ ( coords[2] + 17 );
  r ^= ( r >> 8 ) ^ ( r >> 16 ) ^ ( r >> 24 );
  return r & BPFIELD_HASH_MAX;
}

typedef struct
{
  bpfield_point_t *points[ BPFIELD_HASH_MAX ];
  float initial_value;
  vec3_t resolution;
} bpfield_t;

bpfield_point_t *bpfield_find( bpfield_t *bpf, veci_t coords )
{
  int hash;
  bpfield_point_t *point;

  hash = bpfield_hash( coords );

  for( point = bpf->points[ hash ]; point; point = point->next )
    if( point->origin[ 0 ] == coords[ 0 ] &&
        point->origin[ 1 ] == coords[ 1 ] && 
        point->origin[ 2 ] == coords[ 2 ] )
      return point;

  return NULL;
}

bpfield_point_t *bpfield_insert( bpfield_t *bpf, const veci_t coords )
{
  int hash;
  bpfield_point_t *point;

  point = malloc( sizeof( bpfield_point_t ) );
  memset( point, 0, sizeof( bpfield_point_t ) );

  hash = bpfield_hash( coords );
  point->next = bpf->points[ hash ];
  bpf->points[ hash ] = point;

  VectorCopy( coords, point->origin );
  point->value = bpf->initial_value;

  return point;
}

void bpfield_init( bpfield_t *bpf, float res, float density )
{
  memset( bpf, 0, sizeof( bpfield_t ) );
  VectorSet( bpf->resolution, res, res, res );
  bpf->initial_value = density * res * res * res;
}

void bpfield_octet_coords( const bpfield_t *bpf, const vec3_t point, veci_t *out, float *weights )
{
  int i, j;
  vec3_t base[ 2 ];

  for( i = 0; i < 3; i++ )
    out[ 0 ][ i ] = floor( point[ i ] / bpf->resolution[ i ] );

  for( i = 0; i < 3; i++ )
  {
    base[ 0 ][ i ] = point[ i ] / bpf->resolution[ i ] - out[ 0 ][ i ];
    base[ 1 ][ i ] = 1.0f - base[ 0 ][ i ];
  }

  for( i = 1; i < 8; i++ )
    for( j = 0; j < 3; j++ )
      out[ i ][ j ] = out[ 0 ][ j ] + !!( i & ( 1 << j ) );

  if( weights )
    for( i = 0; i < 8; i++ )
    {
      weights[ i ] = 1.0f;
      for( j = 0; j < 3; j++ )
        weights[ i ] *= base[ !( i & ( 1 << j ) ) ][ j ];
    }
}

float bpfield_get( bpfield_t *bpf, const vec3_t point )
{
  int i;
  veci_t coords[ 8 ];
  float weights[ 8 ];
  float total;

  bpfield_octet_coords( bpf, point, coords, NULL );

  for( total = 0.0f, i = 0; i < 8; i++ )
  {
    bpfield_point_t *point;

    if( ( point = bpfield_find( bpf, coords[ i ] ) ) )
      total += point->value;
    else
      total += bpf->initial_value;
  }

  return total;
}

void bpfield_add( bpfield_t *bpf, const vec3_t point, float value )
{
  int i;
  veci_t coords[ 8 ];
  float weights[ 8 ];

  bpfield_octet_coords( bpf, point, coords, weights );

  for( i = 0; i < 8; i++ )
  {
    bpfield_point_t *point;

    point = bpfield_find( bpf, coords[ i ] );

    if( !point )
      point = bpfield_insert( bpf, coords[ i ] );

    point->value += value * weights[ i ];
  }
}

static struct
{
  int mode;

  int points[ 2 ];
  int max_points[ 2 ];

  bpfield_t fields[ 2 ];

  int last_decay;
} bps;

void G_InitBuildPoints( void )
{
  bps.mode = g_buildPointsMode.integer;

  if( bps.mode == 0 )
  {
    bps.max_points[ 0 ] = g_alienBuildPoints.integer;
    bps.max_points[ 1 ] = g_humanBuildPoints.integer;

    bps.points[ 0 ] = bps.max_points[ 0 ];
    bps.points[ 1 ] = bps.max_points[ 1 ];

    Com_Printf( "G_InitBuildPoints: mode %i, points: %i %i\n",
      bps.mode, bps.points[ 0 ], bps.points[ 1 ] );
  }
  else if( bps.mode == 1 )
  {
    float res;

    res = g_bpfieldResolution.value;

    if( res < 20.0f )
    {
      Com_Printf( "^3WARNING: G_InitBuildPoints: g_bpfieldResolution too low, resetting to a sane value\n" );
      res = 20.0f;
    }

    bpfield_init( bps.fields, res, g_bpfieldAlienDensity.value );
    bpfield_init( bps.fields + 1, res, g_bpfieldHumanDensity.value );

    Com_Printf( "G_InitBuildPoints: mode %d, res: %f, densities: %f %f\n",
      bps.mode, res, g_bpfieldAlienDensity.value, g_bpfieldHumanDensity.value );
  }
  else
    Com_Error( ERR_DROP, "G_InitBuildPoints: invalid g_buildPointsMode" );
}

void G_AddBuildPoints( const vec3_t point, team_t team, int value )
{
  if( !value )
    return;

  switch( bps.mode )
  {
    case 0:
      bps.points[ team == TEAM_HUMANS ] += value;
      break;

    case 1:
      bpfield_add( bps.fields + ( team == TEAM_HUMANS ), point, value );
      break;
  }
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
  switch( bps.mode )
  {
    case 0:
      return bps.points[ team == TEAM_HUMANS ];

    case 1:
      return bpfield_get( bps.fields + ( team == TEAM_HUMANS ), point );
  }

  return 0;
}

void G_RunBuildPoints( void )
{
  if( bps.mode == 0 )
  {
    trap_SetConfigstring( CS_BUILD_POOLS, va( "%d %d %d %d %d %d", 
      bps.points[ 0 ], bps.max_points[ 0 ], level.alienBPFlashTime,
      bps.points[ 1 ], bps.max_points[ 1 ], level.alienBPFlashTime ) );

    if( g_buildPointsDecay.integer &&
      ( level.time - level.startTime ) / 60000 >= g_buildPointsDecayStart.integer &&
      level.time - bps.last_decay >= g_buildPointsDecay.integer )
    {
      bps.points[ 0 ]--;
      bps.points[ 1 ]--;
      bps.last_decay = level.time;
    }
  }
  else
  {
    // TODO
    trap_SetConfigstring( CS_BUILD_POOLS, "666 666 -1 666 666 -1" );
  }
}
