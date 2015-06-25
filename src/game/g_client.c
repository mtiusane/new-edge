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
#include "g_local.h"

// g_client.c -- client functions that don't happen every frame

static vec3_t playerMins = {-15, -15, -24};
static vec3_t playerMaxs = {15, 15, 32};

/*QUAKED info_player_deathmatch (1 0 1) (-16 -16 -24) (16 16 32) initial
potential spawning position for deathmatch games.
The first time a player enters the game, they will be at an 'initial' spot.
Targets will be fired when someone spawns in on them.
"nobots" will prevent bots from using this spot.
"nohumans" will prevent non-bots from using this spot.
*/
void SP_info_player_deathmatch( gentity_t *ent )
{
  int   i;

  G_SpawnInt( "nobots", "0", &i);

  if( i )
    ent->flags |= FL_NO_BOTS;

  G_SpawnInt( "nohumans", "0", &i );
  if( i )
    ent->flags |= FL_NO_HUMANS;
}

/*QUAKED info_player_start (1 0 0) (-16 -16 -24) (16 16 32)
equivelant to info_player_deathmatch
*/
void SP_info_player_start( gentity_t *ent )
{
  ent->classname = "info_player_deathmatch";
  SP_info_player_deathmatch( ent );
}

/*QUAKED info_player_intermission (1 0 1) (-16 -16 -24) (16 16 32)
The intermission will be viewed from this point.  Target an info_notnull for the view direction.
*/
void SP_info_player_intermission( gentity_t *ent )
{
}

/*QUAKED info_alien_intermission (1 0 1) (-16 -16 -24) (16 16 32)
The intermission will be viewed from this point.  Target an info_notnull for the view direction.
*/
void SP_info_alien_intermission( gentity_t *ent )
{
}

/*QUAKED info_human_intermission (1 0 1) (-16 -16 -24) (16 16 32)
The intermission will be viewed from this point.  Target an info_notnull for the view direction.
*/
void SP_info_human_intermission( gentity_t *ent )
{
}

/*
===============
G_AddCreditToClient
===============
*/
void G_AddCreditToClient( gclient_t *client, short credit, qboolean cap )
{
  int capAmount;

  if( !client )
    return;

  if( cap && credit > 0 )
  {
    capAmount = client->pers.teamSelection == TEAM_ALIENS ?
                ALIEN_MAX_CREDITS : HUMAN_MAX_CREDITS;
    if( client->pers.credit < capAmount )
    {
      client->pers.credit += credit;
      if( client->pers.credit > capAmount )
        client->pers.credit = capAmount;
    }
  }
  else
    client->pers.credit += credit;

  if( client->pers.credit < 0 )
    client->pers.credit = 0;

  // Copy to ps so the client can access it
  client->ps.persistant[ PERS_CREDIT ] = client->pers.credit;
}



/*
=======================================================================

  G_SelectSpawnPoint

=======================================================================
*/

/*
================
SpotWouldTelefrag

================
*/
qboolean SpotWouldTelefrag( gentity_t *spot )
{
  int       i, num;
  int       touch[ MAX_GENTITIES ];
  gentity_t *hit;
  vec3_t    mins, maxs;

  VectorAdd( spot->s.origin, playerMins, mins );
  VectorAdd( spot->s.origin, playerMaxs, maxs );
  num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

  for( i = 0; i < num; i++ )
  {
    hit = &g_entities[ touch[ i ] ];
    //if ( hit->client && hit->client->ps.stats[STAT_HEALTH] > 0 ) {
    if( hit->client )
      return qtrue;
  }

  return qfalse;
}


/*
===========
G_SelectRandomFurthestSpawnPoint

Chooses a player start, deathmatch start, etc
============
*/
static gentity_t *G_SelectRandomFurthestSpawnPoint ( vec3_t avoidPoint, vec3_t origin, vec3_t angles )
{
  gentity_t *spot;
  vec3_t    delta;
  float     dist;
  float     list_dist[ 64 ];
  gentity_t *list_spot[ 64 ];
  int       numSpots, rnd, i, j;

  numSpots = 0;
  spot = NULL;

  while( ( spot = G_Find( spot, FOFS( classname ), "info_player_deathmatch" ) ) != NULL )
  {
    if( SpotWouldTelefrag( spot ) )
      continue;

    VectorSubtract( spot->s.origin, avoidPoint, delta );
    dist = VectorLength( delta );

    for( i = 0; i < numSpots; i++ )
    {
      if( dist > list_dist[ i ] )
      {
        if( numSpots >= 64 )
          numSpots = 64 - 1;

        for( j = numSpots; j > i; j-- )
        {
          list_dist[ j ] = list_dist[ j - 1 ];
          list_spot[ j ] = list_spot[ j - 1 ];
        }

        list_dist[ i ] = dist;
        list_spot[ i ] = spot;
        numSpots++;

        if( numSpots > 64 )
          numSpots = 64;

        break;
      }
    }

    if( i >= numSpots && numSpots < 64 )
    {
      list_dist[ numSpots ] = dist;
      list_spot[ numSpots ] = spot;
      numSpots++;
    }
  }

  if( !numSpots )
  {
    spot = G_Find( NULL, FOFS( classname ), "info_player_deathmatch" );

    if( !spot )
      G_Error( "Couldn't find a spawn point" );

    VectorCopy( spot->s.origin, origin );
    origin[ 2 ] += 9;
    VectorCopy( spot->s.angles, angles );
    return spot;
  }

  // select a random spot from the spawn points furthest away
  rnd = random( ) * ( numSpots / 2 );

  VectorCopy( list_spot[ rnd ]->s.origin, origin );
  origin[ 2 ] += 9;
  VectorCopy( list_spot[ rnd ]->s.angles, angles );

  return list_spot[ rnd ];
}


/*
================
G_SelectSpawnBuildable

find the nearest buildable of the right type that is
spawned/healthy/unblocked etc.
================
*/
static gentity_t *G_SelectSpawnBuildable( vec3_t preference, buildable_t buildable )
{
  gentity_t *search, *spot;

  search = spot = NULL;

  while( ( search = G_Find( search, FOFS( classname ),
    BG_Buildable( buildable )->entityName ) ) != NULL )
  {
    if( !search->spawned )
      continue;

    if( search->health <= 0 )
      continue;

    if( !search->s.groundEntityNum )
      continue;

    if( search->clientSpawnTime > 0 )
      continue;

    if( G_CheckSpawnPoint( search->s.number, search->s.origin,
          search->s.origin2, buildable, NULL ) != NULL )
      continue;

    if( !spot || DistanceSquared( preference, search->s.origin ) <
                 DistanceSquared( preference, spot->s.origin ) )
      spot = search;
  }

  return spot;
}

