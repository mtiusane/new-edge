//EDGE MODELS SHADER
//by yalt 2013
//ye.boards.net

models/buildables/infestation_slime/slimetop
{
	cull disable
	{
    map models/buildables/infestation_slime/slimetop.tga
    alphaFunc GE128
    depthWrite
    rgbGen lightingDiffuse
	}
}

models/buildables/infestation_slime/slime
{
  {
    map models/buildables/infestation_slime/slime.tga
	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	alphaFunc GT0
	depthWrite
  }
}
models/buildables/infestation_slime/slime_side
{
  {
    map models/buildables/infestation_slime/slime_side.tga
	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	alphaFunc GT0
	depthWrite
  }
}
//slime hook
models/infestationslimezunge
{
	cull disable
	{
		map textures/slime_ye/zunge.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GE128
		depthWrite
		tcMod scroll 0.2 0
	}
}