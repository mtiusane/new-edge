gfx/level1/warpOverlay
{
	{
		map gfx/level1/warpOverlay.jpg
		blendFunc GL_DST_COLOR GL_ONE
	}
	{
		map gfx/level1/warpOverlay.jpg
		blendFunc GL_DST_COLOR GL_ONE
	}
	{
		map gfx/level1/warpOverlay.jpg
		blendFunc GL_ONE_MINUS_DST_COLOR GL_ONE_MINUS_SRC_COLOR
	}
}

gfx/level1/warpOverlayBlocked
{
	{
		map gfx/level1/warpOverlayBlocked.jpg
		blendFunc filter
	}
}

gfx/level1/warpParticle1
{
	{
		map gfx/level1/warpParticle1.jpg
		blendFunc add
	}
}

gfx/level1/warping
{
	{
		map gfx/level1/warping2.jpg
		blendFunc filter
		tcMod scroll 3 3
	}
	{
		map gfx/level1/warping.jpg
		blendFunc add
		tcMod scroll -2.19384 -1.95281
	}
}

gfx/level1/warpingEnvironment
{
	{
		map gfx/level1/warpingEnvironment.jpg
		tcMod scroll 0.06 -0.029
	}
}
