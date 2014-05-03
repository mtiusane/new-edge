models/buildables/infestation_blister/infestation_texture2
{
  {
    map models/buildables/infestation_blister/infestation_texture.tga
	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	alphaFunc GE128
	depthWrite
  }
}

models/buildables/infestation_blister/infestation_texture
{
  {
    map models/buildables/infestation_blister/infestation_texture.tga
	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	alphaFunc GE128
	depthWrite
  }
    {
    map models/buildables/infestation_blister/base_texture_glow.jpg
	blendfunc add
    rgbGen wave Triangle .5 .5 0 0.2
  }
}