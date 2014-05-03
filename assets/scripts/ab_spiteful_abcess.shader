//EDGE MODELS SHADER
//by yalt 2013
//ye.boards.net

//spiteful abcest
models/buildables/spiteful_abcess/sa_foot_blend
{
  {
    map models/buildables/spiteful_abcess/sa_foot.tga
	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	alphaFunc GE128
	depthWrite
  }
}

models/buildables/spiteful_abcess/sa_organic_head
{
  {
	map models/buildables/spiteful_abcess/sa_organic_head.jpg
  }
    {
	map models/buildables/spiteful_abcess/sa_organic_head_blend.jpg
	blendfunc add
    rgbGen wave Triangle .5 .5 0 0.2
  }
}
