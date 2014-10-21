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

/*
 * GENERAL
 */

#define MIN_FALL_DISTANCE           35.0f 		//the fall distance at which fall damage kicks in
#define MAX_FALL_DISTANCE           120.0f 		//the fall distance at which maximum damage is dealt
#define AVG_FALL_DISTANCE           ((MIN_FALL_DISTANCE+MAX_FALL_DISTANCE)/2.0f)
#define DEFAULT_FREEKILL_PERIOD     "120" 		//seconds
#define FREEKILL_ALIEN              ALIEN_CREDITS_PER_KILL
#define FREEKILL_HUMAN              LEVEL0_VALUE
#define DEFAULT_ALIEN_BUILDPOINTS   "150"
#define DEFAULT_ALIEN_QUEUE_TIME    "12000"
#define DEFAULT_ALIEN_STAGE2_THRESH "6000"
#define DEFAULT_ALIEN_STAGE3_THRESH "12000"
#define DEFAULT_ALIEN_STAGE4_THRESH "18000"
#define DEFAULT_ALIEN_STAGE5_THRESH "24000"
#define DEFAULT_ALIEN_MAX_STAGE     "4"
#define DEFAULT_HUMAN_BUILDPOINTS   "150"
#define DEFAULT_HUMAN_QUEUE_TIME    "8000"
#define DEFAULT_HUMAN_STAGE2_THRESH "3000"
#define DEFAULT_HUMAN_STAGE3_THRESH "6000"
#define DEFAULT_HUMAN_STAGE4_THRESH "9000"
#define DEFAULT_HUMAN_STAGE5_THRESH "12000"
#define DEFAULT_HUMAN_MAX_STAGE     "4"
#define DAMAGE_FRACTION_FOR_KILL    0.5f 		//how much damage players (versus structures) need to
#define MAXIMUM_BUILD_TIME          30000 		// used for pie timer

/*
 * ALIEN 
 */

/*
 * ALIEN classes
 * _SPEED   - fraction of Q3A run speed the class can move
 * _REGEN   - health per second regained
 * ALIEN_HLTH_MODIFIER - overall health modifier for coarse tuning
 * ALIENSENSE_RANGE - the distance alien sense is useful for
 */
#define ALIEN_HLTH_MODIFIER         1.0f
#define AHM(h)                      ((int)((float)h*ALIEN_HLTH_MODIFIER))
#define ALIEN_VALUE_MODIFIER        1.0f
#define AVM(h)                      ((int)((float)h*ALIEN_VALUE_MODIFIER))

#define ALIENSENSE_RANGE            1000.0f
#define REGEN_BOOST_RANGE           200.0f
#define ALIEN_POISON_TIME           10000
#define ALIEN_POISON_DMG            5
#define ALIEN_POISON_DIVIDER        (1.0f/1.32f) 	//about 1.0/(time`th root of damage)
#define ALIEN_INFECTION_TIME        7000
#define ALIEN_INFECTION_DMG         2
#define ALIEN_SPAWN_REPEAT_TIME     11000
#define ALIEN_REGEN_DAMAGE_TIME     2000 			//msec since damage that regen starts again
#define ALIEN_REGEN_NOCREEP_MOD     (1.0f/3.0f) 	//regen off creep
#define ALIEN_MAX_FRAGS             18
#define ALIEN_MAX_CREDITS           (ALIEN_MAX_FRAGS*ALIEN_CREDITS_PER_KILL)
#define ALIEN_CREDITS_PER_KILL      400
#define ALIEN_TK_SUICIDE_PENALTY    150

/*
 * ALIEN weapons
 * _REPEAT  - time in msec until the weapon can be used again
 * _DMG     - amount of damage the weapon does
 * ALIEN_WDMG_MODIFIER - overall damage modifier for coarse tuning
 */
#define ALIEN_WDMG_MODIFIER         1.0f
#define ADM(d)                      ((int)((float)d*ALIEN_WDMG_MODIFIER))

//Granger
#define ABUILDER_SPEED              1.0f
#define ABUILDER_VALUE              AVM(240)
#define ABUILDER_HEALTH             AHM(50)
#define ABUILDER_REGEN              (0.04f * ABUILDER_HEALTH)
#define ABUILDER_COST               0
#define ABUILDER_UPG_SPEED          1.2f
#define ABUILDER_UPG_VALUE          AVM(300)
#define ABUILDER_UPG_HEALTH         AHM(75)
#define ABUILDER_UPG_REGEN          (0.04f * ABUILDER_UPG_HEALTH)
#define ABUILDER_UPG_COST           0

#define ABUILDER_BUILD_REPEAT       1000
#define ABUILDER_CLAW_DMG           ADM(20)
#define ABUILDER_CLAW_RANGE         64.0f
#define ABUILDER_CLAW_WIDTH         4.0f
#define ABUILDER_CLAW_REPEAT        1000
#define ABUILDER_CLAW_K_SCALE       1.0f
#define ABUILDER_BLOB_DMG           ADM(4)
#define ABUILDER_BLOB_REPEAT        1000
#define ABUILDER_BLOB_SPEED         800.0f
#define ABUILDER_BLOB_SPEED_MOD     0.5f
#define ABUILDER_BLOB_TIME          2000

//Dretch
#define LEVEL0_SPEED                1.4f
#define LEVEL0_VALUE                AVM(180)
#define LEVEL0_HEALTH               AHM(30)
#define LEVEL0_REGEN                (0.05f * LEVEL0_HEALTH)
#define LEVEL0_COST                 0
#define LEVEL0_UPG_SPEED            1.5f
#define LEVEL0_UPG_VALUE            AVM(200)
#define LEVEL0_UPG_HEALTH           AHM(40)
#define LEVEL0_UPG_REGEN            (0.06f * LEVEL0_UPG_HEALTH)
#define LEVEL0_UPG_COST             1
#define LEVEL0_UPG_BITE_RANGE       70.0f
#define LEVEL0_UPG_BITE_WIDTH       8.0f

#define LEVEL0_BITE_DMG             ADM(40)
#define LEVEL0_BITE_DMG_UPG         ADM(45) 
#define LEVEL0_BITE_RANGE           64.0f
#define LEVEL0_BITE_WIDTH           6.0f
#define LEVEL0_BITE_REPEAT          500
#define LEVEL0_BITE_K_SCALE         1.0f
#define LEVEL0_DRILL_RANGE          48.0f
#define LEVEL0_DRILL_WIDTH          14.0f
#define LEVEL0_DRILL_TIME           600      // msec for full Dretch pounce
#define LEVEL0_DRILL_TIME_MIN       200      // msec before which pounce cancels  
#define LEVEL0_DRILL_REPEAT         320      // msec before a new pounce starts
#define LEVEL0_DRILL_SPEED_MOD      0.75f    // walking speed modifier for pounce charging
#define LEVEL0_DRILL_JUMP_MAG       500      // Dretch pounce jump power

