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

models/weapons/rocketl/flash
{
	nopicmip
	cull disable
	{
		map gfx/rocketx/0006.jpg
		blendFunc GL_ONE GL_ONE
	}
}

// BEST ROCKET EPXLSION EVAR!
rocketx1
{
	nopicmip
	cull disable
	{
		map gfx/rocketx/0002.jpg
		blendFunc GL_ONE GL_ONE
	}
}

rocketx2
{
	nopicmip
	cull disable
	{
		map gfx/rocketx/0003.jpg
		blendFunc GL_ONE GL_ONE
	}
}

rocketx3
{
	nopicmip
	cull disable
	{
		map gfx/rocketx/0004.jpg
		blendFunc GL_ONE GL_ONE
	}
}

rocketx4
{
	nopicmip
	cull disable
	{
		map gfx/rocketx/0005.jpg
		blendFunc GL_ONE GL_ONE
	}
}

rocketx5
{
	nopicmip
	cull disable
	{
		map gfx/rocketx/0006.jpg
		blendFunc GL_ONE GL_ONE
	}
}

rocketx6
{
	nopicmip
	cull disable
	{
		map gfx/rocketx/0007.jpg
		blendFunc GL_ONE GL_ONE
	}
}

rocketx7
{
	nopicmip
	cull disable
	{
		map gfx/rocketx/0008.jpg
		blendFunc GL_ONE GL_ONE
	}
}

rocketx8
{
	nopicmip
	cull disable
	{
		map gfx/rocketx/0009.jpg
		blendFunc GL_ONE GL_ONE
	}
}

rocketx9
{
	nopicmip
	cull disable
	{
		map gfx/rocketx/0010.jpg
		blendFunc GL_ONE GL_ONE
	}
}

rocketx10
{
	nopicmip
	cull disable
	{
		map gfx/rocketx/0011.jpg
		blendFunc GL_ONE GL_ONE
	}
}

rocketx11
{
	nopicmip
	cull disable
	{
		map gfx/rocketx/0012.jpg
		blendFunc GL_ONE GL_ONE
	}
}

rocketx12
{
	nopicmip
	cull disable
	{
		map gfx/rocketx/0013.jpg
		blendFunc GL_ONE GL_ONE
	}
}

rocketx13
{
	nopicmip
	cull disable
	{
		map gfx/rocketx/0014.jpg
		blendFunc GL_ONE GL_ONE
	}
}

rocketx14
{
	nopicmip
	cull disable
	{
		map gfx/rocketx/0015.jpg
		blendFunc GL_ONE GL_ONE
	}
}

rocketx15
{
	nopicmip
	cull disable
	{
		map gfx/rocketx/0016.jpg
		blendFunc GL_ONE GL_ONE
	}
}

rocketx16
{
	nopicmip
	cull disable
	{
		map gfx/rocketx/0017.jpg
		blendFunc GL_ONE GL_ONE
	}
}

rocketx17
{
	nopicmip
	cull disable
	{
		map gfx/rocketx/0018.jpg
		blendFunc GL_ONE GL_ONE
	}
}

rocketx18
{
	nopicmip
	cull disable
	{
		map gfx/rocketx/0019.jpg
		blendFunc GL_ONE GL_ONE
	}
}

rocketx19
{
	nopicmip
	cull disable
	{
		map gfx/rocketx/0020.jpg
		blendFunc GL_ONE GL_ONE
	}
}

rocketx20
{
	nopicmip
	cull disable
	{
		map gfx/rocketx/0021.jpg
		blendFunc GL_ONE GL_ONE
	}
}

rocketx21
{
	nopicmip
	cull disable
	{
		map gfx/rocketx/0022.jpg
		blendFunc GL_ONE GL_ONE
	}
}

rocketx22
{
	nopicmip
	cull disable
	{
		map gfx/rocketx/0023.jpg
		blendFunc GL_ONE GL_ONE
	}
}

rocketx23
{
	nopicmip
	cull disable
	{
		map gfx/rocketx/0024.jpg
		blendFunc GL_ONE GL_ONE
	}
}

rocketx24
{
	nopicmip
	cull disable
	{
		map gfx/rocketx/0025.jpg
		blendFunc GL_ONE GL_ONE
	}
}

rocketx25
{
	nopicmip
	cull disable
	{
		map gfx/rocketx/0026.jpg
		blendFunc GL_ONE GL_ONE
	}
}

rocketx26
{
	nopicmip
	cull disable
	{
		map gfx/rocketx/0027.jpg
		blendFunc GL_ONE GL_ONE
	}
}

rocketx27
{
	nopicmip
	cull disable
	{
		map gfx/rocketx/0028.jpg
		blendFunc GL_ONE GL_ONE
	}
}


rocketx28
{
	nopicmip
	cull disable
	{
		map gfx/rocketx/0029.jpg
		blendFunc GL_ONE GL_ONE
	}
}

rocketx29
{
	nopicmip
	cull disable
	{
		map gfx/rocketx/0030.jpg
		blendFunc GL_ONE GL_ONE
	}
}

