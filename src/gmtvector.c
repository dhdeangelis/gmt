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
 * Brief synopsis: gmtvector performs basic vector operations such as dot and
 * cross products, sums, and conversion between Cartesian and polar/spherical
 * coordinates systems.
 *
 * Author:	Paul Wessel
 * Date:	10-Aug-2010
 * Version:	6 API
 */

#include "gmt_dev.h"
#include "longopt/gmtvector_inc.h"

#define THIS_MODULE_CLASSIC_NAME	"gmtvector"
#define THIS_MODULE_MODERN_NAME	"gmtvector"
#define THIS_MODULE_LIB		"core"
#define THIS_MODULE_PURPOSE	"Operations on Cartesian vectors in 2-D and 3-D"
#define THIS_MODULE_KEYS	"<D{,>D}"
#define THIS_MODULE_NEEDS	""
#define THIS_MODULE_OPTIONS "-:>Vbdefghijoqs" GMT_OPT("HMm")

enum gmtvector_method {	/* The available methods */
	DO_NOTHING=0,
	DO_AVERAGE,
	DO_DOT2D,
	DO_DOT3D,
	DO_CROSS,
	DO_SUM,
	DO_ROT2D,
	DO_ROT3D,
	DO_ROTVAR2D,
	DO_ROTVAR3D,
	DO_DOT,
	DO_POLE,
	DO_TRANSLATE,
	DO_BISECTOR};

struct GMTVECTOR_CTRL {
	struct GMTVECTOR_Out {	/* -> */
		bool active;
		char *file;
	} Out;
	struct GMTVECTOR_In {	/* infile */
		bool active;
		unsigned int n_args;
		char *arg;
	} In;
	struct GMTVECTOR_A {	/* -A[m[<conf>]|<vec>] */
		bool active;
		unsigned int mode;
		double conf;
		char *arg;
	} A;
	struct GMTVECTOR_C {	/* -C[i|o] */
		bool active[2];
	} C;
	struct GMTVECTOR_E {	/* -E */
		bool active;
	} E;
	struct GMTVECTOR_N {	/* -N */
		bool active;
	} N;
	struct GMTVECTOR_S {	/* -S[vec] */
		bool active;
		char *arg;
	} S;
	struct GMTVECTOR_T {	/* -T[operator][<arg>] */
		char unit;
		bool active;
		bool degree;
		bool a_and_d;
		enum gmtvector_method mode;
		unsigned int dmode;
		double par[3];
	} T;
};

static void *New_Ctrl (struct GMT_CTRL *GMT) {	/* Allocate and initialize a new control structure */
	struct GMTVECTOR_CTRL *C;

	C = gmt_M_memory (GMT, NULL, 1, struct GMTVECTOR_CTRL);

	/* Initialize values whose defaults are not 0/false/NULL */
	C->A.conf = 0.95;	/* 95% conf level */
	C->T.unit = 'e';	/* Unit is meter unless specified */
	return (C);
}

static void Free_Ctrl (struct GMT_CTRL *GMT, struct GMTVECTOR_CTRL *C) {	/* Deallocate control structure */
	if (!C) return;
	gmt_M_str_free (C->In.arg);
	gmt_M_str_free (C->Out.file);
	gmt_M_str_free (C->A.arg);
	gmt_M_str_free (C->S.arg);
	gmt_M_free (GMT, C);
}

