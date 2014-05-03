//EDGE MODELS SHADER
//by yalt 2013
//ye.boards.net

//human light(now: power field)

models/powerfield_base/blue_particle
{  
  cull disable
  {
    map models/buildables/light/blue_particle.tga
	blendFunc add
    rgbGen vertex
    alphaGen vertex
  }
}

models/powerfield_base
{
  {
	map models/buildables/light/model_texture.jpg
	rgbGen identity
  }
 //  {
//	map models/buildables/light/model_texture_add.jpg
//	blendfunc add
//	rgbGen identity
 //}
}

models/powerfield_core
{
  {
	map models/buildables/light/model_texture_3.jpg
    blendfunc add
    rgbGen identity
    tcMod scroll 1 0.5
  }
  {
	map models/buildables/light/model_texture_3.jpg
    blendfunc add
    rgbGen identity
    tcMod scroll -1 -0.5
  }

}

models/powerfield_field
{
  {
	map models/buildables/light/model_texture_3.jpg
    blendfunc add
    tcMod scroll 0.1 0.1
  }

}