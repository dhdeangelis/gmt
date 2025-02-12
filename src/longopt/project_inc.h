/*--------------------------------------------------------------------
 *
 *	Copyright (c) 1991-2025 by the GMT Team (https://www.generic-mapping-tools.org/team.html)
 *	See LICENSE.TXT file for copying and redistribution conditions.
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU Lesser General Public License as published by
 *	the Free Software Foundation; version 3 or any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU Lesser General Public License for more details.
 *
 *	Contact info: www.generic-mapping-tools.org
 *--------------------------------------------------------------------*/

#ifndef PROJECT_INC_H
#define PROJECT_INC_H

/* Translation table from long to short module options, directives and modifiers */

static struct GMT_KEYWORD_DICTIONARY module_kw[] = {
	/* separator, short_option, long_option,
		  short_directives,    long_directives,
		  short_modifiers,     long_modifiers,
		  transproc_mask */
	{ 0, 'A', "azimuth",           "", "", "", "", GMT_TP_STANDARD },
	{ 0, 'C', "center|origin",     "", "", "", "", GMT_TP_STANDARD },
	{ 0, 'E', "endpoint",          "", "", "", "", GMT_TP_STANDARD },
	{ 0, 'F', "outformat|outvars|convention", "", "", "", "", GMT_TP_STANDARD },
	{ 0, 'G', "step|generate",
	          "",                  "",
	          "c,h,n",             "colat,header,npoints",
		  GMT_TP_STANDARD },
	{ 0, 'L', "length",
	          "w",                 "limit",
	          "",                  "",
		  GMT_TP_STANDARD },
	{ 0, 'N', "flat_earth",        "", "", "", "", GMT_TP_STANDARD },
	{ 0, 'Q', "km",                "", "", "", "", GMT_TP_STANDARD },
	{ 0, 'S', "sort",              "", "", "", "", GMT_TP_STANDARD },
	{ 0, 'T', "pole",              "", "", "", "", GMT_TP_STANDARD },
	{ 0, 'W', "width",             "", "", "", "", GMT_TP_STANDARD },
	{ 0, 'Z', "ellipse",
	          "",                  "",
	          "e",                 "equal",
		  GMT_TP_STANDARD },
	{ 0, '\0', "", "", "", "", "", 0 }  /* End of list marked with empty option and strings */
};
#endif  /* !PROJECT_INC_H */
