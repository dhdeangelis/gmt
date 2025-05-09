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
/*
 * Brief synopsis: grdgradient read a grid file and compute gradient in azim direction:
 * azim = azimuth clockwise from north in degrees.
 * gradient = -[(dz/dx)sin(azim) + (dz/dy)cos(azim)].
 * the expression in [] is the correct gradient.  We take
 * -[] in order that data which goes DOWNHILL in the
 * azim direction will give a positive value; this is
 * for image shading purposes.  Note for -D we instead use
 * the common convention of reporting the UPHILL direction.
 *
 * Author:	W.H.F. Smith
 * Date: 	1-JAN-2010
 * Version:	6 API
 */

#include "gmt_dev.h"
#include "longopt/grdgradient_inc.h"

#define THIS_MODULE_CLASSIC_NAME	"grdgradient"
#define THIS_MODULE_MODERN_NAME	"grdgradient"
#define THIS_MODULE_LIB		"core"
#define THIS_MODULE_PURPOSE	"Compute directional gradients from a grid"
#define THIS_MODULE_KEYS	"<G{,AG(,GG},SG)"
#define THIS_MODULE_NEEDS	""
#define THIS_MODULE_OPTIONS "-RVfn"

enum grdgradient_mode {
	GRDGRADIENT_FIX = 1,
	GRDGRADIENT_VAR = 2};

struct GRDGRADIENT_CTRL {
	struct GRDGRADIENT_In {
		bool active;
		char *file;
	} In;
	struct GRDGRADIENT_A {	/* -A<azim>[/<azim2>] | -A<file>*/
		bool active;
		bool two;
		double azimuth[2];
		unsigned int mode;
		char *file;
	} A;
	struct GRDGRADIENT_D {	/* -D[a][c][o][n] */
		bool active;
		unsigned int mode;
	} D;
	struct GRDGRADIENT_E {	/* -E[s|p]<azim>/<elev[+a<ambient>][+d<diffuse>][+p<specular>][+<shine>] */
		bool active;
		unsigned int mode;
		double azimuth, elevation;
		double ambient, diffuse, specular, shine;
	} E;
	struct GRDGRADIENT_G {	/* -G<file> */
		bool active;
		char *file;
	} G;
	struct GRDGRADIENT_N {	/* -N[t_or_e][<amp>][+<ambient>][+o<offset>][+s<sigma>] */
		bool active;
		unsigned int set[3];	/* 1 if values are specified for amp, offset and sigma, 2 means we want last-run values */
		unsigned int mode;	/* 1 = atan, 2 = exp */
		double norm, sigma, offset, ambient;
	} N;
	struct GRDGRADIENT_Q {	/* -Qc|r|R */
		/* Note: If -Qc is set with -N then -G is not required. GMT_Encode_Options turns off the primary output */
		bool active;
		unsigned int mode;
		char *file;
	} Q;
	struct GRDGRADIENT_S {	/* -S<slopefile> */
		bool active;
		char *file;
	} S;
};

static void *New_Ctrl (struct GMT_CTRL *GMT) {	/* Allocate and initialize a new control structure */
	struct GRDGRADIENT_CTRL *C;

	C = gmt_M_memory (GMT, NULL, 1, struct GRDGRADIENT_CTRL);

	/* Initialize values whose defaults are not 0/false/NULL */
	C->E.ambient = 0.55;
	C->E.diffuse = 0.6;
	C->E.specular = 0.4;
	C->E.shine = 10;
	C->N.norm = 1.0;
	return (C);
}

static void Free_Ctrl (struct GMT_CTRL *GMT, struct GRDGRADIENT_CTRL *C) {	/* Deallocate control structure */
	if (!C) return;
	gmt_M_str_free (C->In.file);
	gmt_M_str_free (C->A.file);
	gmt_M_str_free (C->G.file);
	gmt_M_str_free (C->S.file);
	gmt_M_free (GMT, C);
}

GMT_LOCAL double grdgradient_specular (double n_columns, double n_rows, double nz, double *s) {
	/* SPECULAR Specular reflectance.
	   R = SPECULAR(Nx,Ny,Nz,S,V) returns the reflectance of a surface with
	   normal vector components [Nx,Ny,Nz].  S and V specify the direction
	   to the light source and to the viewer, respectively.
	   For the time being I'm using V = [azim elev] = [0 90] so the following

	   V[0] =  sind(V[0])*cosd(V[1]);
	   V[1] = -cosd(V[0])*cosd(V[1]);
	   V[2] =  sind(V[1]);

	   Reduces to V[0] = 0;		V[1] = 0;	V[2] = 1 */

	/*r = MAX(0,2*(s[0]*n_columns+s[1]*n_rows+s[2]*nz).*(v[0]*n_columns+v[1]*n_rows+v[2]*nz) - (v'*s)*ones(m,n)); */

	return (MAX(0, 2 * (s[0]*n_columns + s[1]*n_rows + s[2]*nz) * nz - s[2]));
}