//Basilik
#define LEVEL1_SPEED                1.25f
#define LEVEL1_VALUE                AVM(270)
#define LEVEL1_HEALTH               AHM(60)
#define LEVEL1_REGEN                (0.03f * LEVEL1_HEALTH)
#define LEVEL1_COST                 1
#define LEVEL1_UPG_SPEED            1.25f
#define LEVEL1_UPG_VALUE            AVM(330)
#define LEVEL1_UPG_HEALTH           AHM(80)
#define LEVEL1_UPG_REGEN            (0.03f * LEVEL1_UPG_HEALTH)
#define LEVEL1_UPG_COST             1

#define LEVEL1_CLAW_DMG             ADM(32)
#define LEVEL1_CLAW_RANGE           64.0f
#define LEVEL1_CLAW_U_RANGE         LEVEL1_CLAW_RANGE + 3.0f
#define LEVEL1_CLAW_WIDTH           10.0f
#define LEVEL1_CLAW_REPEAT          600
#define LEVEL1_CLAW_U_REPEAT        500
#define LEVEL1_CLAW_K_SCALE         1.0f
#define LEVEL1_CLAW_U_K_SCALE       1.0f
#define LEVEL1_GRAB_RANGE           96.0f
#define LEVEL1_GRAB_U_RANGE         LEVEL1_GRAB_RANGE + 3.0f
#define LEVEL1_GRAB_TIME            300
#define LEVEL1_GRAB_U_TIME          300
#define LEVEL1_PCLOUD_DMG           ADM(5)
#define LEVEL1_PCLOUD_RANGE         150.0f
#define LEVEL1_PCLOUD_REPEAT        2000
#define LEVEL1_PCLOUD_TIME          10000
#define LEVEL1_REGEN_MOD            2.0f
#define LEVEL1_UPG_REGEN_MOD        3.0f
#define LEVEL1_REGEN_SCOREINC       AVM(100) // score added for healing per 10s
#define LEVEL1_UPG_REGEN_SCOREINC   AVM(200)
#define ABOMB_DMG                   ADM(45)
#define ABOMB_SPLASH_DMG            ADM(190)
#define ABOMB_SPLASH_RADIUS         200
#define ABOMB_SPEED                 400.0f
#define ABOMB_REPEAT                1000
#define ABOMB2_DMG                  ADM(40)
#define ABOMB2_SPLASH_DMG           ADM(165)
#define ABOMB2_SPLASH_RADIUS        180
#define ABOMB2_SPEED                500.0f
#define ABOMB2_REPEAT               1000

//Marauder
#define LEVEL2_SPEED                1.2f
#define LEVEL2_VALUE                AVM(420)
#define LEVEL2_HEALTH               AHM(150)
#define LEVEL2_REGEN                (0.03f * LEVEL2_HEALTH)
#define LEVEL2_COST                 1
#define LEVEL2_UPG_SPEED            1.2f
#define LEVEL2_UPG_VALUE            AVM(540)
#define LEVEL2_UPG_HEALTH           AHM(175)
#define LEVEL2_UPG_REGEN            (0.03f * LEVEL2_UPG_HEALTH)
#define LEVEL2_UPG_COST             1

#define LEVEL2_AREAZAP_DMG          ADM(60)
#define LEVEL2_AREAZAP_RANGE        200.0f
#define LEVEL2_AREAZAP_CHAIN_RANGE  150.0f
#define LEVEL2_AREAZAP_CHAIN_FALLOFF 8.0f
#define LEVEL2_AREAZAP_WIDTH        15.0f
#define LEVEL2_AREAZAP_REPEAT       1500
#define LEVEL2_AREAZAP_TIME         1000
#define LEVEL2_AREAZAP_MAX_TARGETS  5
#define LEVEL2_WALLJUMP_MAXSPEED    1000.0f
#define LEVEL2_BOUNCEBALL_DMG       ADM(35)
#define LEVEL2_BOUNCEBALL_REPEAT    200
#define LEVEL2_BOUNCEBALL_SPEED     1500.0f
#define LEVEL2_BOUNCEBALL_RADIUS    100
#define LEVEL2_BOUNCEBALL_REGEN     9000    // msec until new barb
#define LEVEL2_CLAW_UPG_DMG         ADM(45)
#define LEVEL2_CLAW_U_RANGE_UPG     LEVEL2_CLAW_RANGE + 1.2f
#define LEVEL2_CLAW_WIDTH_UPG       13.0f
#define LEVEL2_CLAW_DMG             ADM(40)
#define LEVEL2_CLAW_RANGE           80.0f
#define LEVEL2_CLAW_U_RANGE         LEVEL2_CLAW_RANGE + 2.0f
#define LEVEL2_CLAW_WIDTH           14.0f
#define LEVEL2_CLAW_REPEAT          500
#define LEVEL2_CLAW_K_SCALE         1.0f
#define LEVEL2_CLAW_U_REPEAT        400
#define LEVEL2_CLAW_U_K_SCALE       1.0f

//Dragoon
#define LEVEL3_SPEED                1.2f
#define LEVEL3_VALUE                AVM(600)
#define LEVEL3_HEALTH               AHM(220)
#define LEVEL3_REGEN                (0.03f * LEVEL3_HEALTH)
#define LEVEL3_COST                 1
#define LEVEL3_UPG_SPEED            1.3f
#define LEVEL3_UPG_VALUE            AVM(720)
#define LEVEL3_UPG_HEALTH           AHM(250)
#define LEVEL3_UPG_REGEN            (0.03f * LEVEL3_UPG_HEALTH)
#define LEVEL3_UPG_COST             1

#define LEVEL3_CLAW_DMG             ADM(80)
#define LEVEL3_CLAW_RANGE           75.0f
#define LEVEL3_CLAW_UPG_RANGE       LEVEL3_CLAW_RANGE + 3.0f          
#define LEVEL3_CLAW_WIDTH           12.0f
#define LEVEL3_CLAW_REPEAT          900
#define LEVEL3_CLAW_K_SCALE         1.0f
#define LEVEL3_CLAW_U_REPEAT        800
#define LEVEL3_CLAW_U_K_SCALE       1.0f
#define LEVEL3_POUNCE_DMG           ADM(80)
#define LEVEL3_POUNCE_RANGE         48.0f
#define LEVEL3_POUNCE_UPG_RANGE     LEVEL3_POUNCE_RANGE + 3.0f
#define LEVEL3_POUNCE_WIDTH         14.0f
#define LEVEL3_POUNCE_TIME          800      // msec for full Dragoon pounce
#define LEVEL3_POUNCE_TIME_UPG      800      // msec for full Adv. Dragoon pounce
#define LEVEL3_POUNCE_TIME_MIN      200      // msec before which pounce cancels  
#define LEVEL3_POUNCE_REPEAT        400      // msec before a new pounce starts
#define LEVEL3_POUNCE_SPEED_MOD     0.75f    // walking speed modifier for pounce charging
#define LEVEL3_POUNCE_JUMP_MAG      850      // Dragoon pounce jump power
#define LEVEL3_POUNCE_JUMP_MAG_UPG  930      // Adv. Dragoon pounce jump power
#define LEVEL3_BOUNCEBALL_DMG       ADM(115)
#define LEVEL3_BOUNCEBALL_REPEAT    1200
#define LEVEL3_BOUNCEBALL_SPEED     1000.0f
#define LEVEL3_BOUNCEBALL_RADIUS    75
#define LEVEL3_BOUNCEBALL_REGEN     13000    // msec until new barb