static int usage (struct GMTAPI_CTRL *API, int level) {
	const char *name = gmt_show_name_and_purpose (API, THIS_MODULE_LIB, THIS_MODULE_CLASSIC_NAME, THIS_MODULE_PURPOSE);
	if (level == GMT_MODULE_PURPOSE) return (GMT_NOERROR);
	GMT_Usage (API, 0, "usage: %s [<table>] [-A[m][<conf>]|<vector>] [-C[i|o]] [-E] [-N] [-S<vector>] "
		"[-Ta|b|d|D|p<azim>|r<rot>|R|s|t[<azim>/<dist>]|x] [%s] [%s] [%s] [%s] [%s] [%s] [%s] [%s] [%s] [%s] [%s] [%s] [%s]\n",
		name, GMT_V_OPT, GMT_b_OPT, GMT_d_OPT, GMT_e_OPT, GMT_f_OPT, GMT_g_OPT, GMT_h_OPT, GMT_i_OPT, GMT_o_OPT, GMT_q_OPT, GMT_s_OPT, GMT_colon_OPT, GMT_PAR_OPT);

	if (level == GMT_SYNOPSIS) return (GMT_MODULE_SYNOPSIS);

	GMT_Message (API, GMT_TIME_NONE, "  OPTIONAL ARGUMENTS:\n");
	GMT_Usage (API, 1, "\n<table>");
	GMT_Usage (API, -2, "<table> is one or more data files (in ASCII, binary, netCDF) with (x,y[,z]), (r,theta) or (lon,lat) in the "
		"first 2-3 input columns. If one item is given and it cannot be opened we will interpret it as a single x/y[/z], r/theta, or lon/lat entry. "
		"If no file(s) is given, standard input is read.");
	GMT_Usage (API, 1, "\n-A[m][<conf>]|<vector>");
	GMT_Usage (API, -2, "Single primary vector, given as lon/lat, r/theta, or x/y[/z].  No tables will be read. "
		"Alternatively, give -Am to compute the single primary vector as the mean of the input vectors. "
		"The confidence ellipse for the mean vector will be determined (95%% level); "
		"optionally append a different confidence level in percent.");
	GMT_Usage (API, 1, "\n-C[i|o]");
	GMT_Usage (API, -2, "Indicate Cartesian coordinates on input/output instead of lon,lat or r/theta. "
		"Append i or o to only affect input or output coordinates.");
	GMT_Usage (API, 1, "\n-E Automatically convert between geodetic and geocentric coordinates [no conversion]. "
		"Ignored unless -fg is given.");
	GMT_Usage (API, 1, "\n-N Normalize the transformed vectors (only affects -Co output).");
	GMT_Usage (API, 1, "\n-S<vector>");
	GMT_Usage (API, -2, "The secondary vector (if needed by -T), given as lon/lat, r/theta, or x/y[/z].");
	GMT_Usage (API, 1, "\n-Ta|b|d|D|p<azim>|r<angle>|R|s|t[<azim>/<dist>]|x");
	GMT_Usage (API, -2, "Specify the desired transformation of the input data:");
	GMT_Usage (API, 3, "a: Average of the primary and secondary vector (see -S).");
	GMT_Usage (API, 3, "b: Find the bisector great circle pole(s) for input and secondary vector (see -S).");
	GMT_Usage (API, 3, "d: Compute dot-product(s) with secondary vector (see -S).");
	GMT_Usage (API, 3, "D: Same as -Td, but returns the angles in degrees between the vectors.");
	GMT_Usage (API, 3, "p: Find pole to great circle with <azim> azimuth trend at input vector location.");
	GMT_Usage (API, 3, "s: Find the sum of the secondary vector (see -S) and the input vector(s).");
	GMT_Usage (API, 3, "r: Rotate the input vectors. Depending on your input (2-D or 3-D), append "
		"<angle> or <plon/plat/angle>, respectively, to define the rotation.");
	GMT_Usage (API, 3, "R: As r, but assumes the input vectors/angles are different rotations and repeatedly "
		"rotate the fixed secondary vector (see -S) using the input rotations.");
	GMT_Usage (API, 3, "t: Translate input vectors to points a distance <dist> away in the azimuth <azim>. "
		"Append <azim>/<dist> for a fixed set of azimuth and distance for all points, "
		"otherwise we expect to read <azim>, <dist> from the input file; append a unit [e]. "
		"A negative distance implies a flip of 180 degrees.");
	GMT_Usage (API, 3, "x: Compute cross-product(s) with secondary vector (see -S).");
	GMT_Option (API, "V,bi0");
	if (gmt_M_showusage (API)) GMT_Usage (API, -2, "Default is 2 [or 3; see -C, -fg] input columns.");
	GMT_Option (API, "bo,d,e,f,g,h,i,o,q,s,:,.");

	return (GMT_MODULE_USAGE);
}

