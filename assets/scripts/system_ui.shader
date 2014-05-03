ui/edge_logo
{
  {
	map ui/assets/edge_logo.jpg
	blendFunc add
	depthWrite
  }
}

ui/edgeterminal
{
  {
	map ui/assets/edge_terminal.tga
	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	alphaFunc GE128
	depthWrite
  }
}

ui/menu_main_aurora_base
{
  {
	map ui/assets/menu_main_aurora_base.jpg
    blendFunc add
  }
  {
	map ui/assets/menu_main_aurora_base_add.jpg
    blendFunc add
	rgbGen wave sin 0.2 0.5 1 0.1
  }

}

ui/menu_main_aurora_base_stars
{
  {
	map ui/assets/menu_main_aurora_base_add_b.jpg
    blendFunc add
	tcMod scroll 0 0.1
  }
  {
	map ui/assets/menu_main_aurora_base_add_b2.jpg
    blendFunc add
	tcMod scroll 0 0.3
  }
  {
	map ui/assets/menu_main_aurora_base_add_b3.jpg
    blendFunc add
	tcMod scroll 0 0.2
  }
}

ui/assets/score_baseb
{
  {
	map ui/assets/score_base.tga
    alphaFunc GE128
  }
}

scripts/loading_add
{
  {
	map ui/assets/loading_add.png
	blendfunc add
  }
}

scripts/stage_add
{
  {
	map ui/assets/stage_add.png
	blendfunc add
  }
}

scripts/main_add
{
  {
	map ui/assets/main_add.png
	blendfunc add
  }
}

scripts/main_add_pulse
{
  {
	map ui/assets/main_add_pulse.png
	blendfunc add
    rgbGen wave sin 0.6 .7 5 0.2
  }
}

scripts/scoreaddlight
{
  {
	map ui/assets/score_flicker.png
	blendfunc add
    rgbGen wave sin 0.4 1 8 0.1
  }
}

scripts/scoreadd
{
  {
	map ui/assets/score_add.png
	blendfunc add
  }
}

//vault boy showing his thumb :D
scripts/yboy
{
  {
	animMap 2 ui/assets/yboy2.jpg ui/assets/yboy2.jpg ui/assets/yboy2.jpg ui/assets/yboy2.jpg ui/assets/yboy2.jpg ui/assets/yboy2.jpg ui/assets/yboy2.jpg ui/assets/yboy.jpg
	blendfunc add
  }
}


scripts/e6
{
  {
	map ui/assets/load_6.jpg
	blendfunc add
  }
  {
	map ui/assets/load.jpg
    blendFunc add
	depthWrite
	tcMod rotate -10
  }
}

//scripts/ye
//{
// {
//	map ui/assets/ye.jpg
//	blendfunc add
//  }
//    {
//	map ui/assets/ye.jpg
//	blendfunc add
//  rgbGen wave sin .8 .5 0 3.0
//  }
//}

gfx/misc/yaltsedges
{
	cull disable
	{
		map gfx/misc/yaltsedges.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}


scripts/ye_full
{
  {
	map ui/assets/ye_full.png
	blendfunc add

  }
    {
	map ui/assets/ye_full.png
	blendfunc add
    rgbGen wave sin 0.1 0.2 0 15
  }
}

ui/assets/neutral/squad_h
{
  nopicmip
  {
    map ui/assets/neutral/squad_h.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbgen vertex
  }
}

ui/assets/neutral/squad_v
{
  nopicmip
  {
    map ui/assets/neutral/squad_v.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbgen vertex
  }
}