//Tyrant
#define LEVEL4_SPEED                1.2f
#define LEVEL4_VALUE                AVM(960)
#define LEVEL4_HEALTH               AHM(360)
#define LEVEL4_REGEN                (0.025f * LEVEL4_HEALTH)
#define LEVEL4_COST                 2

#define LEVEL4_CLAW_DMG             ADM(100)
#define LEVEL4_CLAW_RANGE           100.0f
#define LEVEL4_CLAW_WIDTH           14.0f
#define LEVEL4_CLAW_HEIGHT          20.0f
#define LEVEL4_CLAW_REPEAT          800
#define LEVEL4_CLAW_K_SCALE         1.0f
#define LEVEL4_TRAMPLE_DMG             ADM(111)
#define LEVEL4_TRAMPLE_SPEED           2.0f
#define LEVEL4_TRAMPLE_CHARGE_MIN      375   // minimum msec to start a charge
#define LEVEL4_TRAMPLE_CHARGE_MAX      1000  // msec to maximum charge stored
#define LEVEL4_TRAMPLE_CHARGE_TRIGGER  3000  // msec charge starts on its own
#define LEVEL4_TRAMPLE_DURATION        3000  // msec trample lasts on full charge
#define LEVEL4_TRAMPLE_STOP_PENALTY    1     // charge lost per msec when stopped
#define LEVEL4_TRAMPLE_REPEAT          100   // msec before a trample will rehit a player
#define LEVEL4_CRUSH_DAMAGE_PER_V      0.5f  // damage per falling velocity
#define LEVEL4_CRUSH_DAMAGE            120   // to players only
#define LEVEL4_CRUSH_REPEAT            500   // player damage repeat
#define LEVEL4_FIREBREATHREPEAT     1000
#define LEVEL4_FIREBREATHRELOADTIME 34000
#define LEVEL4_FIREBREATHSPEED 		800
#define LEVEL4_FIREBREATHDMG 		50

//Hummel
#define LEVEL5_UPG_SPEED            0.81f
#define LEVEL5_UPG_VALUE            AVM(400)
#define LEVEL5_UPG_HEALTH           AHM(140)
#define LEVEL5_UPG_REGEN            (0.04f * LEVEL5_UPG_HEALTH)
#define LEVEL5_UPG_COST             2
#define LEVEL5_CLAW_U_REPEAT        300
#define LEVEL5_CLAW_U_K_SCALE       1.0f
#define LEVEL5_CLAW_U_RANGE         80.0f
#define LEVEL5_CLAW_WIDTH           12.0f
#define LEVEL5_CLAW_DMG             ADM(30)
#define LEVEL5_PRICKLES_REPEAT      160
#define LEVEL5_PRICKLES_NEXTTHINK   6000
#define LEVEL5_PRICKLES_RADIUS	    50
#define LEVEL5_PRICKLES_SPEED	    1600.0f
#define LEVEL5_PRICKLESSPLASH_DMG   5
#define LEVEL5_PRICKLES_DMG	    6
#define LEVEL5_PRICKLES_RELOADTIME  160
#define LEVEL5_PRICKLES_AMMO 	    10
#define LEVEL5_PRICKLES_SPREAD      100
#define LEVEL5_POUNCE_JUMP_MAG      900
#define LEVEL5_POUNCE_REPEAT        250
#define LEVEL5_POUNCE_TIME          600
#define LEVEL5_POUNCE_TIME_MIN      250 
#define LEVEL5_POUNCE_DMG           40
#define LEVEL5_POUNCE_RANGE         48.0f
#define LEVEL5_POUNCE_WIDTH         14.0f
#define LEVEL5_POUNCE_SPEED_MOD     0.95f
/*
 * HUMAN
 */

#define HUMAN_SPRINT_MODIFIER       1.28f
#define HUMAN_JOG_MODIFIER          1.1f
#define HUMAN_BACK_MODIFIER         0.8f
#define HUMAN_SIDE_MODIFIER         0.9f
#define HUMAN_DODGE_SIDE_MODIFIER   2.9f
#define HUMAN_DODGE_SLOWED_MODIFIER 0.9f
#define HUMAN_DODGE_UP_MODIFIER     0.5f
#define HUMAN_DODGE_TIMEOUT         500
#define HUMAN_LAND_FRICTION         3.0f
#define STAMINA_STOP_RESTORE        20
#define STAMINA_WALK_RESTORE        15
#define STAMINA_MEDISTAT_RESTORE    30 			 // stacked on STOP or WALK
#define STAMINA_SPRINT_TAKE         8
#define STAMINA_JUMP_TAKE           250
#define STAMINA_DODGE_TAKE          250
#define STAMINA_MAX                 1200
#define STAMINA_BREATHING_LEVEL     0
#define STAMINA_SLOW_LEVEL          -500
#define STAMINA_BLACKOUT_LEVEL      -800
#define HUMAN_SPAWN_REPEAT_TIME     11000
#define HUMAN_REGEN_DAMAGE_TIME     2000 		//msec since damage before dcc repairs
#define HUMAN_MAX_CREDITS           4000
#define HUMAN_TK_SUICIDE_PENALTY    150
#define HUMAN_BUILDER_SCOREINC      50          // builders receive this many points every 10 seconds
#define ALIEN_BUILDER_SCOREINC      AVM(100)    // builders receive this many points every 10 seconds
#define HUMAN_BUILDABLE_INACTIVE_TIME 90000

/*
 * HUMAN weapons
 * _REPEAT  - time between firings
 * _RELOAD  - time needed to reload
 * _PRICE   - amount in credits weapon costs
 * HUMAN_WDMG_MODIFIER - overall damage modifier for coarse tuning
 */

#define HUMAN_WDMG_MODIFIER         1.0f
#define HDM(d)                      ((int)((float)d*HUMAN_WDMG_MODIFIER))

#define BLASTER_REPEAT              600
#define BLASTER_K_SCALE             1.0f
#define BLASTER_SPREAD              200
#define BLASTER_SPEED               1400
#define BLASTER_DMG                 HDM(10)
#define BLASTER_SIZE                5

#define RIFLE_CLIPSIZE              35
#define RIFLE_MAXCLIPS              5
#define RIFLE_REPEAT                90
#define RIFLE_K_SCALE               1.0f
#define RIFLE_RELOAD                2000
#define RIFLE_PRICE                 0
#define RIFLE_SPREAD                200
#define RIFLE_DMG                   HDM(5)

#define RIFLE_REPEAT2               35
#define RIFLE_K_SCALE2              1.2f
#define RIFLE_RELOAD2               4000
#define RIFLE_SPREAD2               2000
#define RIFLE_DMG2                  HDM(4)
#define RIFLE_2NDMOD                0.20f 

#define PAINSAW_PRICE               100
#define PAINSAW_REPEAT              75
#define PAINSAW_K_SCALE             1.0f
#define PAINSAW_DAMAGE              HDM(11)
#define PAINSAW_RANGE               64.0f
#define PAINSAW_WIDTH               0.0f
#define PAINSAW_HEIGHT              8.0f