static int usage (struct GMTAPI_CTRL *API, int level) {
	const char *name = gmt_show_name_and_purpose (API, THIS_MODULE_LIB, THIS_MODULE_CLASSIC_NAME, THIS_MODULE_PURPOSE);
	if (level == GMT_MODULE_PURPOSE) return (GMT_NOERROR);
	GMT_Usage (API, 0, "usage: %s %s -G%s [-A<azim>[/<azim2>]] [-D[a][c][o][n]] "
		"[-E[s|p|m]<azim>/<elev>[+a<ambient>][+d<diffuse>][+p<specular>][+s<shine>]] "
		"[-N[t|e][<amp>][+a<ambient>][+s<sigma>][+o<offset>]] [-Qc|r|R[+f<file>]] [%s] [-S<slopegrid>] [%s] "
		"[-fg] [%s] [%s]\n", name, GMT_INGRID, GMT_OUTGRID, GMT_Rgeo_OPT, GMT_V_OPT, GMT_n_OPT, GMT_PAR_OPT);

	if (level == GMT_SYNOPSIS) return (GMT_MODULE_SYNOPSIS);

	GMT_Message (API, GMT_TIME_NONE, "  REQUIRED ARGUMENTS:\n");
	gmt_ingrid_syntax (API, 0, "Name of input grid");
	gmt_outgrid_syntax (API, 'G', "Set output grid file for results from -A or -D");
	GMT_Message (API, GMT_TIME_NONE, "\n  OPTIONAL ARGUMENTS:\n");
	GMT_Usage (API, 1, "\n-A<azim>[/<azim2>]");
	GMT_Usage (API, -2, "Set azimuth (0-360 CW from North (+y)) for directional derivatives. "
		"Appending /<azim2> will compute two directions and select the one larger in magnitude. "
		"If <azim> is a grid we expect variable azimuths on a grid co-registered with <ingrid>.");
	GMT_Usage (API, 1, "\n-D[a][c][o][n]");
	GMT_Usage (API, -2, "Find the direction of the vector grad z (up-slope direction) given directive:");
	GMT_Usage (API, 3, "a: Get the aspect instead (down-slope direction).");
	GMT_Usage (API, 3, "c: Report Cartesian angle (0-360 CCW from East (+x-axis)) [Default: azimuth].");
	GMT_Usage (API, 3, "o: Get bidirectional orientations [0-180] rather than directions [0-360].");
	GMT_Usage (API, -2, "Append n to add 90 degrees to the values from c or o.");
	GMT_Usage (API, 1, "\n-E[s|p|m]<azim>/<elev>[+a<ambient>][+d<diffuse>][+p<specular>][+s<shine>]");
	GMT_Usage (API, -2, "Compute Lambertian radiance appropriate to use with grdimage|grdview. "
		"Provide the azimuth and elevation of the light vector. "
		"Optionally, append modifiers to further control the reflectance properties of the surface:");
	GMT_Usage (API, 3, "+a Set the <ambient> light value [0.55].");
	GMT_Usage (API, 3, "+d Set the <diffuse> light value [0.6].");
	GMT_Usage (API, 3, "+p Set <specular> value [0.4].");
	GMT_Usage (API, 3, "+s Set <shine> value [10].");
	GMT_Usage (API, -2, "Directives s|p|m can simplify the setup:");
	GMT_Usage (API, 3, "s: Simpler Lambertian algorithm (only requires azimuth and elevation).");
	GMT_Usage (API, 3, "p: Peucker piecewise linear approximation (simpler but faster algorithm).");
	GMT_Usage (API, 3, "m: Similar to ESRI's 'hillshade' but faster. Note: azimuth and elevation are hardwired to 315 and 45 degrees; "
		"if you provide other values they will be ignored.");
	GMT_Usage (API, 1, "\n-N[t|e][<amp>][+a<ambient>][+s<sigma>][+o<offset>]");
	GMT_Usage (API, -2, "Normalize gradients so that max |grad| = <amp> [1.0]. Optional directives:");
	GMT_Usage (API, 3, "t: Make atan transform, then scale to <amp> [1.0].");
	GMT_Usage (API, 3, "e: Make exp  transform, then scale to <amp> [1.0].");
	GMT_Usage (API, -2, "Several modifiers can be used to affect the final result:");
	GMT_Usage (API, 3, "+a Add <ambient> light [0].");
	GMT_Usage (API, 3, "+o Offset gradients by <offset> [Default estimates <offset> from the data].");
	GMT_Usage (API, 3, "+s Normalize gradients by <sigma> [Default estimates <sigma> from the data].");
	GMT_Usage (API, -2, "See -Q to use the same offset, sigma for multiple grid calculations.");
	GMT_Usage (API, 1, "\n-Qc|r|R[+f<file>]");
	GMT_Usage (API, -2, "Control handling of -N arguments from previous calculations. Append directive:");
	GMT_Usage (API, 3, "c: Create statistics file and write the offset and sigma of this run.");
	GMT_Usage (API, 3, "r: Read offset and sigma of the previous run from statistics file.");
	GMT_Usage (API, 3, "R: Remove and read.  As r but also removes the statistics file after use.");
	GMT_Usage (API, -2, "Optionally, append +f<file> to write/read the statistics to/from a specified file. Note: The values "
		"obtained are used if +o and/or +s modifiers in -N are given no argument.");
	GMT_Option (API, "R");
	GMT_Usage (API, 1, "\n-S<slopegrid>");
	GMT_Usage (API, -2, "Output file for |grad z|; requires -D.");
	GMT_Option (API, "V");
	GMT_Usage (API, 1, "\n-fg Convert geographic grids to meters using a \"Flat Earth\" approximation.");
	GMT_Option (API, "n,.");

	return (GMT_MODULE_USAGE);
}

