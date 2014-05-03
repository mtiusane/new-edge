models/weapons/psaw/chain
{
	sort additive
	cull disable
	{
		map models/weapons/psaw/chain.jpg
		blendfunc GL_ONE GL_ONE
		tcMod scroll 1.0 -4.0
	}
}

models/weapons/psaw/battery
{
	sort additive
	cull disable
	{
		map models/weapons/psaw/chain.jpg
		blendfunc GL_ONE GL_ONE
		tcMod scroll 0.04 -0.02
	}
}


gfx/psaw/blue_particle
{  
  cull disable
  {
    map gfx/psaw/blue_particle.jpg
    blendFunc GL_ONE GL_ONE
    alphaGen vertex
    rgbGen vertex
  }
}