#define PAINSAW_REPEAT2             3000
#define PAINSAW_DAMAGE2             HDM(60)
#define PAINSAW_DAMAGESPLASH2       5
#define PAINSAW_BLADELIFETIME       3500
#define PAINSAW_BLADESPEED          800.0f
#define PAINSAW_BLADERANGE          100.0f

#define GRENADE_PRICE               200
#define GRENADE_REPEAT              0
#define GRENADE_K_SCALE             1.0f
#define GRENADE_DAMAGE              HDM(340)
#define GRENADE_RANGE               192.0f
#define GRENADE_SPEED               700.0f

#define MINE_PRICE		    1000
#define MINE_REPEAT                 0
#define MINE_K_SCALE                1.0f
#define MINE_DAMAGE                 HDM(250)
#define MINE_RANGE		    200.0f
#define MINE_DETECT		    100.0f
#define MINE_SPEED                  500.0f
#define	MINE_INIT_TIME	            3000
#define MINE_CHECK_FREQUENCY        100
#define	MINE_BOOM_TIME		    25
#define MINE_TAKEDAMAGE             qtrue
#define MINE_HEALTH                 40

#define NADEFLAMES_SPEED            400.0f
#define CHECK_FREQUENCY             100

#define FLAMES_RANGE			    40.0f
#define FLAMES_DETECT			    50.0f
#define FLAMES_CHECK_FREQUENCY      100
#define FLAMES_SPEED                400.0f

#define SMOKE_PRICE		    250
#define SMOKE_REPEAT                0
#define SMOKE_K_SCALE               1.0f
#define SMOKE_SPEED                 400.0f
#define SMOKE_DAMAGE                HDM(1)
#define SMOKE_RANGE		    10.0f
#define SMOKE_DETECT		    10.0f
#define SMOKE_CHECK_FREQUENCY       100

#define SHOTGUN_PRICE               150
#define SHOTGUN_SHELLS              8
#define SHOTGUN_PELLETS             11 		//used to sync server and client side
#define SHOTGUN_MAXCLIPS            3
#define SHOTGUN_REPEAT              1000
#define SHOTGUN_K_SCALE             1.0f
#define SHOTGUN_RELOAD              2000
#define SHOTGUN_SPREAD              700
#define SHOTGUN_DMG                 HDM(5)
#define SHOTGUN_RANGE               (8192 * 12)

#define LASGUN_PRICE                250
#define LASGUN_AMMO                 250
#define LASGUN_REPEAT               190
#define LASGUN_K_SCALE              1.0f
#define LASGUN_RELOAD               2000
#define LASGUN_DAMAGE               HDM(9)

#define LASGUN_REPEAT2              5000
#define LASGUN_PUSH_DURATION        4000
#define LASGUN_PUSH_REPEAT          40
#define LASGUN_PUSH_COUNT           10
#define LASGUN_PUSH_RANGE           140
#define LASGUN_PUSH_FORCE           900
#define LASGUN_WEAK_FORCE           675

#define MDRIVER_PRICE               350
#define MDRIVER_CLIPSIZE            5
#define MDRIVER_MAXCLIPS            3
#define MDRIVER_DMG                 HDM(40)
#define MDRIVER_REPEAT              1000
#define MDRIVER_K_SCALE             1.0f
#define MDRIVER_RELOAD              2200
#define MDRIVER2ND_DMG              HDM(40)
#define MDRIVER2ND_SPLASH_DMG       10
#define MDRIVER2ND_SPLASHRADAIUS    100
#define MDRIVER2ND_LIFETIME         5000
#define MDRIVER2ND_DIR_CHANGE_PERIOD  50
#define MDRIVER2ND_SPEED            400
#define MDRIVER_REPEAT2              2200

#define CHAINGUN_PRICE              400
#define CHAINGUN_BULLETS            300
#define CHAINGUN_REPEAT             80
#define CHAINGUN_K_SCALE            1.0f
#define CHAINGUN_SPREAD             1100
#define CHAINGUN_DMG                HDM(6)

#define CHAINGUN_REPEAT2            120
#define CHAINGUN_SPREAD2            550
#define CHAINGUN_DMG2               HDM(6)

#define FLAMER_PRICE                550
#define FLAMER_GAS                  200
#define FLAMER_REPEAT               150
#define FLAMER_K_SCALE              2.0f
#define FLAMER_DMG                  HDM(20)
#define FLAMER_SPLASHDAMAGE         HDM(12)
#define FLAMER_RADIUS               50       		// splash radius
#define FLAMER_SIZE                 12       		// missile bounding box
#define FLAMER_LIFETIME             700.0f
#define FLAMER_LIFETIME2             600.0f
#define FLAMER_SPEED                550.0f
#define FLAMER_LAG                  0.65f    		// the amount of player velocity that is added to the fireball

#define PRIFLE_PRICE                450
#define PRIFLE_CLIPS                40
#define PRIFLE_MAXCLIPS             5
#define PRIFLE_REPEAT               100
#define PRIFLE_K_SCALE              1.0f
#define PRIFLE_RELOAD               2000
#define PRIFLE_DMG                  HDM(9)
#define PRIFLE_SPEED                1200
#define PRIFLE_SIZE                 5
#define PRIFLE_SECONDARY_SPEED      800
#define PRIFLE_SECONDARY_REPEAT     400

#define LCANNON_PRICE               600
#define LCANNON_AMMO                80
#define LCANNON_K_SCALE             1.0f
#define LCANNON_REPEAT              500
#define LCANNON_RELOAD              0
#define LCANNON_DAMAGE              HDM(265)
#define LCANNON_RADIUS              150      // primary splash damage radius
#define LCANNON_SIZE                5        // missile bounding box radius
#define LCANNON_SECONDARY_DAMAGE    HDM(30)
#define LCANNON_SECONDARY_RADIUS    80       // secondary splash damage radius
#define LCANNON_SECONDARY_SPEED     1400
#define LCANNON_SECONDARY_RELOAD    2000
#define LCANNON_SECONDARY_REPEAT    1000
#define LCANNON_SPEED               700
#define LCANNON_CHARGE_TIME_MAX     3000
#define LCANNON_CHARGE_TIME_MIN     100
#define LCANNON_CHARGE_TIME_WARN    2000
#define LCANNON_CHARGE_AMMO         10       // ammo cost of a full charge shot

#define FLAMER_CHARGE_TIME_MIN     200
#define FLAMER_CHARGE_TIME_MAX     3000
#define FLAMER_CHARGE_AMMO         50       // ammo cost of a full charge shot
#define FLAMER_SECONDARY_REPEAT    200

#define HBUILD_PRICE                0
#define HBUILD_REPEAT               1000
#define HBUILD_HEALRATE             18

/*
 * HUMAN upgrades
 */
#define LIGHTARMOUR_PRICE           50
#define LIGHTARMOUR_POISON_PROTECTION 1
#define LIGHTARMOUR_INFECTION_PROTECTION 0
#define LIGHTARMOUR_PCLOUD_PROTECTION 1000

#define HELMET_PRICE                80
#define HELMET_RANGE                1000.0f
#define HELMET_POISON_PROTECTION    1
#define HELMET_INFECTION_PROTECTION 1
#define HELMET_PCLOUD_PROTECTION    1000

