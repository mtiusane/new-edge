models/buildables/booster/booster_head
{
	{
		map models/buildables/booster/booster_head.tga
		rgbGen lightingDiffuse
	}
	{
		map models/buildables/booster/ref_map.tga
		blendfunc filter
		rgbGen identity
		tcMod rotate 5
		tcGen environment 
	}
}

models/buildables/booster/booster_sac
{
	{
		map models/buildables/booster/booster_sac.tga
		rgbGen lightingDiffuse
	}
	{
		map models/buildables/booster/poison.tga
		blendfunc add
		tcMod rotate 8
	}
}
models/buildables/booster/pod_strands
{
	cull disable
	{
		map models/buildables/barricade/pod_strands.tga
		rgbGen lightingDiffuse
		alphaFunc GE128
	}
}