static int parse (struct GMT_CTRL *GMT, struct GMTVECTOR_CTRL *Ctrl, struct GMT_OPTION *options) {

	/* This parses the options provided to grdsample and sets parameters in CTRL.
	 * Any GMT common options will override values set previously by other commands.
	 * It also replaces any file names specified as input or output with the data ID
	 * returned when registering these sources/destinations with the API.
	 */

	unsigned int n_in, n_errors = 0;
	int n;
	char txt_a[GMT_LEN64] = {""}, txt_b[GMT_LEN64] = {""}, txt_c[GMT_LEN64] = {""};
	struct GMT_OPTION *opt = NULL;
	struct GMTAPI_CTRL *API = GMT->parent;

	for (opt = options; opt; opt = opt->next) {
		switch (opt->option) {

			case '<':	/* Input files or single point */
				Ctrl->In.active = true;
				if (Ctrl->In.n_args++ == 0) Ctrl->In.arg = strdup (opt->arg);
				break;
			case '>':	/* Got named output file */
				n_errors += gmt_M_repeated_module_option (API, Ctrl->Out.active);
				n_errors += gmt_get_required_file (GMT, opt->arg, opt->option, 0, GMT_IS_DATASET, GMT_OUT, GMT_FILE_LOCAL, &(Ctrl->Out.file));
				break;

			/* Processes program-specific parameters */

			case 'A':	/* Secondary vector */
				n_errors += gmt_M_repeated_module_option (API, Ctrl->A.active);
				if (opt->arg[0] == 'm') {
					Ctrl->A.mode = 1;
					if (opt->arg[1]) Ctrl->A.conf = 0.01 * atof (&opt->arg[1]);
				}
				else
					Ctrl->A.arg = strdup (opt->arg);
				break;
			case 'C':	/* Cartesian coordinates on in|out */
				if (opt->arg[0] == 'i') {
					n_errors += gmt_M_repeated_module_option (API, Ctrl->C.active[GMT_IN]);
				}
				else if (opt->arg[0] == 'o') {
					n_errors += gmt_M_repeated_module_option (API, Ctrl->C.active[GMT_OUT]);
				}
				else if (opt->arg[0] == '\0') {
					n_errors += gmt_M_repeated_module_option (API, Ctrl->C.active[GMT_IN]);
					n_errors += gmt_M_repeated_module_option (API, Ctrl->C.active[GMT_OUT]);
				}
				else {
					GMT_Report (API, GMT_MSG_ERROR, "Bad modifier given to -C (%s)\n", opt->arg);
					n_errors++;
				}
				break;
			case 'E':	/* geodetic/geocentric conversion */
				n_errors += gmt_M_repeated_module_option (API, Ctrl->E.active);
				n_errors += gmt_get_no_argument (GMT, opt->arg, opt->option, 0);
				break;
			case 'N':	/* Normalize vectors */
				n_errors += gmt_M_repeated_module_option (API, Ctrl->N.active);
				n_errors += gmt_get_no_argument (GMT, opt->arg, opt->option, 0);
				break;
			case 'T':	/* Selects transformation */
				n_errors += gmt_M_repeated_module_option (API, Ctrl->T.active);
				switch (opt->arg[0]) {
					case 'a':	/* Average location of the two vectors */
						Ctrl->T.mode = DO_AVERAGE;
						break;
					case 'b':	/* Pole of bisector great circle */
						Ctrl->T.mode = DO_BISECTOR;
						break;
					case 'D':	/* Angle between vectors */
						Ctrl->T.degree = true;
						/* Intentionally fall through - to 'd' */
					case 'd':	/* dot-product of two vectors */
						Ctrl->T.mode = DO_DOT;
						break;
					case 'p':	/* Pole of great circle */
						Ctrl->T.mode = DO_POLE;
						Ctrl->T.par[0] = atof (&opt->arg[1]);
						break;
					case 'r':	/* Rotate vectors */
						n = sscanf (&opt->arg[1], "%[^/]/%[^/]/%s", txt_a, txt_b, txt_c);
						if (n == 1) {	/* 2-D Cartesian rotation */
							Ctrl->T.mode = DO_ROT2D;
							Ctrl->T.par[2] = atof (txt_a);
						}
						else if (n == 3) {	/* 3-D spherical rotation */
							Ctrl->T.mode = DO_ROT3D;
							n_errors += gmt_verify_expectations (GMT, gmt_M_type (GMT, GMT_IN, GMT_X), gmt_scanf_arg (GMT, txt_a, gmt_M_type (GMT, GMT_IN, GMT_X), false, &Ctrl->T.par[0]), txt_a);
							n_errors += gmt_verify_expectations (GMT, gmt_M_type (GMT, GMT_IN, GMT_Y), gmt_scanf_arg (GMT, txt_b, gmt_M_type (GMT, GMT_IN, GMT_Y), false, &Ctrl->T.par[1]), txt_b);
							Ctrl->T.par[2] = atof (txt_c);
						}
						else {
							GMT_Report (API, GMT_MSG_ERROR, "Bad arguments given to -Tr (%s)\n", &opt->arg[1]);
							n_errors++;
						}
						break;
					case 'R':	/* Rotate secondary vector using input rotations */
						Ctrl->T.mode = DO_ROTVAR2D;	/* Change to 3D later if that is what we are doing */
						break;
					case 's':	/* Sum of vectors */
						Ctrl->T.mode = DO_SUM;
						break;
					case 't':	/* Translate vectors */
						Ctrl->T.mode = DO_TRANSLATE;
						if (opt->arg[1]) {	/* Gave azimuth/distance[<unit>] or just <unit> */
							if (strchr (opt->arg, '/')) {	/* Gave a fixed azimuth/distance[<unit>] combination */
								if ((n = sscanf (&opt->arg[1], "%lg/%s", &Ctrl->T.par[0], txt_a)) != 2) {
									GMT_Report (API, GMT_MSG_ERROR, "Bad arguments given to -Tt (%s)\n", &opt->arg[1]);
									n_errors++;
								}
								else {
									unsigned int k = 0;
									if (txt_a[0] == '-') {	/* Skip leading sign and flip vector) */
										k = 1;
										Ctrl->T.par[0] += 180.0;
									}
									Ctrl->T.dmode = gmt_get_distance (GMT, &txt_a[k], &(Ctrl->T.par[1]), &(Ctrl->T.unit));
								}
							}
							else if (strchr (GMT_LEN_UNITS, opt->arg[1])) {	/* Gave the unit of the data in column 3 */
								Ctrl->T.unit = opt->arg[1];
								Ctrl->T.a_and_d = true;
							}
						}
						else /* No arg means use default meters */
							Ctrl->T.a_and_d = true;
						break;
					case 'x':	/* Cross-product between vectors */
						Ctrl->T.mode = DO_CROSS;
						break;
					default:
						GMT_Report (API, GMT_MSG_ERROR, "Unrecognized directive given to -T (%s)\n", opt->arg);
						n_errors++;
						break;
				}
				break;
			case 'S':	/* Secondary vector */
				n_errors += gmt_M_repeated_module_option (API, Ctrl->S.active);
				n_errors += gmt_get_required_string (GMT, opt->arg, opt->option, 0, &Ctrl->S.arg);
				break;
			default:	/* Report bad options */
				n_errors += gmt_default_option_error (GMT, opt);
				break;
		}
	}

	if ((Ctrl->T.mode == DO_ROT3D || Ctrl->T.mode == DO_POLE) && gmt_M_is_cartesian (GMT, GMT_IN)) gmt_parse_common_options (GMT, "f", 'f', "g"); /* Set -fg unless already set for 3-D rots and pole ops */
	if (Ctrl->T.dmode >= GMT_GREATCIRCLE || GMT->common.j.active) gmt_parse_common_options (GMT, "f", 'f', "g"); /* Set -fg implicitly */
	n_in = (Ctrl->C.active[GMT_IN] && gmt_M_is_geographic (GMT, GMT_IN)) ? 3 : 2;
	if (Ctrl->T.a_and_d) n_in += 2;	/* Must read azimuth and distance as well */
	if (GMT->common.b.active[GMT_IN] && GMT->common.b.ncol[GMT_IN] == 0) GMT->common.b.ncol[GMT_IN] = n_in;
	n_errors += gmt_M_check_condition (GMT, GMT->common.b.active[GMT_IN] && GMT->common.b.ncol[GMT_IN] < n_in, "Binary input data (-bi) must have at least %d columns\n", n_in);
	n_errors += gmt_M_check_condition (GMT, Ctrl->S.active && Ctrl->S.arg && !gmt_access (GMT, Ctrl->S.arg, R_OK), "Option -S: Secondary vector cannot be a file!\n");
	n_errors += gmt_M_check_condition (GMT, Ctrl->In.n_args && Ctrl->A.active && Ctrl->A.mode == 0, "Cannot give input files and -A<vec> at the same time\n");

	return (n_errors ? GMT_PARSE_ERROR : GMT_NOERROR);
}