#define SPAWN_WITH_MEDKIT           1
#define MEDKIT_PRICE                50

#define BATTPACK_PRICE              75
#define BATTPACK_MODIFIER           1.5f 		//modifier for extra energy storage available

#define CLOAK_PRICE                 125
#define CLOAK_TIME                  40000

#define JETPACK_PRICE               120
#define JETPACK_FLOAT_SPEED         250.0f 		//up movement speed
#define JETPACK_SINK_SPEED          300.0f 		//down movement speed
#define JETPACK_DISABLE_TIME        800 		//time to disable the jetpack when player damaged
#define JETPACK_DISABLE_CHANCE      0.3f

#define BSUIT_PRICE                 400
#define BSUIT_POISON_PROTECTION     3
#define BSUIT_INFECTION_PROTECTION  1
#define BSUIT_PCLOUD_PROTECTION     3000

#define MGCLIP_PRICE                0

#define CGAMMO_PRICE                0

#define GAS_PRICE                   0

#define MEDKIT_POISON_IMMUNITY_TIME 0
#define MEDKIT_STARTUP_TIME         4000
#define MEDKIT_STARTUP_SPEED        5

#define BIOKIT_PRICE                200
#define BIOKIT_POISON_PROTECTION    1
#define BIOKIT_INFECTION_PROTECTION 3
#define BIOKIT_HEALTH_RATE          2.0 		// 5000/BIOKIT_HEALTH_RATE = * msec before regenerate
#define BIOKIT_STAMINA_RATE         2


//BUILDABLES////////////
/*
 * ALIEN buildables
 * _BP            - build points required for this buildable
 * _BT            - build time required for this buildable
 * _REGEN         - the amount of health per second regained
 * _SPLASHDAMGE   - the amount of damage caused by this buildable when melting
 * _SPLASHRADIUS  - the radius around which it does this damage
 * CREEP_BASESIZE - the maximum distance a buildable can be from an egg/overmind
 * ALIEN_BHLTH_MODIFIER - overall health modifier for coarse tuning
 */

#define ALIEN_BHLTH_MODIFIER        1.0f
#define ABHM(h)                     ((int)((float)h*ALIEN_BHLTH_MODIFIER))
#define ALIEN_BVALUE_MODIFIER       90.0f
#define ABVM(h)                     ((int)((float)h*ALIEN_BVALUE_MODIFIER))

#define CREEP_BASESIZE              700
#define CREEP_TIMEOUT               1000
#define CREEP_MODIFIER              0.5f
#define CREEP_ARMOUR_MODIFIER       0.75f
#define CREEP_SCALEDOWN_TIME        3000

#define PCLOUD_MODIFIER             0.5f
#define PCLOUD_ARMOUR_MODIFIER      0.75f

#define ASPAWN_BP                   10
#define ASPAWN_BT                   15000
#define ASPAWN_HEALTH               ABHM(250)
#define ASPAWN_REGEN                8
#define ASPAWN_SPLASHDAMAGE         50
#define ASPAWN_SPLASHRADIUS         100
#define ASPAWN_CREEPSIZE            80
#define ASPAWN_VALUE                ABVM(ASPAWN_BP)

#define BARRICADE_BP                5
#define BARRICADE_BT                13000
#define BARRICADE_HEALTH            ABHM(350)
#define BARRICADE_REGEN             14
#define BARRICADE_SPLASHDAMAGE      50
#define BARRICADE_SPLASHRADIUS      100
#define BARRICADE_CREEPSIZE         70
#define BARRICADE_SHRINKPROP        0.1875f
#define BARRICADE_SHRINKTIMEOUT     500
#define BARRICADE_VALUE             ABVM(BARRICADE_BP)

#define BOOSTER_BP                  14
#define BOOSTER_BT                  14000
#define BOOSTER_HEALTH              ABHM(150)
#define BOOSTER_REGEN               8
#define BOOSTER_SPLASHDAMAGE        50
#define BOOSTER_SPLASHRADIUS        100
#define BOOSTER_CREEPSIZE           70
#define BOOSTER_REGEN_MOD           3.0f
#define BOOSTER_VALUE               ABVM(BOOSTER_BP)
#define BOOST_TIME                  20000
#define BOOST_WARN_TIME             15000

#define ACIDTUBE_BP                 6
#define ACIDTUBE_BT                 8000
#define ACIDTUBE_HEALTH             ABHM(125)
#define ACIDTUBE_REGEN              10
#define ACIDTUBE_SPLASHDAMAGE       50
#define ACIDTUBE_SPLASHRADIUS       100
#define ACIDTUBE_CREEPSIZE          30
#define ACIDTUBE_DAMAGE             8
#define ACIDTUBE_RANGE              300.0f
#define ACIDTUBE_REPEAT             300
#define ACIDTUBE_REPEAT_ANIM        2000
#define ACIDTUBE_VALUE              ABVM(ACIDTUBE_BP)

#define HIVE_BP                     8
#define HIVE_BT                     13000
#define HIVE_HEALTH                 ABHM(125)
#define HIVE_REGEN                  10
#define HIVE_SPLASHDAMAGE           30
#define HIVE_SPLASHRADIUS           200
#define HIVE_CREEPSIZE              40
#define HIVE_SENSE_RANGE            500.0f
#define HIVE_LIFETIME               3000
#define HIVE_REPEAT                 3000
#define HIVE_K_SCALE                1.0f
#define HIVE_DMG                    80
#define HIVE_SPEED                  320.0f
#define HIVE_DIR_CHANGE_PERIOD      500
#define HIVE_VALUE                  ABVM(HIVE_BP)

#define TRAPPER_BP                  4
#define TRAPPER_BT                  5000
#define TRAPPER_HEALTH              ABHM(50)
#define TRAPPER_REGEN               6
#define TRAPPER_SPLASHDAMAGE        15
#define TRAPPER_SPLASHRADIUS        100
#define TRAPPER_CREEPSIZE           25
#define TRAPPER_RANGE               400
#define TRAPPER_REPEAT              1000
#define TRAPPER_VALUE               ABVM(TRAPPER_BP)
#define LOCKBLOB_SPEED              650.0f
#define LOCKBLOB_LOCKTIME           5000
#define LOCKBLOB_DOT                0.85f // max angle = acos( LOCKBLOB_DOT )
#define LOCKBLOB_K_SCALE            1.0f
#define TRAPPER_ACCURACY            9 // lower is better

#define OVERMIND_BP                 0
#define OVERMIND_BT                 30000
#define OVERMIND_HEALTH             ABHM(750)
#define OVERMIND_REGEN              6
#define OVERMIND_SPLASHDAMAGE       15
#define OVERMIND_SPLASHRADIUS       300
#define OVERMIND_CREEPSIZE          120
#define OVERMIND_ATTACK_RANGE       150.0f
#define OVERMIND_ATTACK_REPEAT      1000
#define OVERMIND_VALUE              ABVM(50)

#define PANZER_SMALL_BP                    1
#define PANZER_SMALL_BT                    6000
#define PANZER_SMALL_HEALTH                ABHM(100)
#define PANZER_SMALL_REGEN                 1
#define PANZER_SMALL_SPLASHDAMAGE          1
#define PANZER_SMALL_SPLASHRADIUS          10
#define PANZER_SMALL_CREEPSIZE             0
#define PANZER_SMALL_VALUE                 ABVM(PANZER_SMALL_BP)

