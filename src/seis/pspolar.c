/*--------------------------------------------------------------------
 *
 *	Copyright (c) 2013-2025 by the GMT Team (https://www.generic-mapping-tools.org/team.html)
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
/*
 *	Copyright (c) 1996-2012 by G. Patau
 *	Donated to the GMT project by G. Patau upon her retirement from IGPG
 *--------------------------------------------------------------------*/

/*
 * pspolar will read azimuth, take-off angle of seismic ray and polarities
 * in stations and plots polarities on the inferior focal half-sphere.
 * A variety of symbols may be specified.
 * Only one event may be plotted at a time.
 * PostScript code is written to stdout.
 *
 * Author:	Genevieve Patau
 * Date:	19-OCT-1995 (psprojstations)
 * Version:	5
 * Roots:	heavily based on psxy.c; ported to GMT5 by P. Wessel
 * Updated March 1, 2021: No longer support the relocation of the
 *   focal sphere and the optional line/point plotting since that is
 *   better handled by meca.  However, it remains backwards supported
 *   via the old -C option, just no longer documented.  The -D option
 *   is used to place the focal sphere wherever you want.
 */

#include "gmt_dev.h"
#include "seis_defaults.h"
#include "longopt/pspolar_inc.h"

#define THIS_MODULE_CLASSIC_NAME	"pspolar"
#define THIS_MODULE_MODERN_NAME	"polar"
#define THIS_MODULE_LIB		"seis"
#define THIS_MODULE_PURPOSE	"Plot polarities on the lower hemisphere of the focal sphere"
#define THIS_MODULE_KEYS	"<D{,>X}"
#define THIS_MODULE_NEEDS	"Jd"
#define THIS_MODULE_OPTIONS "-:>BHJKOPRUVXYdehiqt" GMT_OPT("c")

/* Control structure for pspolar */

struct PSPOLAR_CTRL {
	/* Leave -C available for future cpt use */
	struct PSPOLAR_OLD_C {	/* -C<lon>/<lat>[+p<pen>][+s<size>] */
		bool active;
		double lon2, lat2, size;
		struct GMT_PEN pen;
	} OLD_C;
	struct PSPOLAR_D {	/* -D<lon>/<lat> */
		bool active;
		double lon, lat;
	} D;
 	struct PSPOLAR_E {	/* -E<fill> */
		bool active[2];	/* [0] for fill, [1] for pen */
		struct GMT_FILL fill;
		struct GMT_PEN pen;
	} E;
	struct PSPOLAR_F {	/* -F<fill> */
		bool active[2];	/* [0] for fill, [1] for pen */
		struct GMT_FILL fill;
		struct GMT_PEN pen;
	} F;
 	struct PSPOLAR_G {	/* -G<fill> */
		bool active[2];	/* [0] for fill, [1] for pen */
		struct GMT_FILL fill;
		struct GMT_PEN pen;
	} G;
	struct PSPOLAR_M {	/* -M<scale>[+m<magnitude>] */
		bool active;
		double ech;
	} M;
	struct PSPOLAR_N {	/* -N */
		bool active;
	} N;
	struct PSPOLAR_Q {	/* Repeatable: -Q<mode>[<args>] for various symbol parameters */
		bool active;
	} Q;
	struct PSPOLAR_H2 {	/* -Qh for Hypo71 */
		bool active;
	} H2;
	struct PSPOLAR_S {	/* -S<symbol><size>[c|i|p] */
		bool active;
		int symbol;
		double size;
		struct GMT_FILL fill;
	} S;
	struct PSPOLAR_S2 {	/* -Qs<halfsize>[+v<size>[+<specs>] */
		bool active;
		bool scolor;
		bool vector;
		int symbol;
		int outline;
		char type;
		double size;
		double width, length, head;
		double vector_shape;
		struct GMT_FILL fill;
		struct GMT_SYMBOL S;
	} S2;
	struct PSPOLAR_T { /* New syntax: -T+a<angle>+j<justify>+o<dx>/<dy>+f<font> */
		bool active;
		double angle;
		int justify;
		double offset[2];
		struct GMT_FONT font;
		int form; /* for back-compatibility only */
 	} T;
	struct PSPOLAR_W {	/* -W<pen> */
		bool active;
		struct GMT_PEN pen;
	} W;
};

static void *New_Ctrl (struct GMT_CTRL *GMT) {	/* Allocate and initialize a new control structure */
	struct PSPOLAR_CTRL *C;

	C = gmt_M_memory (GMT, NULL, 1, struct PSPOLAR_CTRL);

	/* Initialize values whose defaults are not 0/false/NULL */

	/* Deprecated -C option */
	C->OLD_C.size = 0.0;	/* Default is to plot no circle */
	C->OLD_C.pen = GMT->current.setting.map_default_pen;

	C->E.pen = C->F.pen = C->G.pen = GMT->current.setting.map_default_pen;

	gmt_init_fill (GMT, &C->E.fill, gmt_M_is255(250), gmt_M_is255(250), gmt_M_is255(250));
	gmt_init_fill (GMT, &C->F.fill, -1.0, -1.0, -1.0);
	gmt_init_fill (GMT, &C->G.fill, 0.0, 0.0, 0.0);
	gmt_init_fill (GMT, &C->S2.fill, -1.0, -1.0, -1.0);
	C->T.justify = PSL_TC;
	C->T.font.size = SEIS_DEFAULT_FONTSIZE;
	return (C);
}

