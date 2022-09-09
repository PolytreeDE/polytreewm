#include "layouts.h"

#include "../dwm.h"

const Layout layouts[] = {
	/* arrange function */
	{ monocle }, /* first entry is default */
	{ NULL },    /* no layout function means floating behavior */
	{ tile },
	{ horizontile },
	{ centeredmaster },
};

size_t layouts_count()
{
	return sizeof(layouts) / sizeof(layouts[0]);
}