GMT_LOCAL unsigned int gmtvector_decode_vector (struct GMT_CTRL *GMT, char *arg, double coord[], int cartesian, int geocentric) {
	unsigned int n_out, n_errors = 0, ix, iy;
	int n;
	char txt_a[GMT_LEN64] = {""}, txt_b[GMT_LEN64] = {""}, txt_c[GMT_LEN64] = {""};

	ix = (GMT->current.setting.io_lonlat_toggle[GMT_IN]);	iy = 1 - ix;
	n = sscanf (arg, "%[^/]/%[^/]/%s", txt_a, txt_b, txt_c);
	assert (n >= 2);
	n_out = n;
	if (n == 2) {	/* Got lon/lat, r/theta, or x/y */
		if (gmt_M_is_geographic (GMT, GMT_IN)) {
			n_errors += gmt_verify_expectations (GMT, gmt_M_type (GMT, GMT_IN, ix), gmt_scanf_arg (GMT, txt_a, gmt_M_type (GMT, GMT_IN, ix), false, &coord[ix]), txt_a);
			n_errors += gmt_verify_expectations (GMT, gmt_M_type (GMT, GMT_IN, iy), gmt_scanf_arg (GMT, txt_b, gmt_M_type (GMT, GMT_IN, iy), false, &coord[iy]), txt_b);
			if (geocentric) coord[GMT_Y] = gmt_lat_swap (GMT, coord[GMT_Y], GMT_LATSWAP_G2O);
			gmt_geo_to_cart (GMT, coord[GMT_Y], coord[GMT_X], coord, true);	/* get x/y/z */
			n_out = 3;
		}
		else if (cartesian) {	/* Cartesian x/y */
			coord[GMT_X] = atof (txt_a);
			coord[GMT_Y] = atof (txt_b);
		}
		else	/* Cylindrical r/theta */
			gmt_polar_to_cart (GMT, atof (txt_a), atof (txt_b), coord, true);
	}
	else if (n == 3) {	/* Got x/y/z */
		coord[GMT_X] = atof (txt_a);
		coord[GMT_Y] = atof (txt_b);
		coord[GMT_Z] = atof (txt_c);
	}
	else {
		GMT_Report (GMT->parent, GMT_MSG_ERROR, "Bad vector argument (%s)\n", arg);
		return (0);
	}
	if (n_errors) {
		GMT_Report (GMT->parent, GMT_MSG_ERROR, "Failed to decode the geographic coordinates (%s)\n", arg);
		return (0);
	}
	return (n_out);
}

GMT_LOCAL void gmtvector_get_bisector (struct GMT_CTRL *GMT, double A[3], double B[3], double P[3]) {
	/* Given points in A and B, return the bisector pole via P */

	unsigned int i;
	double Pa[3], M[3];

	/* Get mid point between A and B */

	for (i = 0; i < 3; i++) M[i] = A[i] + B[i];
	gmt_normalize3v (GMT, M);

	/* Get pole for great circle through A and B */

	gmt_cross3v (GMT, A, B, Pa);
	gmt_normalize3v (GMT, Pa);

	/* Then get pole for bisector of A-B through Pa */

	gmt_cross3v (GMT, M, Pa, P);
	gmt_normalize3v (GMT, P);
}

GMT_LOCAL void gmtvector_get_azpole (struct GMT_CTRL *GMT, double A[3], double P[3], double az) {
	/* Given point in A and azimuth az, return the pole P to the oblique equator with given az at A */
	double R[3][3], tmp[3], B[3] = {0.0, 0.0, 1.0};	/* set B to north pole  */
	gmt_cross3v (GMT, A, B, tmp);	/* Point C is 90 degrees away from plan through A and B */
	gmt_normalize3v (GMT, tmp);	/* Get unit vector */
	gmt_make_rot_matrix2 (GMT, A, -az, R);	/* Make rotation about A of -azim degrees */
	gmt_matrix_vect_mult (GMT, 3U, R, tmp, P);
}