/*
===========
G_SelectSpawnPoint

Chooses a player start, deathmatch start, etc
============
*/
gentity_t *G_SelectSpawnPoint( vec3_t avoidPoint, vec3_t origin, vec3_t angles )
{
  return G_SelectRandomFurthestSpawnPoint( avoidPoint, origin, angles );
}


/*
===========
G_SelectTremulousSpawnPoint

Chooses a player start, deathmatch start, etc
============
*/
gentity_t *G_SelectTremulousSpawnPoint( team_t team, vec3_t preference, vec3_t origin, vec3_t angles )
{
  gentity_t *spot = NULL;

  if( team == TEAM_ALIENS )
  {
    if( level.numAlienSpawns <= 0 )
      return NULL;

    spot = G_SelectSpawnBuildable( preference, BA_A_SPAWN );
  }
  else if( team == TEAM_HUMANS )
  {
    if( level.numHumanSpawns <= 0 )
      return NULL;

    spot = G_SelectSpawnBuildable( preference, BA_H_SPAWN );
  }

  //no available spots
  if( !spot )
    return NULL;

  if( team == TEAM_ALIENS )
    G_CheckSpawnPoint( spot->s.number, spot->s.origin, spot->s.origin2, BA_A_SPAWN, origin );
  else if( team == TEAM_HUMANS )
    G_CheckSpawnPoint( spot->s.number, spot->s.origin, spot->s.origin2, BA_H_SPAWN, origin );

  VectorCopy( spot->s.angles, angles );
  angles[ ROLL ] = 0;

  return spot;

}


/*
===========
G_SelectSpectatorSpawnPoint

============
*/
static gentity_t *G_SelectSpectatorSpawnPoint( vec3_t origin, vec3_t angles )
{
  FindIntermissionPoint( );

  VectorCopy( level.intermission_origin, origin );
  VectorCopy( level.intermission_angle, angles );

  return NULL;
}


/*
===========
G_SelectAlienLockSpawnPoint

Try to find a spawn point for alien intermission otherwise
use normal intermission spawn.
============
*/
gentity_t *G_SelectAlienLockSpawnPoint( vec3_t origin, vec3_t angles )
{
  gentity_t *spot;

  spot = NULL;
  spot = G_Find( spot, FOFS( classname ), "info_alien_intermission" );

  if( !spot )
    return G_SelectSpectatorSpawnPoint( origin, angles );

  VectorCopy( spot->s.origin, origin );
  VectorCopy( spot->s.angles, angles );

  return spot;
}


/*
===========
G_SelectHumanLockSpawnPoint

Try to find a spawn point for human intermission otherwise
use normal intermission spawn.
============
*/
gentity_t *G_SelectHumanLockSpawnPoint( vec3_t origin, vec3_t angles )
{
  gentity_t *spot;

  spot = NULL;
  spot = G_Find( spot, FOFS( classname ), "info_human_intermission" );

  if( !spot )
    return G_SelectSpectatorSpawnPoint( origin, angles );

  VectorCopy( spot->s.origin, origin );
  VectorCopy( spot->s.angles, angles );

  return spot;
}


/*
=======================================================================

BODYQUE

=======================================================================
*/


/*
=============
BodySink

After sitting around for five seconds, fall into the ground and dissapear
=============
*/
static void BodySink( gentity_t *ent )
{
  //run on first BodySink call
  if( !ent->active )
  {
    ent->active = qtrue;

    //sinking bodies can't be infested
    ent->killedBy = ent->s.misc = MAX_CLIENTS;
    ent->timestamp = level.time;

  }

  if( level.time - ent->timestamp > 6500 )
  {
    G_FreeEntity( ent );
    return;
  }

  ent->nextthink = level.time + 100;
  ent->s.pos.trBase[ 2 ] -= 1;
}


/*
=============
SpawnCorpse

A player is respawning, so make an entity that looks
just like the existing corpse to leave behind.
=============
*/
static void SpawnCorpse( gentity_t *ent )
{
  gentity_t   *body;
  int         contents;
  vec3_t      origin, dest;
  trace_t     tr;
  float       vDiff;

  VectorCopy( ent->r.currentOrigin, origin );

  trap_UnlinkEntity( ent );

  // if client is in a nodrop area, don't leave the body
  contents = trap_PointContents( origin, -1 );
  if( contents & CONTENTS_NODROP )
    return;

  body = G_Spawn( );

  VectorCopy( ent->s.apos.trBase, body->s.angles );
  body->s.eFlags = EF_DEAD;
  body->s.eType = ET_CORPSE;
  body->s.number = body - g_entities;
  body->timestamp = level.time;
  body->s.event = 0;
  body->r.contents = CONTENTS_CORPSE;
  body->s.clientNum = ent->client->ps.stats[ STAT_CLASS ];
  body->nonSegModel = ent->client->ps.persistant[ PERS_STATE ] & PS_NONSEGMODEL;

  if( ent->client->ps.stats[ STAT_TEAM ] == TEAM_HUMANS )
    body->classname = "humanCorpse";
  else
    body->classname = "alienCorpse";

  body->s.misc = MAX_CLIENTS;

  body->think = BodySink;
  body->nextthink = level.time + 20000;

  body->s.legsAnim = ent->s.legsAnim;

  if( !body->nonSegModel )
  {
    switch( body->s.legsAnim & ~ANIM_TOGGLEBIT )
    {
      case BOTH_DEATH1:
      case BOTH_DEAD1:
        body->s.torsoAnim = body->s.legsAnim = BOTH_DEAD1;
        break;
      case BOTH_DEATH2:
      case BOTH_DEAD2:
        body->s.torsoAnim = body->s.legsAnim = BOTH_DEAD2;
        break;
      case BOTH_DEATH3:
      case BOTH_DEAD3:
      default:
        body->s.torsoAnim = body->s.legsAnim = BOTH_DEAD3;
        break;
    }
  }
  else
  {
    switch( body->s.legsAnim & ~ANIM_TOGGLEBIT )
    {
      case NSPA_DEATH1:
      case NSPA_DEAD1:
        body->s.legsAnim = NSPA_DEAD1;
        break;
      case NSPA_DEATH2:
      case NSPA_DEAD2:
        body->s.legsAnim = NSPA_DEAD2;
        break;
      case NSPA_DEATH3:
      case NSPA_DEAD3:
      default:
        body->s.legsAnim = NSPA_DEAD3;
        break;
    }
  }

  body->takedamage = qfalse;

  body->health = ent->health = ent->client->ps.stats[ STAT_HEALTH ];
  ent->health = 0;

  //change body dimensions
  BG_ClassBoundingBox( ent->client->ps.stats[ STAT_CLASS ], NULL, NULL, NULL, body->r.mins, body->r.maxs );
  vDiff = body->r.mins[ 2 ] - ent->r.mins[ 2 ];

  //drop down to match the *model* origins of ent and body
  VectorSet( dest, origin[ 0 ], origin[ 1 ], origin[ 2 ] - vDiff );
  trap_Trace( &tr, origin, body->r.mins, body->r.maxs, dest, body->s.number, body->clipmask );
  VectorCopy( tr.endpos, origin );

  G_SetOrigin( body, origin );
  VectorCopy( origin, body->s.origin );
  body->s.pos.trType = TR_GRAVITY;
  body->s.pos.trTime = level.time;
  VectorCopy( ent->client->ps.velocity, body->s.pos.trDelta );

  VectorCopy ( body->s.pos.trBase, body->r.currentOrigin );
  trap_LinkEntity( body );
}

