textures/common-trem/nobuild
{
	qer_trans 0.5
	surfaceparm nodraw
	surfaceparm nolightmap
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm nobuild
}

textures/common-trem/noalienbuild
{
	qer_trans 0.5
	surfaceparm nodraw
	surfaceparm nolightmap
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm noalienbuild
}

textures/common-trem/nohumanbuild
{
	qer_trans 0.5
	surfaceparm nodraw
	surfaceparm nolightmap
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm nohumanbuild
}


gfx/misc/impactpuff2
{
  cull disable
  {
    map gfx/misc/impactpuff2.tga
    blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
    rgbGen vertex
    alphaGen vertex
  }
}

gfx/invisible
{
  nopicmip
  {
    map gfx/invisible.jpg
    //blendFunc GL_ONE GL_ONE
    depthFunc equal
    //tcMod scale 2 2
    //tcMod scroll 0.2 -0.2
    //rgbGen wave sin 0.0 1.0 0.25 0.5
  }
}

gfx/invisible_team
{
  nopicmip
  {
    map gfx/invisible.jpg
    blendFunc GL_ONE GL_ONE
    tcMod scale 2 2
    tcMod scroll 0.2 -0.2
  }
}

gfx/invisible_fade
{
  {
    map models/players/level1/level1upg.jpg
    blendFunc GL_ONE GL_ONE
    tcMod scale 2 2
    tcMod scroll 0.2 -0.2
    tcGen environment
    rgbGen wave sin 0.0 1.0 0.25 0.3
  }
}

gfx/marks/blood_mrk
{
  polygonOffset
  {
    map gfx/marks/blood_mrk.tga
    blendFunc blend
    rgbGen identityLighting
    alphaGen vertex
  }
}

gfx/marks/ablood_mrk
{
  polygonOffset
  {
    map gfx/sprites/green_acid.tga 
    blendFunc blend
    rgbGen identityLighting
    alphaGen vertex
  }
} 

creep
{
  nopicmip
  polygonoffset
  {
    clampmap gfx/misc/creep.tga
    blendfunc blend
    rgbGen identityLighting
    alphaGen Vertex
  }
}

granger_spit
{
  nopicmip
  polygonoffset
  {
    map gfx/misc/creep_add.tga
    blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
    rgbGen vertex
    alphaGen vertex
  }
}

// markShadow is the very cheap blurry blob underneath the player
gfx/marks/shadow
{
  polygonOffset
  {
    map gfx/marks/shadow.tga
    blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
    rgbGen vertex
    alphaGen vertex
  }
}

// wake is the mark on water surfaces for paddling players
gfx/marks/wake
{
  polygonOffset
  {
    clampmap gfx/marks/splash.tga
    blendFunc GL_ONE GL_ONE
    rgbGen vertex
    tcmod rotate 250
    tcMod stretch sin .9 0.1 0 0.7
    rgbGen wave sin .7 .3 .25 .5
  }
  {
    clampmap gfx/marks/splash.tga
    blendFunc GL_ONE GL_ONE
    rgbGen vertex
    tcmod rotate -230
    tcMod stretch sin .9 0.05 0 0.9
    rgbGen wave sin .7 .3 .25 .4
  }
}

gfx/marks/bullet_mrk
{
  polygonOffset
  {
    map gfx/marks/bullet_mrk.tga
    blendFunc GL_ZERO GL_ONE_MINUS_SRC_COLOR
    rgbGen exactVertex
  }
}

gfx/marks/burn_mrk
{
  polygonOffset
  {
    map gfx/marks/burn_mrk.tga
    blendFunc GL_ZERO GL_ONE_MINUS_SRC_COLOR
    rgbGen exactVertex
  }
}


gfx/marks/burn_mrk_flame
{
  polygonOffset
  {
    clampmap gfx/marks/splash.tga
    blendFunc GL_ONE GL_ONE
    rgbGen vertex
    tcmod rotate -230
    tcMod stretch sin .9 0.05 0 0.9
    rgbGen wave sin .7 .3 .25 .4
  }
}

gfx/marks/plasma_mrk
{
  polygonOffset
  {
    map gfx/marks/plasma_mrk.tga
    blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
    rgbGen vertex
    alphaGen vertex
  }
}

