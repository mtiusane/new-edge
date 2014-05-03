models/weapons/mdriver/mdriver
{
	cull disable
	{
		map models/weapons/mdriver/mdriver.jpg
		rgbGen lightingDiffuse
	}
	{
		map $whiteimage
		rgbGen lightingDiffuse
		blendFunc GL_DST_COLOR GL_SRC_ALPHA
		detail
		alphaGen lightingSpecular
	}
	{
		map models/buildables/mgturret/ref_map.jpg
		blendFunc GL_DST_COLOR GL_ONE
		detail
		tcGen environment
	}
}

gfx/mdriver/trail
{
  nomipmaps
  cull disable
  {
    map gfx/mdriver/trail.tga
    blendFunc blend
  }
}

gfx/mdriver/purple_trail
{
  cull disable
  {
    map gfx/mdriver/purple_trail.tga
    blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
    rgbGen vertex
    alphaGen vertex
  }
}

//for new mine model
models/weapons/newmine
{  

  {
    map models/weapons/mine/mine.jpg
  }
  	{	//red blinking
		map models/weapons/mine/mine_blend.jpg
		blendfunc add
	    depthWrite
		//rgbGen lightingDiffuse
		rgbGen wave triangle 0.5 1 1 0.8
	}
}

models/weapons/psaw/blade
{
	sort additive
	cull disable
	{
		map models/weapons/psaw/blade.tga
		alphaFunc GE128
		depthWrite
		rgbGen lightingDiffuse
		tcMod rotate 500
		
	}
}

//new flamer shaders
textures/yeflamer/flamesadd
{
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm trans
	cull disable
	deformvertexes autosprite


	{
		animMap 10 textures/flamer_ye/flame1.tga textures/flamer_ye/flame2.tga textures/flamer_ye/flame3.tga textures/flamer_ye/flame4.tga textures/flamer_ye/flame5.tga textures/flamer_ye/flame6.tga textures/flamer_ye/flame7.tga textures/flamer_ye/flame8.tga
		blendFunc add
	}
	{
		map textures/flamer_ye/flameball.tga
		blendFunc add
	}
}

gfx/sprites/firecloud
{

	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm trans
	cull disable
	deformvertexes autosprite


	{
		animMap 10 textures/flamer_ye/flame1.tga textures/flamer_ye/flame2.tga textures/flamer_ye/flame3.tga textures/flamer_ye/flame4.tga textures/flamer_ye/flame5.tga textures/flamer_ye/flame6.tga textures/flamer_ye/flame7.tga textures/flamer_ye/flame8.tga
		blendFunc add
	}
	{
		map textures/flamer_ye/flameball.tga
		blendFunc add
	}
}

//for flamer napalm mode
gfx/flamer/trail
{  
  cull disable
  {
    map textures/flamer_ye/trail.tga
	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
    alphaGen vertex
  }
}


gfx/flamer/primarycharge
{  
  cull disable
  {
    animmap 3 gfx/flamer/primarycharge_1.jpg gfx/flamer/primarycharge_2.jpg gfx/flamer/primarycharge_3.jpg gfx/flamer/primarycharge_4.jpg gfx/flamer/primarycharge_5.jpg gfx/flamer/primarycharge_6.jpg gfx/flamer/primarycharge_7.jpg gfx/flamer/primarycharge_8.jpg gfx/flamer/primarycharge_9.jpg gfx/flamer/primarycharge_10.jpg gfx/flamer/primarycharge_11.jpg gfx/flamer/primarycharge_10.jpg gfx/flamer/primarycharge_9.jpg gfx/flamer/primarycharge_8.jpg gfx/flamer/primarycharge_7.jpg gfx/flamer/primarycharge_6.jpg gfx/flamer/primarycharge_5.jpg gfx/flamer/primarycharge_4.jpg gfx/flamer/primarycharge_3.jpg gfx/flamer/primarycharge_2.jpg gfx/flamer/primarycharge_1.jpg             
		blendFunc GL_ONE GL_ONE
	    tcmod rotate 500
  }
    {
    animmap 6 gfx/flamer/primarycharge_1.jpg gfx/flamer/primarycharge_2.jpg gfx/flamer/primarycharge_3.jpg gfx/flamer/primarycharge_4.jpg gfx/flamer/primarycharge_5.jpg gfx/flamer/primarycharge_6.jpg gfx/flamer/primarycharge_7.jpg gfx/flamer/primarycharge_8.jpg gfx/flamer/primarycharge_9.jpg gfx/flamer/primarycharge_10.jpg gfx/flamer/primarycharge_11.jpg gfx/flamer/primarycharge_10.jpg gfx/flamer/primarycharge_9.jpg gfx/flamer/primarycharge_8.jpg gfx/flamer/primarycharge_7.jpg gfx/flamer/primarycharge_6.jpg gfx/flamer/primarycharge_5.jpg gfx/flamer/primarycharge_4.jpg gfx/flamer/primarycharge_3.jpg gfx/flamer/primarycharge_2.jpg gfx/flamer/primarycharge_1.jpg             
		blendFunc GL_ONE GL_ONE
	    tcmod rotate 300
  }
}