//======================================================================


/*
==================
G_SetClientViewAngle

==================
*/
void G_SetClientViewAngle( gentity_t *ent, vec3_t angle )
{
  int     i;

  // set the delta angle
  for( i = 0; i < 3; i++ )
  {
    int   cmdAngle;

    cmdAngle = ANGLE2SHORT( angle[ i ] );
    ent->client->ps.delta_angles[ i ] = cmdAngle - ent->client->pers.cmd.angles[ i ];
  }

  VectorCopy( angle, ent->s.angles );
  VectorCopy( ent->s.angles, ent->client->ps.viewangles );
}

/*
================
respawn
================
*/
void respawn( gentity_t *ent )
{
  int i;

  SpawnCorpse( ent );

  // Clients can't respawn - they must go through the class cmd
  ent->client->pers.classSelection = PCL_NONE;
  ClientSpawn( ent, NULL, NULL, NULL );

  // stop any following clients that don't have sticky spec on
  for( i = 0; i < level.maxclients; i++ )
  {
    if( level.clients[ i ].sess.spectatorState == SPECTATOR_FOLLOW &&
        level.clients[ i ].sess.spectatorClient == ent - g_entities )
    {
      if( !( level.clients[ i ].pers.stickySpec ) )
      {
        if( !G_FollowNewClient( &g_entities[ i ], 1 ) )
          G_StopFollowing( &g_entities[ i ] );
      }
      else
        G_FollowLockView( &g_entities[ i ] );
    }
  }
}

static qboolean G_IsEmoticon( const char *s, qboolean *escaped )
{
  int i, j;
  const char *p = s;
  char emoticon[ MAX_EMOTICON_NAME_LEN ] = {""};
  qboolean escape = qfalse;

  if( *p != '[' )
    return qfalse;
  p++;
  if( *p == '[' )
  {
    escape = qtrue;
    p++;
  }
  i = 0;
  while( *p && i < ( MAX_EMOTICON_NAME_LEN - 1 ) )
  {
    if( *p == ']' )
    {
      for( j = 0; j < level.emoticonCount; j++ )
      {
        if( !Q_stricmp( emoticon, level.emoticons[ j ].name ) )
        {
          *escaped = escape;
          return qtrue;
        }
      }
      return qfalse;
    }
    emoticon[ i++ ] = *p;
    emoticon[ i ] = '\0';
    p++;
  }
  return qfalse;
}

/*
===========
G_IsNewbieName
============
*/
qboolean G_IsNewbieName( const char *name )
{
  char testName[ MAX_NAME_LENGTH ];

  G_DecolorString( (char *)name, testName, sizeof( testName ) );

  if( !Q_stricmp( testName, "UnnamedPlayer" ) )
    return qtrue;

  if( g_newbieNameNumbering.integer &&
      g_newbieNamePrefix.string[ 0 ] &&
      !Q_stricmpn( testName, g_newbieNamePrefix.string, strlen( g_newbieNamePrefix.string ) ) )
    return qtrue;

  return qfalse;
}

/*
===========
G_ClientNewbieName
============
*/
static const char *G_ClientNewbieName( gclient_t *client )
{
  static int  nextNumber = 1;
  static char name[ MAX_NAME_LENGTH ];
  int         number;

  if( !g_newbieNameNumbering.integer ||
      !g_newbieNamePrefix.string[ 0 ] ||
      !client )
   return "UnnamedPlayer";

  if( client->pers.namelog->newbieNumber )
  {
    number = client->pers.namelog->newbieNumber;
  }
  else if( g_newbieNameNumbering.integer > 1 )
  {
    number = g_newbieNameNumbering.integer;
    trap_Cvar_Set( "g_newbieNameNumbering", va( "%d", number + 1 ) );
  }
  else
  {
    number = nextNumber++;
  }

  client->pers.namelog->newbieNumber = number;
  Com_sprintf( name, sizeof( name ), "%s%d",
               g_newbieNamePrefix.string, number );
  return name;
}

