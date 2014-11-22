rocketFlash
{
	deformVertexes autosprite
	{
		clampmap models/weapons/rocketl/muzzy.tga
		blendfunc add
		tcMod rotate 1246
	}
	{
		clampmap models/weapons/rocketl/muzzy.tga
		blendfunc add
		tcMod rotate -1246
	}
}

rocketFlare
{
	deformVertexes autosprite
	{
		clampmap models/weapons/rocketl/wide.tga
		blendfunc add
		rgbGen const ( 0.980392 0.882353 0.737255 )
	}
}

rocketFlareParticle
{
	{
		clampmap models/weapons/rocketl/wide.tga
		blendfunc add
		rgbGen const ( 0.980392 0.882353 0.737255 )
	}
}

rocketThrust
{
	cull disable
	{
		clampmap models/weapons/rocketl/flarey.tga
		blendfunc add
		rgbGen const ( 0.541176 0.341176 0.156863 )
		tcMod rotate -96
		tcMod stretch sin 0.3 0.7 0 8 
	}
	{
		clampmap models/weapons/rocketl/wide.tga
		blendfunc add
		rgbGen const ( 0.843137 0.588235 0.4 )
	}
	{
		clampmap models/weapons/rocketl/newflare.tga
		blendfunc add
		rgbGen const ( 0.713726 0.270588 0.121569 )
		tcMod rotate 46
		tcMod stretch inversesawtooth 0.6 0.4 0 4 
	}
}