static void Free_Ctrl (struct GMT_CTRL *GMT, struct PSPOLAR_CTRL *C) {	/* Deallocate control structure */
	if (!C) return;
	gmt_M_free (GMT, C);
}

static int usage (struct GMTAPI_CTRL *API, int level) {
	/* This displays the pspolar synopsis and optionally full usage information */

	struct GMT_FONT font;
	const char *name = gmt_show_name_and_purpose (API, THIS_MODULE_LIB, THIS_MODULE_CLASSIC_NAME, THIS_MODULE_PURPOSE);
	if (level == GMT_MODULE_PURPOSE) return (GMT_NOERROR);
	GMT_Usage (API, 0, "usage: %s [<table>] -D<lon>/<lat> %s %s -M<size>[c|i|p][+m<mag>] -S<symbol><size>[c|i|p] "
		"[%s] [-E<fill>] [-F<fill>] [-G<fill>] %s[-N] %s%s[-Qe[<pen>]] [-Qf[<pen>]] [-Qg[<pen>]] [-Qh] "
		"[-Qs<halfsize>[+v<size>[<specs>]]] [-Qt<pen>] [-T[+a<angle>][+f<font>][+j<justify>][+o<dx>[/<dy>]]] "
		"[%s] [%s] [-W<pen>] [%s] [%s] %s[%s] [%s] [%s] [%s] [%s] [%s] [%s]\n", name, GMT_J_OPT, GMT_Rgeo_OPT,
		GMT_B_OPT, API->K_OPT, API->O_OPT, API->P_OPT, GMT_U_OPT, GMT_V_OPT, GMT_X_OPT, GMT_Y_OPT, API->c_OPT,
		GMT_di_OPT, GMT_e_OPT, GMT_h_OPT, GMT_i_OPT, GMT_qi_OPT, GMT_t_OPT, GMT_PAR_OPT);

	if (level == GMT_SYNOPSIS) return (GMT_MODULE_SYNOPSIS);

	font = API->GMT->current.setting.font_annot[GMT_PRIMARY];
	font.size = SEIS_DEFAULT_FONTSIZE;
	GMT_Message (API, GMT_TIME_NONE, "  REQUIRED ARGUMENTS:\n");
	GMT_Option (API, "<");
	GMT_Usage (API, 1, "\n-D<lon>/<lat>");
	GMT_Usage (API, -2, "Set longitude/latitude of where to center the focal sphere on the map.");
	GMT_Option (API, "J-");
	GMT_Usage (API, 1, "\n-M<size>[c|i|p][+m<mag>]");
	GMT_Usage (API, -2, "Set size of focal sphere in %s. Append +m<mag> to specify a magnitude, and focal sphere size is <mag> / 5.0 * <size>.",
		API->GMT->session.unit_name[API->GMT->current.setting.proj_length_unit]);
	GMT_Option (API, "R");
	GMT_Usage (API, 1, "\n-S<symbol><size>[c|i|p]");
	GMT_Usage (API, -2, "Select symbol type and symbol size (in %s).  Choose between "
		"st(a)r, (c)ircle, (d)iamond, (h)exagon, (i)nvtriangle, "
		"(p)oint, (s)quare, (t)riangle, and (x)cross.",
		API->GMT->session.unit_name[API->GMT->current.setting.proj_length_unit]);
	GMT_Message (API, GMT_TIME_NONE, "\n  OPTIONAL ARGUMENTS:\n");
	GMT_Option (API, "B-");
	gmt_fill_syntax (API->GMT, 'E', NULL, "Specify color symbol for station in extensive part [Default is light gray].");
	gmt_fill_syntax (API->GMT, 'F', NULL, "Specify background color of focal sphere [Default is no fill].");
	gmt_fill_syntax (API->GMT, 'G', NULL, "Specify color symbol for station in compressive part [Default is black].");
	GMT_Option (API, "K");
	GMT_Usage (API, 1, "\n-N Do Not skip/clip symbols that fall outside map border [Default will ignore those outside].");
	GMT_Option (API, "O,P");
	GMT_Usage (API, 1, "\n-Q<directive><parameters> (repeatable)");
	GMT_Usage (API, -2, "Set various attributes of symbols depending on <directive>:");
	GMT_Usage (API, 3, "e: Outline of station symbol in extensive part [Default is current pen].");
	GMT_Usage (API, 3, "f: Outline focal sphere.  Add <pen attributes> [Default is current pen].");
	GMT_Usage (API, 3, "g: Outline of station symbol in compressive part. "
		"Add pen attributes if not current pen.");
	GMT_Usage (API, 3, "h: Use special format derived from HYPO71 output.");
	GMT_Usage (API, 3, "s: Plot S polarity azimuth: Append <halfsize>[+v<size>[<specs>]]. "
		"Azimuth of S polarity is in last column. "
		"Specify a vector (with +v modifier) [Default is segment line. "
		"Default definition of vector is +v0.3i+e+gblack if just +v is given.");
	GMT_Usage (API, 1, "\n-T[+a<angle>][+f<font>][+j<justify>][+o<dx>[/<dy>]]");
	GMT_Usage (API, -2, "Write station code near the symbol. Optional modifiers for the label:");
	GMT_Usage (API, 3, "+a Set the label angle [0].");
	GMT_Usage (API, 3, "+f Set font attributes for the label [%s].", gmt_putfont (API->GMT, &font));
	GMT_Usage (API, 3, "+j Set the label <justification> [TC].");
	GMT_Usage (API, 3, "+o Set the label offset <dx>[/<dy>] [0/0].");
	GMT_Option (API, "U,V");
	GMT_Usage (API, 1, "\n-W<pen>");
	GMT_Usage (API, -2, "Set pen attributes [%s].", gmt_putpen (API->GMT, &API->GMT->current.setting.map_default_pen));
	GMT_Option (API, "X,c,di,e,h,i,qi,t,.");

	return (GMT_MODULE_USAGE);
}