/*
===========
G_ClientCleanName
============
*/
static void G_ClientCleanName( const char *in, char *out, int outSize, gclient_t *client )
{
  int   len, colorlessLen;
  char  *p;
  int   spaces;
  qboolean escaped;
  qboolean invalid = qfalse;

  //save room for trailing null byte
  outSize--;

  len = 0;
  colorlessLen = 0;
  p = out;
  *p = 0;
  spaces = 0;

  for( ; *in; in++ )
  {
    // don't allow leading spaces
    if( colorlessLen == 0 && *in == ' ' )
      continue;

    // don't allow nonprinting characters or (dead) console keys
    if( *in < ' ' || *in > '}' || *in == '`' )
      continue;

    // check colors
    if( Q_IsColorString( in ) )
    {
      in++;

      // make sure room in dest for both chars
      if( len > outSize - 2 )
        break;

      *out++ = Q_COLOR_ESCAPE;

      *out++ = *in;

      len += 2;
      continue;
    }
    else if( !g_emoticonsAllowedInNames.integer && G_IsEmoticon( in, &escaped ) )
    {
      // make sure room in dest for both chars
      if( len > outSize - 2 )
        break;

      *out++ = '['; 
      *out++ = '['; 
      len += 2;
      if( escaped )
        in++;
      continue;
    }

    // don't allow too many consecutive spaces
    if( *in == ' ' )
    {
      spaces++;
      if( spaces > 3 )
        continue;
    }
    else
      spaces = 0;

    if( len > outSize - 1 )
      break;

    *out++ = *in;
    colorlessLen++;
    len++;
  }

  *out = 0;

  // don't allow names beginning with "[skipnotify]" because it messes up /ignore-related code
  if( !Q_stricmpn( p, "[skipnotify]", 12 ) )
    invalid = qtrue;

  // don't allow comment-beginning strings because it messes up various parsers
  if( strstr( p, "//" ) || strstr( p, "/*" ) )
    invalid = qtrue;

  // don't allow empty names
  if( *p == 0 || colorlessLen == 0 )
    invalid = qtrue;

  // if something made the name bad, put them back to UnnamedPlayer
  if( invalid )
    Q_strncpyz( p, G_ClientNewbieName( client ), outSize );
}


/*
======================
G_NonSegModel

Reads an animation.cfg to check for nonsegmentation
======================
*/
static qboolean G_NonSegModel( const char *filename )
{
  char          *text_p;
  int           len;
  char          *token;
  char          text[ 20000 ];
  fileHandle_t  f;

  // load the file
  len = trap_FS_FOpenFile( filename, &f, FS_READ );
  if( !f )
  {
    G_Printf( "File not found: %s\n", filename );
    return qfalse;
  }

  if( len < 0 )
    return qfalse;

  if( len == 0 || len >= sizeof( text ) - 1 )
  {
    trap_FS_FCloseFile( f );
    G_Printf( "File %s is %s\n", filename, len == 0 ? "empty" : "too long" );
    return qfalse;
  }

  trap_FS_Read( text, len, f );
  text[ len ] = 0;
  trap_FS_FCloseFile( f );

  // parse the text
  text_p = text;

  // read optional parameters
  while( 1 )
  {
    token = COM_Parse( &text_p );

    //EOF
    if( !token[ 0 ] )
      break;

    if( !Q_stricmp( token, "nonsegmented" ) )
      return qtrue;
  }

  return qfalse;
}

