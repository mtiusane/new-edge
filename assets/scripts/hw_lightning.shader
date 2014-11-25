gfx/lightning/particle
{
	{
		map models/weapons/lightning/flash.jpg
		blendFunc add
	}
}

gfx/lightning/beam
{
	nomipmaps
	{
		map models/weapons/lightning/bolt.jpg
		blendFunc add
		tcMod scroll -5 0
		tcMod scale 5 1
	}
}

models/weapons/lightning/flash
{
	cull none
	{
		map models/weapons/lightning/flash.jpg
		blendFunc add
	}
}

models/weapons/lightning/bolt
{
	cull none
	{
		map models/weapons/lightning/bolt.jpg
		blendFunc add
	}
}