static int parse (struct GMT_CTRL *GMT, struct GRDGRADIENT_CTRL *Ctrl, struct GMT_OPTION *options) {
	/* This parses the options provided to grdgradient and sets parameters in Ctrl.
	 * Note Ctrl has already been initialized and non-zero default values set.
	 * Any GMT common options will override values set previously by other commands.
	 * It also replaces any file names specified as input or output with the data ID
	 * returned when registering these sources/destinations with the API.
	 */

	unsigned int n_errors = 0, j, entry, pos;
	char p[GMT_BUFSIZ] = {""}, *c = NULL;
	struct GMT_OPTION *opt = NULL;
	struct GMTAPI_CTRL *API = GMT->parent;

	for (opt = options; opt; opt = opt->next) {	/* Process all the options given */

		switch (opt->option) {
			case '<':	/* Input file (only one is accepted) */
				n_errors += gmt_M_repeated_module_option (API, Ctrl->In.active);
				n_errors += gmt_get_required_file (GMT, opt->arg, opt->option, 0, GMT_IS_GRID, GMT_IN, GMT_FILE_REMOTE, &(Ctrl->In.file));
				break;

			/* Processes program-specific parameters */

			case 'A':	/* Set azimuth(s) */
				n_errors += gmt_M_repeated_module_option (API, Ctrl->A.active);
				if (!gmt_access (GMT, opt->arg, F_OK)) {	/* file with variable azimuths exists */
					Ctrl->A.file = strdup (opt->arg);
					Ctrl->A.mode = GRDGRADIENT_VAR;
				}
				else {	/* Get one or two fixed azimuths */
					j = sscanf (opt->arg, "%lf/%lf", &Ctrl->A.azimuth[0], &Ctrl->A.azimuth[1]);
					Ctrl->A.two = (j == 2);
					Ctrl->A.mode = GRDGRADIENT_FIX;
				}
				break;
			case 'D':	/* Find direction of grad|z| */
				n_errors += gmt_M_repeated_module_option (API, Ctrl->D.active);
				j = 0;
				while (opt->arg[j]) {
					switch (opt->arg[j]) {
						case 'c': Ctrl->D.mode |= 1; break;
						case 'o': Ctrl->D.mode |= 2; break;
						case 'n': Ctrl->D.mode |= 4; break;
						case 'a': Ctrl->D.mode |= 8; break;
						default:
							GMT_Report (API, GMT_MSG_ERROR, "Option -D: Unrecognized modifier\n");
							n_errors++;
							break;
					}
					j++;
				}
				break;
			case 'E':	/* Lambertian family radiance */
				n_errors += gmt_M_repeated_module_option (API, Ctrl->E.active);
				switch (opt->arg[0]) {
					case 'p':	/* Peucker */
						Ctrl->E.mode = 1;
						break;
					case 's':	/* "simple" Lambertian case */
						Ctrl->E.mode = 2;
						n_errors += gmt_M_check_condition (GMT, sscanf(&opt->arg[1], "%lf/%lf", &Ctrl->E.azimuth, &Ctrl->E.elevation) != 2, "Option -Es: Must append azimuth/elevation\n");
						break;
					case 'm':	/* Nice algorithm from an old program called manipRaster by Tierry Souriot */
						Ctrl->E.mode = 4;
						j = sscanf (&opt->arg[1], "%lf/%lf", &Ctrl->E.azimuth, &Ctrl->E.elevation);
						if (j == 0) {				/* Use default values */
							Ctrl->E.azimuth = 360 - 45;
							Ctrl->E.elevation = 45;
						}
						else if (j == 1)
							Ctrl->E.elevation = 45;

						Ctrl->E.azimuth = -Ctrl->E.azimuth + 90;	/* The algorithm use mat angles but we want azimuths */
						break;
					default:
						Ctrl->E.mode = 3;	/* "full" Lambertian case */
						if ((c = gmt_first_modifier (GMT, opt->arg, "adps"))) {	/* Process any modifiers */
							pos = 0;	/* Reset to start of new word */
							while (gmt_getmodopt (GMT, 'E', c, "adps", &pos, p, &n_errors) && n_errors == 0) {
								switch (p[0]) {
									case 'a': Ctrl->E.ambient  = atof (&p[1]); break;
									case 'd': Ctrl->E.diffuse  = atof (&p[1]); break;
									case 'p': Ctrl->E.specular = atof (&p[1]); break;
									case 's': Ctrl->E.shine    = atof (&p[1]); break;
									default: break;	/* These are caught in gmt_getmodopt so break is just for Coverity */
								}
							}
							c[0] = '\0';	/* Chop off all modifiers so range can be determined */
							n_errors += gmt_M_check_condition (GMT, sscanf(opt->arg, "%lf/%lf", &Ctrl->E.azimuth, &Ctrl->E.elevation) < 2, "Option -E: Must give at least azimuth and elevation\n");
						}
						else {	/* Old-style args */
							n_errors += gmt_M_check_condition (GMT, sscanf(opt->arg, "%lf/%lf", &Ctrl->E.azimuth, &Ctrl->E.elevation) < 2, "Option -E: Must give at least azimuth and elevation\n");
							entry = pos = 0;
							while (entry < 6 && (gmt_strtok (opt->arg, "/", &pos, p))) {
								switch (entry) {
									case 2:
										if (p[0] != '=') Ctrl->E.ambient = atof (p);
										break;
									case 3:
										if (p[0] != '=') Ctrl->E.diffuse = atof (p);
										break;
									case 4:
										if (p[0] != '=') Ctrl->E.specular = atof (p);
										break;
									case 5:
										if (p[0] != '=') Ctrl->E.shine = atof (p);
										break;
								}
								entry++;
							}
							GMT_Report (API, GMT_MSG_DEBUG, "Settings are %g/%g+a%g+d%g+p%g+s%g\n",
								Ctrl->E.azimuth, Ctrl->E.elevation, Ctrl->E.ambient, Ctrl->E.diffuse, Ctrl->E.specular, Ctrl->E.shine);
						}
						break;
				}
				break;
			case 'G':	/* Output grid */
				n_errors += gmt_M_repeated_module_option (API, Ctrl->G.active);
				n_errors += gmt_get_required_file (GMT, opt->arg, opt->option, 0, GMT_IS_GRID, GMT_OUT, GMT_FILE_LOCAL, &(Ctrl->G.file));
				break;
			case 'L':	/* GMT4 BCs */
				if (gmt_M_compat_check (GMT, 4)) {
					GMT_Report (API, GMT_MSG_COMPAT, "Option -L is deprecated; -n+b%s was set instead, use this common GMT option in the future.\n", opt->arg);
					gmt_strncpy (GMT->common.n.BC, opt->arg, 4U);
					/* We turn on geographic coordinates if -Lg is given by faking -fg */
					/* But since GMT_parse_f_option is private to gmt_init and all it does */
					/* in this case are 2 lines below we code it here */
					if (!strcmp (GMT->common.n.BC, "g")) {
						gmt_set_geographic (GMT, GMT_IN);
						gmt_set_geographic (GMT, GMT_OUT);
					}
				}
				else
					n_errors += gmt_default_option_error (GMT, opt);
				break;

			case 'M':	/* Geographic data */
				if (gmt_M_compat_check (GMT, 4)) {
					GMT_Report (API, GMT_MSG_COMPAT, "Option -M is deprecated; -fg was set instead, use this common GMT option in the future.\n");
					if (gmt_M_is_cartesian (GMT, GMT_IN)) gmt_parse_common_options (GMT, "f", 'f', "g"); /* Set -fg unless already set */
				}
				else
					n_errors += gmt_default_option_error (GMT, opt);
				break;
			case 'N':	/* Normalization */
				n_errors += gmt_M_repeated_module_option (API, Ctrl->N.active);
				j = 0;
				if (opt->arg[0] && strchr ("et", opt->arg[0])) {	/* Got -Ne or -Nt, possibly with arguments */
					Ctrl->N.mode = (opt->arg[0] == 't') ? 1 : 2;
					j = 1;
				}
				if ((c = gmt_first_modifier (GMT, opt->arg, "aos"))) {	/* Process any modifiers */
					pos = 0;	/* Reset to start of new word */
					while (gmt_getmodopt (GMT, 'N', c, "aos", &pos, p, &n_errors) && n_errors == 0) {
						switch (p[0]) {
							case 'a': Ctrl->N.ambient = atof (&p[1]); break;
							case 'o': Ctrl->N.set[1] = 1; if (p[1]) Ctrl->N.offset = atof (&p[1]); else Ctrl->N.set[1] = 2; break;
							case 's': Ctrl->N.set[2] = 1; if (p[1]) Ctrl->N.sigma  = atof (&p[1]); else Ctrl->N.set[2] = 2; break;
							default: break;	/* These are caught in gmt_getmodopt so break is just for Coverity */
						}
					}
					c[0] = '\0';	/* Chop off all modifiers so amplitude can be determined (if given) */
					if (opt->arg[j]) {
						Ctrl->N.set[0] = true;
						Ctrl->N.norm = atof (&opt->arg[j]);
					}
					c[0] = '+';	/* Restore modifiers */
				}
				else if (opt->arg[j]) {	/* Old-style args */
					int n_opt_args = sscanf (&opt->arg[j], "%lf/%lf/%lf", &Ctrl->N.norm, &Ctrl->N.sigma, &Ctrl->N.offset);
					Ctrl->N.set[0] = (n_opt_args >= 1);	/* Had to set the first two to set the amplitude */
					Ctrl->N.set[2] = (n_opt_args >= 2);	/* Had to set the first two to set the sigma */
					Ctrl->N.set[1] = (n_opt_args == 3);	/* Had to set all three to set the offset */
				}
				break;
			case 'Q':	/* Read/write normalization values */
				n_errors += gmt_M_repeated_module_option (API, Ctrl->Q.active);
				if (gmt_validate_modifiers (GMT, opt->arg, 'Q', "f", GMT_MSG_ERROR)) n_errors++;
				if (gmt_get_modifier (opt->arg, 'f', p)) {
					if (p[0]) Ctrl->Q.file = strdup (p);
				}
				switch (opt->arg[0]) {
					case 'r': Ctrl->Q.mode = 1; break;
					case 'c': Ctrl->Q.mode = 2; break;
					case 'R': Ctrl->Q.mode = 3; break;
					default:
						GMT_Report (API, GMT_MSG_ERROR, "Option -Q: Unrecognized directive %s\n", opt->arg);
						n_errors++;
						break;
				}
				break;
			case 'S':	/* Slope grid */
				n_errors += gmt_M_repeated_module_option (API, Ctrl->S.active);
				n_errors += gmt_get_required_file (GMT, opt->arg, opt->option, 0, GMT_IS_GRID, GMT_OUT, GMT_FILE_LOCAL, &(Ctrl->S.file));
				break;

			default:	/* Report bad options */
				n_errors += gmt_default_option_error (GMT, opt);
				break;
		}
	}

	n_errors += gmt_M_check_condition (GMT, !(Ctrl->A.active || Ctrl->D.active || Ctrl->E.active), "Must specify -A, -D, or -E\n");
	n_errors += gmt_M_check_condition (GMT, Ctrl->A.active && Ctrl->D.active, "Cannot specify both -A and -D\n");
	n_errors += gmt_M_check_condition (GMT, Ctrl->S.active && !Ctrl->S.file, "Option -S: Must specify output file\n");
	n_errors += gmt_M_check_condition (GMT, Ctrl->S.active && !Ctrl->D.active, "Option -S: Requires -D\n");
	n_errors += gmt_M_check_condition (GMT, !(Ctrl->N.active && Ctrl->Q.mode == 2) && !Ctrl->G.file && !Ctrl->S.active, "Option -G: Must specify output file\n");
	n_errors += gmt_M_check_condition (GMT, !Ctrl->In.file, "Must specify input file\n");
	n_errors += gmt_M_check_condition (GMT, Ctrl->N.active && (Ctrl->N.set[0] && Ctrl->N.norm <= 0.0), "Option -N: Normalization amplitude must be > 0\n");
	n_errors += gmt_M_check_condition (GMT, Ctrl->N.active && (Ctrl->N.set[2] == 1 && Ctrl->N.sigma <= 0.0) , "Option -N: Sigma must be > 0\n");
	n_errors += gmt_M_check_condition (GMT, Ctrl->E.active && Ctrl->E.mode > 1 && (Ctrl->E.elevation < 0.0 || Ctrl->E.elevation > 90.0), "Option -E: Use 0-90 degree range for elevation\n");
	n_errors += gmt_M_check_condition (GMT, Ctrl->Q.active && !Ctrl->N.active, "Option -Q: Requires -N\n");
	n_errors += gmt_M_check_condition (GMT, Ctrl->N.set[1] == 2 && !(Ctrl->Q.mode & 1), "Must specify -Q if -N+o is given no value\n");
	n_errors += gmt_M_check_condition (GMT, Ctrl->N.set[2] == 2 && !(Ctrl->Q.mode & 1), "Must specify -Q if -N+s is given no value\n");
	if (Ctrl->E.active && (Ctrl->A.active || Ctrl->D.active || Ctrl->S.active)) {
		GMT_Report (API, GMT_MSG_WARNING, "-E option overrides -A, -D or -S\n");
		Ctrl->A.active = Ctrl->D.active = Ctrl->S.active = false;
	}

	return (n_errors ? GMT_PARSE_ERROR : GMT_NOERROR);
}