/*
===========
ClientUserInfoChanged

Called from ClientConnect when the player first connects and
directly by the server system when the player updates a userinfo variable.

The game can override any of the settings and call trap_SetUserinfo
if desired.
============
*/
char *ClientUserinfoChanged( int clientNum, qboolean forceName )
{
  gentity_t *ent;
  char      *s;
  char      model[ MAX_QPATH ];
  char      buffer[ MAX_QPATH ];
  char      filename[ MAX_QPATH ];
  char      oldname[ MAX_NAME_LENGTH ];
  char      newname[ MAX_NAME_LENGTH ];
  char      err[ MAX_STRING_CHARS ];
  qboolean  revertName = qfalse;
  gclient_t *client;
  char      userinfo[ MAX_INFO_STRING ];

  ent = g_entities + clientNum;
  client = ent->client;

  trap_GetUserinfo( clientNum, userinfo, sizeof( userinfo ) );

  // check for malformed or illegal info strings
  if( !Info_Validate(userinfo) )
  {
    trap_SendServerCommand( ent - g_entities,
        "disconnect \"illegal or malformed userinfo\n\"" );
    trap_DropClient( ent - g_entities, 
        "dropped: illegal or malformed userinfo");
    return "Illegal or malformed userinfo";
  }
  // If their userinfo overflowed, tremded is in the process of disconnecting them.
  // If we send our own disconnect, it won't work, so just return to prevent crashes later
  //  in this function. This check must come after the Info_Validate call.
  else if( !userinfo[ 0 ] )
    return "Empty (overflowed) userinfo";

  // stickyspec toggle
  s = Info_ValueForKey( userinfo, "cg_stickySpec" );  
  client->pers.stickySpec = atoi( s ) != 0;

  // set name
  Q_strncpyz( oldname, client->pers.netname, sizeof( oldname ) );
  s = Info_ValueForKey( userinfo, "name" );
  G_ClientCleanName( s, newname, sizeof( newname ), client );

  if( strcmp( oldname, newname ) )
  {
    if( !forceName && client->pers.namelog->nameChangeTime &&
      level.time - client->pers.namelog->nameChangeTime <=
      g_minNameChangePeriod.value * 1000 )
    {
      trap_SendServerCommand( ent - g_entities, va(
        "print \"^5Name change spam protection (g_minNameChangePeriod = %d)\n\"",
         g_minNameChangePeriod.integer ) );
      revertName = qtrue;
    }
    else if( !forceName && g_maxNameChanges.integer > 0 &&
      client->pers.namelog->nameChanges >= g_maxNameChanges.integer  )
    {
      trap_SendServerCommand( ent - g_entities, va(
        "print \"^5Maximum name changes reached (g_maxNameChanges = %d)\n\"",
         g_maxNameChanges.integer ) );
      revertName = qtrue;
    }
    else if( !forceName && client->pers.namelog->muted )
    {
      trap_SendServerCommand( ent - g_entities,
        "print \"^5You cannot change your name while you are muted\n\"" );
      revertName = qtrue;
    }
    else if( !G_admin_name_check( ent, newname, err, sizeof( err ) ) )
    {
      trap_SendServerCommand( ent - g_entities, va( "print \"%s\n\"", err ) );
      revertName = qtrue;
    }

    if( revertName )
    {
      Q_strncpyz( client->pers.netname, *oldname ? oldname : G_ClientNewbieName( client ),
        sizeof( client->pers.netname ) );
      Info_SetValueForKey( userinfo, "name", oldname );
      trap_SetUserinfo( clientNum, userinfo );
    }
    else
    {
      if( G_IsNewbieName( newname ) )
        Q_strncpyz( newname, G_ClientNewbieName( client ), sizeof( newname ) );

      G_CensorString( client->pers.netname, newname,
        sizeof( client->pers.netname ), ent );
      if( !forceName && client->pers.connected == CON_CONNECTED )
      {
        client->pers.namelog->nameChangeTime = level.time;
        client->pers.namelog->nameChanges++;
      }
      if( *oldname )
      {
        G_LogPrintf( "ClientRename: %i [%s] (%s) \"%s^7\" -> \"%s^7\" \"%c%s%c^7\"\n",
                   clientNum, client->pers.ip.str, client->pers.guid,
                   oldname, client->pers.netname,
                   DECOLOR_OFF, client->pers.netname, DECOLOR_ON );
      }
    }
    G_namelog_update_name( client );
  }

  if( client->pers.classSelection == PCL_NONE )
  {
    //This looks hacky and frankly it is. The clientInfo string needs to hold different
    //model details to that of the spawning class or the info change will not be
    //registered and an axis appears instead of the player model. There is zero chance
    //the player can spawn with the battlesuit, hence this choice.
    Com_sprintf( buffer, MAX_QPATH, "%s/%s",  BG_ClassConfig( PCL_HUMAN_BSUIT )->modelName,
                                              BG_ClassConfig( PCL_HUMAN_BSUIT )->skinName );
  }
  else
  {
    Com_sprintf( buffer, MAX_QPATH, "%s/%s",  BG_ClassConfig( client->pers.classSelection )->modelName,
                                              BG_ClassConfig( client->pers.classSelection )->skinName );

    //model segmentation
    Com_sprintf( filename, sizeof( filename ), "models/players/%s/animation.cfg",
                 BG_ClassConfig( client->pers.classSelection )->modelName );

    if( G_NonSegModel( filename ) )
      client->ps.persistant[ PERS_STATE ] |= PS_NONSEGMODEL;
    else
      client->ps.persistant[ PERS_STATE ] &= ~PS_NONSEGMODEL;
  }
  Q_strncpyz( model, buffer, sizeof( model ) );

  // wallwalk follow
  s = Info_ValueForKey( userinfo, "cg_wwFollow" );

  if( atoi( s ) )
    client->ps.persistant[ PERS_STATE ] |= PS_WALLCLIMBINGFOLLOW;
  else
    client->ps.persistant[ PERS_STATE ] &= ~PS_WALLCLIMBINGFOLLOW;

  // wallwalk toggle
  s = Info_ValueForKey( userinfo, "cg_wwToggle" );

  if( atoi( s ) )
    client->ps.persistant[ PERS_STATE ] |= PS_WALLCLIMBINGTOGGLE;
  else
    client->ps.persistant[ PERS_STATE ] &= ~PS_WALLCLIMBINGTOGGLE;

  // always sprint
  s = Info_ValueForKey( userinfo, "cg_sprintToggle" );

  if( atoi( s ) )
    client->ps.persistant[ PERS_STATE ] |= PS_SPRINTTOGGLE;
  else
    client->ps.persistant[ PERS_STATE ] &= ~PS_SPRINTTOGGLE;

  // fly speed
  s = Info_ValueForKey( userinfo, "cg_flySpeed" );

  if( *s )
    client->pers.flySpeed = atoi( s );
  else
    client->pers.flySpeed = BG_Class( PCL_NONE )->speed;

  // disable blueprint errors
  s = Info_ValueForKey( userinfo, "cg_disableBlueprintErrors" );

  if( atoi( s ) )
    client->pers.disableBlueprintErrors = qtrue;
  else
    client->pers.disableBlueprintErrors = qfalse;

  // teamInfo
  s = Info_ValueForKey( userinfo, "teamoverlay" );

  if( atoi( s ) != 0 )
    client->pers.teamInfo = qtrue;
  else
    client->pers.teamInfo = qfalse;

  s = Info_ValueForKey( userinfo, "cg_unlagged" );
  if( !s[0] || atoi( s ) != 0 )
    client->pers.useUnlagged = qtrue;
  else
    client->pers.useUnlagged = qfalse;

  Q_strncpyz( client->pers.voice, Info_ValueForKey( userinfo, "voice" ),
    sizeof( client->pers.voice ) );

  // send over a subset of the userinfo keys so other clients can
  // print scoreboards, display models, and play custom sounds

  Com_sprintf( userinfo, sizeof( userinfo ),
    "n\\%s\\t\\%i\\model\\%s\\v\\%s",
    client->pers.netname, client->pers.teamSelection, model,
    client->pers.voice );

  trap_SetConfigstring( CS_PLAYERS + clientNum, userinfo );

  /*G_LogPrintf( "ClientUserinfoChanged: %i %s\n", clientNum, userinfo );*/

  return NULL;
}