#define PANZER_MEDIUM_BP                    2
#define PANZER_MEDIUM_BT                    15000
#define PANZER_MEDIUM_HEALTH                ABHM(250)
#define PANZER_MEDIUM_REGEN                 1
#define PANZER_MEDIUM_SPLASHDAMAGE          5
#define PANZER_MEDIUM_SPLASHRADIUS          40
#define PANZER_MEDIUM_CREEPSIZE             0
#define PANZER_MEDIUM_VALUE                 ABVM(PANZER_MEDIUM_BP)

#define PANZER_LARGE_BP                     5
#define PANZER_LARGE_BT                     20000
#define PANZER_LARGE_HEALTH                 ABHM(450)
#define PANZER_LARGE_REGEN                  1
#define PANZER_LARGE_SPLASHDAMAGE           10
#define PANZER_LARGE_SPLASHRADIUS           70
#define PANZER_LARGE_CREEPSIZE              0
#define PANZER_LARGE_VALUE                  ABVM(PANZER_LARGE_BP)

#define TENDON_SMALL_BP                     1
#define TENDON_SMALL_BT                     4000
#define TENDON_SMALL_HEALTH                 ABHM(25)
#define TENDON_SMALL_REGEN                  1
#define TENDON_SMALL_SPLASHDAMAGE           5
#define TENDON_SMALL_SPLASHRADIUS           10
#define TENDON_SMALL_CREEPSIZE              0
#define TENDON_SMALL_VALUE                  ABVM(TENDON_SMALL_BP)

#define TENDON_MEDIUM_BP                    2
#define TENDON_MEDIUM_BT                    9000
#define TENDON_MEDIUM_HEALTH                ABHM(50)
#define TENDON_MEDIUM_REGEN                 1
#define TENDON_MEDIUM_SPLASHDAMAGE          5
#define TENDON_MEDIUM_SPLASHRADIUS          25
#define TENDON_MEDIUM_CREEPSIZE             0
#define TENDON_MEDIUM_VALUE                 ABVM(TENDON_MEDIUM_BP)

#define TENDON_LARGE_BP                     3
#define TENDON_LARGE_BT                     13000
#define TENDON_LARGE_HEALTH                 ABHM(100)
#define TENDON_LARGE_REGEN                  1
#define TENDON_LARGE_SPLASHDAMAGE           5
#define TENDON_LARGE_SPLASHRADIUS           50
#define TENDON_LARGE_CREEPSIZE              0
#define TENDON_LARGE_VALUE                  ABVM(TENDON_LARGE_BP)

#define NET_BP                  		    3
#define NET_BT                  			8000
#define NET_HEALTH              			ABHM(80)
#define NET_REGEN                		    10
#define NET_SPLASHDAMAGE         			5
#define NET_SPLASHRADIUS        			50
#define NET_CREEPSIZE           			0
#define NET_VALUE                		    ABVM(NET_BP)

#define NET_SPIKE_BP               			2
#define NET_SPIKE_BT               			5000
#define NET_SPIKE_HEALTH            		ABHM(50)
#define NET_SPIKE_REGEN                	    10
#define NET_SPIKE_SPLASHDAMAGE         	    5
#define NET_SPIKE_SPLASHRADIUS         	    50
#define NET_SPIKE_CREEPSIZE          		0
#define NET_SPIKE_VALUE                	    ABVM(NET_SPIKE_BP)

#define INFESTATION_SLIME_BP                    12
#define INFESTATION_SLIME_BT                    20000
#define INFESTATION_SLIME_HEALTH                ABHM(200)
#define INFESTATION_SLIME_REGEN                 8
#define INFESTATION_SLIME_SPLASHDAMAGE          15
#define INFESTATION_SLIME_SPLASHRADIUS          200
#define INFESTATION_SLIME_CREEPSIZE             170
#define INFESTATION_SLIME_VALUE              	ABVM(INFESTATION_SLIME_BP)

#define INFESTATION_THICKET_BP                    4
#define INFESTATION_THICKET_BT                    12000
#define INFESTATION_THICKET_HEALTH                ABHM(250)
#define INFESTATION_THICKET_REGEN                 1
#define INFESTATION_THICKET_SPLASHDAMAGE          10
#define INFESTATION_THICKET_SPLASHRADIUS          10
#define INFESTATION_THICKET_CREEPSIZE             190
#define INFESTATION_THICKET_VALUE                 ABVM(INFESTATION_THICKET_BP)

#define INFESTATION_BLISTER_BP                    3
#define INFESTATION_BLISTER_BT                    7000
#define INFESTATION_BLISTER_HEALTH                ABHM(250)
#define INFESTATION_BLISTER_REGEN                 1
#define INFESTATION_BLISTER_SPLASHDAMAGE          10
#define INFESTATION_BLISTER_SPLASHRADIUS          10
#define INFESTATION_BLISTER_CREEPSIZE             300
#define INFESTATION_BLISTER_VALUE                 ABVM(INFESTATION_BLISTER_BP)

#define REFLECTOR_BP                 			   3
#define REFLECTOR_BT                 			   4000
#define REFLECTOR_HEALTH              			   ABHM(250)
#define REFLECTOR_REGEN               			   1
#define REFLECTOR_SPLASHDAMAGE        			   5
#define REFLECTOR_SPLASHRADIUS        			   15
#define REFLECTOR_CREEPSIZE           			   0
#define REFLECTOR_VALUE                			   ABVM(REFLECTOR_BP)

#define MUSCLE_BP                   				3
#define MUSCLE_BT                   			    10000
#define MUSCLE_HEALTH              					ABHM(100)
#define MUSCLE_REGEN               					5
#define MUSCLE_SPLASHDAMAGE        					5
#define MUSCLE_SPLASHRADIUS        					10
#define MUSCLE_CREEPSIZE            				50
#define MUSCLE_VALUE                				ABVM(MUSCLE_BP)

#define SPITEFUL_ABCESS_BP                    		3
#define SPITEFUL_ABCESS_BT                    		8000
#define SPITEFUL_ABCESS_HEALTH                		ABHM(60)
#define SPITEFUL_ABCESS_REGEN                 		10
#define SPITEFUL_ABCESS_SPLASHDAMAGE          		100
#define SPITEFUL_ABCESS_SPLASHRADIUS          		100
#define SPITEFUL_ABCESS_CREEPSIZE             		50
#define SPITEFUL_ABCESS_VALUE                		ABVM(SPITEFUL_ABCESS_BP)

#define COCOON_BP                    				35
#define COCOON_BT                    				20000
#define COCOON_HEALTH                				ABHM(250)
#define COCOON_REGEN                 				1
#define COCOON_SPLASHDAMAGE          				10
#define COCOON_SPLASHRADIUS          				200
#define COCOON_CREEPSIZE             				60
#define COCOON_VALUE               				    ABVM(COCOON_BP)