flame1
{
	nopicmip
	cull disable
  {
    map gfx/flame/flame00.tga
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

flame2
{
	nopicmip
	cull disable
  {
    map gfx/flame/flame01.tga
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

flame3
{
	nopicmip
	cull disable
  {
    map gfx/flame/flame02.tga
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

flame4
{
	nopicmip
	cull disable
  {
    map gfx/flame/flame03.tga
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

flame5
{
	nopicmip
	cull disable
  {
    map gfx/flame/flame04.tga
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

flame6
{
	nopicmip
	cull disable
  {
    map gfx/flame/flame05.tga
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

flame7
{
	nopicmip
	cull disable
  {
    map gfx/flame/flame06.tga
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

flame8
{
	nopicmip
	cull disable
  {
    map gfx/flame/flame07.tga
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

flame9
{
	nopicmip
	cull disable
  {
    map gfx/flame/flame08.tga
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

flame10
{
	nopicmip
	cull disable
  {
    map gfx/flame/flame09.tga
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

flame11
{
	nopicmip
	cull disable
  {
    map gfx/flame/flame10.tga
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

flame12
{
	nopicmip
	cull disable
  {
    map gfx/flame/flame11.tga
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

flame13
{
	nopicmip
	cull disable
  {
    map gfx/flame/flame12.tga
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

flame14
{
	nopicmip
	cull disable
  {
    map gfx/flame/flame13.tga
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

flame15
{
	nopicmip
	cull disable
  {
    map gfx/flame/flame14.tga
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

flame16
{
	nopicmip
	cull disable
  {
    map gfx/flame/flame15.tga
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

flame17
{
	nopicmip
	cull disable
  {
    map gfx/flame/flame16.tga
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

flame18
{
	nopicmip
	cull disable
  {
    map gfx/flame/flame17.tga
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

flame19
{
	nopicmip
	cull disable
  {
    map gfx/flame/flame18.tga
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

flame20
{
	nopicmip
	cull disable
  {
    map gfx/flame/flame19.tga
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

flame21
{
	nopicmip
	cull disable
  {
    map gfx/flame/flame20.tga
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

flame22
{
	nopicmip
	cull disable
  {
    map gfx/flame/flame21.tga
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

flame23
{
	nopicmip
	cull disable
  {
    map gfx/flame/flame22.tga
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

flame24
{
	nopicmip
	cull disable
  {
    map gfx/flame/flame23.tga
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

flame25
{
	nopicmip
	cull disable
  {
    map gfx/flame/flame24.tga
    blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}


radialmark
{  
polygonoffset
  {
    map gfx/sprites/radial.tga
    blendFunc add
  }
}

gfx/flamer/primarycharge
{  
  cull disable
  {
    animmap 3 gfx/flamer/primarycharge_1.jpg gfx/flamer/primarycharge_2.jpg gfx/flamer/primarycharge_3.jpg gfx/flamer/primarycharge_4.jpg gfx/flamer/primarycharge_5.jpg gfx/flamer/primarycharge_6.jpg gfx/flamer/primarycharge_7.jpg gfx/flamer/primarycharge_8.jpg gfx/flamer/primarycharge_9.jpg gfx/flamer/primarycharge_10.jpg gfx/flamer/primarycharge_11.jpg gfx/flamer/primarycharge_10.jpg gfx/flamer/primarycharge_9.jpg gfx/flamer/primarycharge_8.jpg gfx/flamer/primarycharge_7.jpg gfx/flamer/primarycharge_6.jpg gfx/flamer/primarycharge_5.jpg gfx/flamer/primarycharge_4.jpg gfx/flamer/primarycharge_3.jpg gfx/flamer/primarycharge_2.jpg gfx/flamer/primarycharge_1.jpg             
		blendFunc GL_ONE GL_ONE
	    tcmod rotate 500
  }
    {
    animmap 6 gfx/flamer/primarycharge_1.jpg gfx/flamer/primarycharge_2.jpg gfx/flamer/primarycharge_3.jpg gfx/flamer/primarycharge_4.jpg gfx/flamer/primarycharge_5.jpg gfx/flamer/primarycharge_6.jpg gfx/flamer/primarycharge_7.jpg gfx/flamer/primarycharge_8.jpg gfx/flamer/primarycharge_9.jpg gfx/flamer/primarycharge_10.jpg gfx/flamer/primarycharge_11.jpg gfx/flamer/primarycharge_10.jpg gfx/flamer/primarycharge_9.jpg gfx/flamer/primarycharge_8.jpg gfx/flamer/primarycharge_7.jpg gfx/flamer/primarycharge_6.jpg gfx/flamer/primarycharge_5.jpg gfx/flamer/primarycharge_4.jpg gfx/flamer/primarycharge_3.jpg gfx/flamer/primarycharge_2.jpg gfx/flamer/primarycharge_1.jpg             
		blendFunc GL_ONE GL_ONE
	    tcmod rotate 300
  }
}