gfx/2d/blip
{
  {
    map gfx/2d/blip.tga
    blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
    rgbGen vertex
    alphaGen vertex
  }
}

gfx/2d/blipPlayer
{
  {
    map gfx/2d/blipPlayer.tga
    blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
    rgbGen vertex
    alphaGen vertex
  }
}

gfx/misc/tracer
{
  cull none
  {
    map gfx/sprites/spark.tga
    blendFunc blend
  }
}

gfx/damage/blood
{
  cull disable
  {
    map gfx/damage/blood.tga
    blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
    rgbGen vertex
    alphaGen vertex
  }
}

gfx/damage/fullscreen_painblend
{
  {
    map gfx/damage/fullscreen_painblend.tga
    blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
    rgbGen vertex
    alphaGen vertex
    tcMod rotate 90
  }

  {
    map gfx/damage/fullscreen_painblend.tga
    blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
    rgbGen vertex
    alphaGen vertex
    tcMod rotate -90
  }
}

models/fx/metal_gibs/metal_gibs
{
  {
    map models/fx/metal_gibs/metal_gibs.tga
    rgbGen lightingDiffuse
  }
  {
    map models/fx/metal_gibs/hot_gibs.tga
    blendfunc add
    rgbGen wave sin 0 1 0 0.0625
  }
}

//building 
gfx/misc/redbuild
{
  {
    map gfx/misc/redbuild.tga
    blendfunc add
    rgbGen identity
  }
}

gfx/misc/nopower
{
  {
    map gfx/misc/nopower.tga
    blendfunc add
    rgbGen identity
  }
}

gfx/sprites/smoke
{
  cull none
  entityMergable
  {
    map gfx/sprites/smoke.tga
    blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
    rgbGen    vertex
    alphaGen  vertex
  }
}

gfx/sprites/smoke2
{
  sort Banner
  cull none
  entityMergable
  {
    map gfx/sprites/smokeye.tga
    blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
    rgbGen    vertex
    alphaGen  vertex
  }
}
gfx/sprites/green_acid
{
  nopicmip
  {
    clampmap gfx/sprites/green_acid.tga
    blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
    rgbGen    vertex
    alphaGen  vertex
  }
}


gfx/sprites/spark
{
  cull none
  {
    map gfx/sprites/spark.tga
    blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
    rgbGen    vertex
    alphaGen  vertex
  }
}

gfx/sprites/bubble
{
  sort  underwater
  cull none
  entityMergable
  {
    map gfx/sprites/bubble.jpg
    blendFunc GL_ONE GL_ONE
    rgbGen    vertex
    alphaGen  vertex
  }
}

gfx/sprites/poisoncloud
{
  cull none
  entityMergable
  {
    map gfx/sprites/poisoncloud.tga
    blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
    rgbGen    vertex
    alphaGen  vertex
  }
}

models/splash/bright_star
{
	nopicmip
	{
		map models/splash/bright_star.tga
		blendfunc add
		rgbGen identity
	}
	{
		map models/splash/bright_star_2.tga
		blendfunc add
		rgbGen wave sin 0 0.5 0 0.09 
	}
}

models/splash/nebula
{
	nopicmip
	{
		map models/splash/nebula.tga
		blendfunc add
		rgbGen wave sin 0.5 1 5 0.05 
		tcMod scale -1 1
	}
	{
		map models/splash/nebula_2.tga
		blendfunc add
		rgbGen wave sin 0.3 1 2.5 0.05 
		tcMod scale -1 1
	}
}

models/splash/trem_black
{
	nopicmip
//	{
//		map models/splash/highlights.tga
//		blendfunc add
//		rgbGen wave sin 0 0.2 0 0.2 
//		tcMod scroll -0.2 0
//	}

	{
		map ui/assets/title.tga
		blendfunc blend
		//alphaFunc GE128
		rgbGen identity
	}
}



gfx/null
{
  {
    map gfx/null.tga
    blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
  }
}


textures/REGION
{
	surfaceparm nolightmap
}

white
{
	{
		map *white
		blendfunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbgen vertex
	}
}