GMT_LOCAL unsigned int pspolar_old_Q_parser (struct GMT_CTRL *GMT, char *arg, struct PSPOLAR_CTRL *Ctrl) {
	/* Deal with the old syntax: -Qs<halfsize>/[V[<v_width/h_length/h_width/shape>]][G<r/g/b>][L] */
	char *c = NULL, *text = strdup (arg);	/* Work on a copy */
	unsigned int n_errors = 0;
	GMT_Report (GMT->parent, GMT_MSG_COMPAT, "-QsV<v_width>/<h_length>/<h_width>/<shape> is deprecated; use -Qs+v<vecpar> instead.\n");
	if ((c = strchr (text, 'L'))) {	/* Found trailing L for outline */
		Ctrl->S2.outline = true;
		c[0] = '\0';	/* Chop off L */
	}
	if ((c = strchr (text, 'G'))) {	/* Found trailing G for fill */
		if (gmt_getfill (GMT, &c[1], &Ctrl->S2.fill)) {
			gmt_fill_syntax (GMT, ' ', "QsG", " ");
			n_errors++;
		}
		Ctrl->S2.scolor = true;
		c[0] = '\0';	/* Chop off G */
	}

	if ((c = strchr (text, 'V'))) {	/* Got the Vector specs */
		Ctrl->S2.vector = true;
		if (c[1] == '\0') {	/* Provided no size information - set defaults */
			Ctrl->S2.width = 0.03; Ctrl->S2.length = 0.12; Ctrl->S2.head = 0.1;
			Ctrl->S2.vector_shape = GMT->current.setting.map_vector_shape;
			if (!GMT->current.setting.proj_length_unit) {
				Ctrl->S2.width = 0.075; Ctrl->S2.length = 0.3; Ctrl->S2.head = 0.25;
				Ctrl->S2.vector_shape = GMT->current.setting.map_vector_shape;
			}
		}
		else {
			char txt_a[GMT_LEN64] = {""}, txt_b[GMT_LEN64] = {""};
			char txt_c[GMT_LEN64] = {""}, txt_d[GMT_LEN64] = {""};
			sscanf (&c[1], "%[^/]/%[^/]/%[^/]/%s", txt_a, txt_b, txt_c, txt_d);
			Ctrl->S2.width  = gmt_M_to_inch (GMT, txt_a);
			Ctrl->S2.length = gmt_M_to_inch (GMT, txt_b);
			Ctrl->S2.head   = gmt_M_to_inch (GMT, txt_c);
			Ctrl->S2.vector_shape = atof (txt_d);
		}
		Ctrl->S2.S.symbol = GMT_SYMBOL_VECTOR_V4;
	}
	gmt_M_str_free (text);
	return (n_errors);

}