GMT_LOCAL void gmtvector_mean_vector (struct GMT_CTRL *GMT, struct GMT_DATASET *D, bool cartesian, double conf, bool geocentric, double *M, double *E) {
	/* Determines the mean vector M and the covariance matrix C */

	unsigned int i, j, k, n_components, nrots, geo = gmt_M_is_geographic (GMT, GMT_IN);
	uint64_t row, n, seg, tbl, p;
	double lambda[3], V[9], work1[3], work2[3], lon, lat, lon2, lat2, scl, L, Y;
	double *P[3], X[3], B[3], C[9];
	struct GMT_DATASEGMENT *S = NULL;

	gmt_M_memset (M, 3, double);
	n_components = (geo || D->n_columns == 3) ? 3 : 2;
	for (k = 0; k < n_components; k++) P[k] = gmt_M_memory (GMT, NULL, D->n_records, double);
	for (tbl = n = 0; tbl < D->n_tables; tbl++) {
		for (seg = 0; seg < D->table[tbl]->n_segments; seg++) {
			S = D->table[tbl]->segment[seg];
			for (row = 0; row < S->n_rows; row++) {
				if (!cartesian) {	/* Want to turn geographic or polar into Cartesian */
					if (geo) {
						lat = S->data[GMT_Y][row];
						if (geocentric) lat = gmt_lat_swap (GMT, lat, GMT_LATSWAP_G2O);	/* Get geocentric latitude */
						gmt_geo_to_cart (GMT, lat, S->data[GMT_X][row], X, true);	/* Get x/y/z */
					}
					else
						gmt_polar_to_cart (GMT, S->data[GMT_X][row], S->data[GMT_Y][row], X, true);
					for (k = 0; k < n_components; k++) P[k][n] = X[k];
				}
				else for (k = 0; k < n_components; k++) P[k][n] = S->data[k][row];
				for (k = 0; k < n_components; k++) M[k] += P[k][n];
				n++;
			}
		}
	}
	if (n > 0)
		for (k = 0; k < n_components; k++) M[k] /= n;	/* Compute mean resultant vector [not normalized] */
	gmt_M_memcpy (X, M, 3, double);

	/* Compute covariance matrix */

	gmt_M_memset (C, 9, double);
	for (j = k = 0; j < n_components; j++) for (i = 0; i < n_components; i++, k++) {
		for (p = 0; p < n; p++) C[k] += (P[j][p] - M[j]) * (P[i][p] - M[i]);
		C[k] /= (n - 1.0);
	}
	for (k = 0; k < n_components; k++) gmt_M_free (GMT, P[k]);

	if (gmt_jacobi (GMT, C, n_components, n_components, lambda, V, work1, work2, &nrots)) {	/* Solve eigen-system */
		GMT_Report (GMT->parent, GMT_MSG_WARNING, "Eigenvalue routine failed to converge in 50 sweeps.\n");
	}
	if (n_components == 3 && geo) {	/* Special case of geographic unit vectors; normalize and recover lon,lat */
		gmt_normalize3v (GMT, M);
		gmt_cart_to_geo (GMT, &lat, &lon, M, true);
		if (geocentric) lat = gmt_lat_swap (GMT, lat, GMT_LATSWAP_G2O+1);	/* Get geodetic latitude */
		if (lon < 0.0) lon += 360.0;
	}
	else	/* Cartesian, get y-component */
		lat = X[GMT_Y];

	/* Find the xy[z] point (B) of end of eigenvector 1: */
	gmt_M_memset (B, 3, double);
	for (k = 0; k < n_components; k++) B[k] = X[k] + sqrt (lambda[0]) * V[k];
	L = sqrt (B[0] * B[0] + B[1] * B[1] + B[2] * B[2]);	/* Length of B */
	for (k = 0; k < n_components; k++) B[k] /= L;	/* Normalize */
	if (n_components == 3 && geo) {	/* Special case of geographic unit vectors; normalize and recover lon,lat */
		gmt_normalize3v (GMT, B);
		gmt_cart_to_geo (GMT, &lat2, &lon2, B, true);
		if (geocentric) lat2 = gmt_lat_swap (GMT, lat2, GMT_LATSWAP_G2O+1);	/* Get geodetic latitude */
		if (lon2 < 0.0) lon2 += 360.0;
		gmt_M_set_delta_lon (lon, lon2, L);
		scl = cosd (lat);	/* Local flat-Earth approximation */
	}
	else {	/* Cartesian, get y-component */
		lat2 = B[GMT_Y];
		scl = 1.0;	/* No flat-Earth scaling here */
	}
	Y = lat2 - lat;

	E[0] = 90.0 - atan2 (Y, L * scl) * R2D;	/* Get azimuth */
	/* Convert to 95% confidence (in km, if geographic) */

	scl = sqrt (gmt_chi2crit (GMT, conf, n_components));
	if (geo) scl *= GMT->current.proj.DIST_KM_PR_DEG * R2D;
	E[1] = 2.0 * sqrt (lambda[0]) * scl;	/* 2x since we need the major axis not semi-major */
	E[2] = 2.0 * sqrt (lambda[1]) * scl;

	GMT_Report (GMT->parent, GMT_MSG_INFORMATION, "%g%% confidence ellipse on mean position: Major axis = %g Minor axis = %g Major axis azimuth = %g\n", 100.0 * conf, E[1], E[2], E[0]);
}

GMT_LOCAL void gmtvector_gmt_make_rot2d_matrix (double angle, double R[3][3]) {
	double s, c;
	gmt_M_memset (R, 9, double);
	sincosd (angle, &s, &c);
	R[0][0] = c;	R[0][1] = -s;
	R[1][0] = s;	R[1][1] = c;
}

GMT_LOCAL double gmtvector_dist_to_meter (struct GMT_CTRL *GMT, double d_in) {
	double d_out = d_in / GMT->current.map.dist[GMT_MAP_DIST].scale;	/* Now in degrees or meters */
	if (GMT->current.map.dist[GMT_MAP_DIST].arc)	/* Got arc measure */
		d_out *= GMT->current.proj.DIST_M_PR_DEG;	/* Now in degrees */
	return (d_out);
}

GMT_LOCAL double gmtvector_dist_to_degree (struct GMT_CTRL *GMT, double d_in) {
	double d_out = d_in / GMT->current.map.dist[GMT_MAP_DIST].scale;	/* Now in degrees or meters */
	if (!GMT->current.map.dist[GMT_MAP_DIST].arc)	/* Got unit distance measure */
		d_out /= GMT->current.proj.DIST_M_PR_DEG;	/* Now in degrees */
	return (d_out);
}

#define bailout(code) {gmt_M_free_options (mode); return (code);}
#define Return(code) {Free_Ctrl (GMT, Ctrl); gmt_end_module (GMT, GMT_cpy); bailout (code);}

