
gfx/mdriver/green_particle
{  
  cull disable
  {
    map gfx/mdriver/green_particle.tga
    blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
    rgbGen vertex
    alphaGen vertex
  }
}

gfx/blaster/bluevap
{  
  sort Banner
  polygonOffset
  cull disable
  {
    map gfx/mdriver/bluevap.tga
	blendFunc add
    alphaGen vertex
    rgbGen vertex
  }
}

gfx/blaster/bluevap_mark
{  
  sort Banner
  polygonOffset
  cull disable
  {
    map gfx/mdriver/bluevap_b.tga
    blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
    alphaGen vertex
    rgbGen vertex
  }
}