static int parse (struct GMT_CTRL *GMT, struct PSPOLAR_CTRL *Ctrl, struct GMT_OPTION *options) {
	/* This parses the options provided to pspolar and sets parameters in Ctrl.
	 * Note Ctrl has already been initialized and non-zero default values set.
	 * Any GMT common options will override values set previously by other commands.
	 * It also replaces any file names specified as input or output with the data ID
	 * returned when registering these sources/destinations with the API.
	 */

	unsigned int n_errors = 0, n;
	char txt_a[GMT_LEN64] = {""}, txt_b[GMT_LEN64] = {""}, *p = NULL;
	struct GMT_OPTION *opt = NULL;
	struct GMTAPI_CTRL *API = GMT->parent;

	for (opt = options; opt; opt = opt->next) {	/* Process all the options given */

		switch (opt->option) {

			case '<':	/* Input files */
				if (GMT_Get_FilePath (API, GMT_IS_DATASET, GMT_IN, GMT_FILE_REMOTE, &(opt->arg))) n_errors++;
				break;

			/* Processes program-specific parameters */

			case 'C':	/* Backwards compatible section for old way to set alternate coordinates. Now deprecated */
				n_errors += gmt_M_repeated_module_option (API, Ctrl->OLD_C.active);
				if (strchr (opt->arg, 'W') || strchr (opt->arg, 'P')) {	/* Old-style -C args, honor them */
					if (gmt_M_compat_check (GMT, 6))
						GMT_Report (API, GMT_MSG_COMPAT, "-Clon/lat[W<pen>][Psize] is deprecated; use -D<lon>/<lat>[+z<lon>/<lat>[+p<pen>][+s<size>]] instead.\n");
					else {	/* Hard error */
						n_errors += gmt_default_option_error (GMT, opt);
						continue;
					}
					sscanf (opt->arg, "%lf/%lf", &Ctrl->OLD_C.lon2, &Ctrl->OLD_C.lat2);
					/* Slightly more modern modifiers, now deprecated */
					if ((p = strstr (opt->arg, "+p"))) {
						char *q = strstr (p, "+s");
						if (q) q[0] = '\0';	/* Chop off the +s modifier */
						if (gmt_getpen (GMT, &p[2], &Ctrl->OLD_C.pen)) {
							gmt_pen_syntax (GMT, 'C', NULL, "Line connecting new and old point [Default current pen]", NULL, 0);
							n_errors++;
						}
						if (q) q[0] = '+';	/* Restore the +s modifier */
					}
					else if ((p = strchr (opt->arg, 'W')) && gmt_getpen (GMT, &p[1], &Ctrl->OLD_C.pen)) {	/* Old syntax */
						gmt_pen_syntax (GMT, 'C', NULL, "Line connecting new and old point [Default current pen]", NULL, 0);
						n_errors++;
					}
					if ((p = strstr (opt->arg, "+s"))) {	/* Found +s<size> */
						char *q = strstr (p, "+p");
						if (q) q[0] = '\0';	/* Chop off the +p modifier */
						if ((Ctrl->OLD_C.size = gmt_M_to_inch (GMT, &p[2]))) {
							GMT_Report (API, GMT_MSG_ERROR, "Option -C: Could not decode diameter %s\n", &p[1]);
							n_errors++;
						}
						if (q) q[0] = '+';	/* Restore the +p modifier */
					}
					else if ((p = strchr (opt->arg, 'P')) && sscanf (&p[1], "%lf", &Ctrl->OLD_C.size)) {	/* Old syntax */
						GMT_Report (API, GMT_MSG_ERROR, "Option -C: Could not decode diameter %s\n", &p[1]);
						n_errors++;
					}
				}
				else if (gmt_count_char (GMT, opt->arg, '/') == 1 && strstr (opt->arg, ".cpt") == NULL) {	/* Just old-style coordinates without modifiers */
					if (gmt_M_compat_check (GMT, 6))
						GMT_Report (API, GMT_MSG_COMPAT, "-Clon/lat[W<pen>][Psize] is deprecated; use -D<lon>/<lat>[+z<lon>/<lat>[+p<pen>][+s<size>]] instead.\n");
					else {	/* Hard error */
						n_errors += gmt_default_option_error (GMT, opt);
						continue;
					}
					sscanf (opt->arg, "%lf/%lf", &Ctrl->OLD_C.lon2, &Ctrl->OLD_C.lat2);
				}
				break;
			case 'D':	/* Location for focal sphere placement, with optional alternate location */
				n_errors += gmt_M_repeated_module_option (API, Ctrl->D.active);
				/* Now get map location to place the focal sphere plot */
				if (sscanf (opt->arg, "%[^/]/%s", txt_a, txt_b) != 2) {
					GMT_Report (API, GMT_MSG_ERROR, "Option -D: Could not extract lon/lat coordinates from location %s\n", opt->arg);
					n_errors++;
				}
				n_errors += gmt_verify_expectations (GMT, GMT_IS_LON, gmt_scanf (GMT, txt_a, GMT_IS_LON, &Ctrl->D.lon), txt_a);
				n_errors += gmt_verify_expectations (GMT, GMT_IS_LAT, gmt_scanf (GMT, txt_b, GMT_IS_LAT, &Ctrl->D.lat), txt_b);
				break;
			case 'E':	/* Set color for station in extensive part */
				n_errors += gmt_M_repeated_module_option (API, Ctrl->E.active[0]);
				if (gmt_getfill (GMT, opt->arg, &Ctrl->E.fill)) {
					gmt_fill_syntax (GMT, 'E', NULL, " ");
					n_errors++;
				}
				break;
			case 'F':	/* Set background color of focal sphere */
				n_errors += gmt_M_repeated_module_option (API, Ctrl->F.active[0]);
				if (gmt_getfill (GMT, opt->arg, &Ctrl->F.fill)) {
					gmt_fill_syntax (GMT, 'F', NULL, " ");
					n_errors++;
				}
				break;
			case 'G':	/* Set color for station in compressive part */
				n_errors += gmt_M_repeated_module_option (API, Ctrl->G.active[0]);
				if (gmt_getfill (GMT, opt->arg, &Ctrl->G.fill)) {
					gmt_fill_syntax (GMT, 'G', NULL, " ");
					n_errors++;
				}
				break;
			case 'Q':	/* Repeatable; Controls various symbol attributes  */
				Ctrl->Q.active = true;
				switch (opt->arg[0]) {
					case 'e':	/* Outline station symbol in extensive part */
						n_errors += gmt_M_repeated_module_option (API, Ctrl->E.active[1]);
						if (strlen (&opt->arg[1]) && gmt_getpen (GMT, &opt->arg[1], &Ctrl->E.pen)) {
							gmt_pen_syntax (GMT, ' ', "Qe", "Outline station symbol (extensive part) [Default current pen]", NULL, 0);
							n_errors++;
						}
						break;
					case 'f':	/* Outline focal sphere */
						n_errors += gmt_M_repeated_module_option (API, Ctrl->F.active[1]);
						if (strlen (&opt->arg[1]) && gmt_getpen (GMT, &opt->arg[1], &Ctrl->F.pen)) {
							gmt_pen_syntax (GMT, ' ', "Qf", "Outline focal sphere [Default current pen]", NULL, 0);
							n_errors++;
						}
						break;
					case 'g':	/* Outline station symbol in compressive part */
						n_errors += gmt_M_repeated_module_option (API, Ctrl->G.active[1]);
						if (strlen (&opt->arg[1]) && gmt_getpen (GMT, &opt->arg[1], &Ctrl->G.pen)) {
							gmt_pen_syntax (GMT, ' ', "Qg", "Outline station symbol (compressive part) [Default current pen]", NULL, 0);
							n_errors++;
						}
						break;
					case 'h':	/* Use HYPO71 format */
						n_errors += gmt_M_repeated_module_option (API, Ctrl->H2.active);
						break;
					case 's':	/* Get S polarity */
						n_errors += gmt_M_repeated_module_option (API, Ctrl->S2.active);
						p = strchr (opt->arg, '/');	/* Find the first slash */
						if (p) p[0] = '\0';	/* Temporarily remove the slash */
						Ctrl->S2.size = gmt_M_to_inch (GMT, opt->arg);
						if (p) p[0] = '/';	/* Restore the slash */
						if (p && p[0] == '/' && (strchr (opt->arg, 'V') || strchr (opt->arg, 'G') || strchr (opt->arg, 'L')))	/* Clearly got the old syntax */
							n_errors += pspolar_old_Q_parser (GMT, &p[1], Ctrl);
						else {	/* New syntax: -Qs[+v[<size>][+parameters]] */
							char symbol = (gmt_M_is_geographic (GMT, GMT_IN)) ? '=' : 'v';	/* Type of vector */
							if ((p = strstr (opt->arg, "+v"))) {	/* Got vector specification +v<size>[+<attributes>] */
								if (p[2] == '\0') {	/* Nothing, use defaults */
									Ctrl->S2.S.size_x = 0.3;	/* Length of vector */
									n_errors += gmt_parse_vector (GMT, symbol, "+e+gblack", &Ctrl->S2.S);
								}
								else if (p[2] == '+') {	/* No size (use default), just attributes */
									Ctrl->S2.S.size_x = 0.3;	/* Length of vector */
									n_errors += gmt_parse_vector (GMT, symbol, &p[2], &Ctrl->S2.S);
								}
								else {	/* Size, plus possible attributes */
									n = sscanf (&p[2], "%[^+]%s", txt_a, txt_b);	/* txt_a should be symbols size with any +<modifiers> in txt_b */
									if (n == 1) txt_b[0] = '\0';	/* No modifiers were present, set txt_b to empty */
									Ctrl->S2.S.size_x = gmt_M_to_inch (GMT, txt_a);	/* Length of vector */
									n_errors += gmt_parse_vector (GMT, symbol, txt_b, &Ctrl->S2.S);
								}
							}
						}
						break;
					case 't':	/* Set color for station label */
						if (gmt_getpen (GMT, &opt->arg[1], &Ctrl->T.font.pen)) {
							gmt_pen_syntax (GMT, ' ', "Qt", "Station code symbol[Default current pen]", NULL, 0);
							n_errors++;
						}
						break;
				}
				break;
			case 'M':	/* Focal sphere size -M<scale>+m<magnitude>*/
				n_errors += gmt_M_repeated_module_option (API, Ctrl->M.active);
				sscanf(opt->arg, "%[^+]%s", txt_a, txt_b);
				Ctrl->M.ech = gmt_M_to_inch (GMT, txt_a);
				if ((p = strstr (txt_b, "+m")) != NULL && p[2]) {
					/* if +m<mag> is used, the focal sphere size is <mag>/5.0 * <size> */
					double magnitude;
					sscanf ((p+2), "%lf", &magnitude);
					Ctrl->M.ech *= (magnitude / 5.0);
				}
				break;
			case 'N':	/* Do not skip points outside border */
				n_errors += gmt_M_repeated_module_option (API, Ctrl->N.active);
				n_errors += gmt_get_no_argument (GMT, opt->arg, opt->option, 0);
				break;
			case 'S':	/* Get symbol [and size] */
				n_errors += gmt_M_repeated_module_option (API, Ctrl->S.active);
				switch (opt->arg[0]) {
					case 'a':	Ctrl->S.symbol = PSL_STAR;		break;
					case 'c':	Ctrl->S.symbol = PSL_CIRCLE;	break;
					case 'd':	Ctrl->S.symbol = PSL_DIAMOND;	break;
					case 'h':	Ctrl->S.symbol = PSL_HEXAGON;	break;
					case 'i':	Ctrl->S.symbol = PSL_INVTRIANGLE;	break;
					case 'p':	Ctrl->S.symbol = PSL_DOT;		break;
					case 's':	Ctrl->S.symbol = PSL_SQUARE;	break;
					case 't':	Ctrl->S.symbol = PSL_TRIANGLE;	break;
					case 'x':	Ctrl->S.symbol = PSL_CROSS;		break;
					default:
						n_errors++;
						GMT_Report (API, GMT_MSG_ERROR, "Option -S: Unrecognized symbol type %c\n", opt->arg[0]);
						break;
				}
				Ctrl->S.size = gmt_M_to_inch (GMT, &opt->arg[1]);
				break;
			case 'T':	/* Information about label printing */
				n_errors += gmt_M_repeated_module_option (API, Ctrl->T.active);
				if (strlen (opt->arg)) {
					/* New syntax: -T+a<angle>+j<justify>+o<dx>/<dy>+f<font> */
					if (gmt_found_modifier (GMT, opt->arg, "afjo")) {
						char word[GMT_LEN64] = {""};
						if (gmt_get_modifier (opt->arg, 'a', word))
							Ctrl->T.angle = atof(word);
						if (gmt_get_modifier (opt->arg, 'j', word) && strchr ("LCRBMT", word[0]) && strchr ("LCRBMT", word[1]))
							Ctrl->T.justify = gmt_just_decode (GMT, word, Ctrl->T.justify);
						if (gmt_get_modifier (opt->arg, 'f', word)) {
							if (word[0] == '-' || (word[0] == '0' && (word[1] == '\0' || word[1] == 'p')))
								Ctrl->T.font.size = 0.0;
							else
								n_errors += gmt_getfont (GMT, word, &(Ctrl->T.font));
						}
						if (gmt_get_modifier (opt->arg, 'o', word)) {
							if (gmt_get_pair (GMT, word, GMT_PAIR_DIM_DUP, Ctrl->T.offset) < 0) n_errors++;
						} else {	/* Set default offset */
							if (Ctrl->T.justify%4 != 2) /* Not center aligned */
								Ctrl->T.offset[0] = SEIS_DEFAULT_OFFSET * GMT->session.u2u[GMT_PT][GMT_INCH];
							if (Ctrl->T.justify/4 != 1) /* Not middle aligned */
								Ctrl->T.offset[1] = SEIS_DEFAULT_OFFSET * GMT->session.u2u[GMT_PT][GMT_INCH];
						}
					} else {
						/* Old syntax: -T<angle>/<form>/<justify>/<fontsize> */
						sscanf (opt->arg, "%lf/%d/%d/%lf/", &Ctrl->T.angle, &Ctrl->T.form, &Ctrl->T.justify, &Ctrl->T.font.size);
					}
				}
				break;
			case 'W':	/* Set line attributes */
				n_errors += gmt_M_repeated_module_option (API, Ctrl->W.active);
				if (opt->arg && gmt_getpen (GMT, opt->arg, &Ctrl->W.pen)) {
					gmt_pen_syntax (GMT, 'W', NULL, " ", NULL, 0);
					n_errors++;
				}
				break;
			default:	/* Report bad options */
				n_errors += gmt_default_option_error (GMT, opt);
				break;

		}
	}

	n_errors += gmt_M_check_condition (GMT, !GMT->common.R.active[RSET], "Must specify -R option\n");
	n_errors += gmt_M_check_condition (GMT, Ctrl->M.ech <= 0.0, "Option -M: must specify a size\n");
	n_errors += gmt_M_check_condition (GMT, Ctrl->D.active + Ctrl->M.active + Ctrl->S.active < 3, "-D, -M, -S must be set together\n");

	return (n_errors ? GMT_PARSE_ERROR : GMT_NOERROR);
}

