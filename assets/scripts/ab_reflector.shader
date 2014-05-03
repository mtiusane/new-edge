//EDGE MODELS SHADER
//by yalt 2013
//ye.boards.net

//for aliens mercury reflector
models/buildables/reflector/mer
{
  {
    map models/buildables/reflector/mer.jpg
    tcGen environment
  }
}

models/buildables/reflector/haut_fetzen_b
{
  {
    map models/buildables/reflector/haut_fetzen_b.tga
	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	alphaFunc GT0
	depthWrite
  }
}