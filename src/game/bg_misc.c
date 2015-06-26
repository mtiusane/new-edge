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

// bg_misc.c -- both games misc functions, all completely stateless
#include "../qcommon/q_shared.h"
#include "bg_public.h"

int  trap_FS_FOpenFile( const char *qpath, fileHandle_t *f, fsMode_t mode );
void trap_FS_Read( void *buffer, int len, fileHandle_t f );
void trap_FS_Write( const void *buffer, int len, fileHandle_t f );
void trap_FS_FCloseFile( fileHandle_t f );
void trap_FS_Seek( fileHandle_t f, long offset, fsOrigin_t origin ); // fsOrigin_t
int  trap_FS_GetFileList(  const char *path, const char *extension, char *listbuf, int bufsize );

#define STAGE_GE_5   ((1 << S5))
#define STAGE_GE_4   ((1 << S4) | STAGE_GE_5)
#define STAGE_GE_3   ((1 << S3) | STAGE_GE_4)
#define STAGE_GE_2   ((1 << S2) | STAGE_GE_3)
#define STAGE_GE_1   ((1 << S1) | STAGE_GE_2)

static const buildableAttributes_t bg_buildableList[ ] =
{
//ALIEN BUILDINGS
  {
    BA_A_SPAWN,            //int       buildNum;
    "eggpod",              //char      *buildName;
    "Egg",                 //char      *humanName;
    "The most basic alien structure. It allows aliens to spawn "
    "and protect the Overmind. Without any of these, the Overmind "
    "is left nearly defenseless and defeat is imminent.",
    "team_alien_spawn",    //char      *entityName;
    TR_GRAVITY,            //trType_t  traj;
    0.0,                   //float     bounce;
    ASPAWN_BP,             //int       buildPoints;
    STAGE_GE_1,            //int  stages
    ASPAWN_HEALTH,         //int       health;
    ASPAWN_REGEN,          //int       regenRate;
    ASPAWN_SPLASHDAMAGE,   //int       splashDamage;
    ASPAWN_SPLASHRADIUS,   //int       splashRadius;
    MOD_ASPAWN,            //int       meansOfDeath;
    TEAM_ALIENS,           //int       team;
    ( 1 << WP_ABUILD ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    100,                   //int       nextthink;
    ASPAWN_BT,             //int       buildTime;
    qfalse,                //qboolean  usable;
    0,                     //int       turretRange;
    0,                     //int       turretFireSpeed;
    WP_NONE,               //weapon_t  turretProjType;
    0.0f,                  //float     minNormal; 
    qtrue,                 //qboolean  invertNormal;
    qfalse,                //qboolean  creepTest;
    ASPAWN_CREEPSIZE,      //int       creepSize;
    qfalse,                //qboolean  dccTest;
    qfalse,                //qboolean  transparentTest;
    qfalse,                //qboolean  uniqueTest;
    ASPAWN_VALUE,          //int       value;
  },
  {
    BA_A_OVERMIND,         //int       buildNum;
    "overmind",            //char      *buildName;
    "Overmind",            //char      *humanName;
    "A collective consciousness that controls all the alien structures "
    "in its vicinity. It must be protected at all costs, since its "
    "death will render alien structures defenseless.",
    "team_alien_overmind", //char      *entityName;
    TR_GRAVITY,            //trType_t  traj;
    0.0,                   //float     bounce;
    OVERMIND_BP,           //int       buildPoints;
    STAGE_GE_1,            //int  stages
    OVERMIND_HEALTH,       //int       health;
    OVERMIND_REGEN,        //int       regenRate;
    OVERMIND_SPLASHDAMAGE, //int       splashDamage;
    OVERMIND_SPLASHRADIUS, //int       splashRadius;
    MOD_ASPAWN,            //int       meansOfDeath;
    TEAM_ALIENS,           //int       team;
    ( 1 << WP_ABUILD ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    OVERMIND_ATTACK_REPEAT,//int       nextthink;
    OVERMIND_BT,           //int       buildTime;
    qfalse,                //qboolean  usable;
    0,                     //int       turretRange;
    0,                     //int       turretFireSpeed;
    WP_NONE,               //weapon_t  turretProjType;
    0.0f,                 //float     minNormal;
    qtrue,                //qboolean  invertNormal;
    qfalse,                //qboolean  creepTest;
    OVERMIND_CREEPSIZE,    //int       creepSize;
    qfalse,                //qboolean  dccTest;
    qfalse,                //qboolean  transparentTest;
    qtrue,                 //qboolean  uniqueTest;
    OVERMIND_VALUE,        //int       value;
  },
  {
    BA_A_BARRICADE,        //int       buildNum;
    "barricade",           //char      *buildName;
    "Barricade",           //char      *humanName;
    "Used to obstruct corridors and doorways, hindering humans from "
    "threatening the spawns and Overmind. Barricades will shrink "
    "to allow aliens to pass over them, however.",
    "team_alien_barricade",//char      *entityName;
    TR_GRAVITY,            //trType_t  traj;
    0.0,                   //float     bounce;
    BARRICADE_BP,          //int       buildPoints;
    STAGE_GE_1,            //int  stages
    BARRICADE_HEALTH,      //int       health;
    BARRICADE_REGEN,       //int       regenRate;
    BARRICADE_SPLASHDAMAGE,//int       splashDamage;
    BARRICADE_SPLASHRADIUS,//int       splashRadius;
    MOD_ASPAWN,            //int       meansOfDeath;
    TEAM_ALIENS,           //int       team;
    ( 1 << WP_ABUILD ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    100,                   //int       nextthink;
    BARRICADE_BT,          //int       buildTime;
    qfalse,                //qboolean  usable;
    0,                     //int       turretRange;
    0,                     //int       turretFireSpeed;
    WP_NONE,               //weapon_t  turretProjType;
    0.0f,                  //float     minNormal;
    qtrue,                 //qboolean  invertNormal;//
    qtrue,                 //qboolean  creepTest;
    BARRICADE_CREEPSIZE,   //int       creepSize;
    qfalse,                //qboolean  dccTest;
    qfalse,                //qboolean  transparentTest;
    qfalse,                //qboolean  uniqueTest;
    BARRICADE_VALUE,       //int       value;
  },
  {
    BA_A_ACIDTUBE,         //int       buildNum;
    "acid_tube",           //char      *buildName;
    "Acid Tube",           //char      *humanName;
    "Ejects lethal poisonous acid at an approaching human. These "
    "are highly effective when used in conjunction with a trapper "
    "to hold the victim in place.",
    "team_alien_acid_tube",//char      *entityName;
    TR_GRAVITY,            //trType_t  traj;
    0.0,                   //float     bounce;
    ACIDTUBE_BP,           //int       buildPoints;
    STAGE_GE_1,            //int  stages
    ACIDTUBE_HEALTH,       //int       health;
    ACIDTUBE_REGEN,        //int       regenRate;
    ACIDTUBE_SPLASHDAMAGE, //int       splashDamage;
    ACIDTUBE_SPLASHRADIUS, //int       splashRadius;
    MOD_ASPAWN,            //int       meansOfDeath;
    TEAM_ALIENS,           //int       team;
    ( 1 << WP_ABUILD ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    200,                   //int       nextthink;
    ACIDTUBE_BT,           //int       buildTime;
    qfalse,                //qboolean  usable;
    0,                     //int       turretRange;
    0,                     //int       turretFireSpeed;
    WP_NONE,               //weapon_t  turretProjType;
    0.0f,                  //float     minNormal;
    qtrue,                 //qboolean  invertNormal;
    qtrue,                 //qboolean  creepTest;
    ACIDTUBE_CREEPSIZE,    //int       creepSize;
    qfalse,                //qboolean  dccTest;
    qfalse,                //qboolean  transparentTest;
    qfalse,                //qboolean  uniqueTest;
    ACIDTUBE_VALUE,        //int       value;
  },
  {
    BA_A_BOOSTER,          //int       buildNum;
    "booster",             //char      *buildName;
    "Booster",             //char      *humanName;
    "Laces the attacks of any alien that touches it with a poison "
    "that will gradually deal damage to any humans exposed to it. "
    "The booster also increases the rate of health regeneration for "
    "any nearby aliens.",
    "team_alien_booster",  //char      *entityName;
    TR_GRAVITY,            //trType_t  traj;
    0.0,                   //float     bounce;
    BOOSTER_BP,            //int       buildPoints;
    STAGE_GE_3,            //int  stages
    BOOSTER_HEALTH,        //int       health;
    BOOSTER_REGEN,         //int       regenRate;
    BOOSTER_SPLASHDAMAGE,  //int       splashDamage;
    BOOSTER_SPLASHRADIUS,  //int       splashRadius;
    MOD_ASPAWN,            //int       meansOfDeath;
    TEAM_ALIENS,            //int       team;
    ( 1 << WP_ABUILD ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    100,                   //int       nextthink;
    BOOSTER_BT,            //int       buildTime;
    qfalse,                //qboolean  usable;
    0,                     //int       turretRange;
    0,                     //int       turretFireSpeed;
    WP_NONE,               //weapon_t  turretProjType;
    0.0f,                  //float     minNormal; 
    qtrue,                 //qboolean  invertNormal; 
    qtrue,                 //qboolean  creepTest;
    BOOSTER_CREEPSIZE,     //int       creepSize;
    qfalse,                //qboolean  dccTest;
    qtrue,                 //qboolean  transparentTest;
    qfalse,                //qboolean  uniqueTest;
    BOOSTER_VALUE,         //int       value;
  },
   {
    BA_A_HIVE,             //int       buildNum;
    "hive",                //char      *buildName;
    "Hive",                //char      *humanName;
    "Houses millions of tiny insectoid aliens. When a human "
    "approaches this structure, the insectoids attack.",
    "team_alien_hive",     //char      *entityName;
    TR_GRAVITY,            //trType_t  traj;
    0.0,                   //float     bounce;
    HIVE_BP,               //int       buildPoints;
    STAGE_GE_4,            //int  stages
    HIVE_HEALTH,           //int       health;
    HIVE_REGEN,            //int       regenRate;
    HIVE_SPLASHDAMAGE,     //int       splashDamage;
    HIVE_SPLASHRADIUS,     //int       splashRadius;
    MOD_ASPAWN,            //int       meansOfDeath;
    TEAM_ALIENS,           //int       team;
    ( 1 << WP_ABUILD ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    500,                   //int       nextthink;
    HIVE_BT,               //int       buildTime;
    qfalse,                //qboolean  usable;
    0,                     //int       turretRange;
    0,                     //int       turretFireSpeed;
    WP_HIVE,               //weapon_t  turretProjType;
    0.0f,                  //float     minNormal;
    qtrue,                 //qboolean  invertNormal;
    qtrue,                 //qboolean  creepTest;
    HIVE_CREEPSIZE,        //int       creepSize;
    qfalse,                //qboolean  dccTest;
    qfalse,                //qboolean  transparentTest;
    qfalse,                //qboolean  uniqueTest;
    HIVE_VALUE,            //int       value;
  },
  {
    BA_A_PANZER_SMALL,        	//int       buildNum;
    "panzer_small",         	//char      *buildName;
    "[ye]Panzer Small",       //char      *humanName;
	"LIGHT ORGANIC STRUCTURE\n"
    "[yei]Used for passive base defense.\n",
    "team_alien_panzer_small", //char      *entityName;
    TR_GRAVITY,          	   //trType_t  traj;
    0,                  	   //float     bounce;
    PANZER_SMALL_BP,           //int       buildPoints;
    STAGE_GE_1,                //int  stages
    PANZER_SMALL_HEALTH,       //int       health;
    PANZER_SMALL_REGEN,        //int       regenRate;
    PANZER_SMALL_SPLASHDAMAGE, //int       splashDamage;
    PANZER_SMALL_SPLASHRADIUS, //int       splashRadius;
    MOD_ASPAWN,                //int       meansOfDeath;
    TEAM_ALIENS,               //int       team;
    ( 1 << WP_ABUILD ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,               //int       idleAnim;
    1000,                 		   //int       nextthink;
    PANZER_SMALL_BT,           //int       buildTime;
    qfalse,              	   //qboolean  usable;
    0,                   	   //int       turretRange;
    0,                   	   //int       turretFireSpeed;
    WP_NONE,             	   //weapon_t  turretProjType;
    0.0f,                	   //float     minNormal;
    qtrue,                	   //qboolean  invertNormal;
    qtrue,                	   //qboolean  creepTest;
    PANZER_SMALL_CREEPSIZE,    //int       creepSize;
    qfalse,               	   //qboolean  dccTest;
    qfalse,               	   //qboolean  transparentTest;
    qfalse,               	   //qboolean  uniqueTest;
    PANZER_SMALL_VALUE,        //int       value;
  },
  {
    BA_A_PANZER_MEDIUM,         //int       buildNum;
    "panzer_medium",            //char      *buildName;
    "[ye]Panzer Medium",      //char      *humanName;
	"AVERAGE ORGANIC STRUCTURE\n"
    "[yei]Used for passive base defense.\n",
    "team_alien_panzer_medium", //char      *entityName;
    TR_GRAVITY,           	    //trType_t  traj;
    0,                 		    //float     bounce;
    PANZER_MEDIUM_BP,           //int       buildPoints;
    STAGE_GE_3,                 //int  stages
    PANZER_MEDIUM_HEALTH,       //int       health;
    PANZER_MEDIUM_REGEN,        //int       regenRate;
    PANZER_MEDIUM_SPLASHDAMAGE, //int       splashDamage;
    PANZER_MEDIUM_SPLASHRADIUS, //int       splashRadius;
    MOD_ASPAWN,           	    //int       meansOfDeath;
    TEAM_ALIENS,           	    //int       team;
    ( 1 << WP_ABUILD ),  	    //weapon_t  buildWeapon;
    BANIM_IDLE1,         	    //int       idleAnim;
    1000,                   		//int       nextthink;
    PANZER_MEDIUM_BT,           //int       buildTime;
    qfalse,               	    //qboolean  usable;
    0,                    	    //int       turretRange;
    0,                    	    //int       turretFireSpeed;
    WP_NONE,             	    //weapon_t  turretProjType;
    0.0f,                       //float     minNormal;
    qtrue,                      //qboolean  invertNormal;
    qtrue,                      //qboolean  creepTest;
    PANZER_MEDIUM_CREEPSIZE,    //int       creepSize;
    qfalse,                     //qboolean  dccTest;
    qfalse,                     //qboolean  transparentTest;
    qfalse,                     //qboolean  uniqueTest;
    PANZER_MEDIUM_VALUE,        //int       value;
  },
  {
    BA_A_PANZER_LARGE,          //int       buildNum;
    "panzer_large",             //char      *buildName;
    "[ye]Panzer Large",       //char      *humanName;
	"HEAVY ORGANIC STRUCTURE\n"
    "[yei]Used for passive base defense.\n",
    "team_alien_panzer_large",  //char      *entityName;
    TR_GRAVITY,                 //trType_t  traj;
    0,                          //float     bounce;
    PANZER_LARGE_BP,            //int       buildPoints;
    STAGE_GE_4,                 //int  stages
    PANZER_LARGE_HEALTH,        //int       health;
    PANZER_LARGE_REGEN,         //int       regenRate;
    PANZER_LARGE_SPLASHDAMAGE,  //int       splashDamage;
    PANZER_LARGE_SPLASHRADIUS,  //int       splashRadius;
    MOD_ASPAWN,                 //int       meansOfDeath;
    TEAM_ALIENS,                //int       team;
    ( 1 << WP_ABUILD ),        //weapon_t  buildWeapon;
    BANIM_IDLE1,                //int       idleAnim;
    1000,                          //int       nextthink;
    PANZER_LARGE_BT,            //int       buildTime;
    qfalse,               	    //qboolean  usable;
    0,                    	    //int       turretRange;
    0,                     		//int       turretFireSpeed;
    WP_NONE,             	    //weapon_t  turretProjType;
    0.0f,                 	    //float     minNormal;
    qtrue,                 		//qboolean  invertNormal;
    qtrue,                 		//qboolean  creepTest;
    PANZER_LARGE_CREEPSIZE,     //int       creepSize;
    qfalse,                		//qboolean  dccTest;
    qfalse,                		//qboolean  transparentTest;
    qfalse,                		//qboolean  uniqueTest;
    PANZER_LARGE_VALUE,         //int       value;
  },
  {
    BA_A_TENDON_SMALL,          //int       buildNum;
    "tendon_small",             //char      *buildName;
    "[ye]Tendon Small",       //char      *humanName;
	"LIGHT ORGANIC STRUCTURE\n"
    "[yei]Used for passive base defense or bridges.\n",
    "team_alien_tendon_small",  //char      *entityName;
    TR_GRAVITY,                 //trType_t  traj;
    0,                          //float     bounce;
    TENDON_SMALL_BP,            //int       buildPoints;
    STAGE_GE_1,                 //int  stages
    TENDON_SMALL_HEALTH,        //int       health;
    TENDON_SMALL_REGEN,         //int       regenRate;
    TENDON_SMALL_SPLASHDAMAGE,  //int       splashDamage;
    TENDON_SMALL_SPLASHRADIUS,  //int       splashRadius;
    MOD_ASPAWN,                 //int       meansOfDeath;
    TEAM_ALIENS,                //int       team;
    ( 1 << WP_ABUILD ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,                //int       idleAnim;
    1000,                          //int       nextthink;
    TENDON_SMALL_BT,            //int       buildTime;
    qfalse,                     //qboolean  usable;
    0,                          //int       turretRange;
    0,                          //int       turretFireSpeed;
    WP_NONE,                    //weapon_t  turretProjType;
    0.0f,                       //float     minNormal;
    qtrue,                      //qboolean  invertNormal;
    qtrue,                      //qboolean  creepTest;
    TENDON_SMALL_CREEPSIZE,     //int       creepSize;
    qfalse,                     //qboolean  dccTest;
    qfalse,                     //qboolean  transparentTest;
    qfalse,                     //qboolean  uniqueTest;
    TENDON_SMALL_VALUE,         //int       value;
  },
  {
    BA_A_TENDON_MEDIUM,         //int       buildNum;
    "tendon_medium",           //char      *buildName;
    "[ye]Tendon Medium",           //char      *humanName;
	"AVERAGE ORGANIC STRUCTURE\n"
    "[yei]Used for passive base defense or bridges.\n"
	"50HP",
    "team_alien_tendon_medium",//char      *entityName;
    TR_GRAVITY,            //trType_t  traj;
    0.0,                   //float     bounce;
    TENDON_MEDIUM_BP,           //int       buildPoints;
    STAGE_GE_2,                 //int  stages
    TENDON_MEDIUM_HEALTH,       //int       health;
    TENDON_MEDIUM_REGEN,        //int       regenRate;
    TENDON_MEDIUM_SPLASHDAMAGE, //int       splashDamage;
    TENDON_MEDIUM_SPLASHRADIUS, //int       splashRadius;
    MOD_ASPAWN,             //int       meansOfDeath;
    TEAM_ALIENS,            //int       team;
    ( 1 << WP_ABUILD ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    1000,                   //int       nextthink;
    TENDON_MEDIUM_BT,           //int       buildTime;
    qfalse,                //qboolean  usable;
    0,                     //int       turretRange;
    0,                     //int       turretFireSpeed;
    WP_NONE,               //weapon_t  turretProjType;
    0.0f,                  //float     minNormal;
    qtrue,                 //qboolean  invertNormal;
    qtrue,                 //qboolean  creepTest;
    TENDON_MEDIUM_CREEPSIZE,    //int       creepSize;
    qfalse,                //qboolean  dccTest;
    qfalse,                //qboolean  transparentTest;
    qfalse,                //qboolean  uniqueTest;
    TENDON_MEDIUM_VALUE,            //int       value;
  },
  {
    BA_A_TENDON_LARGE,         //int       buildNum;
    "tendon_large",           //char      *buildName;
    "[ye]Tendon Large",           //char      *humanName;
	"HEAVY ORGANIC STRUCTURE\n"
    "[yei]Used for passive base defense or big bridges.\n"
	"100HP",
    "team_alien_tendon_large",//char      *entityName;
    TR_GRAVITY,            //trType_t  traj;
    0,                   //float     bounce;
    TENDON_LARGE_BP,           //int       buildPoints;
    STAGE_GE_3,                //int  stages
    TENDON_LARGE_HEALTH,       //int       health;
    TENDON_LARGE_REGEN,        //int       regenRate;
    TENDON_LARGE_SPLASHDAMAGE, //int       splashDamage;
    TENDON_LARGE_SPLASHRADIUS, //int       splashRadius;
    MOD_ASPAWN,             //int       meansOfDeath;
    TEAM_ALIENS,            //int       team;
    ( 1 << WP_ABUILD ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    1000,                   //int       nextthink;
    TENDON_LARGE_BT,           //int       buildTime;
    qfalse,                //qboolean  usable;
    0,                     //int       turretRange;
    0,                     //int       turretFireSpeed;
    WP_NONE,               //weapon_t  turretProjType;
    0.0f,                  //float     minNormal;
    qtrue,                 //qboolean  invertNormal;
    qtrue,                 //qboolean  creepTest;
    TENDON_LARGE_CREEPSIZE,    //int       creepSize;
    qfalse,                //qboolean  dccTest;
    qfalse,                //qboolean  transparentTest;
    qfalse,                //qboolean  uniqueTest;
    TENDON_LARGE_VALUE,            //int       value;
  },
  {
    BA_A_NET,         //int       buildNum;
    "net",           //char      *buildName;
    "[ye]Net",           //char      *humanName;
	"LIGHT ORGANIC STRUCTURE\n"
    "[yei]Used for passive base defense.\n"
	"Can be used for camouflage.\n"
	"80HP",
    "team_alien_net",//char      *entityName;
    TR_GRAVITY,            //trType_t  traj;
    0,                   //float     bounce;
    NET_BP,           //int       buildPoints;
    STAGE_GE_3,       //int  stages
    NET_HEALTH,       //int       health;
    NET_REGEN,        //int       regenRate;
    NET_SPLASHDAMAGE, //int       splashDamage;
    NET_SPLASHRADIUS, //int       splashRadius;
    MOD_ASPAWN,             //int       meansOfDeath;
    TEAM_ALIENS,            //int       team;
    ( 1 << WP_ABUILD ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    1000,                   //int       nextthink;
    NET_BT,           //int       buildTime;
    qfalse,                //qboolean  usable;
    0,                     //int       turretRange;
    0,                     //int       turretFireSpeed;
    WP_NONE,               //weapon_t  turretProjType;
    0.0f,                  //float     minNormal;
    qtrue,                 //qboolean  invertNormal;
    qtrue,                 //qboolean  creepTest;
    NET_CREEPSIZE,    //int       creepSize;
    qfalse,                //qboolean  dccTest;
    qfalse,                //qboolean  transparentTest;
    qfalse,                //qboolean  uniqueTest;
    NET_VALUE,            //int       value;
  },
  {
    BA_A_NET_SPIKE,         //int       buildNum;
    "net_spike",           //char      *buildName;
    "[ye]Net Spike",           //char      *humanName;
	"LIGHT ORGANIC STRUCTURE\n"
    "[yei]Used for passive base defense.\n"
	"Can be used for camouflage.\n"
	"50HP",
    "team_alien_net_spike",//char      *entityName;
    TR_GRAVITY,            //trType_t  traj;
    0,                   //float     bounce;
    NET_SPIKE_BP,           //int       buildPoints;
    STAGE_GE_1,             //int  stages
    NET_SPIKE_HEALTH,       //int       health;
    NET_SPIKE_REGEN,        //int       regenRate;
    NET_SPIKE_SPLASHDAMAGE, //int       splashDamage;
    NET_SPIKE_SPLASHRADIUS, //int       splashRadius;
    MOD_ASPAWN,             //int       meansOfDeath;
    TEAM_ALIENS,            //int       team;
    ( 1 << WP_ABUILD ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    1000,                   //int       nextthink;
    NET_SPIKE_BT,           //int       buildTime;
    qfalse,                //qboolean  usable;
    0,                     //int       turretRange;
    0,                     //int       turretFireSpeed;
    WP_NONE,               //weapon_t  turretProjType;
    0.0f,                  //float     minNormal;
    qtrue,                 //qboolean  invertNormal;
    qtrue,                 //qboolean  creepTest;
    NET_SPIKE_CREEPSIZE,    //int       creepSize;
    qfalse,                //qboolean  dccTest;
    qfalse,                //qboolean  transparentTest;
	qfalse,                //qboolean  uniqueTest;
    NET_SPIKE_VALUE,            //int       value;
  },
    {
    BA_A_INFESTATION_SLIME,         //int       buildNum;
    "infestation_slime",           //char      *buildName;
    "[ye]Infestation Slime",           //char      *humanName;
	"LIGHT ORGANIC INFESTATION\n"
    "[yei]Used for passive and active base defense.\n"
	"Has the ability to slow down human players.\n"
	"Nearby Humans get *sucked* in and take damage.\n"
	"250HP",
    "team_alien_infestation_slime",//char      *entityName;
    TR_GRAVITY,            //trType_t  traj;
    0,                   //float     bounce;
    INFESTATION_SLIME_BP,           //int       buildPoints;
    STAGE_GE_5,                     //int  stages
    INFESTATION_SLIME_HEALTH,       //int       health;
    INFESTATION_SLIME_REGEN,        //int       regenRate;
    INFESTATION_SLIME_SPLASHDAMAGE, //int       splashDamage;
    INFESTATION_SLIME_SPLASHRADIUS, //int       splashRadius;
    MOD_ASPAWN,             //int       meansOfDeath;
    TEAM_ALIENS,            //int       team;
    ( 1 << WP_ABUILD ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    200,                   //int       nextthink;
    INFESTATION_SLIME_BT,           //int       buildTime;
    qfalse,                //qboolean  usable;
    0,                     //int       turretRange;
    0,                     //int       turretFireSpeed;
    WP_NONE,               //weapon_t  turretProjType;
    0.0f,                  //float     minNormal;
    qtrue,                 //qboolean  invertNormal;
    qtrue,                 //qboolean  creepTest;
    INFESTATION_SLIME_CREEPSIZE,    //int       creepSize;
    qfalse,                //qboolean  transparentTest;
    qfalse,                //qboolean  uniqueTest;
	INFESTATION_SLIME_VALUE,        //int       value;
  },
  {
    BA_A_INFESTATION_THICKET,         	   //int       buildNum;
    "infestation_thicket",           	   //char      *buildName;
    "[ye]Infestation Thicket",           //char      *humanName;
	"AVERAGE ORGANIC INFESTATION\n"
    "[yei]Used for passive base defense.\n"
	"Has the ability to slow down human players.\n"
	"It's dense undergrowth can be used for camouflage.\n",
    "team_alien_infestation_thicket",	  //char      *entityName;
    TR_GRAVITY,           				  //trType_t  traj;
    0,                  				  //float     bounce;
    INFESTATION_THICKET_BP,          	  //int       buildPoints;
    STAGE_GE_1,                           //int       stages
    INFESTATION_THICKET_HEALTH,      	  //int       health;
    INFESTATION_THICKET_REGEN,     	      //int       regenRate;
    INFESTATION_THICKET_SPLASHDAMAGE, 	  //int       splashDamage;
    INFESTATION_THICKET_SPLASHRADIUS, 	  //int       splashRadius;
    MOD_ASPAWN,            				  //int       meansOfDeath;
    TEAM_ALIENS,          				  //int       team;
    ( 1 << WP_ABUILD ),              //weapon_t  buildWeapon;
    BANIM_IDLE1,       				      //int       idleAnim;
    1000,                  				  //int       nextthink;
    INFESTATION_THICKET_BT,      	      //int       buildTime;
    qfalse,              				  //qboolean  usable;
    0,                  			      //int       turretRange;
    0,                   				  //int       turretFireSpeed;
    WP_NONE,           				      //weapon_t  turretProjType;
    0.0f,             				      //float     minNormal;
    qtrue,                		          //qboolean  invertNormal;
    qtrue,               				  //qboolean  creepTest;
    INFESTATION_THICKET_CREEPSIZE,    	  //int       creepSize;
    qfalse,               				  //qboolean  dccTest;
    qfalse,               				  //qboolean  transparentTest;
	qfalse,               				  //qboolean  uniqueTest;
    INFESTATION_THICKET_VALUE,            //int       value;
   },
   {
    BA_A_INFESTATION_BLISTER,       	 //int       buildNum;
    "infestation_blister",          	 //char      *buildName;
    "[ye]Infestation Blister",         //char      *humanName;
	"HEAVY ORGANIC INFESTATION\n"
    "[yei]Used for passive base defense.\n"
	"Has the ability to slow down human players.\n"
	"Creates a creep of massive scale.\n",
    "team_alien_infestation_blister",	 //char      *entityName;
    TR_GRAVITY,           				 //trType_t  traj;
    0.0,                   				 //float     bounce;
    INFESTATION_BLISTER_BP,        	     //int       buildPoints;
    STAGE_GE_2,       			 //int  stages
    INFESTATION_BLISTER_HEALTH,      	 //int       health;
    INFESTATION_BLISTER_REGEN,        	 //int       regenRate;
    INFESTATION_BLISTER_SPLASHDAMAGE,	 //int       splashDamage;
    INFESTATION_BLISTER_SPLASHRADIUS, 	 //int       splashRadius;
    MOD_ASPAWN,            				 //int       meansOfDeath;
    TEAM_ALIENS,           				 //int       team;
    ( 1 << WP_ABUILD ),    			     //weapon_t  buildWeapon;
    BANIM_IDLE1,          				 //int       idleAnim;
    1000,                   			     //int       nextthink;
    INFESTATION_BLISTER_BT,              //int       buildTime;
    qfalse,                				 //qboolean  usable;
    0,                     				 //int       turretRange;
    0,                     				 //int       turretFireSpeed;
    WP_NONE,             			     //weapon_t  turretProjType;
    0.0f,               			     //float     minNormal;
    qtrue,                				 //qboolean  invertNormal;
    qtrue,                 				 //qboolean  creepTest;
    INFESTATION_BLISTER_CREEPSIZE,       //int       creepSize;
    qfalse,               				 //qboolean  dccTest;
    qfalse,               				 //qboolean  transparentTest;
    qfalse,              			     //qboolean  uniqueTest;
    INFESTATION_BLISTER_VALUE,           //int       value;
  },
  {
    BA_A_REFLECTOR,        				 //int       buildNum;
    "reflector",         			     //char      *buildName;
    "[ye]Reflector",      	         //char      *humanName;
	"INTELLIGENT ORGANIC STRUCTURE\n"
    "[yei]Used for passive base defense.\n"
	"This structure is filled with mercury.\n"
	"It reflects Pulse Rifle & Lucifer shots.\n"
	"Its vulnerable to mechanical projectiles.\n",
    "team_alien_reflector",				 //char      *entityName;
    TR_GRAVITY,            		         //trType_t  traj;
    0.0,                   				 //float     bounce;
    REFLECTOR_BP,        			     //int       buildPoints;
    STAGE_GE_3,            			 //int  stages
    REFLECTOR_HEALTH,       			 //int       health;
    REFLECTOR_REGEN,       			  	 //int       regenRate;
    REFLECTOR_SPLASHDAMAGE, 			 //int       splashDamage;
    REFLECTOR_SPLASHRADIUS,				 //int       splashRadius;
    MOD_ASPAWN,             			 //int       meansOfDeath;
    TEAM_ALIENS,            			 //int       team;
    ( 1 << WP_ABUILD ),    				 //weapon_t  buildWeapon;
    BANIM_IDLE1,          				 //int       idleAnim;
    1000,                  				 //int       nextthink;
    REFLECTOR_BT,        			     //int       buildTime;
    qfalse,              			     //qboolean  usable;
    0,                   	  		     //int       turretRange;
    0,                   			     //int       turretFireSpeed;
    WP_NONE,            			     //weapon_t  turretProjType;
    0.0f,               			     //float     minNormal;
    qtrue,              			     //qboolean  invertNormal;
    qtrue,               			     //qboolean  creepTest;
    REFLECTOR_CREEPSIZE, 			     //int       creepSize;
    qfalse,                				 //qboolean  dccTest;
    qfalse,                				 //qboolean  transparentTest;
	qfalse,               				 //qboolean  uniqueTest;
    REFLECTOR_VALUE,      		         //int       value;
  },
  {
    BA_A_MUSCLE,        				 //int       buildNum;
    "muscle",          					 //char      *buildName;
    "[ye]Muscle",         		     //char      *humanName;
	"INTELLIGENT ORGANIC STRUCTURE\n"
    "[yei]Used to push alien players up.\n",
    "team_alien_muscle",				//char      *entityName;
    TR_GRAVITY,          			    //trType_t  traj;
    0,                  			    //float     bounce;
    MUSCLE_BP,         			            //int       buildPoints;
    STAGE_GE_2,	                      	                //int  stages
    MUSCLE_HEALTH,       				//int       health;
    MUSCLE_REGEN,        				//int       regenRate;
    MUSCLE_SPLASHDAMAGE, 				//int       splashDamage;
    MUSCLE_SPLASHRADIUS, 				//int       splashRadius;
    MOD_ASPAWN,             			//int       meansOfDeath;
    TEAM_ALIENS,            			//int       team;
    ( 1 << WP_ABUILD ),    				//weapon_t  buildWeapon;
    BANIM_IDLE1,           				//int       idleAnim;
    500,                   				//int       nextthink;
    MUSCLE_BT,           				//int       buildTime;
    qfalse,                				//qboolean  usable;
    0,                     				//int       turretRange;
    0,                     				//int       turretFireSpeed;
    WP_NONE,               				//weapon_t  turretProjType;
    0.0f,                  				//float     minNormal;
    qtrue,                 				//qboolean  invertNormal;
    qtrue,                 				//qboolean  creepTest;
    MUSCLE_CREEPSIZE,    				//int       creepSize;
    qfalse,                				//qboolean  dccTest;
    qfalse,                				//qboolean  transparentTest;
    qfalse,                				//qboolean  uniqueTest;
    MUSCLE_VALUE,           		    //int       value;
  },
  {
    BA_A_SPITEFUL_ABCESS,       	   //int       buildNum;
    "spiteful_abcess",           	   //char      *buildName;
    "[ye]Spiteful Abcess",           //char      *humanName;
	"INTELLIGENT ORGANIC STRUCTURE\n"
    "[yei]Used for active base defense.\n"
	"The glowing top contains gas.\n",
    "team_alien_spiteful_abcess",	  //char      *entityName;
    TR_GRAVITY,           			  //trType_t  traj;
    0.0,                   			  //float     bounce;
    SPITEFUL_ABCESS_BP,         	  //int       buildPoints;
    STAGE_GE_2,		                  //int  stages
    SPITEFUL_ABCESS_HEALTH,      	  //int       health;
    SPITEFUL_ABCESS_REGEN,       	  //int       regenRate;
    SPITEFUL_ABCESS_SPLASHDAMAGE, 	  //int       splashDamage;
    SPITEFUL_ABCESS_SPLASHRADIUS, 	  //int       splashRadius;
    MOD_SPITEFUL_ABCESS,              //int       meansOfDeath;
    TEAM_ALIENS,           			  //int       team;
    ( 1 << WP_ABUILD ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,          			  //int       idleAnim;
    200,                   			  //int       nextthink;
    SPITEFUL_ABCESS_BT,         	  //int       buildTime;
    qfalse,                			  //qboolean  usable;
    0,                    			  //int       turretRange;
    0,                    			  //int       turretFireSpeed;
    WP_NONE,               			  //weapon_t  turretProjType;
    0.0f,                  			  //float     minNormal;
    qtrue,                 			  //qboolean  invertNormal;
    qtrue,                 			  //qboolean  creepTest;
    SPITEFUL_ABCESS_CREEPSIZE,    	  //int       creepSize;
    qfalse,                			  //qboolean  dccTest;
    qfalse,                			  //qboolean  transparentTest;
    qfalse,                			  //qboolean  uniqueTest;
    SPITEFUL_ABCESS_VALUE,            //int       value;
  },
  {
    BA_A_COCOON,         			 //int       buildNum;
    "cocoon",           			 //char      *buildName;
    "[ye]Cocoon",           		 //char      *humanName;
	"INTELLIGENT ORGANIC STRUCTURE\n"
    "[yei]This heals your team faster\n"
    "+faster adv.goon barb regen.\n"
	"Can only be built once\n",
    "team_alien_cocoon",			 //char      *entityName;
    TR_GRAVITY,            			 //trType_t  traj;
    0,                   			 //float     bounce;
    COCOON_BP,           			 //int       buildPoints;
    STAGE_GE_4, 				 //int  stages
    COCOON_HEALTH,       			 //int       health;
    COCOON_REGEN,        			 //int       regenRate;
    COCOON_SPLASHDAMAGE, 			 //int       splashDamage;
    COCOON_SPLASHRADIUS, 			 //int       splashRadius;
    MOD_ASPAWN,             		 //int       meansOfDeath;
    TEAM_ALIENS,            		 //int       team;
    ( 1 << WP_ABUILD ),     		 //weapon_t  buildWeapon;
    BANIM_IDLE1,           			 //int       idleAnim;
    1000,                   			 //int       nextthink;
    COCOON_BT,           			 //int       buildTime;
    qfalse,                			 //qboolean  usable;
    0,                     			 //int       turretRange;
    0,                     			 //int       turretFireSpeed;
    WP_NONE,               			 //weapon_t  turretProjType;
    0.0f,                  			 //float     minNormal;
    qtrue,                 			 //qboolean  invertNormal;
    qtrue,                 			 //qboolean  creepTest;
    COCOON_CREEPSIZE,    			 //int       creepSize;
    qfalse,                			 //qboolean  dccTest;
    qfalse,                			 //qboolean  transparentTest;
    qtrue,                 			 //qboolean  uniqueTest;
    COCOON_VALUE,            		 //int       value;
  },
  {
    BA_A_ORGANIC_BULB,         		//int       buildNum;
    "organic_bulb",          	    //char      *buildName;
    "[ye]Organic Bulb",           //char      *humanName;
	"INTELLIGENT ORGANIC STRUCTURE\n"
    "[yei]Used to light spots.\n",
    "team_alien_organic_bulb",		//char      *entityName;
    TR_GRAVITY,            			//trType_t  traj;
    0.0,                   			//float     bounce;
    ORGANIC_BULB_BP,           		//int       buildPoints;
    STAGE_GE_3,           		//int  stages
    ORGANIC_BULB_HEALTH,       		//int       health;
    ORGANIC_BULB_REGEN,        		//int       regenRate;
    ORGANIC_BULB_SPLASHDAMAGE, 		//int       splashDamage;
    ORGANIC_BULB_SPLASHRADIUS, 		//int       splashRadius;
    MOD_ASPAWN,             		//int       meansOfDeath;
    TEAM_ALIENS,            		//int       team;
    ( 1 << WP_ABUILD ),    			//weapon_t  buildWeapon;
    BANIM_IDLE1,           			//int       idleAnim;
    1000,                   			//int       nextthink;
    ORGANIC_BULB_BT,           		//int       buildTime;
    qfalse,                			//qboolean  usable;
    0,                     			//int       turretRange;
    0,                     			//int       turretFireSpeed;
    WP_NONE,               			//weapon_t  turretProjType;
    0.0f,                  			//float     minNormal;
    qtrue,                 			//qboolean  invertNormal;
    qtrue,                 			//qboolean  creepTest;
    ORGANIC_BULB_CREEPSIZE,    		//int       creepSize;
    qfalse,                			//qboolean  dccTest;
    qfalse,                			//qboolean  transparentTest;
    qfalse,                			//qboolean  uniqueTest;
    ORGANIC_BULB_VALUE,             //int       value;
  },
  {
    BA_A_POD,         				//int       buildNum;
    "pod",           				//char      *buildName;
    "[ye]Pod",           			//char      *humanName;
	"HEAVY ORGANIC STRUCTURE\n"
    "[yei]This contains on top of the Pod Stump.\n"
	"It is used to shield the top of bases.\n",
    "team_alien_pod",				//char      *entityName;
    TR_GRAVITY,            			//trType_t  traj;
    0.0,                   			//float     bounce;
    POD_BP,           				//int       buildPoints;
    STAGE_GE_4,           		        //int  stages
    POD_HEALTH,       				//int       health;
    POD_REGEN,        				//int       regenRate;
    POD_SPLASHDAMAGE, 				//int       splashDamage;
    POD_SPLASHRADIUS, 				//int       splashRadius;
    MOD_ASPAWN,             		//int       meansOfDeath;
    TEAM_ALIENS,            		//int       team;
    ( 1 << WP_ABUILD ),    			//weapon_t  buildWeapon;
    BANIM_IDLE1,           			//int       idleAnim;
    1000,                   			//int       nextthink;
    POD_BT,           				//int       buildTime;
    qfalse,                			//qboolean  usable;
    0,                     			//int       turretRange;
    0,                     			//int       turretFireSpeed;
    WP_NONE,               			//weapon_t  turretProjType;
    0.0f,                  			//float     minNormal;
    qtrue,                 			//qboolean  invertNormal;
    qtrue,                 			//qboolean  creepTest;
    POD_CREEPSIZE,    				//int       creepSize;
    qfalse,                			//qboolean  dccTest;
    qfalse,                			//qboolean  transparentTest;
    qfalse,                			//qboolean  uniqueTest;
    POD_VALUE,            			//int       value;
  },
  {
    BA_A_POD_STUMP,         		//int       buildNum;
    "pod_stump",           			//char      *buildName;
    "[ye]Pod Stump",           	//char      *humanName;
	"HEAVY ORGANIC STRUCTURE\n"
    "[yei]This is the stump for Pod.\n",
    "team_alien_pod_stump",			//char      *entityName;
    TR_GRAVITY,            			//trType_t  traj;
    0.0,                   			//float     bounce;
    POD_STUMP_BP,           		//int       buildPoints;
    STAGE_GE_4,           		//int  stages
    POD_STUMP_HEALTH,       		//int       health;
    POD_STUMP_REGEN,        		//int       regenRate;
    POD_STUMP_SPLASHDAMAGE, 		//int       splashDamage;
    POD_STUMP_SPLASHRADIUS, 		//int       splashRadius;
    MOD_ASPAWN,             		//int       meansOfDeath;
    TEAM_ALIENS,            		//int       team;
    ( 1 << WP_ABUILD ),    			//weapon_t  buildWeapon;
    BANIM_IDLE1,           			//int       idleAnim;
    1000,                   			//int       nextthink;
    POD_STUMP_BT,           		//int       buildTime;
    qfalse,                			//qboolean  usable;
    0,                     			//int       turretRange;
    0,                     			//int       turretFireSpeed;
    WP_NONE,               			//weapon_t  turretProjType;
    1.0f,                  			//float     minNormal;
    qtrue,                 			//qboolean  invertNormal;
    qtrue,                 			//qboolean  creepTest;
    POD_STUMP_CREEPSIZE,    		//int       creepSize;
    qfalse,                			//qboolean  dccTest;
    qfalse,                			//qboolean  transparentTest;
    qfalse,                			//qboolean  uniqueTest;
    POD_STUMP_VALUE,            	//int       value;
  },
  {
    BA_A_CREEPCOLONY,      			//int       buildNum;
    "creepcolony",         			//char      *buildName;
    "[ye]Creep colony",        	//char      *humanName;
    "[yei]Gives your team extra buildpoints.",
    "team_alien_creepcolony" ,		//char      *entityName;
    TR_GRAVITY,            			//trType_t  traj;
    0.0,                   			//float     bounce;
    CREEPCOLONY_BP,        			//int       buildPoints;
    STAGE_GE_3, 		                //int  stages
    CREEPCOLONY_HEALTH,    			//int       health;
    CREEPCOLONY_REGEN,     			//int       regenRate;
    CREEPCOLONY_SPLASHDAMAGE, 		//int       splashDamage;
    CREEPCOLONY_SPLASHRADIUS, 		//int       splashRadius;
    MOD_ASPAWN,            			//int       meansOfDeath;
    TEAM_ALIENS,           			//int       team;
    ( 1 << WP_ABUILD ),    			//weapon_t  buildWeapon;
    BANIM_IDLE1,           			//int       idleAnim;
    1000,                   		//int       nextthink;
    CREEPCOLONY_BT,        			//int       buildTime;
    qfalse,                			//qboolean  usable;
    0,                     			//int       turretRange;
    0,                     			//int       turretFireSpeed;
    WP_NONE,               			//weapon_t  turretProjType;
    0.0f,                 			//float     minNormal;
    qtrue,                 			//qboolean  invertNormal;
    qtrue,                 			//qboolean  creepTest;
    CREEPCOLONY_CREEPSIZE, 			//int       creepSize;
    qfalse,                			//qboolean  dccTest;
    qfalse,                			//qboolean  transparentTest;
    qfalse,                			//qboolean  uniqueTest;
    CREEPCOLONY_VALUE,     			//int       value;
  },
  
//HUMAN BUILDINGS
  {
    BA_H_SPAWN,            			//int       buildNum;
    "telenode",            			//char      *buildName;
    "Telenode",            			//char      *humanName;
    "The most basic human structure. It provides a means for humans "
      "to enter the battle arena. Without any of these the humans "
      "cannot spawn and defeat is imminent.",
    "team_human_spawn",    			//char      *entityName;
    TR_GRAVITY,            			//trType_t  traj;
    0.0,                   			//float     bounce;
    HSPAWN_BP,             			//int       buildPoints;
    STAGE_GE_1,                                 //int  stages
    HSPAWN_HEALTH,         			//int       health;
    0,                     			//int       regenRate;
    HSPAWN_SPLASHDAMAGE,   			//int       splashDamage;
    HSPAWN_SPLASHRADIUS,   			//int       splashRadius;
    MOD_HSPAWN,            			//int       meansOfDeath;
    TEAM_HUMANS,            		        //int       team;
    ( 1 << WP_HBUILD ),    			//weapon_t  buildWeapon;
    BANIM_IDLE1,           			//int       idleAnim;
    100,                   			//int       nextthink;
    HSPAWN_BT,             			//int       buildTime;
    qfalse,                			//qboolean  usable;
    0,                     			//int       turretRange;
    0,                     			//int       turretFireSpeed;
    WP_NONE,               			//weapon_t  turretProjType;
    0.0f,                 			//float     minNormal;
    qfalse,                			//qboolean  invertNormal;
    qfalse,                			//qboolean  creepTest;
    0,                     			//int       creepSize;
    qfalse,                			//qboolean  dccTest;
    qtrue,                 			//qboolean  transparentTest;
    qfalse,                			//qboolean  uniqueTest;
    HSPAWN_VALUE,          			//int       value;
  },
  {
    BA_H_MGTURRET,         			//int       buildNum;
    "mgturret",            			//char      *buildName;
    "Machinegun Turret",   			//char      *humanName;
    "Automated base defense that is effective against large targets "
      "but slow to begin firing. Should always be "
      "backed up by physical support.",
    "team_human_mgturret", 			//char      *entityName;
    TR_GRAVITY,            			//trType_t  traj;
    0.0,                   			//float     bounce;
    MGTURRET_BP,           			//int       buildPoints;
    STAGE_GE_1,                                 //int  stages
    MGTURRET_HEALTH,       			//int       health;
    0,                     			//int       regenRate;
    MGTURRET_SPLASHDAMAGE, 			//int       splashDamage;
    MGTURRET_SPLASHRADIUS, 			//int       splashRadius;
    MOD_HSPAWN,            			//int       meansOfDeath;
    TEAM_HUMANS,            		//int       team;
    ( 1 << WP_HBUILD ),   			//weapon_t  buildWeapon;
    BANIM_IDLE1,           			//int       idleAnim;
    50,                    			//int       nextthink;
    MGTURRET_BT,           			//int       buildTime;
    qfalse,                			//qboolean  usable;
    MGTURRET_RANGE,        			//int       turretRange;
    MGTURRET_REPEAT,       			//int       turretFireSpeed;
    WP_MGTURRET,           			//weapon_t  turretProjType;
    0.0f,                 			//float     minNormal; 
    qtrue,                			//qboolean  invertNormal; 
    qfalse,                			//qboolean  creepTest;
    0,                     			//int       creepSize;
    qfalse,                			//qboolean  dccTest;
    qtrue,                 			//qboolean  transparentTest;
    qfalse,                			//qboolean  uniqueTest;
    MGTURRET_VALUE,        			//int       value;
  },
  {
    BA_H_MGTURRET2,         		//int       buildNum;
    "mgturret2",            		//char      *buildName;
    "[ye]Flame Turret",   		//char      *humanName;
    "Automated base defense that is effective against large targets "
      "but slow to begin firing. Should always be "
      "backed up by physical support.",
    "team_human_mgturret2", 		//char      *entityName;
    TR_GRAVITY,            			//trType_t  traj;
    0.0,                   			//float     bounce;
    MGTURRET2_BP,           		//int       buildPoints;
    STAGE_GE_3,                		//int  stages
    MGTURRET2_HEALTH,       		//int       health;
    0,                     			//int       regenRate;
    MGTURRET2_SPLASHDAMAGE, 		//int       splashDamage;
    MGTURRET2_SPLASHRADIUS, 		//int       splashRadius;
    MOD_HSPAWN,            			//int       meansOfDeath;
    TEAM_HUMANS,            		//int       team;
    ( 1 << WP_HBUILD ),   			//weapon_t  buildWeapon;
    BANIM_IDLE1,           			//int       idleAnim;
    50,                    			//int       nextthink;
    MGTURRET2_BT,           		//int       buildTime;
    qfalse,                			//qboolean  usable;
    MGTURRET2_RANGE,        		//int       turretRange;
    MGTURRET2_REPEAT,       		//int       turretFireSpeed;
    WP_MGTURRET2,           		//weapon_t  turretProjType;
    0.0f,                 			//float     minNormal; 
    qtrue,                			//qboolean  invertNormal; 
    qfalse,                			//qboolean  creepTest;
    0,                     			//int       creepSize;
    qfalse,                			//qboolean  dccTest;
    qtrue,                 			//qboolean  transparentTest;
    qfalse,                			//qboolean  uniqueTest;
    MGTURRET_VALUE,        			//int       value;
  },
  {
    BA_H_TESLAGEN,         			//int       buildNum;
    "tesla",               			//char      *buildName;
    "Tesla Generator",     			//char      *humanName;
    "A structure equipped with a strong electrical attack that fires "
      "instantly and always hits its target. It is effective against smaller "
      "aliens and for consolidating basic defense.",
    "team_human_tesla",    			//char      *entityName;
    TR_GRAVITY,            			//trType_t  traj;
    0.0,                   			//float     bounce;
    TESLAGEN_BP,           			//int       buildPoints;
    STAGE_GE_4,           			//int       stages
    TESLAGEN_HEALTH,       			//int       health;
    0,                     			//int       regenRate;
    TESLAGEN_SPLASHDAMAGE, 			//int       splashDamage;
    TESLAGEN_SPLASHRADIUS, 			//int       splashRadius;
    MOD_HSPAWN,            			//int       meansOfDeath;
    TEAM_HUMANS,            		//int       team;
    ( 1 << WP_HBUILD ),    			//weapon_t  buildWeapon;
    BANIM_IDLE1,           			//int       idleAnim;
    150,                   			//int       nextthink;
    TESLAGEN_BT,           			//int       buildTime;
    qfalse,                			//qboolean  usable;
    TESLAGEN_RANGE,        			//int       turretRange;
    TESLAGEN_REPEAT,       			//int       turretFireSpeed;
    WP_TESLAGEN,           			//weapon_t  turretProjType;
    0.95f,                 			//float     minNormal;
    qtrue,                			//qboolean  invertNormal;
    qfalse,                			//qboolean  creepTest;
    0,                     			//int       creepSize;
    qfalse,                			//qboolean  dccTest;
    qtrue,                 			//qboolean  transparentTest;
    qfalse,                			//qboolean  uniqueTest;
    TESLAGEN_VALUE,        			//int       value;
  },
  {
    BA_H_ARMOURY,          			//int       buildNum;
    "arm",                 			//char      *buildName;
    "Armoury",             			//char      *humanName;
    "An essential part of the human base, providing a means "
      "to upgrade the basic human equipment. A range of upgrades "
      "and weapons are available for sale from the armoury.",
    "team_human_armoury",  			//char      *entityName;
    TR_GRAVITY,            			//trType_t  traj;
    0.0,                   			//float     bounce;
    ARMOURY_BP,            			//int       buildPoints;
    STAGE_GE_1,                                 //int  stages
    ARMOURY_HEALTH,        			//int       health;
    0,                     			//int       regenRate;
    ARMOURY_SPLASHDAMAGE,  			//int       splashDamage;
    ARMOURY_SPLASHRADIUS,  			//int       splashRadius;
    MOD_HSPAWN,            			//int       meansOfDeath;
    TEAM_HUMANS,            		//int       team;
    ( 1 << WP_HBUILD ),    			//weapon_t  buildWeapon;
    BANIM_IDLE1,           			//int       idleAnim;
    100,                   			//int       nextthink;
    ARMOURY_BT,            			//int       buildTime;
    qtrue,                 			//qboolean  usable;
    0,                     			//int       turretRange;
    0,                     			//int       turretFireSpeed;
    WP_NONE,               			//weapon_t  turretProjType;
    0.0f,                 			//float     minNormal;
    qtrue,                			//qboolean  invertNormal;
    qfalse,                			//qboolean  creepTest;
    0,                     			//int       creepSize;
    qfalse,                			//qboolean  dccTest;
    qfalse,                			//qboolean  transparentTest;
    qfalse,                			//qboolean  uniqueTest;
    ARMOURY_VALUE,         			//int       value;
  },
  {
    BA_H_DCC,              			//int       buildNum;
    "dcc",                 			//char      *buildName;
    "Defence Computer",    			//char      *humanName;
    "A structure that enables self-repair functionality in "
      "human structures. Each Defence Computer built increases "
      "repair rate slightly.",
    "team_human_dcc",      			//char      *entityName;
    TR_GRAVITY,            			//trType_t  traj;
    0.0,                   			//float     bounce;
    DC_BP,                 			//int       buildPoints;
    STAGE_GE_2,      			        //int       stages
    DC_HEALTH,             			//int       health;
    0,                     			//int       regenRate;
    DC_SPLASHDAMAGE,       			//int       splashDamage;
    DC_SPLASHRADIUS,       			//int       splashRadius;
    MOD_HSPAWN,            			//int       meansOfDeath;
    TEAM_HUMANS,            			//int       team;
    ( 1 << WP_HBUILD ),    			//weapon_t  buildWeapon;
    BANIM_IDLE1,           			//int       idleAnim;
    100,                   			//int       nextthink;
    DC_BT,                 			//int       buildTime;
    qfalse,                			//qboolean  usable;
    0,                     			//int       turretRange;
    0,                     			//int       turretFireSpeed;
    WP_NONE,               			//weapon_t  turretProjType;
    0.0f,                 			//float     minNormal;
    qtrue,                			//qboolean  invertNormal;
    qfalse,                			//qboolean  creepTest;
    0,                     			//int       creepSize;
    qfalse,                			//qboolean  dccTest;
    qfalse,                			//qboolean  transparentTest;
    qfalse,                			//qboolean  uniqueTest;
    DC_VALUE,              			//int       value;
  },
  {
    BA_H_MEDISTAT,         			//int       buildNum;
    "medistat",            			//char      *buildName;
    "Medistation",         			//char      *humanName;
    "A structure that automatically restores "
      "the health and stamina of any human that stands on it. "
      "It may only be used by one person at a time. This structure "
      "also issues medkits.",
    "team_human_medistat", 			//char      *entityName;
    TR_GRAVITY,            			//trType_t  traj;
    0.0,                   			//float     bounce;
    MEDISTAT_BP,           			//int       buildPoints;
    STAGE_GE_1,                                 //int  stages
    MEDISTAT_HEALTH,       			//int       health;
    0,                     			//int       regenRate;
    MEDISTAT_SPLASHDAMAGE, 			//int       splashDamage;
    MEDISTAT_SPLASHRADIUS, 			//int       splashRadius;
    MOD_HSPAWN,            			//int       meansOfDeath;
    TEAM_HUMANS,            		        //int       team;
    ( 1 << WP_HBUILD ),    			//weapon_t  buildWeapon;
    BANIM_IDLE1,           			//int       idleAnim;
    100,                   			//int       nextthink;
    MEDISTAT_BT,           			//int       buildTime;
    qfalse,                			//qboolean  usable;
    0,                     			//int       turretRange;
    0,                     			//int       turretFireSpeed;
    WP_NONE,               			//weapon_t  turretProjType;
    0.0f,                 			//float     minNormal;
    qtrue,                			//qboolean  invertNormal;
    qfalse,                			//qboolean  creepTest;
    0,                     			//int       creepSize;
    qfalse,                			//qboolean  dccTest;
    qtrue,                 			//qboolean  transparentTest;
    qfalse,                			//qboolean  uniqueTest;
    MEDISTAT_VALUE,        			//int       value;
  },
  {
    BA_H_REACTOR,          			//int       buildNum;
    "reactor",             			//char      *buildName;
    "Reactor",             			//char      *humanName;
    "All structures except the telenode rely on a reactor to operate. "
      "The reactor provides power for all the human structures either "
      "directly or via repeaters. Only one reactor can be built at a time.",
    "team_human_reactor",  			//char      *entityName;
    TR_GRAVITY,            			//trType_t  traj;
    0.0,                   			//float     bounce;
    REACTOR_BP,            			//int       buildPoints;
    STAGE_GE_1,                                 //int  stages
    REACTOR_HEALTH,        			//int       health;
    0,                     			//int       regenRate;
    REACTOR_SPLASHDAMAGE,  			//int       splashDamage;
    REACTOR_SPLASHRADIUS,  			//int       splashRadius;
    MOD_HSPAWN,            			//int       meansOfDeath;
    TEAM_HUMANS,            		//int       team;
    ( 1 << WP_HBUILD ),    			//weapon_t  buildWeapon;
    BANIM_IDLE1,           			//int       idleAnim;
    REACTOR_ATTACK_DCC_REPEAT, 		//int       nextthink;
    REACTOR_BT,            			//int       buildTime;
    qtrue,                 			//qboolean  usable;
    0,                     			//int       turretRange;
    0,                     			//int       turretFireSpeed;
    WP_NONE,               			//weapon_t  turretProjType;
    0.0f,                 			//float     minNormal;
    qtrue,                			//qboolean  invertNormal;
    qfalse,                			//qboolean  creepTest;
    0,                     			//int       creepSize;
    qfalse,                			//qboolean  dccTest;
    qfalse,                			//qboolean  transparentTest;
    qtrue,                 			//qboolean  uniqueTest;
    REACTOR_VALUE,         			//int       value;
  },
  {
    BA_H_REPEATER,         			//int       buildNum;
    "repeater",            			//char      *buildName;
    "Repeater",            			//char      *humanName;
    "A power distributor that transmits power from the reactor "
      "to remote locations, so that bases may be built far "
      "from the reactor.",
    "team_human_repeater", 			//char      *entityName;
    TR_GRAVITY,            			//trType_t  traj;
    0.0,                   			//float     bounce;
    REPEATER_BP,           			//int       buildPoints;
    STAGE_GE_2,                                 //int  stages
    REPEATER_HEALTH,       			//int       health;
    0,                     			//int       regenRate;
    REPEATER_SPLASHDAMAGE, 			//int       splashDamage;
    REPEATER_SPLASHRADIUS, 			//int       splashRadius;
    MOD_HSPAWN,            			//int       meansOfDeath;
    TEAM_HUMANS,            		        //int       team;
    ( 1 << WP_HBUILD ),    			//weapon_t  buildWeapon;
    BANIM_IDLE1,           			//int       idleAnim;
    100,                   			//int       nextthink;
    REPEATER_BT,           			//int       buildTime;
    qtrue,                 			//qboolean  usable;
    0,                     			//int       turretRange;
    0,                     			//int       turretFireSpeed;
    WP_NONE,               			//weapon_t  turretProjType;
    0.0f,                 			//float     minNormal; 
    qtrue,                			//qboolean  invertNormal; 
    qfalse,                			//qboolean  creepTest;
    0,                     			//int       creepSize;
    qfalse,                			//qboolean  dccTest;
    qfalse,                			//qboolean  transparentTest;
    qfalse,                			//qboolean  uniqueTest;
    REPEATER_VALUE,        			//int       value;
  },
  {
    BA_H_CONTAINER_SMALL,         	//int       buildNum;
    "container_small",            	//char      *buildName;
    "[ye]Container Small",   		//char      *humanName;
	"SOLID STRUCTURE\n"
    "[yei]Used for passive base defense.\n"
    "This building doesn't need a powered area!\n",
    "team_human_container_small", 	//char      *entityName;
    TR_GRAVITY,            			//trType_t  traj;
    0.0,                   			//float     bounce;
    CONTAINER_SMALL_BP,           	//int       buildPoints;
    STAGE_GE_1,                         //int  stages
    CONTAINER_SMALL_HEALTH,       	//int       health;
    0,                     			//int       regenRate;
    CONTAINER_SMALL_SPLASHDAMAGE, 	//int       splashDamage;
    CONTAINER_SMALL_SPLASHRADIUS, 	//int       splashRadius;
    MOD_HSPAWN,            			//int       meansOfDeath;
    TEAM_HUMANS,            		//int       team;
    ( 1 << WP_HBUILD ),   			//weapon_t  buildWeapon;
    BANIM_IDLE1,           			//int       idleAnim;
    0,                    			//int       nextthink;
    CONTAINER_SMALL_BT,             //int       buildTime;
    qfalse,                			//qboolean  usable;
    0,        						//int       turretRange;
    0,       						//int       turretFireSpeed;
    WP_NONE,           				//weapon_t  turretProjType;
    0.0f,                 			//float     minNormal; 
    qtrue,                			//qboolean  invertNormal; 
    qfalse,                			//qboolean  creepTest;
    0,                     			//int       creepSize;
    qfalse,                			//qboolean  dccTest;
    qfalse,                 		//qboolean  transparentTest;
    qfalse,                			//qboolean  uniqueTest;
    CONTAINER_SMALL_VALUE,        	//int       value;
  }, 
  {
    BA_H_CONTAINER_MEDIUM,         	//int       buildNum;
    "container_medium",            	//char      *buildName;
    "[ye]Container Medium",   	//char      *humanName;
	"SOLID STRUCTURE\n"
    "[yei]Used for passive base defense.\n"
    "This building doesn't need a powered area!\n",
    "team_human_container_medium", //char      *entityName;
    TR_GRAVITY,            			//trType_t  traj;
    0.0,                   			//float     bounce;
    CONTAINER_MEDIUM_BP,            //int       buildPoints;
    STAGE_GE_3, 		    //int  stages
    CONTAINER_MEDIUM_HEALTH,        //int       health;
    0,                     			//int       regenRate;
    CONTAINER_MEDIUM_SPLASHDAMAGE,  //int       splashDamage;
    CONTAINER_MEDIUM_SPLASHRADIUS,  //int       splashRadius;
    MOD_HSPAWN,            			//int       meansOfDeath;
    TEAM_HUMANS,            		//int       team;
    ( 1 << WP_HBUILD ),   			//weapon_t  buildWeapon;
    BANIM_IDLE1,           			//int       idleAnim;
    0,                    			//int       nextthink;
    CONTAINER_MEDIUM_BT,            //int       buildTime;
    qfalse,                			//qboolean  usable;
    0,        						//int       turretRange;
    0,       						//int       turretFireSpeed;
    WP_NONE,           				//weapon_t  turretProjType;
    0.0f,                 			//float     minNormal; 
    qtrue,                			//qboolean  invertNormal; 
    qfalse,                			//qboolean  creepTest;
    0,                     			//int       creepSize;
    qfalse,                			//qboolean  dccTest;
    qfalse,                 		//qboolean  transparentTest;
    qfalse,                			//qboolean  uniqueTest;
    CONTAINER_MEDIUM_VALUE,         //int       value;
  }, 
  {
    BA_H_CONTAINER_LARGE,         	//int       buildNum;
    "container_large",            	//char      *buildName;
    "[ye]Container Large",   		//char      *humanName;
	"SOLID STRUCTURE\n"
    "[yei]Used for passive base defense.\n"
    "This building doesn't need a powered area!\n",
    "team_human_container_large", 	//char      *entityName;
    TR_GRAVITY,            			//trType_t  traj;
    0.0,                   			//float     bounce;
    CONTAINER_LARGE_BP,           	//int       buildPoints;
    STAGE_GE_4, 					//int  stages
    CONTAINER_LARGE_HEALTH,       	//int       health;
    0,                    		 	//int       regenRate;
    CONTAINER_LARGE_SPLASHDAMAGE, 	//int       splashDamage;
    CONTAINER_LARGE_SPLASHRADIUS, 	//int       splashRadius;
    MOD_HSPAWN,            			//int       meansOfDeath;
    TEAM_HUMANS,            		//int       team;
    ( 1 << WP_HBUILD ),   			//weapon_t  buildWeapon;
    BANIM_IDLE1,           			//int       idleAnim;
    0,                    			//int       nextthink;
    CONTAINER_LARGE_BT,           	//int       buildTime;
    qfalse,                			//qboolean  usable;
    0,        						//int       turretRange;
    0,       						//int       turretFireSpeed;
    WP_NONE,           				//weapon_t  turretProjType;
    0.0f,                 			//float     minNormal; 
    qtrue,                			//qboolean  invertNormal; 
    qfalse,                			//qboolean  creepTest;
    0,                     			//int       creepSize;
    qfalse,                			//qboolean  dccTest;
    qfalse,                 		//qboolean  transparentTest;
    qfalse,                			//qboolean  uniqueTest;
    CONTAINER_LARGE_VALUE,        	//int       value;
  }, 
  {
    BA_H_PLATE_SMALL,         		//int       buildNum;
    "plate_small",            		//char      *buildName;
    "[ye]Plate Small",   			//char      *humanName;
	"SOLID STRUCTURE\n"
    "[yei]Used for passive base defense or bridges.\n"
    "This building doesn't need a powered area!\n",
    "team_human_plate_small", 		//char      *entityName;
    TR_GRAVITY,            			//trType_t  traj;
    0.0,                   			//float     bounce;
    PLATE_SMALL_BP,           		//int       buildPoints;
    STAGE_GE_1,                         //int  stages
    PLATE_SMALL_HEALTH,       		//int       health;
    0,                     			//int       regenRate;
    PLATE_SMALL_SPLASHDAMAGE, 		//int       splashDamage;
    PLATE_SMALL_SPLASHRADIUS, 		//int       splashRadius;
    MOD_HSPAWN,            			//int       meansOfDeath;
    TEAM_HUMANS,            		//int       team;
    ( 1 << WP_HBUILD ),   			//weapon_t  buildWeapon;
    BANIM_IDLE1,           			//int       idleAnim;
    0,                    			//int       nextthink;
    PLATE_SMALL_BT,           		//int       buildTime;
    qfalse,                			//qboolean  usable;
    0,        						//int       turretRange;
    0,       						//int       turretFireSpeed;
    WP_NONE,           				//weapon_t  turretProjType;
    0.0f,                 			//float     minNormal; 
    qtrue,                			//qboolean  invertNormal; 
    qfalse,                			//qboolean  creepTest;
    0,                     			//int       creepSize;
    qfalse,                			//qboolean  dccTest;
    qfalse,                 		//qboolean  transparentTest;
    qfalse,                			//qboolean  uniqueTest;
    PLATE_SMALL_VALUE,        		//int       value;
  }, 
  {
    BA_H_PLATE_LARGE,         		//int       buildNum;
    "plate_large",            		//char      *buildName;
    "[ye]Plate Large",   			//char      *humanName;
	"SOLID STRUCTURE\n"
    "[yei]Used for passive base defense or bridges.\n"
    "This building doesn't need a powered area!\n",
    "team_human_plate_large", 		//char      *entityName;
    TR_GRAVITY,            			//trType_t  traj;
    0.0,                   			//float     bounce;
    PLATE_LARGE_BP,           		//int       buildPoints;
    STAGE_GE_3, 					//int  stages
    PLATE_LARGE_HEALTH,       		//int       health;
    0,                     			//int       regenRate;
    PLATE_LARGE_SPLASHDAMAGE, 		//int       splashDamage;
    PLATE_LARGE_SPLASHRADIUS, 		//int       splashRadius;
    MOD_HSPAWN,            			//int       meansOfDeath;
    TEAM_HUMANS,            		//int       team;
    ( 1 << WP_HBUILD ),   			//weapon_t  buildWeapon;
    BANIM_IDLE1,           			//int       idleAnim;
    0,                    			//int       nextthink;
    PLATE_LARGE_BT,           		//int       buildTime;
    qfalse,                			//qboolean  usable;
    0,        						//int       turretRange;
    0,       						//int       turretFireSpeed;
    WP_NONE,           				//weapon_t  turretProjType;
    0.0f,                 			//float     minNormal; 
    qtrue,                			//qboolean  invertNormal; 
    qfalse,                			//qboolean  creepTest;
    0,                     			//int       creepSize;
    qfalse,                			//qboolean  dccTest;
    qfalse,                 		//qboolean  transparentTest;
    qfalse,                			//qboolean  uniqueTest;
    PLATE_LARGE_VALUE,        		//int       value;
  }, 
  {
    BA_H_FENCE,         			//int       buildNum;
    "fence",            			//char      *buildName;
    "[ye]Fence",   				//char      *humanName;
	"SOLID STRUCTURE\n"
    "[yei]Used for passive base defense.\n"
    "This building doesn't need a powered area!\n",
    "team_human_fence", 			//char      *entityName;
    TR_GRAVITY,            			//trType_t  traj;
    0.0,                   			//float     bounce;
    FENCE_BP,           			//int       buildPoints;
    STAGE_GE_1,	                                //int  stages
    FENCE_HEALTH,       			//int       health;
    0,                     			//int       regenRate;
    FENCE_SPLASHDAMAGE, 			//int       splashDamage;
    FENCE_SPLASHRADIUS, 			//int       splashRadius;
    MOD_HSPAWN,            			//int       meansOfDeath;
    TEAM_HUMANS,            		//int       team;
    ( 1 << WP_HBUILD ),   			//weapon_t  buildWeapon;
    BANIM_IDLE1,           			//int       idleAnim;
    0,                    			//int       nextthink;
    FENCE_BT,           			//int       buildTime;
    qfalse,                			//qboolean  usable;
    0,        						//int       turretRange;
    0,       						//int       turretFireSpeed;
    WP_NONE,           				//weapon_t  turretProjType;
    0.0f,                 			//float     minNormal; 
    qtrue,                			//qboolean  invertNormal; 
    qfalse,                			//qboolean  creepTest;
    0,                     			//int       creepSize;
    qfalse,                			//qboolean  dccTest;
    qfalse,                 		//qboolean  transparentTest;
    qfalse,                			//qboolean  uniqueTest;
    FENCE_VALUE,        			//int       value;
  }, 
  {
    BA_H_FENCE_ROD,         		//int       buildNum;
    "fence_rod",            		//char      *buildName;
    "[ye]Fence Rod",   			//char      *humanName;
	"SOLID STRUCTURE\n"
    "[yei]Used for passive base defense.\n"
    "This building doesn't need a powered area!\n",
    "team_human_fence_rod", 		//char      *entityName;
    TR_GRAVITY,            			//trType_t  traj;
    0.0,                   			//float     bounce;
    FENCE_ROD_BP,           		//int       buildPoints;
    STAGE_GE_1, 		                //int  stages
    FENCE_HEALTH,       			//int       health;
    0,                     			//int       regenRate;
    FENCE_ROD_SPLASHDAMAGE, 		//int       splashDamage;
    FENCE_ROD_SPLASHRADIUS, 		//int       splashRadius;
    MOD_HSPAWN,            			//int       meansOfDeath;
    TEAM_HUMANS,            		//int       team;
    ( 1 << WP_HBUILD ),   			//weapon_t  buildWeapon;
    BANIM_IDLE1,           			//int       idleAnim;
    0,                    			//int       nextthink;
    FENCE_ROD_BT,          	 		//int       buildTime;
    qfalse,                			//qboolean  usable;
    0,        						//int       turretRange;
    0,       						//int       turretFireSpeed;
    WP_NONE,           				//weapon_t  turretProjType;
    0.0f,                 			//float     minNormal; 
    qtrue,                			//qboolean  invertNormal; 
    qfalse,                			//qboolean  creepTest;
    0,                     			//int       creepSize;
    qfalse,                			//qboolean  dccTest;
    qfalse,                 		//qboolean  transparentTest;
    qfalse,                			//qboolean  uniqueTest;
    FENCE_ROD_VALUE,        		//int       value;
  }, 
  {
    BA_H_BARRIER_LINE,         		//int       buildNum;
    "barrier_line",            		//char      *buildName;
    "[ye]Barrier Line",   		//char      *humanName;
	"SOLID STRUCTURE\n"
    "[yei]Used for passive base defense.\n"
    "This building doesn't need a powered area!\n",
    "team_human_barrier_line", 		//char      *entityName;
    TR_GRAVITY,            			//trType_t  traj;
    0.0,                   			//float     bounce;
    BARRIER_LINE_BP,           		//int       buildPoints;
    STAGE_GE_2,                         //int  stages
    BARRIER_LINE_HEALTH,       		//int       health;
    0,                     			//int       regenRate;
    BARRIER_LINE_SPLASHDAMAGE, 		//int       splashDamage;
    BARRIER_LINE_SPLASHRADIUS, 		//int       splashRadius;
    MOD_HSPAWN,            			//int       meansOfDeath;
    TEAM_HUMANS,            		//int       team;
    ( 1 << WP_HBUILD ),   			//weapon_t  buildWeapon;
    BANIM_IDLE1,           			//int       idleAnim;
    0,                    			//int       nextthink;
    BARRIER_LINE_BT,           		//int       buildTime;
    qfalse,                			//qboolean  usable;
    0,        						//int       turretRange;
    0,       						//int       turretFireSpeed;
    WP_NONE,           				//weapon_t  turretProjType;
    0.0f,                 			//float     minNormal; 
    qtrue,                			//qboolean  invertNormal; 
    qfalse,                			//qboolean  creepTest;
    0,                     			//int       creepSize;
    qfalse,                			//qboolean  dccTest;
    qfalse,                 		//qboolean  transparentTest;
    qfalse,                			//qboolean  uniqueTest;
    BARRIER_LINE_VALUE,        		//int       value;
  }, 
  {
    BA_H_BARRIER_CORNER,         	//int       buildNum;
    "barrier_corner",            	//char      *buildName;
    "[ye]Barrier Corner",   		//char      *humanName;
	"SOLID STRUCTURE\n"
    "[yei]Used for passive base defense.\n"
    "This building doesn't need a powered area!\n",
    "team_human_barrier_corner", 	//char      *entityName;
    TR_GRAVITY,            			//trType_t  traj;
    0.0,                   			//float     bounce;
    BARRIER_CORNER_BP,           	//int       buildPoints;
    STAGE_GE_2,               		//int  stages
    BARRIER_CORNER_HEALTH,       	//int       health;
    0,                     			//int       regenRate;
    BARRIER_CORNER_SPLASHDAMAGE, 	//int       splashDamage;
    BARRIER_CORNER_SPLASHRADIUS, 	//int       splashRadius;
    MOD_HSPAWN,            			//int       meansOfDeath;
    TEAM_HUMANS,            		//int       team;
    ( 1 << WP_HBUILD ),   			//weapon_t  buildWeapon;
    BANIM_IDLE1,           			//int       idleAnim;
    0,                    			//int       nextthink;
    BARRIER_CORNER_BT,           	//int       buildTime;
    qfalse,                			//qboolean  usable;
    0,        						//int       turretRange;
    0,       						//int       turretFireSpeed;
    WP_NONE,           				//weapon_t  turretProjType;
    0.0f,                 			//float     minNormal; 
    qtrue,                			//qboolean  invertNormal; 
    qfalse,                			//qboolean  creepTest;
    0,                     			//int       creepSize;
    qfalse,                			//qboolean  dccTest;
    qfalse,                 		//qboolean  transparentTest;
    qfalse,                			//qboolean  uniqueTest;
    BARRIER_CORNER_VALUE,        	//int       value;
  }, 
  {
    BA_H_BARRIER_POINT,         	//int       buildNum;
    "barrier_point",            	//char      *buildName;
    "[ye]Barrier Point",   		//char      *humanName;
	"SOLID STRUCTURE\n"
    "[yei]Used for passive base defense.\n"
    "This building doesn't need a powered area!\n",
    "team_human_barrier_point", 	//char      *entityName;
    TR_GRAVITY,            			//trType_t  traj;
    0.0,                   			//float     bounce;
    BARRIER_POINT_BP,           	//int       buildPoints;
    STAGE_GE_3, 		        //int  stages
    BARRIER_POINT_HEALTH,       	//int       health;
    0,                     			//int       regenRate;
    BARRIER_POINT_SPLASHDAMAGE, 	//int       splashDamage;
    BARRIER_POINT_SPLASHRADIUS, 	//int       splashRadius;
    MOD_HSPAWN,            			//int       meansOfDeath;
    TEAM_HUMANS,            		//int       team;
    ( 1 << WP_HBUILD ),   			//weapon_t  buildWeapon;
    BANIM_IDLE1,           			//int       idleAnim;
    0,                    			//int       nextthink;
    BARRIER_POINT_BT,           	//int       buildTime;
    qfalse,                			//qboolean  usable;
    0,        						//int       turretRange;
    0,       						//int       turretFireSpeed;
    WP_NONE,           				//weapon_t  turretProjType;
    0.0f,                 			//float     minNormal; 
    qtrue,                			//qboolean  invertNormal; 
    qfalse,                			//qboolean  creepTest;
    0,                     			//int       creepSize;
    qfalse,                			//qboolean  dccTest;
    qfalse,                 		//qboolean  transparentTest;
    qfalse,                			//qboolean  uniqueTest;
    BARRIER_POINT_VALUE,        	//int       value;
  }, 
  {
    BA_H_SHIELD,         			//int       buildNum;
    "shield",            			//char      *buildName;
    "[ye]Shield",   				//char      *humanName;
	"INTELLIGENT STRUCTURE\n"
    "[yei]Energy-Shield / Floodgate."
	"Can only be used by humans.",
    "team_human_shield", 			//char      *entityName;
    TR_GRAVITY,            			//trType_t  traj;
    0.0,                   			//float     bounce;
    SHIELD_BP,           			//int       buildPoints;
    STAGE_GE_5,				        //int  stages
    SHIELD_HEALTH,       			//int       health;
    0,                     			//int       regenRate;
    SHIELD_SPLASHDAMAGE, 			//int       splashDamage;
    SHIELD_SPLASHRADIUS, 			//int       splashRadius;
    MOD_HSPAWN,            			//int       meansOfDeath;
    TEAM_HUMANS,            		//int       team;
    ( 1 << WP_HBUILD ),   			//weapon_t  buildWeapon;
    BANIM_IDLE1,           			//int       idleAnim;
    100,                   			//int       nextthink;
    SHIELD_BT,           			//int       buildTime;
    qfalse,                			//qboolean  usable;
    0,        						//int       turretRange;
    0,       						//int       turretFireSpeed;
    WP_NONE,           				//weapon_t  turretProjType;
    0,                 				//float     minNormal;
    qtrue,                			//qboolean  invertNormal;
    qfalse,                			//qboolean  creepTest;
    0,                     			//int       creepSize;
    qfalse,                			//qboolean  dccTest;
    qfalse,                 		//qboolean  transparentTest;
    qfalse,                			//qboolean  uniqueTest;
    SHIELD_VALUE,        			//int       value;
  }, 
  {
    BA_H_LADDER,         			//int       buildNum;
    "ladder",            			//char      *buildName;
    "[ye]Ammo Supply",   			//char      *humanName;
	"INTELLIGENT STRUCTURE\n"
    "[yei]A tiny ammo box for non-energy weapons.\n"
    "This building doesn't need a powered area!\n",
    "team_human_ladder", 			//char      *entityName;
    TR_GRAVITY,            			//trType_t  traj;
    0,                   			//float     bounce;
    LADDER_BP,           			//int       buildPoints;
    STAGE_GE_3, 		                //int  stages
    LADDER_HEALTH,       			//int       health;
    0,                     			//int       regenRate;
    LADDER_SPLASHDAMAGE, 			//int       splashDamage;
    LADDER_SPLASHRADIUS, 			//int       splashRadius;
    MOD_HSPAWN,            			//int       meansOfDeath;
    TEAM_HUMANS,            		//int       team;
    ( 1 << WP_HBUILD ),   			//weapon_t  buildWeapon;
    BANIM_IDLE1,           			//int       idleAnim;
    0,                    			//int       nextthink;
    LADDER_BT,           			//int       buildTime;
    qtrue,                 			//qboolean  usable;
    0,        						//int       turretRange;
    0,       						//int       turretFireSpeed;
    WP_NONE,           				//weapon_t  turretProjType;
    0,                 				//float     minNormal;
    qtrue,                			//qboolean  invertNormal;
    qfalse,                			//qboolean  creepTest;
    0,                     			//int       creepSize;
    qfalse,                			//qboolean  dccTest;
    qfalse,                 		//qboolean  transparentTest;
    qfalse,                			//qboolean  uniqueTest;
    LADDER_VALUE,        			//int       value;
  },
  {
    BA_H_TEFLON_FOIL,         		//int       buildNum;
    "teflon_foil",           	 	//char      *buildName;
    "[ye]Teflon Foil",   			//char      *humanName;
	"SEMI-SOLID STRUCTURE\n"
 	"[yei]Can be used for camouflage.\n"
    "This building doesn't need a powered area!\n",
    "team_human_teflon_foil", 		//char      *entityName;
    TR_GRAVITY,            			//trType_t  traj;
    0.0,                   			//float     bounce;
    TEFLON_FOIL_BP,           		//int       buildPoints;
    STAGE_GE_1,                         //int  stages
    TEFLON_FOIL_HEALTH,       		//int       health;
    0,                     			//int       regenRate;
    TEFLON_FOIL_SPLASHDAMAGE, 		//int       splashDamage;
    TEFLON_FOIL_SPLASHRADIUS, 		//int       splashRadius;
    MOD_HSPAWN,            			//int       meansOfDeath;
    TEAM_HUMANS,            		//int       team;
    ( 1 << WP_HBUILD ),   			//weapon_t  buildWeapon;
    BANIM_IDLE1,           			//int       idleAnim;
    0,                    			//int       nextthink;
    TEFLON_FOIL_BT,           		//int       buildTime;
    qfalse,                			//qboolean  usable;
    0,        						//int       turretRange;
    0,       						//int       turretFireSpeed;
    WP_NONE,           				//weapon_t  turretProjType;
    0.0f,                		 	//float     minNormal; 
    qtrue,                			//qboolean  invertNormal; 
    qfalse,                			//qboolean  creepTest;
    0,                     			//int       creepSize;
    qfalse,                			//qboolean  dccTest;
    qfalse,                 		//qboolean  transparentTest;
    qfalse,                			//qboolean  uniqueTest;
    TEFLON_FOIL_VALUE,        		//int       value;
  },
  {
    BA_H_BARREL,         			//int       buildNum;
    "barrel",            			//char      *buildName;
    "[ye]Barrel",   				//char      *humanName;
	"SOLID STRUCTURE\n"
    "[yei]Used to build bases.\n"
    "This building doesn't need a powered area!\n",
    "team_human_barrel", 			//char      *entityName;
    TR_GRAVITY,            			//trType_t  traj;
    0.0,                   			//float     bounce;
    BARREL_BP,           			//int       buildPoints;
    STAGE_GE_2,                                 //int  stages
    BARREL_HEALTH,       			//int       health;
    0,                     			//int       regenRate;
    BARREL_SPLASHDAMAGE, 			//int       splashDamage;
    BARREL_SPLASHRADIUS, 			//int       splashRadius;
    MOD_HSPAWN,            			//int       meansOfDeath;
    TEAM_HUMANS,            		//int       team;
    ( 1 << WP_HBUILD ),   			//weapon_t  buildWeapon;
    BANIM_IDLE1,           			//int       idleAnim;
    0,                    			//int       nextthink;
    BARREL_BT,           			//int       buildTime;
    qfalse,                			//qboolean  usable;
    0,        						//int       turretRange;
    0,       						//int       turretFireSpeed;
    WP_NONE,           				//weapon_t  turretProjType;
    0.0f,                 			//float     minNormal; 
    qtrue,                			//qboolean  invertNormal; 
    qfalse,                			//qboolean  creepTest;
    0,                     			//int       creepSize;
    qfalse,                			//qboolean  dccTest;
    qfalse,                 		//qboolean  transparentTest;
    qfalse,                			//qboolean  uniqueTest;
    BARREL_VALUE,        			//int       value;
  }, 
  {
    BA_H_LIGHT,         			//int       buildNum;
    "light",            			//char      *buildName;
    "[ye]ForceField",   			//char      *humanName;
	"INTELLIGENT SOLID STRUCTURE\n"
    "[yei]Creates a forcefield wich.\n"
    "pushes alien boddies back.\n",
    "team_human_light", 			//char      *entityName;
    TR_GRAVITY,            			//trType_t  traj;
    0.0,                   			//float     bounce;
    LIGHT_BP,           			//int       buildPoints;
    STAGE_GE_5, 				//int  stages
    LIGHT_HEALTH,       			//int       health;
    0,                     			//int       regenRate;
    LIGHT_SPLASHDAMAGE, 			//int       splashDamage;
    LIGHT_SPLASHRADIUS, 			//int       splashRadius;
    MOD_HSPAWN,            			//int       meansOfDeath;
    TEAM_HUMANS,            		//int       team;
    ( 1 << WP_HBUILD ),   			//weapon_t  buildWeapon;
    BANIM_IDLE1,           			//int       idleAnim;
    400,                   			//int       nextthink;
    LIGHT_BT,           			//int       buildTime;
    qfalse,                			//qboolean  usable;
    0,        						//int       turretRange;
    0,       						//int       turretFireSpeed;
    WP_NONE,           				//weapon_t  turretProjType;
    0.0f,                 			//float     minNormal; 
    qtrue,                			//qboolean  invertNormal; 
    qfalse,                			//qboolean  creepTest;
    0,                     			//int       creepSize;
    qfalse,                			//qboolean  dccTest;
    qfalse,                 		//qboolean  transparentTest;
    qfalse,                			//qboolean  uniqueTest;
    LIGHT_VALUE,        			//int       value;
  }, 
  {
    BA_H_COVER,         			//int       buildNum;
    "cover",            			//char      *buildName;
    "[ye]Cover",   				//char      *humanName;
	"HEAVY SOLID STRUCTURE\n"
    "[yei]This contains on top of the Cover Stump.\n"
	"It is used to shield the top of bases.\n"
    "This building doesn't need a powered area!\n",
    "team_human_cover", 			//char      *entityName;
    TR_GRAVITY,            			//trType_t  traj;
    0.0,                   			//float     bounce;
    COVER_BP,           			//int       buildPoints;
    STAGE_GE_4, 					//int  stages
    COVER_HEALTH,       			//int       health;
    0,                     			//int       regenRate;
    COVER_SPLASHDAMAGE, 			//int       splashDamage;
    COVER_SPLASHRADIUS, 			//int       splashRadius;
    MOD_HSPAWN,            			//int       meansOfDeath;
    TEAM_HUMANS,            		//int       team;
    ( 1 << WP_HBUILD ),   			//weapon_t  buildWeapon;
    BANIM_IDLE1,           			//int       idleAnim;
    0,                    			//int       nextthink;
    COVER_BT,           			//int       buildTime;
    qfalse,                			//qboolean  usable;
    0,        						//int       turretRange;
    0,       						//int       turretFireSpeed;
    WP_NONE,           				//weapon_t  turretProjType;
    0.0f,                 			//float     minNormal; 
    qtrue,                			//qboolean  invertNormal; 
    qfalse,                			//qboolean  creepTest;
    0,                     			//int       creepSize;
    qfalse,                			//qboolean  dccTest;
    qfalse,                 		//qboolean  transparentTest;
    qfalse,                			//qboolean  uniqueTest;
    COVER_VALUE,        			//int       value;
  }, 
  {
    BA_H_COVER_STUMP,         		//int       buildNum;
    "cover_stump",            		//char      *buildName;
    "[ye]Cover Stump",   			//char      *humanName;
	"HEAVY SOLID STRUCTURE\n"
    "[yei]This is the stump for the Cover.\n"
    "This building doesn't need a powered area!\n",
    "team_human_cover_stump", 		//char      *entityName;
    TR_GRAVITY,            			//trType_t  traj;
    0.0,                   			//float     bounce;
    COVER_STUMP_BP,           		//int       buildPoints;
    STAGE_GE_4, 					//int  stages
    COVER_STUMP_HEALTH,       		//int       health;
    0,                     			//int       regenRate;
    COVER_STUMP_SPLASHDAMAGE, 		//int       splashDamage;
    COVER_STUMP_SPLASHRADIUS, 		//int       splashRadius;
    MOD_HSPAWN,            			//int       meansOfDeath;
    TEAM_HUMANS,            		//int       team;
    ( 1 << WP_HBUILD ),   			//weapon_t  buildWeapon;
    BANIM_IDLE1,           			//int       idleAnim;
    0,                    			//int       nextthink;
    COVER_STUMP_BT,           		//int       buildTime;
    qfalse,                			//qboolean  usable;
    0,        						//int       turretRange;
    0,       						//int       turretFireSpeed;
    WP_NONE,           				//weapon_t  turretProjType;
    0.0f,                 			//float     minNormal; 
    qtrue,                			//qboolean  invertNormal; 
    qfalse,                			//qboolean  creepTest;
    0,                     			//int       creepSize;
    qfalse,                			//qboolean  dccTest;
    qfalse,                 		//qboolean  transparentTest;
    qfalse,                			//qboolean  uniqueTest;
    COVER_STUMP_VALUE,        		//int       value;
  }, 
  {
    BA_H_REFINERY,         			//int       buildNum;
    "refinery",            			//char      *buildName;
    "[ye]Refinery",            	//char      *humanName;
	"SUPPLY STRUCTURE\n"
    "[yei]Gives your team 75 buildpoints, ",
    "team_human_refinery", 			//char      *entityName;
    TR_GRAVITY,            			//trType_t  traj;
    0.0,                   			//float     bounce;
    REFINERY_BP,           			//int       buildPoints;
    STAGE_GE_3, 		                //int  stages
    REFINERY_HEALTH,       			//int       health;
    0,                     			//int       regenRate;
    REFINERY_SPLASHDAMAGE, 			//int       splashDamage;
    REFINERY_SPLASHRADIUS, 			//int       splashRadius;
    MOD_HSPAWN,            			//int       meansOfDeath;
    TEAM_HUMANS,            		//int       team;
    ( 1 << WP_HBUILD ),    			//weapon_t  buildWeapon;
    BANIM_IDLE1,           			//int       idleAnim;
    1000,                  			//int       nextthink;
    REFINERY_BT,           			//int       buildTime;
    qfalse,                 		//qboolean  usable;
    0,                     			//int       turretRange;
    0,                     			//int       turretFireSpeed;
    WP_NONE,               			//weapon_t  turretProjType;
    0.0f,                 			//float     minNormal;
    qtrue,                			//qboolean  invertNormal;
    qfalse,                			//qboolean  creepTest;
    0,                     			//int       creepSize;
    qfalse,                			//qboolean  dccTest;
    qfalse,                			//qboolean  transparentTest;
    qfalse,                			//qboolean  uniqueTest;
    REFINERY_VALUE,        			//int       value;
   },
   };

int   bg_numBuildables = sizeof( bg_buildableList ) / sizeof( bg_buildableList[ 0 ] );
static const buildableAttributes_t nullBuildable = { 0 };
/*
==============
BG_BuildableByName
==============
*/
const buildableAttributes_t *BG_BuildableByName( const char *name )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( !Q_stricmp( bg_buildableList[ i ].name, name ) )
      return &bg_buildableList[ i ];
  }
  return &nullBuildable;
}

/*
==============
BG_BuildableByEntityName
==============
*/
const buildableAttributes_t *BG_BuildableByEntityName( const char *name )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( !Q_stricmp( bg_buildableList[ i ].entityName, name ) )
      return &bg_buildableList[ i ];
  }
  return &nullBuildable;
}

/*
==============
BG_Buildable
==============
*/
const buildableAttributes_t *BG_Buildable( buildable_t buildable )
{
  return ( buildable > BA_NONE && buildable < BA_NUM_BUILDABLES ) ?
    &bg_buildableList[ buildable - 1 ] : &nullBuildable;
}

/*
==============
BG_BuildableAllowedInStage
==============
*/
qboolean BG_BuildableAllowedInStage( buildable_t buildable,
                                     stage_t stage )
{
  int stages = BG_Buildable( buildable )->stages;

  if( stages & ( 1 << stage ) )
    return qtrue;
  else
    return qfalse;
}

static buildableConfig_t bg_buildableConfigList[ BA_NUM_BUILDABLES ];

/*
==============
BG_BuildableConfig
==============
*/
buildableConfig_t *BG_BuildableConfig( buildable_t buildable )
{
  return &bg_buildableConfigList[ buildable ];
}

/*
==============
BG_BuildableBoundingBox
==============
*/
void BG_BuildableBoundingBox( buildable_t buildable,
                              vec3_t mins, vec3_t maxs )
{
  buildableConfig_t *buildableConfig = BG_BuildableConfig( buildable );

  if( mins != NULL )
    VectorCopy( buildableConfig->mins, mins );
  if( maxs != NULL )
    VectorCopy( buildableConfig->maxs, maxs );
}

/*
======================
BG_ParseBuildableFile

Parses a configuration file describing a buildable
======================
*/
static qboolean BG_ParseBuildableFile( const char *filename, buildableConfig_t *bc )
{
  char          *text_p;
  int           i;
  int           len;
  char          *token;
  char          text[ 20000 ];
  fileHandle_t  f;
  float         scale;
  int           defined = 0;
  enum
  {
      MODEL         = 1 << 0,
      MODELSCALE    = 1 << 1,
      MINS          = 1 << 2,
      MAXS          = 1 << 3,
      ZOFFSET       = 1 << 4
  };

  // load the file
  len = trap_FS_FOpenFile( filename, &f, FS_READ );
  if( len < 0 )
  {
    Com_Printf( S_COLOR_RED "ERROR: Buildable file %s doesn't exist\n", filename );
    return qfalse;
  }

  if( len == 0 || len >= sizeof( text ) - 1 )
  {
    trap_FS_FCloseFile( f );
    Com_Printf( S_COLOR_RED "ERROR: Buildable file %s is %s\n", filename,
      len == 0 ? "empty" : "too long" );
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

    if( !token )
      break;

    if( !Q_stricmp( token, "" ) )
      break;

    if( !Q_stricmp( token, "model" ) )
    {
      int index = 0;

      token = COM_Parse( &text_p );
      if( !token )
        break;

      index = atoi( token );

      if( index < 0 )
        index = 0;
      else if( index > 3 )
        index = 3;

      token = COM_Parse( &text_p );
      if( !token )
        break;

      Q_strncpyz( bc->models[ index ], token, sizeof( bc->models[ 0 ] ) );

      defined |= MODEL;
      continue;
    }
    else if( !Q_stricmp( token, "modelScale" ) )
    {
      token = COM_Parse( &text_p );
      if( !token )
        break;

      scale = atof( token );

      if( scale < 0.0f )
        scale = 0.0f;

      bc->modelScale = scale;

      defined |= MODELSCALE;
      continue;
    }
    else if( !Q_stricmp( token, "mins" ) )
    {
      for( i = 0; i <= 2; i++ )
      {
        token = COM_Parse( &text_p );
        if( !token )
          break;

        bc->mins[ i ] = atof( token );
      }

      defined |= MINS;
      continue;
    }
    else if( !Q_stricmp( token, "maxs" ) )
    {
      for( i = 0; i <= 2; i++ )
      {
        token = COM_Parse( &text_p );
        if( !token )
          break;

        bc->maxs[ i ] = atof( token );
      }

      defined |= MAXS;
      continue;
    }
    else if( !Q_stricmp( token, "zOffset" ) )
    {
      float offset;

      token = COM_Parse( &text_p );
      if( !token )
        break;

      offset = atof( token );

      bc->zOffset = offset;

      defined |= ZOFFSET;
      continue;
    }


    Com_Printf( S_COLOR_RED "ERROR: unknown token '%s'\n", token );
    return qfalse;
  }

  if(      !( defined & MODEL      ) )  token = "model";
  else if( !( defined & MODELSCALE ) )  token = "modelScale";
  else if( !( defined & MINS       ) )  token = "mins";
  else if( !( defined & MAXS       ) )  token = "maxs";
  else if( !( defined & ZOFFSET    ) )  token = "zOffset";
  else                                  token = "";

  if( strlen( token ) > 0 )
  {
      Com_Printf( S_COLOR_RED "ERROR: %s not defined in %s\n",
                  token, filename );
      return qfalse;
  }
  return qtrue;
}

/*
===============
BG_InitBuildableConfigs
===============
*/
void BG_InitBuildableConfigs( void )
{
  int               i;
  buildableConfig_t *bc;

  for( i = BA_NONE + 1; i < BA_NUM_BUILDABLES; i++ )
  {
    bc = BG_BuildableConfig( i );
    Com_Memset( bc, 0, sizeof( buildableConfig_t ) );

    BG_ParseBuildableFile( va( "configs/buildables/%s.cfg",
                               BG_Buildable( i )->name ), bc );
  }
}

//CLASSES
static const classAttributes_t bg_classList[ ] =
{
  {
    PCL_NONE,                                       //int     classnum;
    "spectator",                                    //char    *className;
    "Observer mode",
    STAGE_GE_1,                                     //int  stages
    0,                                              //int     health;
    0.0f,                                           //float   fallDamage;
    0.0f,                                           //float   regenRate;
    0,                                              //int     abilities;
    WP_NONE,                                        //weapon_t  startWeapon
    0.0f,                                           //float   buildDist;
    90,                                             //int     fov;
    0.000f,                                         //float   bob;
    1.0f,                                           //float   bobCycle;
    0,                                              //int     steptime;
    800,                                            //float   speed;
    10.0f,                                          //float   acceleration;
    1.0f,                                           //float   airAcceleration;
    6.0f,                                           //float   friction;
    100.0f,                                         //float   stopSpeed;
    270.0f,                                         //float   jumpMagnitude;
    1.0f,                                           //float   knockbackScale;
    { PCL_NONE, PCL_NONE, PCL_NONE },               //int     children[ 3 ];
    0,                                              //int     cost;
    0,                                               //int     value;
  },
  {
    PCL_ALIEN_BUILDER0,                             //int     classnum;
    "builder",                                      //char    *className;
    "Responsible for building and maintaining all the alien structures. "
      "Has a weak melee slash attack.",
    STAGE_GE_1,                                     //int  stages
    ABUILDER_HEALTH,                                //int     health;
    0.0f,                                           //float   fallDamage;
    ABUILDER_REGEN,                                 //float   regenRate;
    SCA_FOVWARPS|SCA_ALIENSENSE,                    //int     abilities;
    WP_ABUILD,                                      //weapon_t  startWeapon
    95.0f,                                          //float   buildDist;
    110,                                            //int     fov;
    0.001f,                                         //float   bob;
    2.0f,                                           //float   bobCycle;
    150,                                            //int     steptime;
    ABUILDER_SPEED,                                 //float   speed;
    10.0f,                                          //float   acceleration;
    1.0f,                                           //float   airAcceleration;
    6.0f,                                           //float   friction;
    100.0f,                                         //float   stopSpeed;
    195.0f,                                         //float   jumpMagnitude;
    1.0f,                                           //float   knockbackScale;
    { PCL_ALIEN_BUILDER0_UPG, PCL_ALIEN_LEVEL0, PCL_NONE },       //int     children[ 3 ];
    ABUILDER_COST,                                  //int     cost;
    ABUILDER_VALUE,                                  //int     value;
  },
  {
    PCL_ALIEN_BUILDER0_UPG,                         //int     classnum;
    "builderupg",                                   //char    *classname;
    "Similar to the base Granger, except that in addition to "
      "being able to build structures it has a spit attack "
      "that slows victims and the ability to crawl on walls.",
    STAGE_GE_2,                                     //int  stages
    ABUILDER_UPG_HEALTH,                            //int     health;
    0.0f,                                           //float   fallDamage;
    ABUILDER_UPG_REGEN,                             //float   regenRate;
    SCA_FOVWARPS|SCA_WALLCLIMBER|SCA_ALIENSENSE,    //int     abilities;
    WP_ABUILD,                                      //weapon_t  startWeapon
    105.0f,                                         //float   buildDist;
    110,                                            //int     fov;
    0.001f,                                         //float   bob;
    2.0f,                                           //float   bobCycle;
    100,                                            //int     steptime;
    ABUILDER_UPG_SPEED,                             //float   speed;
    10.0f,                                          //float   acceleration;
    1.0f,                                           //float   airAcceleration;
    6.0f,                                           //float   friction;
    100.0f,                                         //float   stopSpeed;
    270.0f,                                         //float   jumpMagnitude;
    1.0f,                                           //float   knockbackScale;
    { PCL_ALIEN_LEVEL0, PCL_NONE, PCL_NONE },       //int     children[ 3 ];
    ABUILDER_UPG_COST,                              //int     cost;
    ABUILDER_UPG_VALUE,                              //int     value;
  },
  {
    PCL_ALIEN_LEVEL0,                               //int     classnum;
    "level0",                                       //char    *classname;
    "Has a lethal reflexive bite and the ability to crawl on "
      "walls and ceilings.",
    STAGE_GE_1,                                     //int  stages
    LEVEL0_HEALTH,                                  //int     health;
    0.0f,                                           //float   fallDamage;
    LEVEL0_REGEN,                                   //float   regenRate;
    SCA_WALLCLIMBER|SCA_FOVWARPS|SCA_ALIENSENSE,    //int     abilities;
    WP_ALEVEL0,                                     //weapon_t  startWeapon
    0.0f,                                           //float   buildDist;
    140,                                            //int     fov;
    0.0f,                                           //float   bob;
    2.5f,                                           //float   bobCycle;
    25,                                             //int     steptime;
    LEVEL0_SPEED,                                   //float   speed;
    10.0f,                                          //float   acceleration;
    1.0f,                                           //float   airAcceleration;
    6.0f,                                           //float   friction;
    400.0f,                                         //float   stopSpeed;
    250.0f,                                         //float   jumpMagnitude;
    2.0f,                                           //float   knockbackScale;
    { PCL_ALIEN_LEVEL1, PCL_NONE },                 //int     children[ 3 ];
    LEVEL0_COST,                                    //int     cost;
    LEVEL0_VALUE,                                    //int     value;
  },
  {
    PCL_ALIEN_LEVEL1,                               //int     classnum;
    "level1",                                       //char    *classname;
    "Has a melee attack, the ability to crawl on walls and "
    "ceilings and the ability to temporarily become incorporeal "
    "and pass through buildings and other players. In this state "
    "the Wraith cannot be seen, heard or hurt. Provides a healing "
    "aura that accelerates the healing rate of nearby aliens.",
    STAGE_GE_3,                                     //int     stages
    LEVEL1_HEALTH,                                  //int     health;
    0.0f,                                           //float   fallDamage;
    LEVEL1_REGEN,                                   //float   regenRate;
    SCA_FOVWARPS|SCA_WALLCLIMBER|SCA_ALIENSENSE,    //int     abilities;
    WP_ALEVEL1,                                     //weapon_t  startWeapon
    0.0f,                                           //float   buildDist;
    120,                                            //int     fov;
    0.001f,                                         //float   bob;
    1.8f,                                           //float   bobCycle;
    60,                                             //int     steptime;
    LEVEL1_SPEED,                                   //float   speed;
    10.0f,                                          //float   acceleration;
    1.0f,                                           //float   airAcceleration;
    6.0f,                                           //float   friction;
    300.0f,                                         //float   stopSpeed;
    310.0f,                                         //float   jumpMagnitude;
    1.2f,                                           //float   knockbackScale;
    { PCL_ALIEN_LEVEL2, PCL_NONE, PCL_NONE },       //int     children[ 3 ];
    LEVEL1_COST,                                    //int     cost;
    LEVEL1_VALUE,                                   //int     value;
  },
  {
    PCL_ALIEN_LEVEL2,                               //int     classnum;
    "level2",                                       //char    *classname;
    "Has a melee attack and the ability to jump off walls. This "
      "allows the Marauder to gather great speed in enclosed areas.",
    STAGE_GE_2,                                     //int  stages
    LEVEL2_HEALTH,                                  //int     health;
    0.0f,                                           //float   fallDamage;
    LEVEL2_REGEN,                                   //float   regenRate;
    SCA_WALLJUMPER|SCA_FOVWARPS|SCA_ALIENSENSE,     //int     abilities;
    WP_ALEVEL2,                                     //weapon_t  startWeapon
    0.0f,                                           //float   buildDist;
    90,                                             //int     fov;
    0.001f,                                         //float   bob;
    1.5f,                                           //float   bobCycle;
    80,                                             //int     steptime;
    LEVEL2_SPEED,                                   //float   speed;
    10.0f,                                          //float   acceleration;
    3.0f,                                           //float   airAcceleration;
    6.0f,                                           //float   friction;
    100.0f,                                         //float   stopSpeed;
    380.0f,                                         //float   jumpMagnitude;
    0.8f,                                           //float   knockbackScale;
    { PCL_ALIEN_LEVEL5, PCL_ALIEN_LEVEL3, PCL_ALIEN_LEVEL2_UPG },   //int     children[ 3 ];
    LEVEL2_COST,                                    //int     cost;
    LEVEL2_VALUE,                                    //int     value;
  },
  {
    PCL_ALIEN_LEVEL2_UPG,                           //int     classnum;
    "level2upg",                                    //char    *classname;
    "The Advanced Marauder has all the abilities of the basic Marauder "
      "with the addition of a zap attack that chains through other "
      "adv. marauders and can explode mines.",
    STAGE_GE_3,                                     //int  stages
    LEVEL2_UPG_HEALTH,                              //int     health;
    0.0f,                                           //float   fallDamage;
    LEVEL2_UPG_REGEN,                               //float   regenRate;
    SCA_WALLJUMPER|SCA_FOVWARPS|SCA_ALIENSENSE,     //int     abilities;
    WP_ALEVEL2_UPG,                                 //weapon_t  startWeapon
    0.0f,                                           //float   buildDist;
    90,                                             //int     fov;
    0.001f,                                         //float   bob;
    1.5f,                                           //float   bobCycle;
    80,                                             //int     steptime;
    LEVEL2_UPG_SPEED,                               //float   speed;
    10.0f,                                          //float   acceleration;
    3.0f,                                           //float   airAcceleration;
    6.0f,                                           //float   friction;
    100.0f,                                         //float   stopSpeed;
    380.0f,                                         //float   jumpMagnitude;
    0.7f,                                           //float   knockbackScale;
    { PCL_ALIEN_LEVEL5, PCL_ALIEN_LEVEL3, PCL_NONE },   //int     children[ 3 ];
    LEVEL2_UPG_COST,                                //int     cost;
    LEVEL2_UPG_VALUE,                                //int     value;
  },
  {
    PCL_ALIEN_LEVEL5,                               //int     classnum;
    "level5",                                       //char    *classname;
    "The Hummel is a flying alien with air pounce ability, "
      "fast melee attacks and unlimited low damage shooting attack "
      "that does not spread poison.",
    STAGE_GE_4,                                     //int  stages
    LEVEL5_UPG_HEALTH,                              //int     health;
    0.0f,                                           //float   fallDamage;
    LEVEL5_UPG_REGEN,                               //float   regenRate;
    SCA_FOVWARPS|SCA_ALIENSENSE|SCA_FLYING,         //int     abilities;
    WP_ALEVEL5,                                 	//weapon_t  startWeapon
    0.0f,                                           //float   buildDist;
    90,                                             //int     fov;
    0.001f,                                         //float   bob;
    1.5f,                                           //float   bobCycle;
    80,                                             //int     steptime;
    LEVEL5_UPG_SPEED,                               //float   speed;
    9.0f,                                          //float   acceleration;
    5.0f,                                          //float   airAcceleration;
    6.0f,                                           //float   friction;
    90.0f,                                         //float   stopSpeed;
    250.0f,                                         //float   jumpMagnitude;
    0.7f,                                           //float   knockbackScale;
    { PCL_ALIEN_LEVEL3, PCL_ALIEN_LEVEL3_UPG, PCL_NONE },       //int     children[ 3 ];
    LEVEL5_UPG_COST,                                //int     cost;
    LEVEL5_UPG_VALUE,                                //int     value;
  },
  {
    PCL_ALIEN_LEVEL3,                               //int     classnum;
    "level3",                                       //char    *classname;
    "Possesses a melee attack and the pounce ability, which may "
      "be used as both an attack and a means to reach remote "
      "locations inaccessible from the ground.",
    STAGE_GE_3,                                     //int  stages
    LEVEL3_HEALTH,                                  //int     health;
    0.0f,                                           //float   fallDamage;
    LEVEL3_REGEN,                                   //float   regenRate;
    SCA_FOVWARPS|SCA_ALIENSENSE,                    //int     abilities;
    WP_ALEVEL3,                                     //weapon_t  startWeapon
    0.0f,                                           //float   buildDist;
    90,                                            //int     fov;
    0.0005f,                                        //float   bob;
    1.3f,                                           //float   bobCycle;
    90,                                             //int     steptime;
    LEVEL3_SPEED,                                   //float   speed;
    10.0f,                                          //float   acceleration;
    1.0f,                                           //float   airAcceleration;
    6.0f,                                           //float   friction;
    200.0f,                                         //float   stopSpeed;
    270.0f,                                         //float   jumpMagnitude;
    0.3f,                                           //float   knockbackScale;
    { PCL_ALIEN_LEVEL4, PCL_ALIEN_LEVEL3_UPG, PCL_NONE },   //int     children[ 3 ];
    LEVEL3_COST,                                    //int     cost;
    LEVEL3_VALUE,                                    //int     value;
  },
  {
    PCL_ALIEN_LEVEL3_UPG,                           //int     classnum;
    "level3upg",                                    //char    *classname;
    "In addition to the basic Dragoon abilities, the Advanced "
      "Dragoon has 3 barbs which may be used to attack humans "
      "from a distance and it has a higher jump height.",
    STAGE_GE_4,                                     //int  stages
    LEVEL3_UPG_HEALTH,                              //int     health;
    0.0f,                                           //float   fallDamage;
    LEVEL3_UPG_REGEN,                               //float   regenRate;
    SCA_FOVWARPS|SCA_ALIENSENSE,                    //int     abilities;
    WP_ALEVEL3_UPG,                                 //weapon_t  startWeapon
    0.0f,                                           //float   buildDist;
    90,                                             //int     fov;
    0.0005f,                                        //float   bob;
    1.3f,                                           //float   bobCycle;
    90,                                             //int     steptime;
    LEVEL3_UPG_SPEED,                               //float   speed;
    10.0f,                                          //float   acceleration;
    1.0f,                                           //float   airAcceleration;
    6.0f,                                           //float   friction;
    200.0f,                                         //float   stopSpeed;
    270.0f,                                         //float   jumpMagnitude;
    0.2f,                                           //float   knockbackScale;
    { PCL_ALIEN_LEVEL4, PCL_NONE, PCL_NONE },       //int     children[ 3 ];
    LEVEL3_UPG_COST,                                //int     cost;
    LEVEL3_UPG_VALUE,                               //int     value;
  },
  {
    PCL_ALIEN_LEVEL4,                               //int     classnum;
    "level4",                                       //char    *classname;
    "A large alien with a strong melee attack. "
      "This class can also charge at enemy players and structures, "
      "inflicting great damage. Any humans caught under a falling "
      "Tyrant will be crushed by its weight.",
    STAGE_GE_5,                                     //int  stages
    LEVEL4_HEALTH,                                  //int     health;
    0.0f,                                           //float   fallDamage;
    LEVEL4_REGEN,                                   //float   regenRate;
    SCA_FOVWARPS|SCA_ALIENSENSE,                    //int     abilities;
    WP_ALEVEL4,                                     //weapon_t  startWeapon
    0.0f,                                           //float   buildDist;
    90,                                             //int     fov;
    0.001f,                                         //float   bob;
    1.1f,                                           //float   bobCycle;
    100,                                            //int     steptime;
    LEVEL4_SPEED,                                   //float   speed;
    10.0f,                                          //float   acceleration;
    1.0f,                                           //float   airAcceleration;
    6.0f,                                           //float   friction;
    100.0f,                                         //float   stopSpeed;
    170.0f,                                         //float   jumpMagnitude;
    0.1f,                                           //float   knockbackScale;
    { PCL_NONE, PCL_NONE, PCL_NONE },               //int     children[ 3 ];
    LEVEL4_COST,                                    //int     cost;
    LEVEL4_VALUE,                                   //int     value;
  },
  {
    PCL_HUMAN,                                      //int     classnum;
    "human_base",                                   //char    *classname;
    "",
    STAGE_GE_1,                                     //int  stages
    100,                                            //int     health;
    1.0f,                                           //float   fallDamage;
    0.0f,                                           //float   regenRate;
    SCA_TAKESFALLDAMAGE|SCA_CANUSELADDERS,          //int     abilities;
    WP_NONE, //special-cased in g_client.c          //weapon_t  startWeapon
    110.0f,                                         //float   buildDist;
    90,                                             //int     fov;
    0.002f,                                         //float   bob;
    1.0f,                                           //float   bobCycle;
    100,                                            //int     steptime;
    1.0f,                                           //float   speed;
    10.0f,                                          //float   acceleration;
    1.0f,                                           //float   airAcceleration;
    6.0f,                                           //float   friction;
    100.0f,                                         //float   stopSpeed;
    220.0f,                                         //float   jumpMagnitude;
    1.0f,                                           //float   knockbackScale;
    { PCL_NONE, PCL_NONE, PCL_NONE },               //int     children[ 3 ];
    0,                                              //int     cost;
    ALIEN_CREDITS_PER_KILL,                          //int     value;
  },
  {
    PCL_HUMAN_BSUIT,                                //int     classnum;
    "human_bsuit",                                  //char    *classname;
    "",
    STAGE_GE_5,                                     //int  stages
    100,                                            //int     health;
    1.0f,                                           //float   fallDamage;
    0.0f,                                           //float   regenRate;
    SCA_TAKESFALLDAMAGE|
      SCA_CANUSELADDERS,                            //int     abilities;
    WP_NONE, //special-cased in g_client.c          //weapon_t  startWeapon
    110.0f,                                         //float   buildDist;
    90,                                             //int     fov;
    0.002f,                                         //float   bob;
    1.0f,                                           //float   bobCycle;
    100,                                            //int     steptime;
    1.0f,                                           //float   speed;
    10.0f,                                          //float   acceleration;
    1.0f,                                           //float   airAcceleration;
    6.0f,                                           //float   friction;
    100.0f,                                         //float   stopSpeed;
    220.0f,                                         //float   jumpMagnitude;
    1.0f,                                           //float   knockbackScale;
    { PCL_NONE, PCL_NONE, PCL_NONE },               //int     children[ 3 ];
    0,                                              //int     cost;
    ALIEN_CREDITS_PER_KILL,                          //int     value;
  }
};

int   bg_numClasses = sizeof( bg_classList ) / sizeof( bg_classList[ 0 ] );
static const classAttributes_t nullClass = { 0 };

/*
==============
BG_ClassByName
==============
*/
const classAttributes_t *BG_ClassByName( const char *name )
{
  int i;

  for( i = 0; i < bg_numClasses; i++ )
  {
    if( !Q_stricmp( bg_classList[ i ].name, name ) )
      return &bg_classList[ i ];
  }
  return &nullClass;
}

/*
==============
BG_Class
==============
*/
const classAttributes_t *BG_Class( class_t class )
{
  return ( class >= PCL_NONE && class < PCL_NUM_CLASSES ) ?
    &bg_classList[ class ] : &nullClass;
}

/*
==============
BG_ClassAllowedInStage
==============
*/
qboolean BG_ClassAllowedInStage( class_t class,
                                 stage_t stage )
{
  int stages = BG_Class( class )->stages;

  return stages & ( 1 << stage );
}

static classConfig_t bg_classConfigList[ PCL_NUM_CLASSES ];
/*
==============
BG_ClassConfig
==============
*/
classConfig_t *BG_ClassConfig( class_t class )
{
  return &bg_classConfigList[ class ];
}

/*
==============
BG_ClassBoundingBox
==============
*/
void BG_ClassBoundingBox( class_t class,
                          vec3_t mins, vec3_t maxs,
                          vec3_t cmaxs, vec3_t dmins, vec3_t dmaxs )
{
  classConfig_t *classConfig = BG_ClassConfig( class );

  if( mins != NULL )
    VectorCopy( classConfig->mins, mins );

  if( maxs != NULL )
    VectorCopy( classConfig->maxs, maxs );

  if( cmaxs != NULL )
    VectorCopy( classConfig->crouchMaxs, cmaxs );

  if( dmins != NULL )
    VectorCopy( classConfig->deadMins, dmins );

  if( dmaxs != NULL )
    VectorCopy( classConfig->deadMaxs, dmaxs );
}

/*
==============
BG_ClassHasAbility
==============
*/
qboolean BG_ClassHasAbility( class_t class, int ability )
{
  int abilities = BG_Class( class )->abilities;
  return abilities & ability;
}

/*
==============
BG_ClassCanEvolveFromTo
==============
*/
int BG_ClassCanEvolveFromTo( class_t fclass,
                             class_t tclass,
                             int credits, int stage,
                             int cost )
{
  int i, j, best, value;

  if( credits < cost || fclass == PCL_NONE || tclass == PCL_NONE ||
      fclass == tclass )
    return -1;

  for( i = 0; i < bg_numClasses; i++ )

  {
    if( bg_classList[ i ].number != fclass )
      continue;

    best = credits + 1;
    for( j = 0; j < 3; j++ )
    {
      int thruClass, evolveCost;
      
      thruClass = bg_classList[ i ].children[ j ];
      if( thruClass == PCL_NONE || !BG_ClassAllowedInStage( thruClass, stage ) ||
          !BG_ClassIsAllowed( thruClass ) )
        continue;

      evolveCost = BG_Class( thruClass )->cost * ALIEN_CREDITS_PER_KILL;
      if( thruClass == tclass )
        value = cost + evolveCost;
      else
        value = BG_ClassCanEvolveFromTo( thruClass, tclass, credits, stage,
                                         cost + evolveCost );

      if( value >= 0 && value < best )
        best = value;
    }

    return best <= credits ? best : -1;
  }

  Com_Printf( S_COLOR_YELLOW "WARNING: fallthrough in BG_ClassCanEvolveFromTo\n" );
  return -1;
}

/*
==============
BG_AlienCanEvolve
==============
*/
qboolean BG_AlienCanEvolve( class_t class, int credits, int stage )
{
  int i, j, tclass;

  for( i = 0; i < bg_numClasses; i++ )
  {
    if( bg_classList[ i ].number != class )
      continue;

    for( j = 0; j < 3; j++ )
    {
      tclass = bg_classList[ i ].children[ j ];
      if( tclass != PCL_NONE && 
	  BG_ClassAllowedInStage( tclass, stage ) &&
          BG_ClassIsAllowed( tclass ) &&
          credits >= BG_Class( tclass )->cost * ALIEN_CREDITS_PER_KILL )
        return qtrue;
    }
    return qfalse;
  }
  Com_Printf( S_COLOR_YELLOW "WARNING: fallthrough in BG_AlienCanEvolve\n" );
  return qfalse;
}

/*
======================
BG_ParseClassFile

Parses a configuration file describing a class
======================
*/
static qboolean BG_ParseClassFile( const char *filename, classConfig_t *cc )
{
  char          *text_p;
  int           i;
  int           len;
  char          *token;
  char          text[ 20000 ];
  fileHandle_t  f;
  float         scale = 0.0f;
  int           defined = 0;
  enum
  {
      MODEL           = 1 << 0,
      SKIN            = 1 << 1,
      HUD             = 1 << 2,
      MODELSCALE      = 1 << 3,
      SHADOWSCALE     = 1 << 4,
      MINS            = 1 << 5,
      MAXS            = 1 << 6,
      DEADMINS        = 1 << 7,
      DEADMAXS        = 1 << 8,
      CROUCHMAXS      = 1 << 9,
      VIEWHEIGHT      = 1 << 10,
      CVIEWHEIGHT     = 1 << 11,
      ZOFFSET         = 1 << 12,
      NAME            = 1 << 13,
      SHOULDEROFFSETS = 1 << 14
  };

  // load the file
  len = trap_FS_FOpenFile( filename, &f, FS_READ );
  if( len < 0 )
    return qfalse;

  if( len == 0 || len >= sizeof( text ) - 1 )
  {
    trap_FS_FCloseFile( f );
    Com_Printf( S_COLOR_RED "ERROR: Class file %s is %s\n", filename,
      len == 0 ? "empty" : "too long" );
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

    if( !token )
      break;

    if( !Q_stricmp( token, "" ) )
      break;

    if( !Q_stricmp( token, "model" ) )
    {
      token = COM_Parse( &text_p );
      if( !token )
        break;

      Q_strncpyz( cc->modelName, token, sizeof( cc->modelName ) );

      defined |= MODEL;
      continue;
    }
    else if( !Q_stricmp( token, "skin" ) )
    {
      token = COM_Parse( &text_p );
      if( !token )
        break;

      Q_strncpyz( cc->skinName, token, sizeof( cc->skinName ) );

      defined |= SKIN;
      continue;
    }
    else if( !Q_stricmp( token, "hud" ) )
    {
      token = COM_Parse( &text_p );
      if( !token )
        break;

      Q_strncpyz( cc->hudName, token, sizeof( cc->hudName ) );

      defined |= HUD;
      continue;
    }
    else if( !Q_stricmp( token, "modelScale" ) )
    {
      token = COM_Parse( &text_p );
      if( !token )
        break;

      scale = atof( token );

      if( scale < 0.0f )
        scale = 0.0f;

      cc->modelScale = scale;

      defined |= MODELSCALE;
      continue;
    }
    else if( !Q_stricmp( token, "shadowScale" ) )
    {
      token = COM_Parse( &text_p );
      if( !token )
        break;

      scale = atof( token );

      if( scale < 0.0f )
        scale = 0.0f;

      cc->shadowScale = scale;

      defined |= SHADOWSCALE;
      continue;
    }
    else if( !Q_stricmp( token, "mins" ) )
    {
      for( i = 0; i <= 2; i++ )
      {
        token = COM_Parse( &text_p );
        if( !token )
          break;

        cc->mins[ i ] = atof( token );
      }

      defined |= MINS;
      continue;
    }
    else if( !Q_stricmp( token, "maxs" ) )
    {
      for( i = 0; i <= 2; i++ )
      {
        token = COM_Parse( &text_p );
        if( !token )
          break;

        cc->maxs[ i ] = atof( token );
      }

      defined |= MAXS;
      continue;
    }
    else if( !Q_stricmp( token, "deadMins" ) )
    {
      for( i = 0; i <= 2; i++ )
      {
        token = COM_Parse( &text_p );
        if( !token )
          break;

        cc->deadMins[ i ] = atof( token );
      }

      defined |= DEADMINS;
      continue;
    }
    else if( !Q_stricmp( token, "deadMaxs" ) )
    {
      for( i = 0; i <= 2; i++ )
      {
        token = COM_Parse( &text_p );
        if( !token )
          break;

        cc->deadMaxs[ i ] = atof( token );
      }

      defined |= DEADMAXS;
      continue;
    }
    else if( !Q_stricmp( token, "crouchMaxs" ) )
    {
      for( i = 0; i <= 2; i++ )
      {
        token = COM_Parse( &text_p );
        if( !token )
          break;

        cc->crouchMaxs[ i ] = atof( token );
      }

      defined |= CROUCHMAXS;
      continue;
    }
    else if( !Q_stricmp( token, "viewheight" ) )
    {
      token = COM_Parse( &text_p );
      cc->viewheight = atoi( token );
      defined |= VIEWHEIGHT;
      continue;
    }
    else if( !Q_stricmp( token, "crouchViewheight" ) )
    {
      token = COM_Parse( &text_p );
      cc->crouchViewheight = atoi( token );
      defined |= CVIEWHEIGHT;
      continue;
    }
    else if( !Q_stricmp( token, "zOffset" ) )
    {
      float offset;

      token = COM_Parse( &text_p );
      if( !token )
        break;

      offset = atof( token );

      cc->zOffset = offset;

      defined |= ZOFFSET;
      continue;
    }
    else if( !Q_stricmp( token, "name" ) )
    {
      token = COM_Parse( &text_p );
      if( !token )
        break;

      Q_strncpyz( cc->humanName, token, sizeof( cc->humanName ) );

      defined |= NAME;
      continue;
    }
    else if( !Q_stricmp( token, "shoulderOffsets" ) )
    {
      for( i = 0; i <= 2; i++ )
      {
        token = COM_Parse( &text_p );
        if( !token )
          break;

        cc->shoulderOffsets[ i ] = atof( token );
      }

      defined |= SHOULDEROFFSETS;
      continue;
    }

    Com_Printf( S_COLOR_RED "ERROR: unknown token '%s'\n", token );
    return qfalse;
  }

  if(      !( defined & MODEL           ) ) token = "model";
  else if( !( defined & SKIN            ) ) token = "skin";
  else if( !( defined & HUD             ) ) token = "hud";
  else if( !( defined & MODELSCALE      ) ) token = "modelScale";
  else if( !( defined & SHADOWSCALE     ) ) token = "shadowScale";
  else if( !( defined & MINS            ) ) token = "mins";
  else if( !( defined & MAXS            ) ) token = "maxs";
  else if( !( defined & DEADMINS        ) ) token = "deadMins";
  else if( !( defined & DEADMAXS        ) ) token = "deadMaxs";
  else if( !( defined & CROUCHMAXS      ) ) token = "crouchMaxs";
  else if( !( defined & VIEWHEIGHT      ) ) token = "viewheight";
  else if( !( defined & CVIEWHEIGHT     ) ) token = "crouchViewheight";
  else if( !( defined & ZOFFSET         ) ) token = "zOffset";
  else if( !( defined & NAME            ) ) token = "name";
  else if( !( defined & SHOULDEROFFSETS ) ) token = "shoulderOffsets";
  else                                      token = "";

  if( strlen( token ) > 0 )
  {
      Com_Printf( S_COLOR_RED "ERROR: %s not defined in %s\n",
                  token, filename );
      return qfalse;
  }
  return qtrue;
}

/*
===============
BG_InitClassConfigs
===============
*/
void BG_InitClassConfigs( void )
{
  int           i;
  classConfig_t *cc;

  for( i = PCL_NONE; i < PCL_NUM_CLASSES; i++ )
  {
    cc = BG_ClassConfig( i );

    BG_ParseClassFile( va( "configs/classes/%s.cfg",
                           BG_Class( i )->name ), cc );
  }
}

static const weaponAttributes_t bg_weapons[ ] =
{
  {
    WP_ALEVEL0,           //int       weaponNum;
    0,                    //int       price;
    STAGE_GE_1,           //int  stages
    SLOT_WEAPON,          //int       slots;
    "level0",             //char      *weaponName;
    "Bite",               //char      *humanName;
    "",
    0,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qtrue,                //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    LEVEL0_BITE_REPEAT,   //int       repeatRate1;
    0,                    //int       repeatRate2;
    0,                    //int       repeatRate3;
    0,                    //int       reloadTime;
    LEVEL0_BITE_K_SCALE,  //float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qfalse,               //qboolean  purchasable;
    qfalse,               //qboolean  longRanged;
    TEAM_ALIENS           //team_t    team;
  },
  {
    WP_ALEVEL1,           //int       weaponNum;
    0,                    //int       price;
    STAGE_GE_1,           //int  stages
    SLOT_WEAPON,          //int       slots;
    "level1",             //char      *weaponName;
    "Claws",              //char      *humanName;
    "",
    0,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qtrue,                //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    LEVEL1_CLAW_REPEAT,   //int       repeatRate1;
    0,                    //int       repeatRate2;
    0,                    //int       repeatRate3;
    0,                    //int       reloadTime;
    LEVEL1_CLAW_K_SCALE,  //float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qfalse,               //qboolean  purchasable;
    qfalse,               //qboolean  longRanged;
    TEAM_ALIENS           //team_t    team;
  },
  {
    WP_ALEVEL2,           //int       weaponNum;
    0,                    //int       price;
    STAGE_GE_2,           //int  stages
    SLOT_WEAPON,          //int       slots;
    "level2",             //char      *weaponName;
    "Claws",               //char      *humanName;
    "",
    0,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qtrue,                //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    LEVEL2_CLAW_REPEAT,   //int       repeatRate1;
    0,                    //int       repeatRate2;
    0,                    //int       repeatRate3;
    0,                    //int       reloadTime;
    LEVEL2_CLAW_K_SCALE,        //float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qfalse,               //qboolean  purchasable;
    qfalse,               //qboolean  longRanged;
    TEAM_ALIENS           //team_t    team;
  },
  {
    WP_ALEVEL2_UPG,       //int       weaponNum;
    0,                    //int       price;
    STAGE_GE_3,           //int  stages
    SLOT_WEAPON,          //int       slots;
    "level2upg",          //char      *weaponName;
    "Zap",                //char      *humanName;
    "",
    0,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qtrue,                //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    LEVEL2_CLAW_U_REPEAT, //int       repeatRate1;
    LEVEL2_AREAZAP_REPEAT,//int       repeatRate2;
    LEVEL2_BOUNCEBALL_REPEAT,//int    repeatRate3;
    0,                    //int       reloadTime;
    LEVEL2_CLAW_U_K_SCALE,//float     knockbackScale;
    qtrue,                //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qfalse,               //qboolean  purchasable;
    qtrue,                //qboolean  longRanged;
    TEAM_ALIENS           //team_t    team;
  },
  {
    WP_ALEVEL5,       						//int       weaponNum;
    0,                    					//int       price;
    STAGE_GE_4, 	  //int  stages
    SLOT_WEAPON,          					//int       slots;
    "level5",             					//char      *weaponName;
    "prickles",                				//char      *humanName;
    "",
    10,                    					//int       maxAmmo; 
    0,                    					//int       maxClips;
    qtrue,                					//int       infiniteAmmo;
    qfalse,               					//int       usesEnergy;
    LEVEL5_CLAW_U_REPEAT, 					//int       repeatRate1;
    LEVEL5_POUNCE_REPEAT,					//int       repeatRate2;
    LEVEL5_PRICKLES_REPEAT,				    //int       repeatRate3;
    0,                     					//int       reloadTime;
    LEVEL5_CLAW_U_K_SCALE,					//float     knockbackScale;
    qfalse,                					//qboolean  hasAltMode;
    qtrue,                					//qboolean  hasThirdMode;
    qtrue,                					//qboolean  canZoom;
    90.0f,                					//float     zoomFov;
    qfalse,               					//qboolean  purchasable;
    qtrue,                					//qboolean  longRanged;
    TEAM_ALIENS           					//team_t    team;
  },
  {
    WP_ALEVEL3,           //int       weaponNum;
    0,                    //int       price;
    STAGE_GE_3,           //int  stages
    SLOT_WEAPON,          //int       slots;
    "level3",             //char      *weaponName;
    "Pounce",             //char      *humanName;
    "",
    0,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qtrue,                //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    LEVEL3_CLAW_REPEAT,   //int       repeatRate1;
    0,                    //int       repeatRate2;
    0,                    //int       repeatRate3;
    0,                    //int       reloadTime;
    LEVEL3_CLAW_K_SCALE,  //float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qtrue,                //qboolean  canZoom;
    100.0f,               //float     zoomFov;
    qfalse,               //qboolean  purchasable;
    qfalse,               //qboolean  longRanged;
    TEAM_ALIENS           //team_t    team;
  },
  {
    WP_ALEVEL3_UPG,       //int       weaponNum;
    0,                    //int       price;
    STAGE_GE_4,           //int  stages
    SLOT_WEAPON,          //int       slots;
    "level3upg",          //char      *weaponName;
    "Pounce (upgrade)",   //char      *humanName;
    "",
    3,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qtrue,                //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    LEVEL3_CLAW_U_REPEAT, //int       repeatRate1;
    0,                    //int       repeatRate2;
    LEVEL3_BOUNCEBALL_REPEAT,//int       repeatRate3;
    0,                    //int       reloadTime;
    LEVEL3_CLAW_U_K_SCALE,//float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qtrue,                //qboolean  hasThirdMode;
    qtrue,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qfalse,               //qboolean  purchasable;
    qtrue,                //qboolean  longRanged;
    TEAM_ALIENS           //team_t    team;
  },
  {
    WP_ALEVEL4,           //int       weaponNum;
    0,                    //int       price;
    STAGE_GE_5,           //int  stages
    SLOT_WEAPON,          //int       slots;
    "level4",             //char      *weaponName;
    "Charge",             //char      *humanName;
    "",
    3,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qtrue,                //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    LEVEL4_CLAW_REPEAT,   //int       repeatRate1;
    0,                    //int       repeatRate2;
    LEVEL4_FIREBREATHREPEAT, //int    repeatRate3;
    0,                    //int       reloadTime;
    LEVEL4_CLAW_K_SCALE,  //float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qfalse,               //qboolean  purchasable;
    qfalse,               //qboolean  longRanged;
    TEAM_ALIENS           //team_t    team;
  },
  
    {
    WP_BLASTER,           //int       weaponNum;
    0,                    //int       price;
    STAGE_GE_1,           //int  stages
    0,                    //int       slots;
    "blaster",            //char      *weaponName;
    "[yefarms]Blaster",   //char      *humanName;
    "",
    0,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qtrue,                //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    BLASTER_REPEAT,       //int       repeatRate1;
    0,                    //int       repeatRate2;
    0,                    //int       repeatRate3;
    0,                    //int       reloadTime;
    BLASTER_K_SCALE,      //float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qfalse,               //qboolean  purchasable;
    qtrue,                //qboolean  longRanged;
    TEAM_HUMANS           //team_t    team;
  },
  {
    WP_MACHINEGUN,        //int       weaponNum;
    RIFLE_PRICE,          //int       price;
    STAGE_GE_1,           //int  stages
    SLOT_WEAPON,          //int       slots;
    "rifle",              //char      *weaponName;
    "[yefarms]Rifle",              //char      *humanName;
    "Basic weapon. Cased projectile weapon, with a slow clip based "
      "reload system & a fast 2nd mode.",
    RIFLE_CLIPSIZE,       //int       maxAmmo;
    RIFLE_MAXCLIPS,       //int       maxClips;
    qfalse,               //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    RIFLE_REPEAT,         //int       repeatRate1;
    RIFLE_REPEAT2,        //int       repeatRate2;
    0,                    //int       repeatRate3;
    RIFLE_RELOAD,         //int       reloadTime;
    RIFLE_K_SCALE,        //float     knockbackScale;
    qtrue,                //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qtrue,                //qboolean  purchasable;
    qtrue,                //qboolean  longRanged;
    TEAM_HUMANS           //team_t    team;
  },
  {
    WP_PAIN_SAW,          //int       weaponNum;
    PAINSAW_PRICE,        //int       price;
    STAGE_GE_1,           //int       stages
    SLOT_WEAPON,          //int       slots;
    "psaw",               //char      *weaponName;
    "[yefarms]Pain Saw",           //char      *humanName;
    "Similar to a chainsaw, but instead of a chain it has an "
      "electric arc capable of dealing a great deal of damage at "
      "close range. It has a blade mode for secondary fire",
    0,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qtrue,                //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    PAINSAW_REPEAT,       //int       repeatRate1;
    PAINSAW_REPEAT2,       //int       repeatRate2;
    0,                    //int       repeatRate3;
    0,                    //int       reloadTime;
    PAINSAW_K_SCALE,      //float     knockbackScale;
    qtrue,                //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qtrue,                //qboolean  purchasable;
    qfalse,               //qboolean  longRanged;
    TEAM_HUMANS           //team_t    team;
  },
  {
    WP_SHOTGUN,           //int       weaponNum;
    SHOTGUN_PRICE,        //int       price;
    STAGE_GE_3,           //int  stages
    SLOT_WEAPON,          //int       slots;
    "shotgun",            //char      *weaponName;
    "[yefarms]Shotgun",            //char      *humanName;
    "Close range weapon that is useful against larger foes. "
      "It has a slow repeat rate, but can be devastatingly "
      "effective.",
    SHOTGUN_SHELLS,       //int       maxAmmo;
    SHOTGUN_MAXCLIPS,     //int       maxClips;
    qfalse,               //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    SHOTGUN_REPEAT,       //int       repeatRate1;
    0,                    //int       repeatRate2;
    0,                    //int       repeatRate3;
    SHOTGUN_RELOAD,       //int       reloadTime;
    SHOTGUN_K_SCALE,       //float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qtrue,                //qboolean  purchasable;
    qtrue,                //qboolean  longRanged;
    TEAM_HUMANS           //team_t    team;
  },
  {
    WP_LAS_GUN,           //int       weaponNum;
    LASGUN_PRICE,         //int       price;
    STAGE_GE_2,           //int  stages
    SLOT_WEAPON,          //int       slots;
    "lgun",               //char      *weaponName;
    "[yefarms]Las Gun",            //char      *humanName;
    "Slightly more powerful than the basic rifle, rapidly fires "
      "small packets of energy.",
    LASGUN_AMMO,          //int       maxAmmo;
    0,                    //int       maxClips;
    qfalse,               //int       infiniteAmmo;
    qtrue,                //int       usesEnergy;
    LASGUN_REPEAT,        //int       repeatRate1;
    LASGUN_REPEAT2,       //int       repeatRate2;
    0,                    //int       repeatRate3;
    LASGUN_RELOAD,        //int       reloadTime;
    LASGUN_K_SCALE,       //float     knockbackScale;
    qtrue,                //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qtrue,                //qboolean  purchasable;
    qtrue,                //qboolean  longRanged;
    TEAM_HUMANS           //team_t    team;
  },
  {
    WP_MASS_DRIVER,       //int       weaponNum;
    MDRIVER_PRICE,        //int       price;
    STAGE_GE_3,           //int  stages
    SLOT_WEAPON,          //int       slots;
    "mdriver",            //char      *weaponName;
    "[yefarms]Mass Driver",        //char      *humanName;
    "A portable particle accelerator which causes minor nuclear "
    "reactions at the point of impact. It has a very large "
    "payload, but fires slowly.",
    MDRIVER_CLIPSIZE,     //int       maxAmmo;
    MDRIVER_MAXCLIPS,     //int       maxClips;
    qfalse,               //int       infiniteAmmo;
    qtrue,                //int       usesEnergy;
    MDRIVER_REPEAT,       //int       repeatRate1;
    0,                    //int       repeatRate2;
    MDRIVER_REPEAT2,      //int       repeatRate3;
    MDRIVER_RELOAD,       //int       reloadTime;
    MDRIVER_K_SCALE,      //float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qtrue,                //qboolean  canZoom;
    25.0f,                //float     zoomFov;
    qtrue,                //qboolean  purchasable;
    qtrue,                //qboolean  longRanged;
    TEAM_HUMANS           //team_t    team;
  },
  {
    WP_CHAINGUN,          //int       weaponNum;
    CHAINGUN_PRICE,       //int       price;
    STAGE_GE_3,           //int  stages
    SLOT_WEAPON,          //int       slots;
    "chaingun",           //char      *weaponName;
    "[yefarms]Chaingun",           //char      *humanName;
    "Belt drive, cased projectile weapon. It has a high repeat "
      "rate but a wide firing angle and is therefore relatively "
      "inaccurate.",
    CHAINGUN_BULLETS,     //int       maxAmmo;
    0,                    //int       maxClips;
    qfalse,               //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    CHAINGUN_REPEAT,      //int       repeatRate1;
    CHAINGUN_REPEAT2,      //int       repeatRate2;
    0,                    //int       repeatRate3;
    0,                    //int       reloadTime;
    CHAINGUN_K_SCALE,     //float     knockbackScale;
    qtrue,               //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qtrue,                //qboolean  purchasable;
    qtrue,                //qboolean  longRanged;
    TEAM_HUMANS           //team_t    team;
  },
   {
    WP_FLAMER,            //int       weaponNum;
    FLAMER_PRICE,         //int       price;
    STAGE_GE_4,           //int  stages
    SLOT_WEAPON,          //int       slots;
    "flamer",             //char      *weaponName;
    "[yefarms]Flame Thrower",      //char      *humanName;
    "Sprays fire at its target. It is powered by compressed "
      "gas. The relatively low rate of fire means this weapon is most "
      "effective against static targets.",
    150,           		  //int       maxAmmo;
    1,                    //int       maxClips;
    qfalse,               //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    FLAMER_REPEAT,        //int       repeatRate1;
    0,                    //int       repeatRate2;
    0,                    //int       repeatRate3;
    3000,                 //int       reloadTime;
    FLAMER_K_SCALE,       //float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qtrue,                //qboolean  purchasable;
    qtrue,                //qboolean  longRanged;
    TEAM_HUMANS           //team_t    team;
  },
  {
    WP_PULSE_RIFLE,       //int       weaponNum;
    PRIFLE_PRICE,         //int       price;
    STAGE_GE_4,           //int  stages
    SLOT_WEAPON,          //int       slots;
    "prifle",             //char      *weaponName;
    "[yefarms]Pulse Rifle",        //char      *humanName;
    "An energy weapon that fires rapid pulses of concentrated energy.",
    PRIFLE_CLIPS,         //int       maxAmmo;
    PRIFLE_MAXCLIPS,      //int       maxClips;
    qfalse,               //int       infiniteAmmo;
    qtrue,                //int       usesEnergy;
    PRIFLE_REPEAT,        //int       repeatRate1;
    PRIFLE_SECONDARY_REPEAT,                    //int       repeatRate2;
    0,                    //int       repeatRate3;
    PRIFLE_RELOAD,        //int       reloadTime;
    PRIFLE_K_SCALE,       //float     knockbackScale;
    qtrue,                //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qtrue,                //qboolean  purchasable;
    qtrue,                //qboolean  longRanged;
    TEAM_HUMANS           //team_t    team;
  },
  {
    WP_LIGHTNING_GUN,     //int       weaponNum;
    LIGHTNING_PRICE,      //int       price;
    STAGE_GE_5,           //int  stages
    SLOT_WEAPON,          //int       slots;
    "lightning",          //char      *weaponName;
    "[yefarms]Lightning Gun",     //char      *humanName;
    "This is a lightning gun. It guns lightning.",
    LIGHTNING_AMMO,       //int       maxAmmo;
    0,                    //int       maxClips;
    qfalse,               //int       infiniteAmmo;
    qtrue,                //int       usesEnergy;
    LIGHTNING_REPEAT,     //int       repeatRate1;
    0,                    //int       repeatRate2;
    0,                    //int       repeatRate3;
    0,                    //int       reloadTime;
    LIGHTNING_K_SCALE,    //float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qtrue,                //qboolean  purchasable;
    qtrue,                //qboolean  longRanged;
    TEAM_HUMANS           //team_t    team;
  },
  {
    WP_LUCIFER_CANNON,    //int       weaponNum;
    LCANNON_PRICE,        //int       price;
    STAGE_GE_5,           //int  stages
    SLOT_WEAPON,          //int       slots;
    "lcannon",            //char      *weaponName;
    "[yefarms]Lucifer Cannon",     //char      *humanName;
    "Blaster technology scaled up to deliver devastating power. "
      "Primary fire must be charged before firing. It has a quick "
      "secondary attack that does not require charging.",
    LCANNON_AMMO,         //int       maxAmmo;
    0,                    //int       maxClips;
    qfalse,               //int       infiniteAmmo;
    qtrue,                //int       usesEnergy;
    LCANNON_REPEAT,       //int       repeatRate1;
    LCANNON_SECONDARY_REPEAT, //int       repeatRate2;
    0,                    //int       repeatRate3;
    LCANNON_RELOAD,       //int       reloadTime;
    LCANNON_K_SCALE,      //float     knockbackScale;
    qtrue,                //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qtrue,                //qboolean  purchasable;
    qtrue,                //qboolean  longRanged;
    TEAM_HUMANS           //team_t    team;
  },
  {
    WP_ROCKET_LAUNCHER,   //int       weaponNum;
    ROCKETL_PRICE,        //int       price;
    STAGE_GE_5,           //int  stages
    SLOT_WEAPON,          //int       slots;
    "rocketl",            //char      *weaponName;
    "[yefarms]Rocket Launcher",     //char      *humanName;
    "This is a rocket launcher. It launches rockets.",
    ROCKETL_AMMO,         //int       maxAmmo;
    ROCKETL_CLIPS,        //int       maxClips;
    qfalse,               //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    ROCKETL_REPEAT,       //int       repeatRate1;
    0,                    //int       repeatRate2;
    0,                    //int       repeatRate3;
    ROCKETL_RELOAD,       //int       reloadTime;
    ROCKETL_K_SCALE,      //float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qtrue,                //qboolean  purchasable;
    qtrue,                //qboolean  longRanged;
    TEAM_HUMANS           //team_t    team;
  },
  {
    WP_GRENADE,           //int       weaponNum;
    GRENADE_PRICE,        //int       price;
    STAGE_GE_3,           //int  stages
    SLOT_NONE,            //int       slots;
    "grenade",            //char      *weaponName;
    "Grenade",            //char      *humanName;
    "",
    1,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qfalse,               //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    GRENADE_REPEAT,       //int       repeatRate1;
    0,                    //int       repeatRate2;
    0,                    //int       repeatRate3;
    0,                    //int       reloadTime;
    GRENADE_K_SCALE,      //float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qfalse,               //qboolean  purchasable;
    qfalse,               //qboolean  longRanged;
    TEAM_HUMANS           //team_t    team;
  },
  {
    WP_MINE,         //int       weaponNum;
    MINE_PRICE,        //int       price;
    STAGE_GE_5,           //int  stages
    SLOT_NONE,            //int       slots;
    "mine",            //char      *weaponName;
    "Sticky Grenade",    //char      *weaponHumanName;
    "",
    1,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qfalse,               //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    MINE_REPEAT,       //int       repeatRate1;
    0,                    //int       repeatRate2;
    0,                    //int       repeatRate3;
    0,                    //int       reloadTime;
    MINE_K_SCALE,      //float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qfalse,               //qboolean  purchasable;
    qfalse,               //qboolean  longRanged;
    TEAM_HUMANS            //WUTeam_t  team;
  },
  {
    WP_HIVE,              //int       weaponNum;
    0,                    //int       price;
    STAGE_GE_1,           //int  stages
    SLOT_WEAPON,          //int       slots;
    "hive",               //char      *weaponName;
    "Hive",               //char      *humanName;
    "",
    0,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qtrue,                //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    500,                  //int       repeatRate1;
    500,                  //int       repeatRate2;
    500,                  //int       repeatRate3;
    0,                    //int       reloadTime;
    HIVE_K_SCALE,         //float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qfalse,               //qboolean  purchasable;
    qfalse,               //qboolean  longRanged;
    TEAM_ALIENS           //team_t    team;
  },
  {
    WP_TESLAGEN,          //int       weaponNum;
    0,                    //int       price;
    STAGE_GE_1,           //int  stages
    SLOT_WEAPON,          //int       slots;
    "teslagen",           //char      *weaponName;
    "Tesla Generator",    //char      *humanName;
    "",
    0,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qtrue,                //int       infiniteAmmo;
    qtrue,                //int       usesEnergy;
    500,                  //int       repeatRate1;
    500,                  //int       repeatRate2;
    500,                  //int       repeatRate3;
    0,                    //int       reloadTime;
    TESLAGEN_K_SCALE,     //float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qfalse,               //qboolean  purchasable;
    qfalse,               //qboolean  longRanged;
    TEAM_HUMANS           //team_t    team;
  },
  {
    WP_MGTURRET,          //int       weaponNum;
    0,                    //int       price;
    STAGE_GE_1,           //int  stages
    SLOT_WEAPON,          //int       slots;
    "mgturret",           //char      *weaponName;
    "Machinegun Turret",  //char      *humanName;
    "",
    0,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qtrue,                //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    0,                    //int       repeatRate1;
    0,                    //int       repeatRate2;
    0,                    //int       repeatRate3;
    0,                    //int       reloadTime;
    MGTURRET_K_SCALE,     //float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qfalse,               //qboolean  purchasable;
    qfalse,               //qboolean  longRanged;
    TEAM_HUMANS           //team_t    team;
  },
    {
    WP_MGTURRET2,         //int       weaponNum;
    0,                    //int       price;
    STAGE_GE_1,           //int  stages
    SLOT_WEAPON,          //int       slots;
    "mgturret2",          //char      *weaponName;
    "Machinegun Turret2", //char      *humanName;
    "",
    0,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qtrue,                //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    0,                    //int       repeatRate1;
    0,                    //int       repeatRate2;
    0,                    //int       repeatRate3;
    0,                    //int       reloadTime;
    MGTURRET2_K_SCALE,    //float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qfalse,               //qboolean  purchasable;
    qfalse,               //qboolean  longRanged;
    TEAM_HUMANS           //team_t    team;
  },
  {
    WP_ABUILD,            //int       weaponNum;
    0,                    //int       price;
    STAGE_GE_1,           //int  stages
    SLOT_WEAPON,          //int       slots;
    "abuild",             //char      *weaponName;
    "Alien build weapon", //char      *humanName;
    "",
    0,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qtrue,                //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    ABUILDER_BUILD_REPEAT,//int       repeatRate1;
    ABUILDER_CLAW_REPEAT, //int       repeatRate2;
    ABUILDER_BLOB_REPEAT, //int       repeatRate3;
    0,                    //int       reloadTime;
    ABUILDER_CLAW_K_SCALE,//float     knockbackScale;
    qtrue,                //qboolean  hasAltMode;
    qtrue,                //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qtrue,                //qboolean  purchasable;
    qfalse,               //qboolean  longRanged;
    TEAM_ALIENS           //team_t    team;
  },
  {
    WP_HBUILD,            //int       weaponNum;
    HBUILD_PRICE,         //int       price;
    STAGE_GE_1,           //int  stages
    SLOT_WEAPON,          //int       slots;
    "ckit",               //char      *weaponName;
    "[yetool]Construction Kit",   //char      *humanName;
    "Used for building structures. This includes "
      "spawns, power and basic defense. More structures become "
      "available with new stages.",
    0,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qtrue,                //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    HBUILD_REPEAT,        //int       repeatRate1;
    0,                    //int       repeatRate2;
    0,                    //int       repeatRate3;
    0,                    //int       reloadTime;
    0.0f,                 //float     knockbackScale;
    qtrue,                //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qtrue,                //qboolean  purchasable;
    qfalse,               //qboolean  longRanged;
    TEAM_HUMANS           //team_t    team;
  }
};

int   bg_numWeapons = sizeof( bg_weapons ) / sizeof( bg_weapons[ 0 ] );
static const weaponAttributes_t nullWeapon = { 0 };

/*
==============
BG_WeaponByName
==============
*/
const weaponAttributes_t *BG_WeaponByName( const char *name )
{
  int i;

  for( i = 0; i < bg_numWeapons; i++ )
  {
    if( !Q_stricmp( bg_weapons[ i ].name, name ) )
    {
      return &bg_weapons[ i ];
    }
  }
  return &nullWeapon;
}

/*
==============
BG_Weapon
==============
*/
const weaponAttributes_t *BG_Weapon( weapon_t weapon )
{
  return ( weapon > WP_NONE && weapon < WP_NUM_WEAPONS ) ?
    &bg_weapons[ weapon - 1 ] : &nullWeapon;
}

/*
==============
BG_WeaponAllowedInStage
==============
*/
qboolean BG_WeaponAllowedInStage( weapon_t weapon, stage_t stage )
{
  int stages = BG_Weapon( weapon )->stages;

  return stages & ( 1 << stage );
}

static const upgradeAttributes_t bg_upgrades[ ] =
{
  {
    UP_LIGHTARMOUR,         //int   upgradeNum;
    LIGHTARMOUR_PRICE,      //int   price;
    STAGE_GE_1,             //int  stages
    SLOT_TORSO|SLOT_ARMS|SLOT_LEGS, //int   slots;
    "larmour",              //char  *upgradeName;
    "[yeshield]Light Armour",         //char  *humanName;
    "Protective armour that helps to defend against light alien melee "
    "attacks.",
    "icons/iconu_larmour",
    qtrue,                  //qboolean purchasable
    qfalse,                 //qboolean usable
    TEAM_HUMANS             //team_t  team;
  },
  {
    UP_HELMET,              //int   upgradeNum;
    HELMET_PRICE,           //int   price;
    STAGE_GE_3,             //int  stages
    SLOT_HEAD,              //int   slots;
    "helmet",               //char  *upgradeName;
    "[yeshield]Helmet",               //char  *humanName;
    "In addition to protecting your head, the helmet provides a "
      "scanner indicating the presence of any friendly or hostile "
      "lifeforms and structures in your immediate vicinity.",
    "icons/iconu_helmet",
    qtrue,                  //qboolean purchasable
    qfalse,                 //qboolean usable
    TEAM_HUMANS             //team_t  team;
  },
  {
    UP_MEDKIT,              //int   upgradeNum;
    MEDKIT_PRICE,           //int   price;
    STAGE_GE_1,             //int  stages
    SLOT_NONE,              //int   slots;
    "medkit",               //char  *upgradeName;
    "[yemed]Medkit",        //char  *humanName;
    "Basic health kit. ",
    "icons/iconu_atoxin",
    (SPAWN_WITH_MEDKIT ? qfalse : qtrue), //qboolean purchasable
    qtrue,                  //qboolean usable
    TEAM_HUMANS             //team_t  team;
  },
  {
    UP_BIOKIT,              //int   upgradeNum;
    BIOKIT_PRICE,           //int   price;
    STAGE_GE_2,             //int  stages
    SLOT_NONE,              //int   slots;
    "biokit",               //char  *upgradeName;
    "[yebiok]Biokit",               //char  *humanName;
    "The Biokit is a biological enhancement system that heals "
      "wounds, improves stamina, and provides some resistance to "
      "alien infection.",
    "icons/iconu_biokit",
    qtrue,                  //qboolean purchasable
    qfalse,                 //qboolean usable
    TEAM_HUMANS             //team_t  team;
  },
  {
    UP_BATTPACK,            //int   upgradeNum;
    BATTPACK_PRICE,         //int   price;
    STAGE_GE_3,             //int  stages
    SLOT_BACKPACK,          //int   slots;
    "battpack",             //char  *upgradeName;
    "[yebat]Battery Pack",         //char  *humanName;
    "Back-mounted battery pack that permits storage of one and a half "
      "times the normal energy capacity for energy weapons.",
    "icons/iconu_battpack",
    qtrue,                  //qboolean purchasable
    qfalse,                 //qboolean usable
    TEAM_HUMANS             //team_t  team;
  },
  {
    UP_JETPACK,             //int   upgradeNum;
    JETPACK_PRICE,          //int   price;
    STAGE_GE_4,             //int  stages
    SLOT_BACKPACK,          //int   slots;
    "jetpack",              //char  *upgradeName;
    "[yejet]Jet Pack",             //char  *humanName;
    "Back-mounted jet pack that enables the user to fly to remote "
      "locations. It is very useful against alien spawns in hard "
      "to reach spots.",
    "icons/iconu_jetpack",
    qtrue,                  //qboolean purchasable
    qtrue,                  //qboolean usable
    TEAM_HUMANS             //team_t  team;
  },
  {
    UP_BATTLESUIT,          //int   upgradeNum;
    BSUIT_PRICE,            //int   price;
    STAGE_GE_5,             //int  stages
    SLOT_HEAD|SLOT_TORSO|SLOT_ARMS|SLOT_LEGS|SLOT_BACKPACK, //int   slots;
    "bsuit",                //char  *upgradeName;
    "[yeshield]Battlesuit",           //char  *humanName;
    "A full body armour that is highly effective at repelling alien attacks. "
      "It allows the user to enter hostile situations with a greater degree "
      "of confidence.",
    "icons/iconu_bsuit",
    qtrue,                  //qboolean purchasable
    qfalse,                 //qboolean usable
    TEAM_HUMANS             //team_t  team;
  },
  {
    UP_GRENADE,             //int   upgradeNum;
    GRENADE_PRICE,          //int   price;
    STAGE_GE_4,             //int  stages
    SLOT_NONE,              //int   slots;
    "gren",                 //char  *upgradeName;
    "[yenade]Explosive Grenade",              //char  *humanName;
    "A small incendinary device ideal for damaging tightly packed "
      "alien structures. Has a five second timer.",
    0,
    qtrue,                  //qboolean purchasable
    qtrue,                  //qboolean usable
    TEAM_HUMANS             //team_t  team;
  },
  {
    UP_MINE,             	//int   upgradeNum;
    MINE_PRICE,          	//int   price;
    STAGE_GE_5,             //int  stages
    SLOT_NONE,              //int   slots;
    "min",                  //char  *upgradeName;
    "[yenade]Mine",              //char  *upgradeHumanName;
    "",
    0,
    qtrue,                  //qboolean purchasable
    qtrue,                  //qboolean usable
    TEAM_HUMANS             //WUTeam_t  team;
  },
  {
    UP_SMOKE,               //int   upgradeNum;
    SMOKE_PRICE,            //int   price;
    STAGE_GE_5,             //int  stages
    SLOT_NONE,              //int   slots;
    "smo",                  //char  *upgradeName;
    "[yenade]Smoke Grenade",              //char  *upgradeHumanName;
    "",
    0,
    qfalse,                 //qboolean purchasable
    qtrue,                  //qboolean usable
    TEAM_HUMANS              //WUTeam_t  team;
  },
  {
    UP_AMMO,                //int   upgradeNum;
    0,                      //int   price;
    STAGE_GE_1,             //int  stages
    SLOT_NONE,              //int   slots;
    "ammo",                 //char  *upgradeName;
    "[yeammo]Ammunition",           //char  *humanName;
    "Ammunition for the currently held weapon.",
    0,
    qtrue,                  //qboolean purchasable
    qfalse,                 //qboolean usable
    TEAM_HUMANS             //team_t  team;
  }
};

int   bg_numUpgrades = sizeof( bg_upgrades ) / sizeof( bg_upgrades[ 0 ] );
static const upgradeAttributes_t nullUpgrade = { 0 };

/*
==============
BG_UpgradeByName
==============
*/
const upgradeAttributes_t *BG_UpgradeByName( const char *name )
{
  int i;

  for( i = 0; i < bg_numUpgrades; i++ )
  {
    if( !Q_stricmp( bg_upgrades[ i ].name, name ) )
    {
      return &bg_upgrades[ i ];
    }
  }
  return &nullUpgrade;
}

/*
==============
BG_Upgrade
==============
*/
const upgradeAttributes_t *BG_Upgrade( upgrade_t upgrade )
{
  return ( upgrade > UP_NONE && upgrade < UP_NUM_UPGRADES ) ?
    &bg_upgrades[ upgrade - 1 ] : &nullUpgrade;
}

/*
==============
BG_UpgradeAllowedInStage
==============
*/
qboolean BG_UpgradeAllowedInStage( upgrade_t upgrade, stage_t stage )
{
  int stages = BG_Upgrade( upgrade )->stages;

  return stages & ( 1 << stage );
}

/*
================
BG_EvaluateTrajectory
================
*/
void BG_EvaluateTrajectory( const trajectory_t *tr, int atTime, vec3_t result )
{
  float   deltaTime;
  float   phase;

  switch( tr->trType )
  {
    case TR_STATIONARY:
    case TR_INTERPOLATE:
      VectorCopy( tr->trBase, result );
      break;

    case TR_LINEAR:
      deltaTime = ( atTime - tr->trTime ) * 0.001;  // milliseconds to seconds
      VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
      break;

    case TR_SINE:
      deltaTime = ( atTime - tr->trTime ) / (float)tr->trDuration;
      phase = sin( deltaTime * M_PI * 2 );
      VectorMA( tr->trBase, phase, tr->trDelta, result );
      break;

    case TR_LINEAR_STOP:
      if( atTime > tr->trTime + tr->trDuration )
        atTime = tr->trTime + tr->trDuration;

      deltaTime = ( atTime - tr->trTime ) * 0.001;  // milliseconds to seconds
      if( deltaTime < 0 )
        deltaTime = 0;

      VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
      break;

    case TR_GRAVITY:
      deltaTime = ( atTime - tr->trTime ) * 0.001;  // milliseconds to seconds
      VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
      result[ 2 ] -= 0.5 * DEFAULT_GRAVITY * deltaTime * deltaTime;   // FIXME: local gravity...
      break;

    case TR_BUOYANCY:
      deltaTime = ( atTime - tr->trTime ) * 0.001;  // milliseconds to seconds
      VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
      result[ 2 ] += 0.5 * DEFAULT_GRAVITY * deltaTime * deltaTime;   // FIXME: local gravity...
      break;

    default:
      Com_Error( ERR_DROP, "BG_EvaluateTrajectory: unknown trType: %i", tr->trTime );
      break;
  }
}

/*
================
BG_EvaluateTrajectoryDelta
For determining velocity at a given time
================
*/
void BG_EvaluateTrajectoryDelta( const trajectory_t *tr, int atTime, vec3_t result )
{
  float deltaTime;
  float phase;

  switch( tr->trType )
  {
    case TR_STATIONARY:
    case TR_INTERPOLATE:
      VectorClear( result );
      break;

    case TR_LINEAR:
      VectorCopy( tr->trDelta, result );
      break;

    case TR_SINE:
      deltaTime = ( atTime - tr->trTime ) / (float)tr->trDuration;
      phase = cos( deltaTime * M_PI * 2 );  // derivative of sin = cos
      phase *= 0.5;
      VectorScale( tr->trDelta, phase, result );
      break;

    case TR_LINEAR_STOP:
      if( atTime > tr->trTime + tr->trDuration )
      {
        VectorClear( result );
        return;
      }
      VectorCopy( tr->trDelta, result );
      break;

    case TR_GRAVITY:
      deltaTime = ( atTime - tr->trTime ) * 0.001;  // milliseconds to seconds
      VectorCopy( tr->trDelta, result );
      result[ 2 ] -= DEFAULT_GRAVITY * deltaTime;   // FIXME: local gravity...
      break;

    case TR_BUOYANCY:
      deltaTime = ( atTime - tr->trTime ) * 0.001;  // milliseconds to seconds
      VectorCopy( tr->trDelta, result );
      result[ 2 ] += DEFAULT_GRAVITY * deltaTime;   // FIXME: local gravity...
      break;

    default:
      Com_Error( ERR_DROP, "BG_EvaluateTrajectoryDelta: unknown trType: %i", tr->trTime );
      break;
  }
}

char *eventnames[ ] =
{
  "EV_NONE",
  "EV_FOOTSTEP",
  "EV_FOOTSTEP_METAL",
  "EV_FOOTSTEP_SQUELCH",
  "EV_FOOTSPLASH",
  "EV_FOOTWADE",
  "EV_SWIM",
  "EV_STEP_4",
  "EV_STEP_8",
  "EV_STEP_12",
  "EV_STEP_16",
  "EV_STEPDN_4",
  "EV_STEPDN_8",
  "EV_STEPDN_12",
  "EV_STEPDN_16",
  "EV_FALL_SHORT",
  "EV_FALL_MEDIUM",
  "EV_FALL_FAR",
  "EV_FALLING",
  "EV_JUMP",
  "EV_AIRPOUNCE",
  "EV_WATER_TOUCH", // foot touches
  "EV_WATER_LEAVE", // foot leaves
  "EV_WATER_UNDER", // head touches
  "EV_WATER_CLEAR", // head leaves
  "EV_NOAMMO",
  "EV_CHANGE_WEAPON",
  "EV_FIRE_WEAPON",
  "EV_FIRE_WEAPON2",
  "EV_FIRE_WEAPON3",
  "EV_PLAYER_RESPAWN", // for fovwarp effects
  "EV_PLAYER_TELEPORT_IN",
  "EV_PLAYER_TELEPORT_OUT",
  "EV_GRENADE_BOUNCE",  // eventParm will be the soundindex
  "EV_MINE_BOUNCE",  // eventParm will be the soundindex
  "EV_GENERAL_SOUND",
  "EV_GLOBAL_SOUND",  // no attenuation
  "EV_BULLET_HIT_FLESH",
  "EV_BULLET_HIT_WALL",
  "EV_SHOTGUN",
  "EV_MASS_DRIVER",
  "EV_MISSILE_HIT",
  "EV_MISSILE_MISS",
  "EV_MISSILE_MISS_METAL",
  "EV_TESLATRAIL",
  "EV_SLIMETRAIL",
  "EV_BULLET",        // otherEntity is the shooter
  "EV_LEV1_GRAB",
  "EV_LEV4_TRAMPLE_PREPARE",
  "EV_LEV4_TRAMPLE_START",
  "EV_PAIN",
  "EV_DEATH1",
  "EV_DEATH2",
  "EV_DEATH3",
  "EV_OBITUARY",
  "EV_GIB_PLAYER",      // gib a previously living player
  "EV_BLEED",
  "EV_BUILD_CONSTRUCT",
  "EV_BUILD_DESTROY",
  "EV_BUILD_DELAY",     // can't build yet
  "EV_BUILD_REPAIR",    // repairing buildable
  "EV_BUILD_REPAIRED",  // buildable has full health
  "EV_HUMAN_BUILDABLE_EXPLOSION",
  "EV_ALIEN_BUILDABLE_EXPLOSION",
  "EV_ALIEN_ACIDTUBE",
  "EV_ALIEN_SLIME",
  "EV_FORCE_FIELD",
  "EV_MEDKIT_USED",
  "EV_ALIEN_EVOLVE",
  "EV_ALIEN_EVOLVE_FAILED",
  "EV_DEBUG_LINE",
  "EV_STOPLOOPINGSOUND",
  "EV_TAUNT",
  "EV_HUMMEL",
  "EV_OVERMIND_ATTACK", // overmind under attack
  "EV_OVERMIND_DYING",  // overmind close to death
  "EV_OVERMIND_SPAWNS", // overmind needs spawns
  "EV_DCC_ATTACK",      // dcc under attack
  "EV_MGTURRET_SPINUP", // trigger a sound
  "EV_RPTUSE_SOUND",    // trigger a sound
  "EV_LEV2_ZAP",
  "EV_ACIDBOMB_BOUNCE"
};

/*
===============
BG_EventName
===============
*/
const char *BG_EventName( int num )
{
  if( num < 0 || num >= sizeof( eventnames ) / sizeof( char * ) )
    return "UNKNOWN";

  return eventnames[ num ];
}

/*
===============
BG_AddPredictableEventToPlayerstate

Handles the sequence numbers
===============
*/

void  trap_Cvar_VariableStringBuffer( const char *var_name, char *buffer, int bufsize );

void BG_AddPredictableEventToPlayerstate( int newEvent, int eventParm, playerState_t *ps )
{
#ifdef _DEBUG
  {
    char buf[ 256 ];
    trap_Cvar_VariableStringBuffer( "showevents", buf, sizeof( buf ) );

    if( atof( buf ) != 0 )
    {
#ifdef GAME
      Com_Printf( " game event svt %5d -> %5d: num = %20s parm %d\n",
                  ps->pmove_framecount/*ps->commandTime*/, ps->eventSequence,
                  BG_EventName( newEvent ), eventParm );
#else
      Com_Printf( "Cgame event svt %5d -> %5d: num = %20s parm %d\n",
                  ps->pmove_framecount/*ps->commandTime*/, ps->eventSequence,
                  BG_EventName( newEvent ), eventParm );
#endif
    }
  }
#endif
  ps->events[ ps->eventSequence & ( MAX_PS_EVENTS - 1 ) ] = newEvent;
  ps->eventParms[ ps->eventSequence & ( MAX_PS_EVENTS - 1 ) ] = eventParm;
  ps->eventSequence++;
}


/*
========================
BG_PlayerStateToEntityState

This is done after each set of usercmd_t on the server,
and after local prediction on the client
========================
*/
void BG_PlayerStateToEntityState( playerState_t *ps, entityState_t *s, qboolean snap )
{
  int     i;

  if( ps->pm_type == PM_INTERMISSION || ps->pm_type == PM_SPECTATOR || ps->pm_type == PM_FREEZE )
    s->eType = ET_INVISIBLE;
  else if( ps->persistant[ PERS_SPECSTATE ] != SPECTATOR_NOT )
    s->eType = ET_INVISIBLE;
  else
    s->eType = ET_PLAYER;

  s->number = ps->clientNum;

  s->pos.trType = TR_INTERPOLATE;
  VectorCopy( ps->origin, s->pos.trBase );

  if( snap )
    SnapVector( s->pos.trBase );

  //set the trDelta for flag direction
  VectorCopy( ps->velocity, s->pos.trDelta );

  s->apos.trType = TR_INTERPOLATE;
  VectorCopy( ps->viewangles, s->apos.trBase );

  if( snap )
    SnapVector( s->apos.trBase );

  s->time2 = ps->movementDir;
  s->legsAnim = ps->legsAnim;
  s->torsoAnim = ps->torsoAnim;
  s->weaponAnim = ps->weaponAnim;
  s->clientNum = ps->clientNum;   // ET_PLAYER looks here instead of at number
                    // so corpses can also reference the proper config
  s->eFlags = ps->eFlags;
  if( ps->stats[STAT_HEALTH] <= 0 )
    s->eFlags |= EF_DEAD;
  else
    s->eFlags &= ~EF_DEAD;

  if( ps->externalEvent )
  {
    s->event = ps->externalEvent;
    s->eventParm = ps->externalEventParm;
  }
  else if( ps->entityEventSequence < ps->eventSequence )
  {
    int   seq;

    if( ps->entityEventSequence < ps->eventSequence - MAX_PS_EVENTS )
      ps->entityEventSequence = ps->eventSequence - MAX_PS_EVENTS;

    seq = ps->entityEventSequence & ( MAX_PS_EVENTS - 1 );
    s->event = ps->events[ seq ] | ( ( ps->entityEventSequence & 3 ) << 8 );
    s->eventParm = ps->eventParms[ seq ];
    ps->entityEventSequence++;
  }

  s->weapon = ps->weapon;
  s->groundEntityNum = ps->groundEntityNum;

  //store items held and active items in modelindex and modelindex2
  s->modelindex = 0;
  s->modelindex2 = 0;
  for( i = UP_NONE + 1; i < UP_NUM_UPGRADES; i++ )
  {
    if( BG_InventoryContainsUpgrade( i, ps->stats ) )
    {
      s->modelindex |= 1 << i;

      if( BG_UpgradeIsActive( i, ps->stats ) )
        s->modelindex2 |= 1 << i;
    }
  }

  // use misc field to store team/class info:
  s->misc = ps->stats[ STAT_TEAM ] | ( ps->stats[ STAT_CLASS ] << 8 );

  // have to get the surfNormal through somehow...
  VectorCopy( ps->grapplePoint, s->angles2 );

  s->loopSound = ps->loopSound;
  s->generic1 = ps->generic1;

  if( s->generic1 <= WPM_NONE || s->generic1 >= WPM_NUM_WEAPONMODES )
    s->generic1 = WPM_PRIMARY;

  s->otherEntityNum = ps->otherEntityNum;  
}

/*
========================
BG_PlayerStateToEntityStateExtraPolate
This is done after each set of usercmd_t on the server,
and after local prediction on the client
========================
*/
void BG_PlayerStateToEntityStateExtraPolate( playerState_t *ps, entityState_t *s, int time, qboolean snap )
{
  int     i;

  if( ps->pm_type == PM_INTERMISSION || ps->pm_type == PM_SPECTATOR || ps->pm_type == PM_FREEZE )
    s->eType = ET_INVISIBLE;
  else if( ps->persistant[ PERS_SPECSTATE ] != SPECTATOR_NOT )
    s->eType = ET_INVISIBLE;
  else
    s->eType = ET_PLAYER;

  s->number = ps->clientNum;

  s->pos.trType = TR_LINEAR_STOP;
  VectorCopy( ps->origin, s->pos.trBase );

  if( snap )
    SnapVector( s->pos.trBase );

  // set the trDelta for flag direction and linear prediction
  VectorCopy( ps->velocity, s->pos.trDelta );
  // set the time for linear prediction
  s->pos.trTime = time;
  // set maximum extra polation time
  s->pos.trDuration = 50; // 1000 / sv_fps (default = 20)

  s->apos.trType = TR_INTERPOLATE;
  VectorCopy( ps->viewangles, s->apos.trBase );
  if( snap )
    SnapVector( s->apos.trBase );

  s->time2 = ps->movementDir;
  s->legsAnim = ps->legsAnim;
  s->torsoAnim = ps->torsoAnim;
  s->weaponAnim = ps->weaponAnim;
  s->clientNum = ps->clientNum;   // ET_PLAYER looks here instead of at number
                    // so corpses can also reference the proper config
  s->eFlags = ps->eFlags;

  if( ps->stats[STAT_HEALTH] <= 0 )
    s->eFlags |= EF_DEAD;
  else
    s->eFlags &= ~EF_DEAD;

  if( ps->externalEvent )
  {
    s->event = ps->externalEvent;
    s->eventParm = ps->externalEventParm;
  }
  else if( ps->entityEventSequence < ps->eventSequence )
  {
    int   seq;

    if( ps->entityEventSequence < ps->eventSequence - MAX_PS_EVENTS )
      ps->entityEventSequence = ps->eventSequence - MAX_PS_EVENTS;

    seq = ps->entityEventSequence & ( MAX_PS_EVENTS - 1 );
    s->event = ps->events[ seq ] | ( ( ps->entityEventSequence & 3 ) << 8 );
    s->eventParm = ps->eventParms[ seq ];
    ps->entityEventSequence++;
  }

  s->weapon = ps->weapon;
  s->groundEntityNum = ps->groundEntityNum;

  //store items held and active items in modelindex and modelindex2
  s->modelindex = 0;
  s->modelindex2 = 0;

  for( i = UP_NONE + 1; i < UP_NUM_UPGRADES; i++ )
  {
    if( BG_InventoryContainsUpgrade( i, ps->stats ) )
    {
      s->modelindex |= 1 << i;

      if( BG_UpgradeIsActive( i, ps->stats ) )
        s->modelindex2 |= 1 << i;
    }
  }

  // use misc field to store team/class info:
  s->misc = ps->stats[ STAT_TEAM ] | ( ps->stats[ STAT_CLASS ] << 8 );

  // have to get the surfNormal through somehow...
  VectorCopy( ps->grapplePoint, s->angles2 );

  s->loopSound = ps->loopSound;
  s->generic1 = ps->generic1;

  if( s->generic1 <= WPM_NONE || s->generic1 >= WPM_NUM_WEAPONMODES )
    s->generic1 = WPM_PRIMARY;

  s->otherEntityNum = ps->otherEntityNum;
}

/*
========================
BG_WeaponIsFull
Check if a weapon has full ammo
========================
*/
qboolean BG_WeaponIsFull( weapon_t weapon, int stats[ ], int ammo, int clips )
{
  int maxAmmo, maxClips;

  maxAmmo = BG_Weapon( weapon )->maxAmmo;
  maxClips = BG_Weapon( weapon )->maxClips;

  if( BG_InventoryContainsUpgrade( UP_BATTPACK, stats ) )
    maxAmmo = (int)( (float)maxAmmo * BATTPACK_MODIFIER );


  return ( maxAmmo == ammo ) && ( maxClips == clips );
}

/*
========================
BG_InventoryContainsWeapon
Does the player hold a weapon?
========================
*/
qboolean BG_InventoryContainsWeapon( int weapon, int stats[ ] )
{
  // humans always have a blaster
  if( stats[ STAT_TEAM ] == TEAM_HUMANS && weapon == WP_BLASTER )
    return qtrue;

  return ( stats[ STAT_WEAPON ] == weapon );
}

/*
========================
BG_SlotsForInventory
Calculate the slots used by an inventory and warn of conflicts
========================
*/
int BG_SlotsForInventory( int stats[ ] )
{
  int i, slot, slots;

  slots = BG_Weapon( stats[ STAT_WEAPON ] )->slots;
  if( stats[ STAT_TEAM ] == TEAM_HUMANS )
    slots |= BG_Weapon( WP_BLASTER )->slots;

  for( i = UP_NONE; i < UP_NUM_UPGRADES; i++ )
  {
    if( BG_InventoryContainsUpgrade( i, stats ) )
    {
      slot = BG_Upgrade( i )->slots;

      // this check should never be true
      if( slots & slot )
      {
        Com_Printf( S_COLOR_YELLOW "WARNING: held item %d conflicts with "
                    "inventory slot %d\n", i, slot );
      }

      slots |= slot;
    }
  }
  return slots;
}

/*
========================
BG_AddUpgradeToInventory
Give the player an upgrade
========================
*/
void BG_AddUpgradeToInventory( int item, int stats[ ] )
{
  stats[ STAT_ITEMS ] |= ( 1 << item );
}

/*
========================
BG_RemoveUpgradeFromInventory
Take an upgrade from the player
========================
*/
void BG_RemoveUpgradeFromInventory( int item, int stats[ ] )
{
  stats[ STAT_ITEMS ] &= ~( 1 << item );
}

/*
========================
BG_InventoryContainsUpgrade
Does the player hold an upgrade?
========================
*/
qboolean BG_InventoryContainsUpgrade( int item, int stats[ ] )
{
  return( stats[ STAT_ITEMS ] & ( 1 << item ) );
}

/*
========================
BG_ActivateUpgrade
Activates an upgrade
========================
*/
void BG_ActivateUpgrade( int item, int stats[ ] )
{
  stats[ STAT_ACTIVEITEMS ] |= ( 1 << item );
}

/*
========================
BG_DeactivateUpgrade
Deactivates an upgrade
========================
*/
void BG_DeactivateUpgrade( int item, int stats[ ] )
{
  stats[ STAT_ACTIVEITEMS ] &= ~( 1 << item );
}

/*
========================
BG_UpgradeIsActive
Is this upgrade active?
========================
*/
qboolean BG_UpgradeIsActive( int item, int stats[ ] )
{
  return( stats[ STAT_ACTIVEITEMS ] & ( 1 << item ) );
}

/*
===============
BG_RotateAxis
Shared axis rotation function
===============
*/
qboolean BG_RotateAxis( vec3_t surfNormal, vec3_t inAxis[ 3 ],
                        vec3_t outAxis[ 3 ], qboolean inverse, qboolean ceiling )
{
  vec3_t  refNormal = { 0.0f, 0.0f, 1.0f };
  vec3_t  ceilingNormal = { 0.0f, 0.0f, -1.0f };
  vec3_t  localNormal, xNormal;
  float   rotAngle;

  //the grapplePoint being a surfNormal rotation Normal hack... see above :)
  if( ceiling )
  {
    VectorCopy( ceilingNormal, localNormal );
    VectorCopy( surfNormal, xNormal );
  }
  else
  {
    //cross the reference normal and the surface normal to get the rotation axis
    VectorCopy( surfNormal, localNormal );
    CrossProduct( localNormal, refNormal, xNormal );
    VectorNormalize( xNormal );
  }

  //can't rotate with no rotation vector
  if( VectorLength( xNormal ) != 0.0f )
  {
    rotAngle = RAD2DEG( acos( DotProduct( localNormal, refNormal ) ) );

    if( inverse )
      rotAngle = -rotAngle;

    AngleNormalize180( rotAngle );

    //hmmm could get away with only one rotation and some clever stuff later... but i'm lazy
    RotatePointAroundVector( outAxis[ 0 ], xNormal, inAxis[ 0 ], -rotAngle );
    RotatePointAroundVector( outAxis[ 1 ], xNormal, inAxis[ 1 ], -rotAngle );
    RotatePointAroundVector( outAxis[ 2 ], xNormal, inAxis[ 2 ], -rotAngle );
  }
  else
    return qfalse;
  return qtrue;
}

/*
===============
BG_GetClientNormal
Get the normal for the surface the client is walking on
===============
*/
void BG_GetClientNormal( const playerState_t *ps, vec3_t normal )
{
  if( ps->stats[ STAT_STATE ] & SS_WALLCLIMBING )
  {
    if( ps->eFlags & EF_WALLCLIMBCEILING )
      VectorSet( normal, 0.0f, 0.0f, -1.0f );
    else
      VectorCopy( ps->grapplePoint, normal );
  }
  else
    VectorSet( normal, 0.0f, 0.0f, 1.0f );
}

/*
===============
BG_GetClientViewOrigin
Get the position of the client's eye, based on the client's position, the surface's normal, and client's view height
===============
*/
void BG_GetClientViewOrigin( const playerState_t *ps, vec3_t viewOrigin )
{
  vec3_t normal;
  BG_GetClientNormal( ps, normal );
  VectorMA( ps->origin, ps->viewheight, normal, viewOrigin );
}

/*
===============
BG_PositionBuildableRelativeToPlayer
Find a place to build a buildable
===============
*/
void BG_PositionBuildableRelativeToPlayer( const playerState_t *ps,
                                           const vec3_t mins, const vec3_t maxs,
                                           void (*trace)( trace_t *, const vec3_t, const vec3_t,
                                                          const vec3_t, const vec3_t, int, int ),
                                           vec3_t outOrigin, vec3_t outAngles, trace_t *tr )
{
  vec3_t  forward, entityOrigin, targetOrigin;
  vec3_t  angles, playerOrigin, playerNormal;
  float   buildDist;

  BG_GetClientNormal( ps, playerNormal );

  VectorCopy( ps->viewangles, angles );
  VectorCopy( ps->origin, playerOrigin );
  buildDist = BG_Class( ps->stats[ STAT_CLASS ] )->buildDist;

  AngleVectors( angles, forward, NULL, NULL );
  ProjectPointOnPlane( forward, forward, playerNormal );
  VectorNormalize( forward );

  VectorMA( playerOrigin, buildDist, forward, entityOrigin );

  VectorCopy( entityOrigin, targetOrigin );

  //so buildings can be placed facing slopes
  VectorMA( entityOrigin, 32, playerNormal, entityOrigin );

  //so buildings drop to floor
  VectorMA( targetOrigin, -128, playerNormal, targetOrigin );

  // The mask is MASK_DEADSOLID on purpose to avoid collisions with other entities
  (*trace)( tr, entityOrigin, mins, maxs, targetOrigin, ps->clientNum, MASK_PLAYERSOLID );
  VectorCopy( tr->endpos, entityOrigin );
  VectorMA( entityOrigin, 0.1f, playerNormal, outOrigin );
  vectoangles( forward, outAngles );
}

/*
===============
BG_GetValueOfPlayer
Returns the credit value of a player
===============
*/
int BG_GetValueOfPlayer( playerState_t *ps )
{
  int i, worth = 0;
  
  worth = BG_Class( ps->stats[ STAT_CLASS ] )->value;

  // Humans have worth from their equipment as well
  if( ps->stats[ STAT_TEAM ] == TEAM_HUMANS )
  {
    for( i = UP_NONE + 1; i < UP_NUM_UPGRADES; i++ )
    {
      if( BG_InventoryContainsUpgrade( i, ps->stats ) )
        worth += BG_Upgrade( i )->price;
    }

    for( i = WP_NONE + 1; i < WP_NUM_WEAPONS; i++ )
    {
      if( BG_InventoryContainsWeapon( i, ps->stats ) )
        worth += BG_Weapon( i )->price;
    }
  }
  return worth;
}

/*
=================
BG_PlayerCanChangeWeapon
=================
*/
qboolean BG_PlayerCanChangeWeapon( playerState_t *ps )
{
  // Do not allow Lucifer Cannon "canceling" via weapon switch
  if( ps->weapon == WP_LUCIFER_CANNON &&
      ps->stats[ STAT_MISC ] > LCANNON_CHARGE_TIME_MIN )
    return qfalse;

  return ps->weaponTime <= 0 || ps->weaponstate != WEAPON_FIRING;
}

/*
=================
BG_GetPlayerWeapon

Returns the players current weapon or the weapon they are switching to.
Only needs to be used for human weapons.
=================
*/
weapon_t BG_GetPlayerWeapon( playerState_t *ps )
{
  if( ps->persistant[ PERS_NEWWEAPON ] )
    return ps->persistant[ PERS_NEWWEAPON ];

  return ps->weapon;
}

/*
===============
atof_neg
atof with an allowance for negative values
===============
*/
float atof_neg( char *token, qboolean allowNegative )
{
  float value;

  value = atof( token );

  if( !allowNegative && value < 0.0f )
    value = 1.0f;

  return value;
}

/*
===============
atoi_neg
atoi with an allowance for negative values
===============
*/
int atoi_neg( char *token, qboolean allowNegative )
{
  int value;

  value = atoi( token );

  if( !allowNegative && value < 0 )
    value = 1;

  return value;
}

#define MAX_NUM_PACKED_ENTITY_NUMS 10

/*
===============
BG_PackEntityNumbers
Pack entity numbers into an entityState_t
===============
*/
void BG_PackEntityNumbers( entityState_t *es, const int *entityNums, int count )
{
  int i;

  if( count > MAX_NUM_PACKED_ENTITY_NUMS )
  {
    count = MAX_NUM_PACKED_ENTITY_NUMS;
    Com_Printf( S_COLOR_YELLOW "WARNING: A maximum of %d entity numbers can be "
      "packed, but BG_PackEntityNumbers was passed %d entities",
      MAX_NUM_PACKED_ENTITY_NUMS, count );
  }

  es->misc = es->time = es->time2 = es->constantLight = 0;

  for( i = 0; i < MAX_NUM_PACKED_ENTITY_NUMS; i++ )
  {
    int entityNum;

    if( i < count )
      entityNum = entityNums[ i ];
    else
      entityNum = ENTITYNUM_NONE;

    if( entityNum & ~GENTITYNUM_MASK )
    {
      Com_Error( ERR_FATAL, "BG_PackEntityNumbers passed an entity number (%d) which "
        "exceeds %d bits", entityNum, GENTITYNUM_BITS );
    }

    switch( i )
    {
      case 0: es->misc |= entityNum;                                       break;
      case 1: es->time |= entityNum;                                       break;
      case 2: es->time |= entityNum << GENTITYNUM_BITS;                    break;
      case 3: es->time |= entityNum << (GENTITYNUM_BITS * 2);              break;
      case 4: es->time2 |= entityNum;                                      break;
      case 5: es->time2 |= entityNum << GENTITYNUM_BITS;                   break;
      case 6: es->time2 |= entityNum << (GENTITYNUM_BITS * 2);             break;
      case 7: es->constantLight |= entityNum;                              break;
      case 8: es->constantLight |= entityNum << GENTITYNUM_BITS;           break;
      case 9: es->constantLight |= entityNum << (GENTITYNUM_BITS * 2);     break;
      default: Com_Error( ERR_FATAL, "Entity index %d not handled", i );   break;
    }
  }
}

/*
===============
BG_UnpackEntityNumbers
Unpack entity numbers from an entityState_t
===============
*/
int BG_UnpackEntityNumbers( entityState_t *es, int *entityNums, int count )
{
  int i;

  if( count > MAX_NUM_PACKED_ENTITY_NUMS )
    count = MAX_NUM_PACKED_ENTITY_NUMS;

  for( i = 0; i < count; i++ )
  {
    int *entityNum = &entityNums[ i ];

    switch( i )
    {
      case 0: *entityNum = es->misc;                                      break;
      case 1: *entityNum = es->time;                                      break;
      case 2: *entityNum = (es->time >> GENTITYNUM_BITS);                 break;
      case 3: *entityNum = (es->time >> (GENTITYNUM_BITS * 2));           break;
      case 4: *entityNum = es->time2;                                     break;
      case 5: *entityNum = (es->time2 >> GENTITYNUM_BITS);                break;
      case 6: *entityNum = (es->time2 >> (GENTITYNUM_BITS * 2));          break;
      case 7: *entityNum = es->constantLight;                             break;
      case 8: *entityNum = (es->constantLight >> GENTITYNUM_BITS);        break;
      case 9: *entityNum = (es->constantLight >> (GENTITYNUM_BITS * 2));  break;
      default: Com_Error( ERR_FATAL, "Entity index %d not handled", i );  break;
    }
    *entityNum &= GENTITYNUM_MASK;

    if( *entityNum == ENTITYNUM_NONE )
      break;
  }
  return i;
}

/*
===============
BG_ParseCSVEquipmentList
===============
*/
void BG_ParseCSVEquipmentList( const char *string, weapon_t *weapons, int weaponsSize,
    upgrade_t *upgrades, int upgradesSize )
{
  char      buffer[ MAX_STRING_CHARS ];
  int       i = 0, j = 0;
  char      *p, *q;
  qboolean  EOS = qfalse;

  Q_strncpyz( buffer, string, MAX_STRING_CHARS );

  p = q = buffer;

  while( *p != '\0' )
  {
    //skip to first , or EOS
    while( *p != ',' && *p != '\0' )
      p++;

    if( *p == '\0' )
      EOS = qtrue;

    *p = '\0';

    //strip leading whitespace
    while( *q == ' ' )
      q++;

    if( weaponsSize )
      weapons[ i ] = BG_WeaponByName( q )->number;

    if( upgradesSize )
      upgrades[ j ] = BG_UpgradeByName( q )->number;

    if( weaponsSize && weapons[ i ] == WP_NONE &&
        upgradesSize && upgrades[ j ] == UP_NONE )
      Com_Printf( S_COLOR_YELLOW "WARNING: unknown equipment %s\n", q );
    else if( weaponsSize && weapons[ i ] != WP_NONE )
      i++;
    else if( upgradesSize && upgrades[ j ] != UP_NONE )
      j++;

    if( !EOS )
    {
      p++;
      q = p;
    }
    else
      break;

    if( i == ( weaponsSize - 1 ) || j == ( upgradesSize - 1 ) )
      break;
  }

  if( weaponsSize )
    weapons[ i ] = WP_NONE;

  if( upgradesSize )
    upgrades[ j ] = UP_NONE;
}

/*
===============
BG_ParseCSVClassList
===============
*/
void BG_ParseCSVClassList( const char *string, class_t *classes, int classesSize )
{
  char      buffer[ MAX_STRING_CHARS ];
  int       i = 0;
  char      *p, *q;
  qboolean  EOS = qfalse;

  Q_strncpyz( buffer, string, MAX_STRING_CHARS );

  p = q = buffer;

  while( *p != '\0' && i < classesSize - 1 )
  {
    //skip to first , or EOS
    while( *p != ',' && *p != '\0' )
      p++;

    if( *p == '\0' )
      EOS = qtrue;

    *p = '\0';

    //strip leading whitespace
    while( *q == ' ' )
      q++;

    classes[ i ] = BG_ClassByName( q )->number;

    if( classes[ i ] == PCL_NONE )
      Com_Printf( S_COLOR_YELLOW "WARNING: unknown class %s\n", q );
    else
      i++;

    if( !EOS )
    {
      p++;
      q = p;
    }
    else
      break;
  }

  classes[ i ] = PCL_NONE;
}

/*
===============
BG_ParseCSVBuildableList
===============
*/
void BG_ParseCSVBuildableList( const char *string, buildable_t *buildables, int buildablesSize )
{
  char      buffer[ MAX_STRING_CHARS ];
  int       i = 0;
  char      *p, *q;
  qboolean  EOS = qfalse;

  Q_strncpyz( buffer, string, MAX_STRING_CHARS );

  p = q = buffer;

  while( *p != '\0' && i < buildablesSize - 1 )
  {
    //skip to first , or EOS
    while( *p != ',' && *p != '\0' )
      p++;

    if( *p == '\0' )
      EOS = qtrue;

    *p = '\0';

    //strip leading whitespace
    while( *q == ' ' )
      q++;

    buildables[ i ] = BG_BuildableByName( q )->number;

    if( buildables[ i ] == BA_NONE )
      Com_Printf( S_COLOR_YELLOW "WARNING: unknown buildable %s\n", q );
    else
      i++;

    if( !EOS )
    {
      p++;
      q = p;
    }
    else
      break;
  }

  buildables[ i ] = BA_NONE;
}

typedef struct gameElements_s
{
  buildable_t       buildables[ BA_NUM_BUILDABLES ];
  class_t           classes[ PCL_NUM_CLASSES ];
  weapon_t          weapons[ WP_NUM_WEAPONS ];
  upgrade_t         upgrades[ UP_NUM_UPGRADES ];
} gameElements_t;

static gameElements_t bg_disabledGameElements;

/*
============
BG_InitAllowedGameElements
============
*/
void BG_InitAllowedGameElements( void )
{
  char cvar[ MAX_CVAR_VALUE_STRING ];

  trap_Cvar_VariableStringBuffer( "g_disabledEquipment",
      cvar, MAX_CVAR_VALUE_STRING );

  BG_ParseCSVEquipmentList( cvar,
      bg_disabledGameElements.weapons, WP_NUM_WEAPONS,
      bg_disabledGameElements.upgrades, UP_NUM_UPGRADES );

  trap_Cvar_VariableStringBuffer( "g_disabledClasses",
      cvar, MAX_CVAR_VALUE_STRING );

  BG_ParseCSVClassList( cvar,
      bg_disabledGameElements.classes, PCL_NUM_CLASSES );

  trap_Cvar_VariableStringBuffer( "g_disabledBuildables",
      cvar, MAX_CVAR_VALUE_STRING );

  BG_ParseCSVBuildableList( cvar,
      bg_disabledGameElements.buildables, BA_NUM_BUILDABLES );
}

/*
============
BG_WeaponIsAllowed
============
*/
qboolean BG_WeaponIsAllowed( weapon_t weapon )
{
  int i;

  for( i = 0; i < WP_NUM_WEAPONS &&
      bg_disabledGameElements.weapons[ i ] != WP_NONE; i++ )
  {
    if( bg_disabledGameElements.weapons[ i ] == weapon )
      return qfalse;
  }

  return qtrue;
}

/*
============
BG_UpgradeIsAllowed
============
*/
qboolean BG_UpgradeIsAllowed( upgrade_t upgrade )
{
  int i;

  for( i = 0; i < UP_NUM_UPGRADES &&
      bg_disabledGameElements.upgrades[ i ] != UP_NONE; i++ )
  {
    if( bg_disabledGameElements.upgrades[ i ] == upgrade )
      return qfalse;
  }

  return qtrue;
}

/*
============
BG_ClassIsAllowed
============
*/
qboolean BG_ClassIsAllowed( class_t class )
{
  int i;

  for( i = 0; i < PCL_NUM_CLASSES &&
      bg_disabledGameElements.classes[ i ] != PCL_NONE; i++ )
  {
    if( bg_disabledGameElements.classes[ i ] == class )
      return qfalse;
  }

  return qtrue;
}

/*
============
BG_BuildableIsAllowed
============
*/
qboolean BG_BuildableIsAllowed( buildable_t buildable )
{
  int i;

  for( i = 0; i < BA_NUM_BUILDABLES &&
      bg_disabledGameElements.buildables[ i ] != BA_NONE; i++ )
  {
    if( bg_disabledGameElements.buildables[ i ] == buildable )
      return qfalse;
  }

  return qtrue;
}

/*
============
BG_PrimaryWeapon
============
*/
weapon_t BG_PrimaryWeapon( int stats[ ] )
{
  int i;

  for( i = WP_NONE; i < WP_NUM_WEAPONS; i++ )
  {
    if( BG_Weapon( i )->slots != SLOT_WEAPON )
      continue;
    if( BG_InventoryContainsWeapon( i, stats ) )
      return i;
  }

  if( BG_InventoryContainsWeapon( WP_BLASTER, stats ) )
    return WP_BLASTER;

  return WP_NONE;
}

/*
============
BG_LoadEmoticons
============
*/
int BG_LoadEmoticons( emoticon_t *emoticons, int num )
{
  int numFiles;
  char fileList[ MAX_EMOTICONS * ( MAX_EMOTICON_NAME_LEN + 9 ) ] = {""};
  int i;
  char *filePtr;
  int fileLen;
  int count;

  numFiles = trap_FS_GetFileList( "emoticons", "x1.tga", fileList,
    sizeof( fileList ) );

  if( numFiles < 1 )
    return 0;

  filePtr = fileList;
  fileLen = 0;
  count = 0;
  for( i = 0; i < numFiles && count < num; i++, filePtr += fileLen + 1 )
  {
    fileLen = strlen( filePtr );
    if( fileLen < 9 || filePtr[ fileLen - 8 ] != '_' ||
        filePtr[ fileLen - 7 ] < '1' || filePtr[ fileLen - 7 ] > '9' )
    {
      Com_Printf( S_COLOR_YELLOW "skipping invalidly named emoticon \"%s\"\n",
        filePtr );
      continue;
    }
    if( fileLen - 8 > MAX_EMOTICON_NAME_LEN )
    {
      Com_Printf( S_COLOR_YELLOW "emoticon file name \"%s\" too long (>%d)\n",
        filePtr, MAX_EMOTICON_NAME_LEN + 8 );
      continue;
    }
    if( !trap_FS_FOpenFile( va( "emoticons/%s", filePtr ), NULL, FS_READ ) )
    {
      Com_Printf( S_COLOR_YELLOW "could not open \"emoticons/%s\"\n", filePtr );
      continue;
    }

    Q_strncpyz( emoticons[ count ].name, filePtr, fileLen - 8 + 1 );
#ifndef GAME
    emoticons[ count ].width = filePtr[ fileLen - 7 ] - '0';
#endif
    count++;
  }

  Com_Printf( "Loaded %d of %d emoticons (MAX_EMOTICONS is %d)\n",
    count, numFiles, MAX_EMOTICONS );
  return count;
}

/*
============
BG_TeamName
============
*/
char *BG_TeamName( team_t team )
{
  if( team == TEAM_NONE )
    return "spectator";
  if( team == TEAM_ALIENS )
    return "alien";
  if( team == TEAM_HUMANS )
    return "human";
  return "<team>";
}

int cmdcmp( const void *a, const void *b )
{
  return Q_stricmp( (const char *)a, ((dummyCmd_t *)b)->name );
}

/*
============
BG_ForceFieldForEntity
============
*/
qboolean BG_ForceFieldForEntity( playerState_t *ps, entityState_t *es, forceField_t *ff )
{
  if( es->eType == ET_BUILDABLE )
  {
    if( !( es->eFlags & EF_B_POWERED ) )
      return qfalse;

    if( !( es->eFlags & EF_B_SPAWNED ) )
      return qfalse;

    // health
    if( es->generic1 <= 0 )
      return qfalse;

    switch( es->modelindex )
    {
      case BA_H_LIGHT: //force field
        if( ps && ps->stats[ STAT_TEAM ] != TEAM_ALIENS )
          return qfalse;

        ff->type = 0;
        VectorCopy( es->origin, ff->origin );
        ff->range = LIGHT_RANGE;
        ff->force = LIGHT_FORCE;

        return qtrue;
    }
  }

  return qfalse;
}