EXTERN_MSC int GMT_gmtvector (void *V_API, int mode, void *args) {
	unsigned int tbl, error = 0, k, n, n_in, n_components, n_out, add_cols = 0, geo, n_for_output = 0;
	bool single = false, convert;

	uint64_t row, seg;

	double out[3], vector_1[3], vector_2[3], vector_3[3], R[3][3], E[3];

	struct GMT_DATASET *Din = NULL, *Dout = NULL;
	struct GMT_DATASEGMENT *Sin = NULL,  *Sout = NULL;
	struct GMT_DATASET_HIDDEN *DH = NULL;
	struct GMTVECTOR_CTRL *Ctrl = NULL;
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
	geo = gmt_M_is_geographic (GMT, GMT_IN);

	/*---------------------------- This is the gmtvector main code ----------------------------*/

	gmt_M_memset (vector_1, 3, double);
	gmt_M_memset (vector_2, 3, double);
	gmt_M_memset (vector_3, 3, double);
	if (Ctrl->T.mode == DO_ROT3D)	/* Spherical 3-D rotation */
		gmt_make_rot_matrix (GMT, Ctrl->T.par[0], Ctrl->T.par[1], Ctrl->T.par[2], R);
	else if (Ctrl->T.mode == DO_ROT2D)	/* Cartesian 2-D rotation */
		gmtvector_gmt_make_rot2d_matrix (Ctrl->T.par[2], R);
	else if (!(Ctrl->T.mode == DO_NOTHING || Ctrl->T.mode == DO_POLE || Ctrl->T.mode == DO_TRANSLATE)) {	/* Will need secondary vector, get that first before input file */
		n = gmtvector_decode_vector (GMT, Ctrl->S.arg, vector_2, Ctrl->C.active[GMT_IN], Ctrl->E.active);
		if (n == 0) Return (GMT_RUNTIME_ERROR);
		if (Ctrl->T.mode == DO_DOT) {	/* Must normalize to turn dot-product into angle */
			if (n == 2)
				gmt_normalize2v (GMT, vector_2);
			else
				gmt_normalize3v (GMT, vector_2);
			Ctrl->C.active[GMT_OUT] = true;	/* Since we just want to return the angle */
		}
	}

	if (Ctrl->T.mode == DO_TRANSLATE && geo) {	/* Initialize distance machinery */
		int def_mode = (GMT->common.j.active) ? GMT->common.j.mode : GMT_GREATCIRCLE;
		if (Ctrl->T.a_and_d) {	/* Read az and dist from file, set units here */
			Ctrl->T.dmode = def_mode;	/* Store the setting here */
			error = gmt_init_distaz (GMT, Ctrl->T.unit, Ctrl->T.dmode, GMT_MAP_DIST);
		}
		else {	/* Got a fixed set, set units here */
			if (GMT->common.j.active) Ctrl->T.dmode = def_mode;	/* Override any setting in -T */
			error = gmt_init_distaz (GMT, Ctrl->T.unit, Ctrl->T.dmode, GMT_MAP_DIST);
			if (Ctrl->T.dmode == GMT_GREATCIRCLE)
				Ctrl->T.par[1] = gmtvector_dist_to_degree (GMT, Ctrl->T.par[1]);	/* Make sure we have degrees from whatever -Tt set */
			else 	/* Get meters */
				Ctrl->T.par[1] = gmtvector_dist_to_meter (GMT, Ctrl->T.par[1]);	/* Make sure we have degrees from whatever -Tt set */
		}
		if (error == GMT_NOT_A_VALID_TYPE) Return (error);
	}

	/* Read input data set */

	n_in = (geo && Ctrl->C.active[GMT_IN]) ? 3 : 2;
	if (Ctrl->T.a_and_d) n_in += 2;	/* Must read azimuth and distance as well */

	if (Ctrl->A.active) {	/* Want a single primary vector */
		uint64_t dim[GMT_DIM_SIZE] = {1, 1, 1, 3};
		GMT_Report (API, GMT_MSG_INFORMATION, "Processing single input vector; no files are read\n");
		if (Ctrl->A.mode) {	/* Compute the mean of all input vectors */
			if (GMT_Init_IO (API, GMT_IS_DATASET, GMT_IS_POINT, GMT_IN, GMT_ADD_DEFAULT, 0, options) != GMT_NOERROR) {	/* Registers default input sources, unless already set */
				Return (API->error);
			}
			if ((Din = GMT_Read_Data (API, GMT_IS_DATASET, GMT_IS_FILE, 0, GMT_READ_NORMAL, NULL, NULL, NULL)) == NULL) {
				Return (API->error);
			}
			if (Din->n_columns < 2) {
				GMT_Report (API, GMT_MSG_ERROR, "Input data have %d column(s) but at least %u are needed\n", (int)Din->n_columns, n_in);
				Return (GMT_DIM_TOO_SMALL);
			}
			n = n_out = (Ctrl->C.active[GMT_OUT] && (Din->n_columns == 3 || geo)) ? 3 : 2;
			gmtvector_mean_vector (GMT, Din, Ctrl->C.active[GMT_IN], Ctrl->A.conf, Ctrl->E.active, vector_1, E);	/* Get mean vector and confidence ellipse parameters */
			if (GMT_Destroy_Data (API, &Din) != GMT_NOERROR) {
				Return (API->error);
			}
			add_cols = 3;	/* Make space for angle major minor */
		}
		else {	/* Decode a single vector */
			n = gmtvector_decode_vector (GMT, Ctrl->A.arg, vector_1, Ctrl->C.active[GMT_IN], Ctrl->E.active);
			if (n == 0) Return (GMT_RUNTIME_ERROR);
			if (Ctrl->T.mode == DO_DOT) {	/* Must normalize before we turn dot-product into angle */
				if (n == 2)
					gmt_normalize2v (GMT, vector_1);
				else
					gmt_normalize3v (GMT, vector_1);
			}
			n_out = (Ctrl->C.active[GMT_OUT] && n == 3) ? 3 : 2;
		}
		if ((Din = GMT_Create_Data (API, GMT_IS_DATASET, GMT_IS_POINT, 0, dim, NULL, NULL, 0, 0, NULL)) == NULL) Return (API->error);
		n_components = (n == 3 || geo) ? 3 : 2;	/* Number of Cartesian vector components */
		for (k = 0; k < n_components; k++) Din->table[0]->segment[0]->data[k][0] = vector_1[k];
		Din->table[0]->segment[0]->n_rows = 1;
		single = true;
	}
	else {	/* Read input files or stdin */
		GMT_Report (API, GMT_MSG_INFORMATION, "Processing input table data\n");
		if (GMT_Init_IO (API, GMT_IS_DATASET, GMT_IS_POINT, GMT_IN, GMT_ADD_DEFAULT, 0, options) != GMT_NOERROR) {	/* Registers default input sources, unless already set */
			Return (API->error);
		}
		if ((Din = GMT_Read_Data (API, GMT_IS_DATASET, GMT_IS_FILE, 0, GMT_READ_NORMAL, NULL, NULL, NULL)) == NULL) {
			Return (API->error);
		}
		if (Din->n_columns < n_in) {
			GMT_Report (API, GMT_MSG_ERROR, "Input data have %d column(s) but at least %u are needed\n", (int)Din->n_columns, n_in);
			Return (GMT_DIM_TOO_SMALL);
		}
		n = n_out = (Ctrl->C.active[GMT_OUT] && (Din->n_columns == 3 || geo)) ? 3 : 2;
	}

	if (Ctrl->T.mode == DO_DOT) {
		n_out = 1;	/* Override prior setting since we just will report an angle in one column */
		gmt_set_column_type (GMT, GMT_OUT, GMT_X, GMT_IS_FLOAT);
	}
	else if (Ctrl->T.mode == DO_ROTVAR2D) {	/* 2D or 3D */
		if (Din->n_columns == 1) n_out = 2;
		if (Din->n_columns == 3) Ctrl->T.mode = DO_ROTVAR3D;	/* OK, it is 3D */
	}
	else if (Ctrl->C.active[GMT_OUT] || !gmt_M_is_geographic (GMT, GMT_OUT))	/* Override types since output is Cartesian or polar coordinates, not lon/lat */
		gmt_set_cartesian (GMT, GMT_OUT);
	DH = gmt_get_DD_hidden (Din);
	DH->dim[GMT_COL] = n_out + add_cols;	/* State we want a different set of columns on output */
	Dout = GMT_Duplicate_Data (API, GMT_IS_DATASET, GMT_DUPLICATE_ALLOC, Din);
	gmt_M_memset (out, 3, double);

	/* OK, with data in hand we can do some damage */

	if (Ctrl->T.mode == DO_DOT) Ctrl->T.mode = (n == 3 || geo) ? DO_DOT3D : DO_DOT2D;
	n_components = (n == 3 || geo) ? 3 : 2;	/* Number of Cartesian vector components */
	if (Ctrl->T.mode == DO_ROTVAR2D) n_components = 1;	/* Override in case of 2-D Cartesian rotation angles on input */
	convert = (!single && !Ctrl->C.active[GMT_IN] && !(Ctrl->T.mode == DO_ROTVAR2D || Ctrl->T.mode == DO_ROTVAR3D));
	if (Ctrl->T.mode == DO_TRANSLATE) {
		convert = false;
		n_components = 2;
	}
	for (tbl = 0; tbl < Din->n_tables; tbl++) {
		for (seg = 0; seg < Din->table[tbl]->n_segments; seg++) {
			Sin = Din->table[tbl]->segment[seg];
			Sout = Dout->table[tbl]->segment[seg];
			if (Sin->header) Sout->header = strdup (Sin->header);
			for (row = 0; row < Sin->n_rows; row++) {
				if (convert) {	/* Want to turn geographic or polar into Cartesian */
					if (geo)
						gmt_geo_to_cart (GMT, Sin->data[GMT_Y][row], Sin->data[GMT_X][row], vector_1, true);	/* get x/y/z */
					else
						gmt_polar_to_cart (GMT, Sin->data[GMT_X][row], Sin->data[GMT_Y][row], vector_1, true);
				}
				else for (k = 0; k < n_components; k++) vector_1[k] = Sin->data[k][row];

				switch (Ctrl->T.mode) {
					case DO_AVERAGE:	/* Get sum of 2-D or 3-D vectors and compute average */
						for (k = 0; k < n_components; k++) vector_3[k] = 0.5 * (vector_1[k] + vector_2[k]);
						break;
					case DO_BISECTOR:	/* Compute pole or bisector of vector 1 and 2 */
						gmtvector_get_bisector (GMT, vector_1, vector_2, vector_3);
						break;
					case DO_DOT:	/* Just here to quiet the compiler as DO_DOT has been replaced in line 552 above */
					case DO_DOT2D:	/* Get angle between 2-D vectors */
						if (!single) gmt_normalize2v (GMT, vector_1);
						vector_3[0] = gmt_dot2v (GMT, vector_1, vector_2);
						if (Ctrl->T.degree) vector_3[0] = acosd (vector_3[0]);
						break;
					case DO_DOT3D:	/* Get angle between 3-D vectors */
						if (!single) gmt_normalize3v (GMT, vector_1);
						vector_3[0] = gmt_dot3v (GMT, vector_1, vector_2);
						if (Ctrl->T.degree) vector_3[0] = acosd (vector_3[0]);
						break;
					case DO_CROSS:	/* Get cross-product of 3-D vectors */
						gmt_cross3v (GMT, vector_1, vector_2, vector_3);
						break;
					case DO_SUM:	/* Get sum of 2-D or 3-D vectors */
						for (k = 0; k < n_components; k++) vector_3[k] = vector_1[k] + vector_2[k];
						break;
					case DO_ROT2D:	/* Rotate a 2-D vector about the z_axis */
						gmt_matrix_vect_mult (GMT, 2U, R, vector_1, vector_3);
						break;
					case DO_ROT3D:	/* Rotate a 3-D vector about an arbitrary pole encoded in 3x3 matrix R */
						gmt_matrix_vect_mult (GMT, 3U, R, vector_1, vector_3);
						break;
					case DO_ROTVAR2D:	/* Rotate a 2-D vector about the z_axis */
						gmtvector_gmt_make_rot2d_matrix (vector_1[0], R);
						gmt_matrix_vect_mult (GMT, 2U, R, vector_2, vector_3);
						break;
					case DO_ROTVAR3D:	/* Rotate a 3-D vector about an arbitrary pole encoded in 3x3 matrix R */
						gmt_make_rot_matrix (GMT, vector_1[0], vector_1[1], vector_1[2], R);
						gmt_matrix_vect_mult (GMT, 3U, R, vector_2, vector_3);
						break;
					case DO_POLE:	/* Return pole of great circle defined by center point an azimuth */
						gmtvector_get_azpole (GMT, vector_1, vector_3, Ctrl->T.par[0]);
						break;
					case DO_TRANSLATE:	/* Return translated points moved a distance d in the direction of azimuth  */
						if (Ctrl->T.a_and_d) {	/* Get azimuth and distance from input file */
							if (Sin->data[3][row] < 0.0) {	/* FLip the vector 180 */
								Sin->data[3][row] = fabs (Sin->data[3][row]);
								Ctrl->T.par[0] = Sin->data[2][row] + 180.0;
							}
							Ctrl->T.par[0] = Sin->data[2][row];
							if (geo)
								Ctrl->T.par[1] = (Ctrl->T.dmode == GMT_GREATCIRCLE) ? gmtvector_dist_to_degree (GMT, Sin->data[3][row]) : gmtvector_dist_to_meter (GMT, Sin->data[3][row]);	/* Make sure we have degrees or meters for calculations */
							else
								Ctrl->T.par[1] = Sin->data[3][row];	/* Pass as is */
						}
						gmt_translate_point (GMT, vector_1, vector_3, Ctrl->T.par, geo);
						break;
					case DO_NOTHING:	/* Probably just want the effect of -C, -E, -N */
						gmt_M_memcpy (vector_3, vector_1, n_components, double);
						break;
				}
				if (Ctrl->T.mode == DO_TRANSLATE)	/* Want to turn Cartesian output into something else */
					gmt_M_memcpy (out, vector_3, 2, double);
				else if (Ctrl->C.active[GMT_OUT]) {	/* Report Cartesian output... */
					if (Ctrl->N.active) {	/* ...but normalize first */
						if (n_out == 3)
							gmt_normalize3v (GMT, vector_3);
						else if (n_out == 2)
							gmt_normalize2v (GMT, vector_3);
					}
					gmt_M_memcpy (out, vector_3, n_out, double);
				}
				else {	/* Want to turn Cartesian output into something else */
					if (gmt_M_is_geographic (GMT, GMT_OUT)) {
						gmt_normalize3v (GMT, vector_3);	/* Must always normalize before calling gmt_cart_to_geo */
						gmt_cart_to_geo (GMT, &(out[GMT_Y]), &(out[GMT_X]), vector_3, true);	/* Get lon/lat */
						if (Ctrl->E.active) out[GMT_Y] = gmt_lat_swap (GMT, out[GMT_Y], GMT_LATSWAP_G2O + 1);	/* Convert to geodetic */
					}
					else {
						if (Ctrl->N.active) gmt_normalize2v (GMT, vector_3);	/* Optional for cart_to_polar */
						gmt_cart_to_polar (GMT, &(out[GMT_X]), &(out[GMT_Y]), vector_3, true);	/* Get r/theta */
					}
				}
				for (k = 0; k < n_out; k++) Sout->data[k][row] = out[k];
				n_for_output++;
			}
		}
	}

	if (Ctrl->A.mode && Sout) for (k = 0; k < 3; k++) Sout->data[k+n_out][0] = E[k];	/* Place az, major, minor in the single output record */

	/* Time to write out the results */

	if (n_for_output) {
		if (GMT_Init_IO (API, GMT_IS_DATASET, GMT_IS_POINT, GMT_OUT, GMT_ADD_DEFAULT, 0, options) != GMT_NOERROR) {	/* Establishes data output */
			Return (API->error);
		}
		if (GMT_Write_Data (API, GMT_IS_DATASET, GMT_IS_FILE, GMT_IS_POINT, GMT_WRITE_SET, NULL, Ctrl->Out.file, Dout) != GMT_NOERROR) {
			Return (API->error);
		}
	}
	if (single && GMT_Destroy_Data (API, &Din) != GMT_NOERROR) {
		Return (API->error);
	}

	Return (GMT_NOERROR);
}