#define bailout(code) {gmt_M_free_options (mode); return (code);}
#define Return(code) {Free_Ctrl (GMT, Ctrl); gmt_end_module (GMT, GMT_cpy); bailout (code);}

EXTERN_MSC int GMT_pspolar (void *V_API, int mode, void *args) {
	int n = 0, error = 0;
	bool old_is_world;

	double plot_x, plot_y, symbol_size2 = 0, plot_x0, plot_y0, azS = 0, si, co;
	double new_plot_x0, new_plot_y0, radius, azimut = 0, ih = 0, plongement = 0.0;

	char col[4][GMT_LEN64], pol, stacode[GMT_LEN64] = {""};

	struct GMT_RECORD *In = NULL;
	struct PSPOLAR_CTRL *Ctrl = NULL;
	struct GMT_CTRL *GMT = NULL, *GMT_cpy = NULL;		/* General GMT internal parameters */
	struct GMT_OPTION *options = NULL;
	struct PSL_CTRL *PSL = NULL;				/* General PSL internal parameters */
	struct GMTAPI_CTRL *API = gmt_get_api_ptr (V_API);	/* Cast from void to GMTAPI_CTRL pointer */

	/*----------------------- Standard module initialization and parsing ----------------------*/

	if (API == NULL) return (GMT_NOT_A_SESSION);
	if (mode == GMT_MODULE_PURPOSE) return (usage (API, GMT_MODULE_PURPOSE));	/* Return the purpose of program */
	options = GMT_Create_Options (API, mode, args);	if (API->error) return (API->error);	/* Set or get option list */

	if ((error = gmt_report_usage (API, options, 0, usage)) != GMT_NOERROR) bailout (error);	/* Give usage if requested */

	/* Parse the command-line arguments; return if errors are encountered */

	if ((GMT = gmt_init_module (API, THIS_MODULE_LIB, THIS_MODULE_CLASSIC_NAME, THIS_MODULE_KEYS, THIS_MODULE_NEEDS, module_kw, &options, &GMT_cpy)) == NULL) bailout (API->error); /* Save current state */
	if (GMT_Parse_Common (API, THIS_MODULE_OPTIONS, options)) Return (API->error);
	Ctrl = New_Ctrl (GMT);	/* Allocate and initialize a new control structure */
	if ((error = parse (GMT, Ctrl, options)) != 0) Return (error);

	/*---------------------------- This is the pspolar main code ----------------------------*/

	gmt_M_memset (col, GMT_LEN64*4, char);
	if (gmt_map_setup (GMT, GMT->common.R.wesn)) Return (GMT_PROJECTION_ERROR);

	if ((PSL = gmt_plotinit (GMT, options)) == NULL) Return (GMT_RUNTIME_ERROR);
 	gmt_plotcanvas (GMT);	/* Fill canvas if requested */
	gmt_set_basemap_orders (GMT, Ctrl->N.active ? GMT_BASEMAP_FRAME_BEFORE : GMT_BASEMAP_FRAME_AFTER, GMT_BASEMAP_GRID_BEFORE, GMT_BASEMAP_ANNOT_BEFORE);
	gmt_map_basemap (GMT);	/* Lay down gridlines */

	PSL_setfont (PSL, GMT->current.setting.font_annot[GMT_PRIMARY].id);

	if (!Ctrl->N.active) gmt_map_clip_on (GMT, GMT->session.no_rgb, 3);

	old_is_world = GMT->current.map.is_world;

	GMT->current.map.is_world = true;

	gmt_geo_to_xy (GMT, Ctrl->D.lon, Ctrl->D.lat, &plot_x0, &plot_y0);
	if (Ctrl->OLD_C.active) {	/* This is deprecated but honored in a backwards compatible way */
		gmt_setpen (GMT, &Ctrl->OLD_C.pen);
		gmt_geo_to_xy (GMT, Ctrl->OLD_C.lon2, Ctrl->OLD_C.lat2, &new_plot_x0, &new_plot_y0);
		if (Ctrl->OLD_C.size > 0.0) PSL_plotsymbol (PSL, plot_x0, plot_y0, &(Ctrl->OLD_C.size), PSL_CIRCLE);
		PSL_plotsegment (PSL, plot_x0, plot_y0, new_plot_x0, new_plot_y0);
		plot_x0 = new_plot_x0;
		plot_y0 = new_plot_y0;
	}
	if (Ctrl->N.active) {
		gmt_map_outside (GMT, Ctrl->D.lon, Ctrl->D.lat);
		if (abs (GMT->current.map.this_x_status) > 1 || abs (GMT->current.map.this_y_status) > 1) {
			GMT_Report (API, GMT_MSG_WARNING, "Point given by -D is outside map; no plotting occurs.");
			Return (GMT_NOERROR);
		};
	}

	gmt_setpen (GMT, &Ctrl->F.pen);
	gmt_setfill (GMT, &(Ctrl->F.fill), Ctrl->F.active[1]);
	PSL_plotsymbol (PSL, plot_x0, plot_y0, &(Ctrl->M.ech), PSL_CIRCLE);

	GMT_Set_Columns (API, GMT_IN, 0, GMT_COL_FIX);	/* Only text expected */

	if (GMT_Init_IO (API, GMT_IS_DATASET, GMT_IS_TEXT, GMT_IN, GMT_ADD_DEFAULT, 0, options) != GMT_NOERROR) {	/* Register data input */
		Return (API->error);
	}
	if (GMT_Begin_IO (API, GMT_IS_DATASET, GMT_IN, GMT_HEADER_ON) != GMT_NOERROR) {	/* Enables data input and sets access mode */
		Return (API->error);
	}

	do {	/* Keep returning records until we reach EOF */
		if ((In = GMT_Get_Record (API, GMT_READ_TEXT, NULL)) == NULL) {	/* Read next record, get NULL if special case */
			if (gmt_M_rec_is_error (GMT)) 		/* Bail if there are any read errors */
				Return (GMT_RUNTIME_ERROR);
			if (gmt_M_rec_is_any_header (GMT)) 	/* Skip all table and segment headers */
				continue;
			if (gmt_M_rec_is_eof (GMT)) 		/* Reached end of file */
				break;
		}

		if (gmt_M_is_dnan (In->data[GMT_X]) || gmt_M_is_dnan (In->data[GMT_Y]))	/* Probably a non-recognized header since we got NaNs */
			continue;

		/* Data record to process */

		if (Ctrl->H2.active) {
			if (Ctrl->S2.active) {
				n = sscanf (In->text, "%s %s %s %s %lf %lf %c %lf", col[0], col[1], col[2], stacode, &azimut, &ih, col[3], &azS);
				pol = col[3][2];
				if (n == 7)
					azS = -1.0;
			}
			else { /* !Ctrl->S2.active */
				sscanf (In->text, "%s %s %s %s %lf %lf %s", col[0], col[1], col[2], stacode, &azimut, &ih, col[3]);
				pol = col[3][2];
			}
		}
		else { /* !Ctrl->H2.active */
			if (Ctrl->S2.active) {
				n = sscanf (In->text, "%s %lf %lf %c %lf", stacode, &azimut, &ih, &pol, &azS);
				if (n == 4)
					azS = -1.0;
			}
			else /* !Ctrl->S2.active */
				sscanf (In->text, "%s %lf %lf %c", stacode, &azimut, &ih, &pol);
		}

		if (strcmp (col[0], "000000")) {
			plongement = (ih - 90.0) * D2R;
			if (plongement  < 0.0) {
				plongement = -plongement;
				azimut += 180.0;
				symbol_size2 = Ctrl->S.size * 0.8;
			} else
				symbol_size2 = Ctrl->S.size;
		}
		else
			symbol_size2 = Ctrl->S.size;
		radius = sqrt (1.0 - sin (plongement));
		if (radius >= 0.97) radius = 0.97;
		azimut += 180.0;
		sincosd (azimut, &si, &co);
		plot_x = radius * si * Ctrl->M.ech / 2.0 + plot_x0;
		plot_y = radius * co * Ctrl->M.ech / 2.0 + plot_y0;
		if (Ctrl->S.symbol == PSL_CROSS || Ctrl->S.symbol == PSL_DOT) PSL_setcolor (PSL, GMT->session.no_rgb, PSL_IS_STROKE);

		if (Ctrl->T.active) {
			int form;
			int label_justify = 0;
			double label_x, label_y;
			double label_offset[2];

			label_justify = gmt_flip_justify(GMT, Ctrl->T.justify);
			label_offset[0] = label_offset[1] = GMT_TEXT_CLEARANCE * 0.01 * Ctrl->T.font.size / PSL_POINTS_PER_INCH;

			label_x = plot_x + 0.5 * (Ctrl->T.justify%4 - label_justify%4) * Ctrl->S.size * 0.5;
			label_y = plot_y + 0.5 * (Ctrl->T.justify/4 - label_justify/4) * Ctrl->S.size * 0.5;

			/* Also deal with any justified offsets if given */
			if (Ctrl->T.justify%4 == 1) /* Left aligned */
				label_x -= Ctrl->T.offset[0];
			else /* Right or center aligned */
				label_x += Ctrl->T.offset[0];
			if (Ctrl->T.justify/4 == 0) /* Bottom aligned */
				label_y -= Ctrl->T.offset[1];
			else /* Top or middle aligned */
				label_y += Ctrl->T.offset[1];

			form = gmt_setfont (GMT, &Ctrl->T.font);
			PSL_plottext (PSL, label_x, label_y, Ctrl->T.font.size, stacode, Ctrl->T.angle, label_justify, form);
		}
		if (Ctrl->S.symbol == PSL_DOT) symbol_size2 = GMT_DOT_SIZE;

		if (pol == 'u' || pol == 'U' || pol == 'c' || pol == 'C' || pol == '+') {
			gmt_setpen (GMT, &Ctrl->G.pen);
			gmt_setfill (GMT, &(Ctrl->G.fill), Ctrl->G.active[1]);
			PSL_plotsymbol (PSL, plot_x, plot_y, &symbol_size2, Ctrl->S.symbol);
		}
		else if (pol == 'r' || pol == 'R' || pol == 'd' || pol == 'D' || pol == '-') {
			gmt_setpen (GMT, &Ctrl->E.pen);
			gmt_setfill (GMT, &(Ctrl->E.fill), Ctrl->E.active[1]);
			PSL_plotsymbol (PSL, plot_x, plot_y, &symbol_size2, Ctrl->S.symbol);
		}
		else {
			gmt_setpen (GMT, &Ctrl->W.pen);
			PSL_plotsymbol (PSL, plot_x, plot_y, &symbol_size2, Ctrl->S.symbol);
		}
		if (Ctrl->S2.active && azS >= 0.0) {
			gmt_setpen (GMT, &Ctrl->W.pen);
			sincosd (azS, &si, &co);
			if (Ctrl->S2.vector) {
				double dim[PSL_MAX_DIMS];
				gmt_M_memset (dim, PSL_MAX_DIMS, double);
				dim[PSL_VEC_XTIP]        = plot_x + Ctrl->S2.size*si;
				dim[PSL_VEC_YTIP]        = plot_y + Ctrl->S2.size*co;
				dim[PSL_VEC_TAIL_WIDTH]  = Ctrl->S2.width;
				dim[PSL_VEC_HEAD_LENGTH] = Ctrl->S2.length;
				dim[PSL_VEC_HEAD_WIDTH]  = Ctrl->S2.head;
				dim[PSL_VEC_HEAD_SHAPE]  = Ctrl->S2.vector_shape;
				dim[PSL_VEC_STATUS]      = PSL_VEC_END | PSL_VEC_FILL;
				if (Ctrl->S2.S.symbol == GMT_SYMBOL_VECTOR_V4)
					psl_vector_v4 (PSL, plot_x - Ctrl->S2.size*si, plot_y - Ctrl->S2.size*co, dim, Ctrl->S2.fill.rgb, Ctrl->S2.outline);
				else {	/* Modern vector */
					gmt_setfill (GMT, &(Ctrl->S2.fill), Ctrl->S2.outline);
					PSL_plotsymbol (PSL, plot_x - Ctrl->S2.size*si, plot_y - Ctrl->S2.size*co, dim, PSL_VECTOR);
				}
			}
			else {	/* Just draw a segment line */
				if (Ctrl->S2.scolor) PSL_setcolor (PSL, Ctrl->S2.fill.rgb, PSL_IS_STROKE);
				else PSL_setcolor (PSL, Ctrl->W.pen.rgb, PSL_IS_STROKE);
				PSL_plotsegment (PSL, plot_x - Ctrl->S2.size*si, plot_y - Ctrl->S2.size*co, plot_x + Ctrl->S2.size*si, plot_y + Ctrl->S2.size*co);
			}
		}
	} while (true);

	if (GMT_End_IO (API, GMT_IN, 0) != GMT_NOERROR) {	/* Disables further data input */
		Return (API->error);
	}

	GMT->current.map.is_world = old_is_world;

	if (!Ctrl->N.active) gmt_map_clip_off (GMT);

	PSL_setdash (PSL, NULL, 0);

	gmt_map_basemap (GMT);

	gmt_plotend (GMT);

	Return (GMT_NOERROR);
}

EXTERN_MSC int GMT_polar (void *V_API, int mode, void *args) {
	/* This is the GMT6 modern mode name */
	struct GMTAPI_CTRL *API = gmt_get_api_ptr (V_API);	/* Cast from void to GMTAPI_CTRL pointer */
	if (API->GMT->current.setting.run_mode == GMT_CLASSIC && !API->usage) {
		GMT_Report (API, GMT_MSG_ERROR, "Shared GMT module not found: polar\n");
		return (GMT_NOT_A_VALID_MODULE);
	}
	return GMT_pspolar (V_API, mode, args);
}