/*
===========
ClientConnect

Called when a player begins connecting to the server.
Called again for every map change or tournement restart.

The session information will be valid after exit.

Return NULL if the client should be allowed, otherwise return
a string with the reason for denial.

Otherwise, the client will be sent the current gamestate
and will eventually get to ClientBegin.

firstTime will be qtrue the very first time a client connects
to the server machine, but qfalse on map changes and tournement
restarts.
============
*/
char *ClientConnect( int clientNum, qboolean firstTime )
{
  char      *value;
  char      *userInfoError;
  gclient_t *client;
  char      userinfo[ MAX_INFO_STRING ];
  gentity_t *ent;
  char      reason[ MAX_STRING_CHARS ] = {""};
  int       i;

  ent = &g_entities[ clientNum ];
  client = &level.clients[ clientNum ];

  // ignore if client already connected
  if( client->pers.connected != CON_DISCONNECTED )
    return NULL;

  ent->client = client;
  memset( client, 0, sizeof( *client ) );

  trap_GetUserinfo( clientNum, userinfo, sizeof( userinfo ) );

  value = Info_ValueForKey( userinfo, "cl_guid" );
  Q_strncpyz( client->pers.guid, value, sizeof( client->pers.guid ) );

  value = Info_ValueForKey( userinfo, "ip" );
  // check for local client
  if( !strcmp( value, "localhost" ) )
    client->pers.localClient = qtrue;
  G_AddressParse( value, &client->pers.ip );

  client->pers.admin = G_admin_admin( client->pers.guid );

  // check for admin ban
  if( G_admin_ban_check( ent, reason, sizeof( reason ) ) )
  {
    return va( "%s", reason );
  }

  // check for a password
  value = Info_ValueForKey( userinfo, "password" );

  if( g_password.string[ 0 ] && Q_stricmp( g_password.string, "none" ) &&
      strcmp( g_password.string, value ) != 0 )
    return "Invalid password";

  // check if connections per IP is limited
  if( g_maxIPConnections.integer > 1 )
  {
    gclient_t *other;
    int       count = 1;

    for( i = 0 ; i < level.maxclients; i++ )
    {
      other = &level.clients[ i ];
      if( other &&
          ( other->pers.connected == CON_CONNECTED || other->pers.connected == CON_CONNECTING ) &&
          strcmp( client->pers.ip.str, other->pers.ip.str ) == 0 )
        count++;
    }

    if( count > g_maxIPConnections.integer )
    {
      G_Log_NoSpam( client->pers.ip.str,
        va( "Deny: maximum %d ip connections for \"%s^6\" from %s\n",
            g_maxIPConnections.integer,
            Info_ValueForKey( userinfo, "name" ),
            client->pers.ip.str ) );
      return "Maximum simultaneous clients per IP exceeded";
    }
  }

  // add guid to session so we don't have to keep parsing userinfo everywhere
  for( i = 0; i < sizeof( client->pers.guid ) - 1 &&
              isxdigit( client->pers.guid[ i ] ); i++ );

  if( i < sizeof( client->pers.guid ) - 1 )
    return "Invalid GUID";

  for( i = 0; i < level.maxclients; i++ )
  {
    if( level.clients[ i ].pers.connected == CON_DISCONNECTED )
      continue;

    if( !Q_stricmp( client->pers.guid, level.clients[ i ].pers.guid ) )
    {
      if( !G_ClientIsLagging( level.clients + i ) )
      {
        trap_SendServerCommand( i, "cp \"^1Your GUID is not secure\"" );
        return "Duplicate GUID";
      }
      trap_DropClient( i, "Ghost" );
    }
  }

  client->pers.connected = CON_CONNECTING;

  // read or initialize the session data
  if( firstTime || level.newSession )
    G_InitSessionData( client, userinfo );

  G_ReadSessionData( client );

  // get and distribute relevent paramters
  G_namelog_connect( client );
  userInfoError = ClientUserinfoChanged( clientNum, qfalse );
  if( userInfoError != NULL )
    return userInfoError;

  G_LogPrintf( "ClientConnect: %i [%s] (%s) \"%s^7\" \"%c%s%c^7\"\n",
               clientNum, client->pers.ip.str, client->pers.guid,
               client->pers.netname,
               DECOLOR_OFF, client->pers.netname, DECOLOR_ON );

  // don't do the "xxx connected" messages if they were caried over from previous level
  if( firstTime )
    trap_SendServerCommand( -1, va( "print \"%s" S_COLOR_CYAN " ^5connected\n\"", 
                                    client->pers.netname ) );

  if( client->pers.admin )
    G_admin_authlog( ent );

  // count current clients and rank for scoreboard
  CalculateRanks( );
  

  // if this is after !restart keepteams or !restart switchteams, apply said selection
  if ( client->sess.restartTeam != TEAM_NONE )
  {
    G_ChangeTeam( ent, client->sess.restartTeam );
    client->sess.restartTeam = TEAM_NONE;
  }
  
  return NULL;
}

/*
===========
ClientBegin

called when a client has finished connecting, and is ready
to be placed into the level.  This will happen every level load,
and on transition between teams, but doesn't happen on respawns
============
*/
void ClientBegin( int clientNum )
{
  gentity_t *ent;
  gclient_t *client;
  int       flags;

  ent = g_entities + clientNum;

  client = level.clients + clientNum;

  // ignore if client already entered the game
  if( client->pers.connected != CON_CONNECTING )
    return;

  if( ent->r.linked )
    trap_UnlinkEntity( ent );

  G_InitGentity( ent );
  ent->touch = 0;
  ent->pain = 0;
  ent->client = client;

  client->pers.connected = CON_CONNECTED;
  client->pers.enterTime = level.time;

  // save eflags around this, because changing teams will
  // cause this to happen with a valid entity, and we
  // want to make sure the teleport bit is set right
  // so the viewpoint doesn't interpolate through the
  // world to the new position
  flags = client->ps.eFlags;
  memset( &client->ps, 0, sizeof( client->ps ) );
  memset( &client->pmext, 0, sizeof( client->pmext ) );
  client->ps.eFlags = flags;

  // locate ent at a spawn point
  ClientSpawn( ent, NULL, NULL, NULL );

  trap_SendServerCommand( -1, va( "print \"%s" S_COLOR_CYAN " ^5entered the game\n\"", client->pers.netname ) );

  G_namelog_restore( client );

  G_LogPrintf( "ClientBegin: %i\n", clientNum );

  // count current clients and rank for scoreboard
  CalculateRanks( );

  // send the client a list of commands that can be used
  G_ListCommands( ent );
  
  // display the help menu, if connecting the first time
  if( !client->sess.seenWelcome )
  {
    client->sess.seenWelcome = 1;
    G_TriggerMenu( client->ps.clientNum, MN_WELCOME );
  } 

  if ( g_ForceRandomTeams.integer ) {
    if( level.numAlienClients > level.numHumanClients ) {
      G_ChangeTeam( ent, TEAM_HUMANS );
    } else if( level.numHumanClients > level.numAlienClients ) {
      G_ChangeTeam( ent, TEAM_ALIENS );
    } else G_ChangeTeam( ent, random() >= 0.5f ? TEAM_ALIENS : TEAM_HUMANS );
  }
}

