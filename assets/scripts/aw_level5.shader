level5/trailshader
{  
  cull disable
  {
    map models/weapons/level5/sparks.tga
	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
    alphaGen vertex
  }
}

airpounceflash
{  
  cull disable
  nopicmip
  {
    map gfx/sprites/airpounceflash.tga
    blendFunc add
  }
}

radialblast
{  
  cull disable
  nopicmip
  {
    map gfx/sprites/radial.tga
    blendFunc add
  }
}
