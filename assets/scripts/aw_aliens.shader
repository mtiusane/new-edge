spore001
{
	nopicmip
	cull disable
  {
    map gfx/spore/spore001.bmp
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

spore002
{
	nopicmip
	cull disable
  {
    map gfx/spore/spore002.bmp
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

spore003
{
	nopicmip
	cull disable
  {
    map gfx/spore/spore003.bmp
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

spore004
{
	nopicmip
	cull disable
  {
    map gfx/spore/spore004.bmp
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

spore005
{
	nopicmip
	cull disable
  {
    map gfx/spore/spore005.bmp
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

spore006
{
	nopicmip
	cull disable
  {
    map gfx/spore/spore006.bmp
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

spore007
{
	nopicmip
	cull disable
  {
    map gfx/spore/spore007.bmp
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

spore008
{
	nopicmip
	cull disable
  {
    map gfx/spore/spore008.bmp
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

spore009
{
	nopicmip
	cull disable
  {
    map gfx/spore/spore009.bmp
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

spore010
{
	nopicmip
	cull disable
  {
    map gfx/spore/spore010.bmp
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

spore011
{
	nopicmip
	cull disable
  {
    map gfx/spore/spore011.bmp
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

spore012
{
	nopicmip
	cull disable
  {
    map gfx/spore/spore012.bmp
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

spore013
{
	nopicmip
	cull disable
  {
    map gfx/spore/spore013.bmp
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

spore014
{
	nopicmip
	cull disable
  {
    map gfx/spore/spore014.bmp
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

spore015
{
	nopicmip
	cull disable
  {
    map gfx/spore/spore015.bmp
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

spore016
{
	nopicmip
	cull disable
  {
    map gfx/spore/spore016.bmp
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

spore017
{
	nopicmip
	cull disable
  {
    map gfx/spore/spore017.bmp
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}


models/weapons/acidbomb/acid_move
{
	{
  	map $lightmap
  	rgbGen identity
	}
	{
		map models/weapons/acidbomb/acidb.jpg
		rgbGen wave sin 0.7 0.3 0 0.25 
		tcMod scroll 0.2 0.2
	}
	{
  	map models/weapons/acidbomb/acid.tga
  	blendFunc blend
    alphaFunc GE128
    depthWrite
    rgbGen lightingDiffuse
	}
}

models/weapons/acidbomb/acid_static
{
	{
	  map models/weapons/acidbomb/acid.tga
	}
	{
  	map $lightmap
  	rgbGen identity
  	blendFunc filter
    alphaFunc GE128
    depthWrite
    rgbGen lightingDiffuse
	}
}