#define ORGANIC_BULB_BP                    			3
#define ORGANIC_BULB_BT                    			12000
#define ORGANIC_BULB_HEALTH                			ABHM(35)
#define ORGANIC_BULB_REGEN                 			5
#define ORGANIC_BULB_SPLASHDAMAGE          			0
#define ORGANIC_BULB_SPLASHRADIUS          			0
#define ORGANIC_BULB_CREEPSIZE             			15
#define ORGANIC_BULB_VALUE              		    ABVM(ORGANIC_BULB_BP)

#define POD_BP                    				20
#define POD_BT                    				20000
#define POD_HEALTH                				ABHM(850)
#define POD_REGEN                 				5
#define POD_SPLASHDAMAGE          				50
#define POD_SPLASHRADIUS          				150
#define POD_CREEPSIZE             				0
#define POD_VALUE               			    ABVM(POD_BP)

#define POD_STUMP_BP                    		15
#define POD_STUMP_BT                    		20000
#define POD_STUMP_HEALTH                		ABHM(650)
#define POD_STUMP_REGEN                 		15
#define POD_STUMP_SPLASHDAMAGE          		25
#define POD_STUMP_SPLASHRADIUS          		100
#define POD_STUMP_CREEPSIZE             		0
#define POD_STUMP_VALUE              		    ABVM(POD_STUMP_BP)

#define CREEPCOLONY_BP              	   5
#define CREEPCOLONY_BT              	   18000
#define CREEPCOLONY_HEALTH          	   ABHM(350)
#define CREEPCOLONY_REGEN           	   6
#define CREEPCOLONY_SPLASHDAMAGE   		   200
#define CREEPCOLONY_SPLASHRADIUS   		   300
#define CREEPCOLONY_CREEPSIZE      		   60
#define CREEPCOLONY_RADIUS          	   "200"
#define CREEPCOLONY_VALUE                  ABVM(CREEPCOLONY_BP)

/*
 * HUMAN buildables
 * _BP            - build points required for this buildable
 * _BT            - build time required for this buildable
 * _SPLASHDAMGE   - the amount of damage caused by this buildable when it blows up
 * _SPLASHRADIUS  - the radius around which it does this damage
 * REACTOR_BASESIZE - the maximum distance a buildable can be from an reactor
 * REPEATER_BASESIZE - the maximum distance a buildable can be from a repeater
 * HUMAN_BHLTH_MODIFIER - overall health modifier for coarse tuning
 */

#define HUMAN_BHLTH_MODIFIER        1.0f
#define HBHM(h)                     ((int)((float)h*HUMAN_BHLTH_MODIFIER))
#define HUMAN_BVALUE_MODIFIER       240.0f
#define HBVM(h)                     ((int)((float)h*(float)HUMAN_BVALUE_MODIFIER)) // remember these are measured in credits not frags (c.f. ALIEN_CREDITS_PER_KILL)

#define REACTOR_BASESIZE            1000
#define REPEATER_BASESIZE           500
#define HUMAN_DETONATION_DELAY      5000

#define HSPAWN_BP                   10
#define HSPAWN_BT                   10000
#define HSPAWN_HEALTH               HBHM(310)
#define HSPAWN_SPLASHDAMAGE         50
#define HSPAWN_SPLASHRADIUS         100
#define HSPAWN_VALUE                HBVM(HSPAWN_BP)

#define MEDISTAT_BP                 8
#define MEDISTAT_BT                 8000
#define MEDISTAT_HEALTH             HBHM(190)
#define MEDISTAT_SPLASHDAMAGE       50
#define MEDISTAT_SPLASHRADIUS       100
#define MEDISTAT_VALUE              HBVM(MEDISTAT_BP)

#define MGTURRET_BP                 8
#define MGTURRET_BT                 8000
#define MGTURRET_HEALTH             HBHM(180)
#define MGTURRET_SPLASHDAMAGE       100
#define MGTURRET_SPLASHRADIUS       100
#define MGTURRET_ANGULARSPEED       12
#define MGTURRET_ACCURACY_TO_FIRE   0
#define MGTURRET_VERTICALCAP        30  		// +/- maximum pitch
#define MGTURRET_REPEAT             150
#define MGTURRET_K_SCALE            1.0f
#define MGTURRET_RANGE              380.0f
#define MGTURRET_SPREAD             200
#define MGTURRET_DMG                HDM(10)
#define MGTURRET_SPINUP_TIME        750 		// time between target sighted and fire
#define MGTURRET_VALUE              HBVM(MGTURRET_BP)

#define MGTURRET2_BP                 14
#define MGTURRET2_BT                 20000
#define MGTURRET2_HEALTH             HBHM(300)
#define MGTURRET2_SPLASHDAMAGE       1
#define MGTURRET2_SPLASHRADIUS       170
#define MGTURRET2_ANGULARSPEED       12
#define MGTURRET2_ACCURACY_TO_FIRE   0
#define MGTURRET2_VERTICALCAP        40  		// +/- maximum pitch
#define MGTURRET2_REPEAT             140
#define MGTURRET2_K_SCALE            1.0f
#define MGTURRET2_RANGE              50.0f
#define MGTURRET2_SPREAD             300
#define MGTURRET2_DMG                HDM(16)
#define MGTURRET2_SPINUP_TIME        600 		// time between target sighted and fire
#define MGTURRET2_VALUE              HBVM(MGTURRET2_BP)
#define FLAMER2_LIFETIME             350.0f

#define TESLAGEN_BP                 10
#define TESLAGEN_BT                 15000
#define TESLAGEN_HEALTH             HBHM(300)
#define TESLAGEN_SPLASHDAMAGE       50
#define TESLAGEN_SPLASHRADIUS       100
#define TESLAGEN_REPEAT             250
#define TESLAGEN_K_SCALE            4.0f
#define TESLAGEN_RANGE              225
#define TESLAGEN_DMG                HDM(11)
#define TESLAGEN_VALUE              HBVM(TESLAGEN_BP)

#define DC_BP                       15
#define DC_BT                       16000
#define DC_HEALTH                   HBHM(190)
#define DC_SPLASHDAMAGE             50
#define DC_SPLASHRADIUS             100
#define DC_ATTACK_PERIOD            50000 		// how often to spam "under attack"
#define DC_HEALRATE                 4
#define DC_RANGE                    1000
#define DC_VALUE                    HBVM(DC_BP)

#define ARMOURY_BP                  10
#define ARMOURY_BT                  10000
#define ARMOURY_HEALTH              HBHM(420)
#define ARMOURY_SPLASHDAMAGE        50
#define ARMOURY_SPLASHRADIUS        100
#define ARMOURY_VALUE               HBVM(ARMOURY_BP)

#define REACTOR_BP                  0
#define REACTOR_BT                  20000
#define REACTOR_HEALTH              HBHM(930)
#define REACTOR_SPLASHDAMAGE        200
#define REACTOR_SPLASHRADIUS        300
#define REACTOR_ATTACK_RANGE        100.0f
#define REACTOR_ATTACK_REPEAT       1000
#define REACTOR_ATTACK_DAMAGE       40
#define REACTOR_ATTACK_DCC_REPEAT   1000
#define REACTOR_ATTACK_DCC_RANGE    150.0f
#define REACTOR_ATTACK_DCC_DAMAGE   40
#define REACTOR_VALUE               HBVM(50)