/*
===========
ClientSpawn

Called every time a client is placed fresh in the world:
after the first ClientBegin, and after each respawn
Initializes all non-persistant parts of playerState
============
*/
void ClientSpawn( gentity_t *ent, gentity_t *spawn, vec3_t origin, vec3_t angles )
{
  int                 index;
  vec3_t              spawn_origin, spawn_angles;
  gclient_t           *client;
  int                 i;
  clientPersistant_t  saved;
  clientSession_t     savedSess;
  int                 persistant[ MAX_PERSISTANT ];
  gentity_t           *spawnPoint = NULL;
  int                 flags;
  int                 savedPing;
  int                 teamLocal;
  int                 eventSequence;
  char                userinfo[ MAX_INFO_STRING ];
  vec3_t              up = { 0.0f, 0.0f, 1.0f };
  int                 maxAmmo, maxClips;
  weapon_t            weapon;

  index = ent - g_entities;
  client = ent->client;

  teamLocal = client->pers.teamSelection;

  //if client is dead and following teammate, stop following before spawning
  if( client->sess.spectatorClient != -1 )
  {
    client->sess.spectatorClient = -1;
    client->sess.spectatorState = SPECTATOR_FREE;
  }

  // only start client if chosen a class and joined a team
  if( client->pers.classSelection == PCL_NONE && teamLocal == TEAM_NONE )
    client->sess.spectatorState = SPECTATOR_FREE;
  else if( client->pers.classSelection == PCL_NONE )
    client->sess.spectatorState = SPECTATOR_LOCKED;

  // if client is dead and following teammate, stop following before spawning
  if( ent->client->sess.spectatorState == SPECTATOR_FOLLOW )
    G_StopFollowing( ent );

  if( origin != NULL )
    VectorCopy( origin, spawn_origin );

  if( angles != NULL )
    VectorCopy( angles, spawn_angles );

  // find a spawn point
  // do it before setting health back up, so farthest
  // ranging doesn't count this client
  if( client->sess.spectatorState != SPECTATOR_NOT )
  {
    if( teamLocal == TEAM_NONE )
      spawnPoint = G_SelectSpectatorSpawnPoint( spawn_origin, spawn_angles );
    else if( teamLocal == TEAM_ALIENS )
      spawnPoint = G_SelectAlienLockSpawnPoint( spawn_origin, spawn_angles );
    else if( teamLocal == TEAM_HUMANS )
      spawnPoint = G_SelectHumanLockSpawnPoint( spawn_origin, spawn_angles );
  }
  else
  {
    if( spawn == NULL )
    {
      G_Error( "ClientSpawn: spawn is NULL\n" );
      return;
    }

    spawnPoint = spawn;

    if( ent != spawn )
    {
      //start spawn animation on spawnPoint
      G_SetBuildableAnim( spawnPoint, BANIM_SPAWN1, qtrue );

      if( spawnPoint->buildableTeam == TEAM_ALIENS )
        spawnPoint->clientSpawnTime = ALIEN_SPAWN_REPEAT_TIME;
      else if( spawnPoint->buildableTeam == TEAM_HUMANS )
        spawnPoint->clientSpawnTime = HUMAN_SPAWN_REPEAT_TIME;
    }
  }

  // toggle the teleport bit so the client knows to not lerp
  flags = ( ent->client->ps.eFlags & EF_TELEPORT_BIT ) ^ EF_TELEPORT_BIT;
  G_UnlaggedClear( ent );

  // clear everything but the persistant data

  saved = client->pers;
  savedSess = client->sess;
  savedPing = client->ps.ping;

  for( i = 0; i < MAX_PERSISTANT; i++ )
    persistant[ i ] = client->ps.persistant[ i ];

  eventSequence = client->ps.eventSequence;
  memset( client, 0, sizeof( *client ) );

  client->pers = saved;
  client->sess = savedSess;
  client->ps.ping = savedPing;
  client->lastkilled_client = -1;
  client->notrackEndTime = level.time;

  for( i = 0; i < MAX_PERSISTANT; i++ )
    client->ps.persistant[ i ] = persistant[ i ];

  client->ps.eventSequence = eventSequence;

  // increment the spawncount so the client will detect the respawn
  client->ps.persistant[ PERS_SPAWN_COUNT ]++;
  client->ps.persistant[ PERS_SPECSTATE ] = client->sess.spectatorState;

  client->airOutTime = level.time + 12000;

  trap_GetUserinfo( index, userinfo, sizeof( userinfo ) );
  client->ps.eFlags = flags;

  //Com_Printf( "ent->client->pers->pclass = %i\n", ent->client->pers.classSelection );

  ent->s.groundEntityNum = ENTITYNUM_NONE;
  ent->client = &level.clients[ index ];
  ent->takedamage = qtrue;
  ent->inuse = qtrue;
  ent->classname = "player";
  ent->r.contents = CONTENTS_BODY;
  ent->clipmask = MASK_PLAYERSOLID;
  ent->die = player_die;
  ent->waterlevel = 0;
  ent->watertype = 0;
  ent->flags = 0;

  // calculate each client's acceleration
  ent->evaluateAcceleration = qtrue;

  client->ps.stats[ STAT_MISC ] = 0;

  client->ps.eFlags = flags;
  client->ps.clientNum = index;

  BG_ClassBoundingBox( ent->client->pers.classSelection, ent->r.mins, ent->r.maxs, NULL, NULL, NULL );

  if( client->sess.spectatorState == SPECTATOR_NOT )
    client->ps.stats[ STAT_MAX_HEALTH ] =
      BG_Class( ent->client->pers.classSelection )->health;
  else
    client->ps.stats[ STAT_MAX_HEALTH ] = 100;

  // clear entity values
  if( ent->client->pers.classSelection == PCL_HUMAN )
  {
    if (SPAWN_WITH_MEDKIT) BG_AddUpgradeToInventory( UP_MEDKIT, client->ps.stats );
    weapon = client->pers.humanItemSelection;
  }
  else if( client->sess.spectatorState == SPECTATOR_NOT )
    weapon = BG_Class( ent->client->pers.classSelection )->startWeapon;
  else
    weapon = WP_NONE;

  maxAmmo = BG_Weapon( weapon )->maxAmmo;
  maxClips = BG_Weapon( weapon )->maxClips;
  client->ps.stats[ STAT_WEAPON ] = weapon;
  client->ps.ammo = maxAmmo;
  client->ps.clips = maxClips;

  if( weapon == WP_ALEVEL1 )
  {
    client->ps.stats[ STAT_MISC ] = LEVEL1_WARP_TIME;
  }

  // We just spawned, not changing weapons
  client->ps.persistant[ PERS_NEWWEAPON ] = 0;

  ent->client->ps.stats[ STAT_CLASS ] = ent->client->pers.classSelection;
  ent->client->ps.stats[ STAT_TEAM ] = ent->client->pers.teamSelection;

  ent->client->ps.stats[ STAT_BUILDABLE ] = BA_NONE;
  ent->client->ps.stats[ STAT_STATE ] = 0;
  VectorSet( ent->client->ps.grapplePoint, 0.0f, 0.0f, 1.0f );

  // health will count down towards max_health
  ent->health = client->ps.stats[ STAT_HEALTH ] = client->ps.stats[ STAT_MAX_HEALTH ]; //* 1.25;

  //if evolving scale health
  if( ent == spawn )
  {
    ent->health *= ent->client->pers.evolveHealthFraction;
    client->ps.stats[ STAT_HEALTH ] *= ent->client->pers.evolveHealthFraction;
  }

  //clear the credits array
  for( i = 0; i < MAX_CLIENTS; i++ )
    ent->credits[ i ] = 0;

  client->ps.stats[ STAT_STAMINA ] = STAMINA_MAX;

  G_SetOrigin( ent, spawn_origin );
  VectorCopy( spawn_origin, client->ps.origin );

#define UP_VEL  150.0f
#define F_VEL   50.0f

  //give aliens some spawn velocity
  if( client->sess.spectatorState == SPECTATOR_NOT &&
      client->ps.stats[ STAT_TEAM ] == TEAM_ALIENS )
  {
    if( ent == spawn )
    {
      //evolution particle system
      G_AddPredictableEvent( ent, EV_ALIEN_EVOLVE, DirToByte( up ) );
    }
    else
    {
      spawn_angles[ YAW ] += 180.0f;
      AngleNormalize360( spawn_angles[ YAW ] );

      if( spawnPoint->s.origin2[ 2 ] > 0.0f )
      {
        vec3_t  forward, dir;

        AngleVectors( spawn_angles, forward, NULL, NULL );
        VectorScale( forward, F_VEL, forward );
        VectorAdd( spawnPoint->s.origin2, forward, dir );
        VectorNormalize( dir );

        VectorScale( dir, UP_VEL, client->ps.velocity );
      }

      G_AddPredictableEvent( ent, EV_PLAYER_RESPAWN, 0 );
      
      //
      G_Sound( ent, CHAN_VOICE, G_SoundIndex( "sound/buildables/alien/spawn1.wav" ) );
    }
  }
  else if( client->sess.spectatorState == SPECTATOR_NOT &&
           client->ps.stats[ STAT_TEAM ] == TEAM_HUMANS )
  {
    spawn_angles[ YAW ] += 180.0f;
    AngleNormalize360( spawn_angles[ YAW ] );
    
    //
    G_Sound( ent, CHAN_VOICE, G_SoundIndex( "sound/buildables/human/spawn1.wav" ) );
  }

  // the respawned flag will be cleared after the attack and jump keys come up
  client->ps.pm_flags |= PMF_RESPAWNED;

  trap_GetUsercmd( client - level.clients, &ent->client->pers.cmd );
  G_SetClientViewAngle( ent, spawn_angles );

  if( client->sess.spectatorState == SPECTATOR_NOT )
  {
    trap_LinkEntity( ent );

    // force the base weapon up
    if( client->pers.teamSelection == TEAM_HUMANS )
      G_ForceWeaponChange( ent, weapon );

    client->ps.weaponstate = WEAPON_READY;
  }

  // don't allow full run speed for a bit
  client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
  client->ps.pm_time = 100;

  client->respawnTime = level.time;
  ent->nextRegenTime = level.time;

  client->inactivityTime = level.time + g_inactivity.integer * 1000;
  client->latched_buttons = 0;

  // set default animations
  client->ps.torsoAnim = TORSO_STAND;
  client->ps.legsAnim = LEGS_IDLE;

  if( level.intermissiontime )
    MoveClientToIntermission( ent );
  else
  {
    // fire the targets of the spawn point
    if( !spawn )
      G_UseTargets( spawnPoint, ent );

    // select the highest weapon number available, after any
    // spawn given items have fired
    client->ps.weapon = 1;

    for( i = WP_NUM_WEAPONS - 1; i > 0 ; i-- )
    {
      if( BG_InventoryContainsWeapon( i, client->ps.stats ) )
      {
        client->ps.weapon = i;
        break;
      }
    }
  }

  // run a client frame to drop exactly to the floor,
  // initialize animations and other things
  client->ps.commandTime = level.time - 100;
  ent->client->pers.cmd.serverTime = level.time;
  ClientThink( ent-g_entities );

  // positively link the client, even if the command times are weird
  if( client->sess.spectatorState == SPECTATOR_NOT )
  {
    BG_PlayerStateToEntityState( &client->ps, &ent->s, qtrue );
    VectorCopy( ent->client->ps.origin, ent->r.currentOrigin );
    trap_LinkEntity( ent );
  }

  // must do this here so the number of active clients is calculated
  CalculateRanks( );

  // run the presend to set anything else
  ClientEndFrame( ent );

  // clear entity state values
  BG_PlayerStateToEntityState( &client->ps, &ent->s, qtrue );
}