outline
{
  cull none
  nopicmip
  nomipmaps
  {
    map gfx/2d/outline.tga
    blendfunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
    rgbgen vertex
  }
}

console
{
	nopicmip
	nomipmaps
  {
		map gfx/colors/black.tga
	} 
}

gfx/misc/detail
{
	nopicmip
	{
		map gfx/misc/detail.jpg
    blendFunc GL_DST_COLOR GL_SRC_COLOR
		rgbgen identity
	}
}

// The console font
gfx/2d/bigchars
{
	nopicmip
	nomipmaps
	{
		map gfx/2d/bigchars.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbgen vertex
	}
}

gfx/2d/cursor
{
  nopicmip
	nomipmaps
  {
    map gfx/2d/cursor.tga
  }
}

// projectionShadow is used for cheap squashed model shadows
projectionShadow
{
	polygonOffset
	deformVertexes projectionShadow
	{
		map			*white
		blendFunc GL_ONE GL_ZERO
		rgbGen wave square 0 0 0 0
	}	
}

// blinked on top of lagometer when connection is interrupted
gfx/2d/net
{
	nopicmip
	{
		map gfx/2d/net.jpg
	}
}

//===============================================================

gfx/2d/numbers/zero_32b
{
	nopicmip
	{
		map gfx/2d/numbers/zero_32b.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbgen vertex
	}
}
gfx/2d/numbers/one_32b
{
	nopicmip
	{
		map gfx/2d/numbers/one_32b.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbgen vertex
	}
}
gfx/2d/numbers/two_32b
{
	nopicmip
	{
		map gfx/2d/numbers/two_32b.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbgen vertex
	}
}
gfx/2d/numbers/three_32b
{
	nopicmip
	{
		map gfx/2d/numbers/three_32b.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbgen vertex
	}
}
gfx/2d/numbers/four_32b
{
	nopicmip
	{
		map gfx/2d/numbers/four_32b.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbgen vertex
	}
}
gfx/2d/numbers/five_32b
{
	nopicmip
	{
		map gfx/2d/numbers/five_32b.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbgen vertex
	}
}
gfx/2d/numbers/six_32b
{
	nopicmip
	{
		map gfx/2d/numbers/six_32b.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbgen vertex
	}
}
gfx/2d/numbers/seven_32b
{
	nopicmip
	{
		map gfx/2d/numbers/seven_32b.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbgen vertex
	}
}
gfx/2d/numbers/eight_32b
{
	nopicmip
	{
		map gfx/2d/numbers/eight_32b.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbgen vertex
	}
}
gfx/2d/numbers/nine_32b
{
	nopicmip
	{
		map gfx/2d/numbers/nine_32b.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbgen vertex
	}
}
gfx/2d/numbers/minus_32b
{
	nopicmip
	{
		map gfx/2d/numbers/minus_32b.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbgen vertex
	}
}


gfx/misc/tracer
{
  cull none
  {
    map gfx/sprites/spark.tga
    blendFunc blend
  }
}

gfx/damage/blood
{
  cull disable
  {
    map gfx/damage/blood.tga
    blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
    rgbGen vertex
    alphaGen vertex
  }
}

gfx/damage/fullscreen_painblend
{
  {
    map gfx/damage/fullscreen_painblend.tga
    blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
    rgbGen vertex
    alphaGen vertex
    tcMod rotate 90
  }

  {
    map gfx/damage/fullscreen_painblend.tga
    blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
    rgbGen vertex
    alphaGen vertex
    tcMod rotate -90
  }
}

models/fx/metal_gibs/metal_gibs
{
  {
    map models/fx/metal_gibs/metal_gibs.tga
    rgbGen lightingDiffuse
  }
  {
    map models/fx/metal_gibs/hot_gibs.tga
    blendfunc add
    rgbGen wave sin 0 1 0 0.0625
  }
}

gfx/misc/redbuild
{
  {
    map gfx/misc/redbuild.tga
    blendfunc add
    rgbGen identity
  }
}

gfx/misc/nopower
{
  {
    map gfx/misc/nopower.tga
    blendfunc add
    rgbGen identity
  }
}