#define REPEATER_BP                 4
#define REPEATER_BT                 10000
#define REPEATER_HEALTH             HBHM(250)
#define REPEATER_SPLASHDAMAGE       50
#define REPEATER_SPLASHRADIUS       100
#define REPEATER_VALUE              HBVM(REPEATER_BP)

//EDGE BUILDINGS HUMANS
#define CONTAINER_SMALL_BP                    1
#define CONTAINER_SMALL_BT                    5000
#define CONTAINER_SMALL_HEALTH                HBHM(60)
#define CONTAINER_SMALL_SPLASHDAMAGE          1
#define CONTAINER_SMALL_SPLASHRADIUS          10
#define CONTAINER_SMALL_VALUE                 HBVM(CONTAINER_SMALL_BP)

#define CONTAINER_MEDIUM_BP                   2
#define CONTAINER_MEDIUM_BT                   14000
#define CONTAINER_MEDIUM_HEALTH               HBHM(250)
#define CONTAINER_MEDIUM_SPLASHDAMAGE         5
#define CONTAINER_MEDIUM_SPLASHRADIUS         40
#define CONTAINER_MEDIUM_VALUE                HBVM(CONTAINER_MEDIUM_BP)

#define CONTAINER_LARGE_BP                    5
#define CONTAINER_LARGE_BT                    20000
#define CONTAINER_LARGE_HEALTH                HBHM(450)
#define CONTAINER_LARGE_SPLASHDAMAGE          10
#define CONTAINER_LARGE_SPLASHRADIUS          70
#define CONTAINER_LARGE_VALUE                 HBVM(CONTAINER_LARGE_BP)

#define PLATE_SMALL_BP       	              1
#define PLATE_SMALL_BT        	              4000
#define PLATE_SMALL_HEALTH     	              HBHM(25)
#define PLATE_SMALL_SPLASHDAMAGE              5
#define PLATE_SMALL_SPLASHRADIUS              10
#define PLATE_SMALL_VALUE               	  HBVM(PLATE_SMALL_BP)

#define PLATE_LARGE_BP       	              3
#define PLATE_LARGE_BT        	              13000
#define PLATE_LARGE_HEALTH     	              HBHM(100)
#define PLATE_LARGE_SPLASHDAMAGE              5
#define PLATE_LARGE_SPLASHRADIUS              100
#define PLATE_LARGE_VALUE               	  HBVM(PLATE_LARGE_BP)

#define FENCE_BP                 			  3
#define FENCE_BT                 			  7000
#define FENCE_HEALTH              			  HBHM(100)
#define FENCE_SPLASHDAMAGE        			  5
#define FENCE_SPLASHRADIUS        			  50
#define FENCE_VALUE               			  HBVM(FENCE_BP)

#define FENCE_ROD_BP                 		  2
#define FENCE_ROD_BT                 		  5000
#define FENCE_ROD_HEALTH              		  HBHM(50)
#define FENCE_ROD_SPLASHDAMAGE        		  5
#define FENCE_ROD_SPLASHRADIUS        		  20
#define FENCE_ROD_VALUE              		  HBVM(FENCE_ROD_BP)

#define BARRIER_LINE_BP                 	  2
#define BARRIER_LINE_BT                 	  12000
#define BARRIER_LINE_HEALTH              	  HBHM(300)
#define BARRIER_LINE_SPLASHDAMAGE        	  10
#define BARRIER_LINE_SPLASHRADIUS        	  50
#define BARRIER_LINE_VALUE              	  HBVM(BARRIER_LINE_BP)

#define BARRIER_CORNER_BP                 	  2
#define BARRIER_CORNER_BT                 	  12000
#define BARRIER_CORNER_HEALTH              	  HBHM(300)
#define BARRIER_CORNER_SPLASHDAMAGE        	  10
#define BARRIER_CORNER_SPLASHRADIUS        	  50
#define BARRIER_CORNER_VALUE               	  HBVM(BARRIER_CORNER_BP)

#define BARRIER_POINT_BP                 	  3
#define BARRIER_POINT_BT                 	  12000
#define BARRIER_POINT_HEALTH              	  HBHM(350)
#define BARRIER_POINT_SPLASHDAMAGE        	  10
#define BARRIER_POINT_SPLASHRADIUS        	  50
#define BARRIER_POINT_VALUE               	  HBVM(BARRIER_POINT_BP)

#define SHIELD_BP                		      8
#define SHIELD_BT                 			  20000
#define SHIELD_HEALTH              			  HBHM(600)
#define SHIELD_SPLASHDAMAGE        			  50
#define SHIELD_SPLASHRADIUS        			  150
#define SHIELD_SHRINKPROP     		          0.001f
#define SHIELD_SHRINKTIMEOUT     			  800
#define SHIELD_VALUE               			  HBVM(SHIELD_BP)

#define LADDER_BP                		 	  5
#define LADDER_BT                 		      8000
#define LADDER_HEALTH              		      HBHM(155)
#define LADDER_SPLASHDAMAGE        			  15
#define LADDER_SPLASHRADIUS        			  250
#define LADDER_VALUE               			  HBVM(LADDER_BP)

#define TEFLON_FOIL_BP                		  1
#define TEFLON_FOIL_BT                 		  2000
#define TEFLON_FOIL_HEALTH             		  HBHM(50)
#define TEFLON_FOIL_SPLASHDAMAGE       		  1
#define TEFLON_FOIL_SPLASHRADIUS       		  1
#define TEFLON_FOIL_VALUE               	  HBVM(TEFLON_FOIL_BP)

#define BARREL_BP                		 	 1
#define BARREL_BT                 			 6000
#define BARREL_HEALTH              		     HBHM(250)
#define BARREL_SPLASHDAMAGE        		     40
#define BARREL_SPLASHRADIUS        		     150
#define BARREL_VALUE               			 HBVM(BARREL_BP)

#define LIGHT_BP                		     12
#define LIGHT_BT                 			 12000
#define LIGHT_HEALTH              		     HBHM(200)
#define LIGHT_SPLASHDAMAGE        		     15
#define LIGHT_SPLASHRADIUS        		     180
#define LIGHT_VALUE               			 HBVM(LIGHT_BP)

#define COVER_BP                	    	 20
#define COVER_BT                 			 20000
#define COVER_HEALTH              	         HBHM(850)
#define COVER_SPLASHDAMAGE        			 50
#define COVER_SPLASHRADIUS        			 150
#define COVER_VALUE               			 HBVM(COVER_BP)

#define COVER_STUMP_BP                		 15
#define COVER_STUMP_BT                 	     20000
#define COVER_STUMP_HEALTH              	 HBHM(650)
#define COVER_STUMP_SPLASHDAMAGE        	 50
#define COVER_STUMP_SPLASHRADIUS        	 150
#define COVER_STUMP_VALUE               	 HBVM(COVER_STUMP_BP)

#define REFINERY_BP                			 5
#define REFINERY_BT                			 18000
#define REFINERY_HEALTH            			 HBHM(350)
#define REFINERY_SPLASHDAMAGE      			 50
#define REFINERY_SPLASHRADIUS      			 300
#define REFINERY_RADIUS            			 "200"
#define REFINERY_VALUE               		 HBVM(REFINERY_BP)