/*
===========
ClientDisconnect

Called when a player drops from the server.
Will not be called between levels.

This should NOT be called directly by any game logic,
call trap_DropClient(), which will call this and do
server system housekeeping.
============
*/
void ClientDisconnect( int clientNum )
{
  gentity_t *ent;
  gentity_t *tent;
  int       i;

  ent = g_entities + clientNum;

  if( !ent->client || ent->client->pers.connected == CON_DISCONNECTED )
    return;

  G_LeaveTeam( ent );
  G_namelog_disconnect( ent->client );
  G_Vote( ent, TEAM_NONE, qfalse );

  // send effect if they were completely connected
  if( ent->client->pers.connected == CON_CONNECTED &&
      ent->client->sess.spectatorState == SPECTATOR_NOT )
  {
    tent = G_TempEntity( ent->client->ps.origin, EV_PLAYER_TELEPORT_OUT );
    tent->s.clientNum = ent->s.clientNum;
  }

  G_LogCombatStats( ent );

  G_LogPrintf( "^5ClientDisconnect: ^7%i [%s] (%s) \"%s^7\"\n", clientNum,
   ent->client->pers.ip.str, ent->client->pers.guid, ent->client->pers.netname );

  trap_UnlinkEntity( ent );
  ent->s.modelindex = 0;
  ent->inuse = qfalse;
  ent->classname = "disconnected";
  ent->client->pers.connected = CON_DISCONNECTED;
  ent->client->sess.spectatorState =
      ent->client->ps.persistant[ PERS_SPECSTATE ] = SPECTATOR_NOT;

  trap_SetConfigstring( CS_PLAYERS + clientNum, "");

  CalculateRanks( );
}