#define bailout(code) {gmt_M_free_options (mode); return (code);}
#define Return(code) {Free_Ctrl (GMT, Ctrl); gmt_end_module (GMT, GMT_cpy); bailout (code);}

EXTERN_MSC int GMT_grdgradient (void *V_API, int mode, void *args) {
	bool bad, new_grid = false, separate = false;
	int p[4], mx, error = 0;
	openmp_int row, col;
	unsigned int n, orig_pad[4];
	uint64_t ij, ij0, n_used = 0;

	char format[GMT_BUFSIZ] = {""}, buffer[GMT_GRID_REMARK_LEN160] = {""};

	double dx_grid, dy_grid, x_factor = 0.0, x_factor_set, y_factor, y_factor_set, dzdx, dzdy, ave_gradient, wesn[4];
	double azim, denom, max_gradient = 0.0, min_gradient = 0.0, rpi, lat, output, one;
	double x_factor2 = 0.0, x_factor2_set = 0.0, y_factor2 = 0.0, dzdx2 = 0.0, dzdy2 = 0.0, dzds1, dzds2;
	double p0 = 0.0, q0 = 0.0, p0q0_cte = 1.0, norm_z, mag, s[3], lim_x, lim_y, lim_z;
	double k_ads = 0.0, diffuse, spec, r_min = DBL_MAX, r_max = -DBL_MAX, scale, sin_Az[2] = {0.0, 0.0};
	double def_offset = 0.0, def_sigma = 0.0;

	struct GMT_GRID *In = NULL, *Slope = NULL, *Grid = NULL, *A = NULL;
	struct GRDGRADIENT_CTRL *Ctrl = NULL;
	struct GMT_CTRL *GMT = NULL, *GMT_cpy = NULL;
	struct GMT_OPTION *options = NULL;
	struct GMTAPI_CTRL *API = gmt_get_api_ptr (V_API);	/* Cast from void to GMTAPI_CTRL pointer */

	/*----------------------- Standard module initialization and parsing ----------------------*/

	if (API == NULL) return (GMT_NOT_A_SESSION);
	if (mode == GMT_MODULE_PURPOSE) return (usage (API, GMT_MODULE_PURPOSE));	/* Return the purpose of program */
	options = GMT_Create_Options (API, mode, args);	if (API->error) return (API->error);	/* Set or get option list */

	if ((error = gmt_report_usage (API, options, 0, usage)) != GMT_NOERROR) bailout (error);	/* Give usage if requested */

	/* Parse the command-line arguments */

	if ((GMT = gmt_init_module (API, THIS_MODULE_LIB, THIS_MODULE_CLASSIC_NAME, THIS_MODULE_KEYS, THIS_MODULE_NEEDS, module_kw, &options, &GMT_cpy)) == NULL) bailout (API->error); /* Save current state */
	if (GMT_Parse_Common (API, THIS_MODULE_OPTIONS, options)) Return (API->error);
	Ctrl = New_Ctrl (GMT);	/* Allocate and initialize a new control structure */
	if ((error = parse (GMT, Ctrl, options)) != 0) Return (error);

	/*---------------------------- This is the grdgradient main code ----------------------------*/

	gmt_grd_set_datapadding (GMT, true);	/* Turn on gridpadding when reading a subset */

	if (Ctrl->Q.mode & 1) {	/* Read in previous statistics */
		char sfile[PATH_MAX] = {""};
		FILE *fp = NULL;
		if (Ctrl->Q.file)
			sprintf (sfile, "%s", Ctrl->Q.file);
		else if (GMT->session.TMPDIR)
			sprintf (sfile, "%s/grdgradient.stat", GMT->session.TMPDIR);
		else if (API->tmp_dir)
			sprintf (sfile, "%s/grdgradient.stat", API->tmp_dir);
		else
			sprintf (sfile, "grdgradient.stat");
		GMT_Report (API, GMT_MSG_DEBUG, "Read statistics file [%s] with offset and sigma\n", sfile);
		if (access (sfile, F_OK)) {
			GMT_Report (API, GMT_MSG_ERROR, "Unable to find statistics file from last run [%s]!\n", sfile);
			Return (GMT_FILE_NOT_FOUND);
		}
		if ((fp = fopen (sfile, "r")) == NULL) {
			GMT_Report (API, GMT_MSG_ERROR, "Cannot open statistics file from last run [%s]!\n", sfile);
			Return (GMT_ERROR_ON_FOPEN);
		}
		if (fscanf (fp, "%lg %lg", &def_offset, &def_sigma) != 2) {
			GMT_Report (API, GMT_MSG_ERROR, "Unable to read record from statistics file from last run [%s]!\n", sfile);
			fclose (fp);
			Return (GMT_RUNTIME_ERROR);
		}
		fclose (fp);
		if (Ctrl->Q.mode == 3) {	/* Gave -FR to delete after we have read the file */
			GMT_Report (API, GMT_MSG_DEBUG, "Remove statistics file [%s]\n", sfile);
			if (Ctrl->Q.mode == 3 && gmt_remove_file (GMT, sfile)) {	/* Gave -FR to read and delete */
				GMT_Report (API, GMT_MSG_ERROR, "Cannot remove statistics file from last run [%s]!\n", sfile);
				Return (GMT_RUNTIME_ERROR);
			}
		}
	}

	if (Ctrl->N.active) {	/* Report what was set if debug is enabled */
		char *answer = "NY";
		if (Ctrl->N.set[1] == 2) Ctrl->N.offset = def_offset, Ctrl->N.set[1] = 1;
		if (Ctrl->N.set[2] == 2) Ctrl->N.sigma  = def_sigma,  Ctrl->N.set[2] = 1;
		GMT_Report (API, GMT_MSG_DEBUG, "amplitude_set = %c offset_set = %c sigma_set = %c\n", answer[Ctrl->N.set[0]], answer[Ctrl->N.set[1]], answer[Ctrl->N.set[2]]);
	}

	GMT_Report (API, GMT_MSG_INFORMATION, "Processing input grid\n");
	gmt_M_memset (s, 3, double);
	gmt_M_memset (wesn, 4, double);
	gmt_set_pad (GMT, 2U);	/* Ensure space for BCs in case an API passed pad == 0 */

	if (Ctrl->A.active) {	/* Get azimuth in 0-360 range */
		if (Ctrl->A.mode == GRDGRADIENT_VAR) {	/* Got variable azimuth(s) */
			if ((A = GMT_Read_Data (API, GMT_IS_GRID, GMT_IS_FILE, GMT_IS_SURFACE, GMT_CONTAINER_AND_DATA, NULL, Ctrl->A.file, NULL)) == NULL) {
				Return (API->error);
			}
		}
		else {	/* Got fixed azimuth(s) */
			while (Ctrl->A.azimuth[0] < 0.0) Ctrl->A.azimuth[0] += 360.0;
			while (Ctrl->A.azimuth[0] > 360.0) Ctrl->A.azimuth[0] -= 360.0;
			if (Ctrl->A.two) {
				while (Ctrl->A.azimuth[1] < 0.0) Ctrl->A.azimuth[1] += 360.0;
				while (Ctrl->A.azimuth[1] > 360.0) Ctrl->A.azimuth[1] -= 360.0;
			}
		}
	}
	if (Ctrl->E.active) {	/* Get azimuth in 0-360 range */
		while (Ctrl->E.azimuth < 0.0) Ctrl->E.azimuth += 360.0;
		while (Ctrl->E.azimuth > 360.0) Ctrl->E.azimuth -= 360.0;
	}
	if (Ctrl->E.mode == 2) {	/* Precalculate constants */
		p0 = cosd (90.0 - Ctrl->E.azimuth) * tand (90.0 - Ctrl->E.elevation);
		q0 = sind (90.0 - Ctrl->E.azimuth) * tand (90.0 - Ctrl->E.elevation);
		p0q0_cte = sqrt (1.0 + p0*p0 + q0*q0);
	}
	else if (Ctrl->E.mode == 3 || Ctrl->E.mode == 4) {	/* Precalculate constants */
		if (Ctrl->E.mode == 3) {
			Ctrl->E.elevation = 90 - Ctrl->E.elevation;
			k_ads = Ctrl->E.ambient + Ctrl->E.diffuse + Ctrl->E.specular;
		}
		s[0] = sind (Ctrl->E.azimuth) * cosd (Ctrl->E.elevation);
		s[1] = cosd (Ctrl->E.azimuth) * cosd (Ctrl->E.elevation);
		s[2] = sind (Ctrl->E.elevation);
	}

	if (GMT->common.R.active[RSET]) gmt_M_memcpy (wesn, GMT->common.R.wesn, 4, double);	/* Current -R setting, if any */

	if ((In = GMT_Read_Data (API, GMT_IS_GRID, GMT_IS_FILE, GMT_IS_SURFACE, GMT_CONTAINER_ONLY, NULL, Ctrl->In.file, NULL)) == NULL) {
		Return (API->error);
	}
	if (gmt_M_is_subset (GMT, In->header, wesn)) {	/* Subset requested; make sure wesn matches header spacing */
		if ((error = gmt_M_err_fail (GMT, gmt_adjust_loose_wesn (GMT, wesn, In->header), "")))
			Return (error);
	}
	gmt_grd_init (GMT, In->header, options, true);

	if (GMT_Read_Data (API, GMT_IS_GRID, GMT_IS_FILE, GMT_IS_SURFACE, GMT_DATA_ONLY, wesn, Ctrl->In.file, In) == NULL) {	/* Get subset */
		Return (API->error);
	}

	if (Ctrl->A.mode == GRDGRADIENT_VAR) {	/* IGiven 2 grids, make sure they are co-registered and has same size, registration, etc. */
		if (In->header->registration != A->header->registration) {
			GMT_Report (API, GMT_MSG_ERROR, "Input and azimuth grids have different registrations!\n");
			Return (GMT_RUNTIME_ERROR);
		}
		if (!gmt_M_grd_same_shape (GMT, In, A)) {
			GMT_Report (API, GMT_MSG_ERROR, "Input and azimuth grids have different dimensions\n");
			Return (GMT_RUNTIME_ERROR);
		}
		if (!gmt_M_grd_same_region (GMT, In, A)) {
			GMT_Report (API, GMT_MSG_ERROR, "Input and azimuth grids have different regions\n");
			Return (GMT_RUNTIME_ERROR);
		}
		if (!gmt_M_grd_same_inc (GMT, In, A)) {
			GMT_Report (API, GMT_MSG_ERROR, "Input and azimuth grids have different intervals\n");
			Return (GMT_RUNTIME_ERROR);
		}
	}

#ifdef OPENMP
#if 0
	separate = true;	/* Cannot use input grid to hold output grid when doing things in parallel */
#endif
#endif
	new_grid = gmt_set_outgrid (GMT, Ctrl->In.file, separate, 2, In, &Grid);	/* true if input is a read-only array */
	if (new_grid) GMT_Report (API, GMT_MSG_DEBUG, "Input grid duplicated as it was read-only\n");

	/* If new_grid is true then Grid points to a duplicate of In but will have two boundary rows,columns padding.
	 * If new_grid is false then Grid simply points to In which presumably has two boundary row,column padding.
	 * In either case, the algorithm below assumes there is at least one extra row column in Grid */

	if (Ctrl->S.active) {	/* Want slope grid, ensure same padding as Grid */
		if ((Slope = GMT_Duplicate_Data (API, GMT_IS_GRID, GMT_DUPLICATE_ALLOC, Grid)) == NULL) Return (API->error);
	}

	if (gmt_M_is_geographic (GMT, GMT_IN) && !Ctrl->E.active) {	/* Flat-Earth approximation */
		dx_grid = GMT->current.proj.DIST_M_PR_DEG * Grid->header->inc[GMT_X] * cosd ((Grid->header->wesn[YHI] + Grid->header->wesn[YLO]) / 2.0);
		dy_grid = GMT->current.proj.DIST_M_PR_DEG * Grid->header->inc[GMT_Y];
	}
	else {	/* Cartesian */
		dx_grid = Grid->header->inc[GMT_X];
		dy_grid = Grid->header->inc[GMT_Y];
	}
	one = (Ctrl->D.active) ? +1.0 : -1.0;	/* With -D we want positive grad direction, not negative as for shading (-A, -E) */
	x_factor_set = one / (2.0 * dx_grid);
	y_factor = y_factor_set = one / (2.0 * dy_grid);
	if (Ctrl->A.mode == GRDGRADIENT_FIX) {	/* Convert fixed azimuths to radians to save multiplication later */
		if (Ctrl->A.two) {
			Ctrl->A.azimuth[1] *= D2R;
			sin_Az[1] = sin (Ctrl->A.azimuth[1]);
			x_factor2_set = x_factor_set * sin_Az[1];
			y_factor2 = y_factor * cos (Ctrl->A.azimuth[1]);
		}
		Ctrl->A.azimuth[0] *= D2R;
		sin_Az[0] = sin (Ctrl->A.azimuth[0]);
		x_factor_set *= sin_Az[0];
		y_factor *= cos (Ctrl->A.azimuth[0]);
	}

	/* Index offset of 4-star points relative to current node in Grid */
	mx = Grid->header->mx;	/* Need a signed mx for p[3] in line below */
	p[0] = 1;	p[1] = -1;	p[2] = mx;	p[3] = -mx;

	min_gradient = DBL_MAX;	max_gradient = -DBL_MAX;	ave_gradient = 0.0;
	if (Ctrl->E.mode == 3) {
		lim_x = Grid->header->wesn[XHI] - Grid->header->wesn[XLO];
		lim_y = Grid->header->wesn[YHI] - Grid->header->wesn[YLO];
		lim_z = Grid->header->z_max - Grid->header->z_min;
		scale = MAX (lim_z, MAX (lim_x, lim_y));
		lim_x /= scale;	lim_y /= scale;		lim_z /= scale;
		dx_grid /= lim_x;	dy_grid /= lim_y;
		x_factor_set = -dy_grid / (2.0 * lim_z);	y_factor = -dx_grid / (2.0 * lim_z);
	}

#if 0	/* Not active since we have no way to do min/max via OpenMP in C.  So rethink this part */
#ifdef _OPENMP
#pragma omp parallel for private(row,ij0,lat,dx_grid,col,ij,n,bad,index,dzdx,dzdy,dzdx2,dzdy2,dzds1,dzds2,output,azim,norm_z,mag,diffuse,spec) \
	shared(In,GMT,Ctrl,x_factor_set,x_factor2_set,sin_Az,new_grid,Grid,Slope,y_factor,y_factor2,p0,q0,p0q0_cte,k_ads,s) \
	reduction(+:ave_gradient)
#endif
#endif
	for (row = 0, ij0 = 0ULL; row < (openmp_int)Grid->header->n_rows; row++) {	/* ij0 is the index in a non-padded grid */
		if (gmt_M_is_geographic (GMT, GMT_IN) && !Ctrl->E.active) {	/* Evaluate latitude-dependent factors */
			lat = gmt_M_grd_row_to_y (GMT, row, Grid->header);
			dx_grid = GMT->current.proj.DIST_M_PR_DEG * Grid->header->inc[GMT_X] * cosd (lat);
			if (dx_grid > 0.0) x_factor = x_factor_set = one / (2.0 * dx_grid);	/* Use previous value at the poles */
			if (Ctrl->A.mode == GRDGRADIENT_FIX) {
				if (Ctrl->A.two) x_factor2 = x_factor * sin_Az[1];
				x_factor *= sin_Az[0];
			}
		}
		else {	/* Use the constant factors */
			x_factor = x_factor_set;
			if (Ctrl->A.mode == GRDGRADIENT_FIX && Ctrl->A.two) x_factor2 = x_factor2_set;
		}
		for (col = 0; col < (openmp_int)Grid->header->n_columns; col++, ij0++) {
			ij = gmt_M_ijp (Grid->header, row, col);	/* Index into padded grid (with at least 1 row/col padding) */
			for (n = 0, bad = false; !bad && n < 4; n++) if (gmt_M_is_fnan (Grid->data[ij+p[n]])) bad = true;
			if (bad) {	/* One of the 4-star corners = NaN; assign NaN answer and skip to next node */
				Grid->data[ij0] = GMT->session.f_NaN;
				if (Ctrl->S.active) Slope->data[ij] = GMT->session.f_NaN;
				continue;
			}
			if (Ctrl->A.mode == GRDGRADIENT_VAR) {	/* Must update azimuth for every node */
				Ctrl->A.azimuth[0] = A->data[ij] * D2R;
				x_factor = x_factor_set * sin (Ctrl->A.azimuth[0]);
				y_factor = y_factor_set * cos (Ctrl->A.azimuth[0]);
			}

			/* We can now evaluate the central finite differences */
			dzdx = (Grid->data[ij+1] - Grid->data[ij-1]) * x_factor;
			dzdy = (Grid->data[ij-Grid->header->mx] - Grid->data[ij+Grid->header->mx]) * y_factor;
			if (Ctrl->A.two) {
				dzdx2 = (Grid->data[ij+1] - Grid->data[ij-1]) * x_factor2;
				dzdy2 = (Grid->data[ij-Grid->header->mx] - Grid->data[ij+Grid->header->mx]) * y_factor2;
			}

			/* Write output to unused NW corner */

			if (Ctrl->A.active) {	/* Directional derivatives requested */
				if (Ctrl->A.two) {
					dzds1 = dzdx + dzdy;
					dzds2 = dzdx2 + dzdy2;
					output = (fabs (dzds1) > fabs (dzds2)) ? dzds1 : dzds2;
				}
				else
					output = dzdx + dzdy;
				ave_gradient += output;
				min_gradient = MIN (min_gradient, output);
				max_gradient = MAX (max_gradient, output);
			}
			else if (Ctrl->D.active) {
				if (dzdx == 0.0 && dzdy == 0.0)	/* Flat, so no preferred direction */
					azim = GMT->session.d_NaN;
				else {
					azim = (Ctrl->D.mode & 1) ? atan2d (dzdy, dzdx) : 90.0 - atan2d (dzdy, dzdx);
					if (Ctrl->D.mode & 4) azim += 90.0;
					if (Ctrl->D.mode & 8) azim += 180.0;
					if (azim < 0.0) azim += 360.0;
					if (azim >= 360.0) azim -= 360.0;
					if (Ctrl->D.mode & 2 && azim >= 180) azim -= 180.0;
				}
				output = azim;
				if (Ctrl->S.active) Slope->data[ij] = (gmt_grdfloat)hypot (dzdx, dzdy);
			}
			else {	/* Ctrl->E.active */
				if (Ctrl->E.mode == 2)
					output = (1.0 + p0 * dzdx + q0 * dzdy) / (sqrt (1.0 + dzdx * dzdx + dzdy * dzdy) * p0q0_cte);
				else if (Ctrl->E.mode == 3) {
					norm_z = dx_grid * dy_grid;
					mag = d_sqrt (dzdx * dzdx + dzdy * dzdy + norm_z * norm_z);
					dzdx /= mag;	dzdy /= mag;	norm_z /= mag;
					diffuse = MAX (0, s[0] * dzdx + s[1] * dzdy + s[2] * norm_z);
					spec = grdgradient_specular (dzdx, dzdy, norm_z, s);
					spec = pow (spec, Ctrl->E.shine);
					output = (Ctrl->E.ambient + Ctrl->E.diffuse * diffuse + Ctrl->E.specular * spec) / k_ads;
				}
				else if (Ctrl->E.mode == 4)
					output = (dzdy*s[0] + dzdx*s[1] + 2*s[2]) / (sqrt(dzdy * dzdy + dzdx * dzdx + 4));
				else	/* Peucker method */
					output = -0.4285 * (dzdx - dzdy) - 0.0844 * fabs (dzdx  + dzdy) + 0.6599;
				r_min = MIN (r_min, output);
				r_max = MAX (r_max, output);
			}
			Grid->data[ij0] = (gmt_grdfloat)output;
			n_used++;
		}
	}

	/* Now deal with the fact that the result is unpadded in a padded array */
	gmt_M_memcpy (orig_pad, Grid->header->pad, 4, unsigned int);	/* This can be either 1/1/1/1/ or 2/2/2/2, depending on circumstances */
	Grid->header->mx = Grid->header->n_columns;	Grid->header->my = Grid->header->n_rows;	/* Since there is no pad as far as the computed grid is concerned */
	gmt_M_memset (Grid->header->pad, 4, int);	/* Must set pad to zero first otherwise we cannot add the pad back in */
	gmt_grd_pad_on (GMT, Grid, orig_pad);	/* Now reinstate the original pad */

	if (gmt_M_is_geographic (GMT, GMT_IN)) {	/* Data is geographic */
		double sum;
		/* If the N or S poles are included then we only want a single estimate at these repeating points */
		if (Grid->header->wesn[YLO] == -90.0 && Grid->header->registration == GMT_GRID_NODE_REG) {	/* Average all the multiple N pole estimates */
			for (col = 0, ij = gmt_M_ijp (Grid->header, 0, 0), sum = 0.0; col < (openmp_int)Grid->header->n_columns; col++, ij++) sum += Grid->data[ij];
			sum /= Grid->header->n_columns;	/* Average gradient */
			for (col = 0, ij = gmt_M_ijp (Grid->header, 0, 0); col < (openmp_int)Grid->header->n_columns; col++, ij++) Grid->data[ij] = (gmt_grdfloat)sum;
		}
		if (Grid->header->wesn[YLO] == -90.0 && Grid->header->registration == GMT_GRID_NODE_REG) {	/* Average all the multiple S pole estimates */
			for (col = 0, ij = gmt_M_ijp (Grid->header, Grid->header->n_rows - 1, 0), sum = 0.0; col < (openmp_int)Grid->header->n_columns; col++, ij++) sum += Grid->data[ij];
			sum /= Grid->header->n_columns;	/* Average gradient */
			for (col = 0, ij = gmt_M_ijp (Grid->header, Grid->header->n_rows - 1, 0); col < (openmp_int)Grid->header->n_columns; col++, ij++) Grid->data[ij] = (gmt_grdfloat)sum;
		}
	}

	if (Ctrl->E.active) {	/* data must be scaled to the [-1,1] interval, but we'll do it into [-.95, .95] to not get too bright */
		scale = 1.0 / (r_max - r_min);
		gmt_M_grd_loop (GMT, Grid, row, col, ij) {
			if (gmt_M_is_fnan (Grid->data[ij])) continue;
			Grid->data[ij] = (gmt_grdfloat)((-1.0 + 2.0 * ((Grid->data[ij] - r_min) * scale)) * 0.95);
		}
	}

	if (Ctrl->N.set[1])
		ave_gradient = Ctrl->N.offset;
	else
		ave_gradient /= n_used;

	if (Ctrl->A.active) {	/* Report some statistics */

		if (Ctrl->N.active) {	/* Chose normalization */
			if (Ctrl->N.mode == 1) {	/* atan transformation */
				if (Ctrl->N.set[2])
					denom = 1.0 / Ctrl->N.sigma;
				else {
					denom = 0.0;
					gmt_M_grd_loop (GMT, Grid, row, col, ij) {
						if (!gmt_M_is_fnan (Grid->data[ij])) denom += pow (Grid->data[ij] - ave_gradient, 2.0);
					}
					denom = sqrt ((n_used - 1) / denom);
					Ctrl->N.sigma = 1.0 / denom;
				}
				rpi = 2.0 * Ctrl->N.norm / M_PI;
				gmt_M_grd_loop (GMT, Grid, row, col, ij) {
					if (!gmt_M_is_fnan (Grid->data[ij])) Grid->data[ij] = (gmt_grdfloat)(rpi * atan ((Grid->data[ij] - ave_gradient) * denom) + Ctrl->N.ambient);
				}
				Grid->header->z_max = rpi * atan ((max_gradient - ave_gradient) * denom) + Ctrl->N.ambient;
				Grid->header->z_min = rpi * atan ((min_gradient - ave_gradient) * denom) + Ctrl->N.ambient;
			}
			else if (Ctrl->N.mode == 2) {	/* Exp transformation */
				if (!Ctrl->N.set[2]) {
					Ctrl->N.sigma = 0.0;
					gmt_M_grd_loop (GMT, Grid, row, col, ij) {
#ifdef DOUBLE_PRECISION_GRID
						if (!gmt_M_is_fnan (Grid->data[ij])) Ctrl->N.sigma += fabs (Grid->data[ij]);
#else
						if (!gmt_M_is_fnan (Grid->data[ij])) Ctrl->N.sigma += fabsf (Grid->data[ij]);
#endif
					}
					Ctrl->N.sigma = M_SQRT2 * Ctrl->N.sigma / n_used;
				}
				denom = M_SQRT2 / Ctrl->N.sigma;
				gmt_M_grd_loop (GMT, Grid, row, col, ij) {
					if (gmt_M_is_fnan (Grid->data[ij])) continue;
					if (Grid->data[ij] < ave_gradient) {
						Grid->data[ij] = (gmt_grdfloat)(-Ctrl->N.norm * (1.0 - exp ( (Grid->data[ij] - ave_gradient) * denom)) + Ctrl->N.ambient);
					}
					else {
						Grid->data[ij] = (gmt_grdfloat)( Ctrl->N.norm * (1.0 - exp (-(Grid->data[ij] - ave_gradient) * denom)) + Ctrl->N.ambient);
					}
				}
				Grid->header->z_max =  Ctrl->N.norm * (1.0 - exp (-(max_gradient - ave_gradient) * denom)) + Ctrl->N.ambient;
				Grid->header->z_min = -Ctrl->N.norm * (1.0 - exp ( (min_gradient - ave_gradient) * denom)) + Ctrl->N.ambient;
			}
			else {	/* Linear transformation */
				if ((max_gradient - ave_gradient) > (ave_gradient - min_gradient))
					denom = Ctrl->N.norm / (max_gradient - ave_gradient);
				else
					denom = Ctrl->N.norm / (ave_gradient - min_gradient);
				gmt_M_grd_loop (GMT, Grid, row, col, ij) {
					if (!gmt_M_is_fnan (Grid->data[ij])) Grid->data[ij] = (gmt_grdfloat)((Grid->data[ij] - ave_gradient) * denom) + Ctrl->N.ambient;
				}
				Grid->header->z_max = (max_gradient - ave_gradient) * denom + Ctrl->N.ambient;
				Grid->header->z_min = (min_gradient - ave_gradient) * denom + Ctrl->N.ambient;
			}
		}
	}

	gmt_set_pad (GMT, API->pad);	/* Reset to session default pad before output */

	/* Now we write out: */

	if (!(Ctrl->Q.mode == 2 && !Ctrl->G.active)) {	/* Not the special case when we don't really need a grid out */
		if (Ctrl->A.active) {
			if (Ctrl->N.active)
				strcpy (buffer, "Normalized directional derivative(s)");
			else
				strcpy (buffer, "Directional derivative(s)");
			sprintf (format, "\t%s\t%s\t%s\t%s\n", GMT->current.setting.format_float_out, GMT->current.setting.format_float_out, GMT->current.setting.format_float_out, GMT->current.setting.format_float_out);
			GMT_Report (API, GMT_MSG_INFORMATION, " Min Mean Max sigma intensities:");
			GMT_Report (API, GMT_MSG_INFORMATION, format, min_gradient, ave_gradient, max_gradient, Ctrl->N.sigma);
		}
		else {
			if (Ctrl->E.mode > 1)
				strcpy (buffer, "Lambertian radiance");
			else if (Ctrl->E.mode == 1)
				strcpy (buffer, "Peucker piecewise linear radiance");
			else
				strcpy (buffer, "Directions of grad (z) [uphill direction]");
		}

		if (GMT_Set_Comment (API, GMT_IS_GRID, GMT_COMMENT_IS_OPTION | GMT_COMMENT_IS_COMMAND, options, Grid)) Return (API->error);
		if (GMT_Set_Comment (API, GMT_IS_GRID, GMT_COMMENT_IS_REMARK, buffer, Grid)) Return (API->error);
		if (Ctrl->G.active && GMT_Write_Data (API, GMT_IS_GRID, GMT_IS_FILE, GMT_IS_SURFACE, GMT_CONTAINER_AND_DATA, NULL, Ctrl->G.file, Grid) != GMT_NOERROR) {
			Return (API->error);
		}

		if (Ctrl->S.active) {
			if (GMT_Set_Comment (API, GMT_IS_GRID, GMT_COMMENT_IS_OPTION | GMT_COMMENT_IS_COMMAND, options, Slope)) Return (API->error);
			if (GMT_Set_Comment (API, GMT_IS_GRID, GMT_COMMENT_IS_REMARK, "Magnitude of grad (z)", Slope)) Return (API->error);
			if (GMT_Write_Data (API, GMT_IS_GRID, GMT_IS_FILE, GMT_IS_SURFACE, GMT_CONTAINER_AND_DATA, NULL, Ctrl->S.file, Slope) != GMT_NOERROR) {
				Return (API->error);
			}
		}
	}

	if (Ctrl->Q.mode == 2) {	/* Write the new statistics */
		char sfile[PATH_MAX] = {""};
		FILE *fp = NULL;
		if (Ctrl->Q.file)
			sprintf (sfile, "%s", Ctrl->Q.file);
		else if (GMT->session.TMPDIR)
			sprintf (sfile, "%s/grdgradient.stat", GMT->session.TMPDIR);
		else if (API->tmp_dir)
			sprintf (sfile, "%s/grdgradient.stat", API->tmp_dir);
		else
			sprintf (sfile, "grdgradient.stat");
		GMT_Report (API, GMT_MSG_DEBUG, "Create statistics file [%s] with offset and sigma\n", sfile);
		if ((fp = fopen (sfile, "w")) == NULL) {
			GMT_Report (API, GMT_MSG_ERROR, "Cannot create statistics file from this run [%s]!\n", sfile);
			Return (GMT_ERROR_ON_FOPEN);
		}
		if (fprintf (fp, "%.16lg %.16lg\n", ave_gradient, Ctrl->N.sigma) < 0) {
			GMT_Report (API, GMT_MSG_ERROR, "Unable to write record to statistics file from this run [%s]!\n", sfile);
			fclose (fp);
			Return (GMT_RUNTIME_ERROR);
		}
		fclose (fp);
	}

	Return (GMT_NOERROR);
}
