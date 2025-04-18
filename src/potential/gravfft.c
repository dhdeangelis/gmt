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
 * Author:      Joaquim Luis
 * Date:        09-OCT-1994
 * Updated:	28-Feb-1999
 *         	05-SEP-2002
 *         	13-SEP-2002
 * GMT5ed   17-MAR-2012
 *          14-Feb-2013		Big rework by PW
 *          01-Dec-2013		Added -T with infill density approximation
 *
 *
 * For details, see Luis, J.F. and M.C. Neves. 2006, "The isostatic compensation of the Azores Plateau:
 * a 3D admittance and coherence analysis. J. Geotermal Vulc. Res. Volume 156, Issues 1-2 ,
 * Pages 10-22, https://doi.org/10.1016/j.jvolgeores.2006.03.010
 *
 * */

#include "gmt_dev.h"
#include "longopt/gravfft_inc.h"
#include "newton.h"

#define THIS_MODULE_CLASSIC_NAME	"gravfft"
#define THIS_MODULE_MODERN_NAME	"gravfft"
#define THIS_MODULE_LIB		"potential"
#define THIS_MODULE_PURPOSE	"Spectral calculations of gravity, isostasy, admittance, and coherence for grids"
#define THIS_MODULE_KEYS	"<G{+,GG},DG(,GDC,GDI"
#define THIS_MODULE_NEEDS	""
#define THIS_MODULE_OPTIONS "-Vf"

#define GMT_FFT_DIM	2	/* Dimension of FFT needed */

enum Gravfft_fields {
	GRAVFFT_FAA	= 0,
	GRAVFFT_GEOID,
	GRAVFFT_VGG,
	GRAVFFT_DEFL_EAST,
	GRAVFFT_DEFL_NORTH
};

struct GRAVFFT_CTRL {
	unsigned int n_par;
	double *par;

	struct GRAVFFT_In {
		bool active;
		unsigned int n_grids;	/* 1 or 2 */
		char *file[2];
	} In;
	struct GRAVFFT_C {	/* -C<zlevel> */
		bool active;
		unsigned int n_pt;
		double theor_inc;
	} C;
	struct GRAVFFT_D {	/* -D[<rho>|<rhofile>] */
		bool active;
		bool variable;
		char *file;
	} D;
	struct GRAVFFT_E {	/* -E */
		bool active;
		unsigned int n_terms;
	} E;
	struct GRAVFFT_F {	/* -F[f[+s|z]|b|g|e|n|v] */
		bool active;
		bool slab;
		bool bouguer;
		bool zero;
		unsigned int mode;
	} F;
	struct GRAVFFT_G {	/* -G<outfile> */
		bool active;
		char *file;
	} G;
	struct GRAVFFT_I {	/* -I[<scale>|g] */
		bool active;
		double value;
	} I;
	struct GRAVFFT_M {	/* -M<mgal_at_45>  */
		bool active;
		double mgal_at_45, normal_gravity;
	} M;
	struct GRAVFFT_N {	/* -N[f|q|s<n_columns>/<n_rows>][+e|m|n][+t<width>][+w[<suffix>]][+z[p]]  */
		bool active;
		struct GMT_FFT_INFO *info;
	} N;
	struct GRAVFFT_Q {
		bool active;
	} Q;
	struct GRAVFFT_S {	/* -S<scale> */
		bool active;
	} S;
	struct GRAVFFT_T {	/* -T<te/rl/rm/rw[/ri>][+m] */
		bool active, moho, approx;
		double te, rhol, rhom, rhow, rhoi;
		double rho_cw;		/* crust-water density contrast */
		double rho_mc;		/* mantle-crust density contrast */
		double rho_mw;		/* mantle-water density contrast */
	} T;
	struct GRAVFFT_W {	/* Water depth/observation level */
		bool active;
		double water_depth;	/* Reference water depth [0] */
	} W;
	struct GRAVFFT_Z {
		bool active;
		double zm;		/* mean Moho depth (given by user) */
		double zl;		/* mean depth of swell compensation (user given) */
	} Z;
	struct GRAVFFT_misc {	/* -T */
		bool coherence;
		bool give_wavelength;
		bool give_km;
		bool from_below;
		bool from_top;
		double z_level;	/* mean bathymetry level computed from data */
		double rho;	/* general density contrast */
	} misc;
};

#ifndef FSIGNIF
#define FSIGNIF 24
#endif

#define	YOUNGS_MODULUS	7.0e10		/* Pascal = Nt/m**2  */
//#define	NORMAL_GRAVITY	9.806199203	/* Moritz's 1980 IGF value for gravity at 45 degrees latitude (m/s) */
#define	POISSONS_RATIO	0.25

static bool sphericity = false;

static void *New_Ctrl (struct GMT_CTRL *GMT) {	/* Allocate and initialize a new control structure */
	struct GRAVFFT_CTRL *C = NULL;

	C = gmt_M_memory (GMT, NULL, 1, struct GRAVFFT_CTRL);

	/* Initialize values whose defaults are not 0/false/NULL */

	C->E.n_terms = 3;
	return (C);
}

static void Free_Ctrl (struct GMT_CTRL *GMT, struct GRAVFFT_CTRL *C) {	/* Deallocate control structure */
	if (!C) return;
	gmt_M_free (GMT, C->par);
	gmt_M_str_free (C->In.file[0]);
	gmt_M_str_free (C->In.file[1]);
	gmt_M_str_free (C->D.file);
	gmt_M_str_free (C->G.file);
	gmt_M_free (GMT, C->N.info);
	gmt_M_free (GMT, C);
}

static double	scale_out = 1.0;
static double	earth_rad = 6371008.7714;	/* GRS-80 sphere */

GMT_LOCAL void gravfft_do_parker (struct GMT_CTRL *GMT, struct GMT_GRID *Grid, struct GRAVFFT_CTRL *Ctrl, struct GMT_FFT_WAVENUMBER *K,
                          gmt_grdfloat *raised, uint64_t n, double rho);
GMT_LOCAL void gravfft_do_isostasy(struct GMT_CTRL *GMT, struct GMT_GRID *Grid, struct GRAVFFT_CTRL *Ctrl, struct GMT_FFT_WAVENUMBER *K);
GMT_LOCAL void gravfft_load_from_below_admitt(struct GMT_CTRL *GMT, struct GRAVFFT_CTRL *Ctrl, struct GMT_FFT_WAVENUMBER *K, double *z_below);
GMT_LOCAL void gravfft_load_from_top_admitt(struct GMT_CTRL *GMT, struct GRAVFFT_CTRL *Ctrl, struct GMT_FFT_WAVENUMBER *K, double *z_top);
GMT_LOCAL void gravfft_load_from_top_grid(struct GMT_CTRL *GMT, struct GMT_GRID *Grid, struct GRAVFFT_CTRL *Ctrl,
                                  struct GMT_FFT_WAVENUMBER *K, gmt_grdfloat *raised, unsigned int n);
GMT_LOCAL void gravfft_load_from_below_grid(struct GMT_CTRL *GMT, struct GMT_GRID *Grid, struct GRAVFFT_CTRL *Ctrl,
                                    struct GMT_FFT_WAVENUMBER *K, gmt_grdfloat *raised, unsigned int n);
GMT_LOCAL void gravfft_compute_only_admitts(struct GMT_CTRL *GMT, struct GRAVFFT_CTRL *Ctrl, struct GMT_FFT_WAVENUMBER *K,
                                    double *z_top_or_bot, double delta_pt);
GMT_LOCAL int gravfft_do_admittance(struct GMT_CTRL *GMT, struct GMT_GRID *Grid, struct GMT_GRID *GridB, struct GRAVFFT_CTRL *Ctrl,
                            struct GMT_FFT_WAVENUMBER *K);

static int parse (struct GMT_CTRL *GMT, struct GRAVFFT_CTRL *Ctrl, struct GMT_OPTION *options) {

	unsigned int n_errors = 0;
	int n, override_mode = GMT_FFT_REMOVE_MID;
	struct GMT_OPTION *opt = NULL,  *popt = NULL;
	struct GMTAPI_CTRL *API = GMT->parent;
	char   ptr[GMT_BUFSIZ] = {""}, t_or_b[4] = {""}, argument[GMT_LEN16] = {""}, combined[GMT_BUFSIZ] = {""};

	if (gmt_M_compat_check (GMT, 4)) {
		char *mod = NULL;
		if ((popt = GMT_Find_Option (API, 'L', options)) != 0) {	/* Gave old -L */
			mod = popt->arg; /* Gave old -L option */
			if (mod[0] == '\0') strcat (argument, "+l");		/* Leave trend alone -L */
			else if (mod[0] == 'm') strcat (argument, "+a");	/* Remove mean -Lm */
			else if (mod[0] == 'h') strcat (argument, "+h");	/* Remove mid-value -Lh */
		}
	}

	for (opt = options; opt; opt = opt->next) {		/* Process all the options given */
		switch (opt->option) {

			case '<':	/* Input file (repeatable up to 2 times) */
				Ctrl->In.active = true;
				if (Ctrl->In.n_grids >= 2) {
					n_errors++;
					GMT_Report (API, GMT_MSG_ERROR, "A maximum of two input grids may be processed\n");
				}
				else {
					n_errors += gmt_get_required_file (GMT, opt->arg, opt->option, 0, GMT_IS_GRID, GMT_IN, GMT_FILE_REMOTE, &(Ctrl->In.file[Ctrl->In.n_grids]));
					Ctrl->In.n_grids++;
				}
				break;
			case 'C':	/* For theoretical curves only */
				n_errors += gmt_M_repeated_module_option (API, Ctrl->C.active);
				sscanf (opt->arg, "%d/%lf/%lf/%s", &Ctrl->C.n_pt, &Ctrl->C.theor_inc, &Ctrl->misc.z_level, t_or_b);
				for (n = 0; t_or_b[n]; n++) {
					switch (t_or_b[n]) {
						case 'w':
							Ctrl->misc.give_wavelength = true;
							break;
						case 'b':
							Ctrl->misc.from_below = true;
							break;
						case 't':
							Ctrl->misc.from_top = true;
							break;
						default:
							GMT_Report (API, GMT_MSG_ERROR, "Option -C: [%s] is not valid, chose from [tbw]\n", &t_or_b[n]);
							n_errors++;
							break;
					}
				}
				break;
			case 'D':
				n_errors += gmt_M_repeated_module_option (API, Ctrl->D.active);
				if (!opt->arg) {
					GMT_Report (API, GMT_MSG_ERROR,
					            "Option -D: must give constant density contrast or grid with density contrasts\n");
					n_errors++;
				}
				else {
					if (!gmt_access (GMT, opt->arg, R_OK)) {	/* Gave a grid with density contrast */
						Ctrl->D.file = strdup (opt->arg);
						Ctrl->D.variable = true;
						Ctrl->misc.rho = 1.0;
					}
					else
						Ctrl->misc.rho = atof (opt->arg);
					override_mode = GMT_FFT_REMOVE_MID;		/* Leave trend alone and remove mid value */
				}
				break;
			case 'E':
				n_errors += gmt_M_repeated_module_option (API, Ctrl->E.active);
				n_errors += gmt_get_required_uint (GMT, opt->arg, opt->option, 0, &Ctrl->E.n_terms);
				if (Ctrl->E.n_terms > 10) {
					GMT_Report (API, GMT_MSG_ERROR, "Option -E: n_terms must be <= 10\n");
					n_errors++;
				}
				else if (Ctrl->E.n_terms <= 0) {
					GMT_Report (API, GMT_MSG_WARNING, "Option -E: n_terms were 0 or nonsense. Reset to 3\n");
					Ctrl->E.n_terms = 3;
				}
				break;
			case 'F':
				n_errors += gmt_M_repeated_module_option (API, Ctrl->F.active);
				switch (opt->arg[0]) {
					case 'g': Ctrl->F.mode = GRAVFFT_GEOID;      break;
					case 'v': Ctrl->F.mode = GRAVFFT_VGG;        break;
					case 'e': Ctrl->F.mode = GRAVFFT_DEFL_EAST;  break;
					case 'n': Ctrl->F.mode = GRAVFFT_DEFL_NORTH; break;
					case 'b':
						Ctrl->F.mode   = GRAVFFT_FAA;
						Ctrl->F.slab   = true;
						Ctrl->F.bouguer = true;
						break;
					case 'f': default:
						Ctrl->F.mode = GRAVFFT_FAA; 	   /* FAA */
					 	if (opt->arg[1] == '+' && (opt->arg[2] == 's' || opt->arg[2] == '\0')) Ctrl->F.slab = true;
					 	else if (opt->arg[1] == '+' && (opt->arg[2] == 'z' || opt->arg[2] == '\0')) Ctrl->F.zero = true;
						break;
				}
				break;
			case 'G':
				n_errors += gmt_M_repeated_module_option (API, Ctrl->G.active);
				n_errors += gmt_get_required_file (GMT, opt->arg, opt->option, 0, GMT_IS_GRID, GMT_OUT, GMT_FILE_LOCAL, &(Ctrl->G.file));
				break;
			case 'I':
				n_errors += gmt_M_repeated_module_option (API, Ctrl->I.active);
				for (n = 0; opt->arg[n]; n++) {
					switch (opt->arg[n]) {
						case 'w':
							Ctrl->misc.give_wavelength = true;
							break;
						case 'b':
							Ctrl->misc.from_below = true;
							break;
						case 'c':
							Ctrl->misc.coherence = true;
							break;
						case 'k':
							Ctrl->misc.give_km = true;
							break;
						case 't':
							Ctrl->misc.from_top = true;
							break;
						default:
							GMT_Report (API, GMT_MSG_ERROR, "Option -I : [%s] is not valid, chose from [wbct]\n", &ptr[n]);
							n_errors++;
							break;
					}
				}
				break;
			case 'L':	/* Leave trend alone */
				if (gmt_M_compat_check (GMT, 4))
					GMT_Report (API, GMT_MSG_COMPAT, "Option -L is deprecated; use -N modifiers in the future.\n");
				else
					n_errors += gmt_default_option_error (GMT, opt);
				break;
			case 'M':	/* Miligals at 45 degrees */
				n_errors += gmt_M_repeated_module_option(API, Ctrl->M.active);
				sscanf(opt->arg, "%lf", &Ctrl->M.mgal_at_45);
				Ctrl->M.normal_gravity = Ctrl->M.mgal_at_45 * 1e-5;
				break;
			case 'N':
				n_errors += gmt_M_repeated_module_option (API, Ctrl->N.active);
				if (popt && gmt_M_compat_check (GMT, 4)) {	/* Got both old -L and -N; append */
					sprintf (combined, "%s%s", opt->arg, argument);
					Ctrl->N.info = GMT_FFT_Parse (API, 'N', GMT_FFT_DIM, combined);
				}
				else
					Ctrl->N.info = GMT_FFT_Parse (API, 'N', GMT_FFT_DIM, opt->arg);
				if (Ctrl->N.info == NULL) n_errors++;
				break;
			case 'Q':
				n_errors += gmt_M_repeated_module_option (API, Ctrl->Q.active);
				n_errors += gmt_get_no_argument (GMT, opt->arg, opt->option, 0);
				override_mode = GMT_FFT_REMOVE_MID;	/* Leave trend alone and remove mid value */
				break;
			case 'S':
				n_errors += gmt_M_repeated_module_option (API, Ctrl->S.active);
				n_errors += gmt_get_no_argument (GMT, opt->arg, opt->option, 0);
				break;
			case 'T':
				n_errors += gmt_M_repeated_module_option (API, Ctrl->T.active);
				n = sscanf (opt->arg, "%lf/%lf/%lf/%lf/%lf", &Ctrl->T.te, &Ctrl->T.rhol, &Ctrl->T.rhom, &Ctrl->T.rhow, &Ctrl->T.rhoi);
				Ctrl->T.rho_cw = Ctrl->T.rhol - Ctrl->T.rhow;
				Ctrl->T.rho_mc = Ctrl->T.rhom - Ctrl->T.rhol;
				Ctrl->T.rho_mw = Ctrl->T.rhom - Ctrl->T.rhow;
				if (n < 5) Ctrl->T.rhoi = Ctrl->T.rhol;
				if (n == 5 && Ctrl->T.rhoi != Ctrl->T.rhol) Ctrl->T.approx = true;	/* Calculate approximate flexure */
				if (Ctrl->T.te > 1e10) { /* Given flexural rigidity, compute Te */
					Ctrl->T.te = pow ((12.0 * (1.0 - POISSONS_RATIO * POISSONS_RATIO)) * Ctrl->T.te / YOUNGS_MODULUS, 1./3.);
				}
				if (opt->arg[strlen(opt->arg)-2] == '+') {	/* Fragile. Needs further testing unless -Q is used */
					Ctrl->T.moho = true;
					override_mode = GMT_FFT_REMOVE_MID;		/* Leave trend alone and remove mid value */
				}
				break;
			case 'W':	/* Water depth */
				n_errors += gmt_M_repeated_module_option (API, Ctrl->W.active);
				GMT_Get_Values (API, opt->arg, &Ctrl->W.water_depth, 1);
				break;
			case 'Z':
				n_errors += gmt_M_repeated_module_option (API, Ctrl->Z.active);
				sscanf (opt->arg, "%lf/%lf", &Ctrl->Z.zm, &Ctrl->Z.zl);
				break;
			default:
				n_errors += gmt_default_option_error (GMT, opt);
				break;
		}
	}
	if (gmt_M_compat_check (GMT, 4) && !Ctrl->N.active && popt) {	/* User set -L but no -N so nothing got appended above... Sigh...*/
		Ctrl->N.info = GMT_FFT_Parse (API, 'N', GMT_FFT_DIM, argument);
	}

	if (override_mode >= 0) {		/* FAKE TEST AS IT WAS SET ABOVE override_mode = GMT_FFT_REMOVE_MEAN */
		if (Ctrl->N.info == NULL) {	/* User neither gave -L nor -N... Sigh...*/
			if ((Ctrl->N.info = GMT_FFT_Parse (API, 'N', GMT_FFT_DIM, "")) == NULL)	/* Error messages are issued inside parse function */
				n_errors++;
			else
				Ctrl->N.info->trend_mode = override_mode;
		}
		if (!n_errors && Ctrl->N.info->trend_mode == GMT_FFT_REMOVE_NOT_SET)		/* No explicit detrending mode, so apply default */
			Ctrl->N.info->trend_mode = override_mode;
	}

	if (Ctrl->N.active && Ctrl->N.info && Ctrl->N.info->info_mode == GMT_FFT_LIST) {
		return (GMT_PARSE_ERROR);	/* So that we exit the program */
	}

	if (Ctrl->C.active) {
		n_errors += gmt_M_check_condition (GMT, !Ctrl->T.active, "-T not set\n");
		n_errors += gmt_M_check_condition (GMT, Ctrl->misc.from_top && !Ctrl->Z.zm,
		                                   "-Z not set, must give moho compensation depth\n");
		n_errors += gmt_M_check_condition (GMT, Ctrl->misc.from_below && !(Ctrl->Z.zm && Ctrl->Z.zl),
		                                   "-Z not set, must give moho and swell compensation depths\n");
	}
	else {
		n_errors += gmt_M_check_condition (GMT, !Ctrl->In.file[0], "Must specify input file\n");
		n_errors += gmt_M_check_condition (GMT, !Ctrl->G.file && !Ctrl->I.active,
					"Option -G: Must specify output file\n");
		n_errors += gmt_M_check_condition (GMT, Ctrl->Q.active && !Ctrl->T.active, "-Q implies also -T\n");
		n_errors += gmt_M_check_condition (GMT, Ctrl->S.active && !Ctrl->T.active, "-S implies also -T\n");
		n_errors += gmt_M_check_condition (GMT, Ctrl->T.moho && !Ctrl->Z.zm,
					"For computing the Moho's effect I need to know it's average depth (see -Z<zm>)\n");
		n_errors += gmt_M_check_condition (GMT, !(Ctrl->D.active || Ctrl->T.active || Ctrl->S.active ||
		                                   Ctrl->I.active), "Must set density contrast\n");
		n_errors += gmt_M_check_condition (GMT, Ctrl->I.active && !Ctrl->In.file[1],
					"For admittance|coherence need a gravity or geoid grid\n");
		n_errors += gmt_M_check_condition (GMT, Ctrl->misc.from_below && Ctrl->misc.from_top,
					"Option -I: Choose only one model\n");
		n_errors += gmt_M_check_condition (GMT, Ctrl->I.active && Ctrl->misc.from_top &&
					!(Ctrl->T.rhow && Ctrl->T.rhol && Ctrl->T.rhom && Ctrl->Z.zm),
					"Not all parameters needed for computing \"loading from top\" admittance were set\n");
		n_errors += gmt_M_check_condition (GMT, Ctrl->I.active && Ctrl->misc.from_below &&
			!(Ctrl->T.rhow && Ctrl->T.rhol && Ctrl->T.rhom && Ctrl->Z.zm && Ctrl->Z.zl),
					"Not all parameters for computing \"loading from below\" admittance were set\n");
	}

	n_errors += gmt_M_check_condition (GMT, (Ctrl->misc.from_top || Ctrl->misc.from_below) &&
			!(Ctrl->F.mode == GRAVFFT_FAA || Ctrl->F.mode == GRAVFFT_GEOID),
				"Theoretical admittances are only defined for FAA or GEOID.\n");
	
	if (!Ctrl->M.active) {
		Ctrl->M.mgal_at_45 = 980619.9203;	/* Moritz's 1980 IGF value for gravity in mGal at 45 degrees latitude */
		Ctrl->M.normal_gravity = 9.806199203;
	}

	return (n_errors ? GMT_PARSE_ERROR : GMT_NOERROR);
}

static int usage (struct GMTAPI_CTRL *API, int level) {
	const char *name = gmt_show_name_and_purpose (API, THIS_MODULE_LIB, THIS_MODULE_CLASSIC_NAME, THIS_MODULE_PURPOSE);
	if (level == GMT_MODULE_PURPOSE) return (GMT_NOERROR);
	GMT_Usage (API, 0, "usage: %s %s [<ingrid2>] -G%s [-C<n/wavelength/mean_depth/tbw>] "
		"[-D<density>] [-E<n_terms>] [-F[f[+s|z]|b|g|v|n|e]] [-I<cbktw>] [-M<mgal_at_45>] [-N%s] [-Q] [-S] "
		"[-T<te/rl/rm/rw>[/<ri>][+m]] [%s] [-W<wd>[k]] [-Z<zm>[/<zl>]] [-fg] [%s]\n",
		name, GMT_INGRID, GMT_OUTGRID, GMT_FFT_OPT, GMT_V_OPT, GMT_PAR_OPT);

	if (level == GMT_SYNOPSIS) return (GMT_MODULE_SYNOPSIS);

	GMT_Message (API, GMT_TIME_NONE, "  REQUIRED ARGUMENTS:\n");
	gmt_ingrid_syntax (API, 0, "Name of input grid with topography values.  Optionally, give a second grid <ingrid2> for cross-spectral computations (same modifiers apply)");
	gmt_outgrid_syntax (API, 'G', "Set name of the output grid file with gravity [or geoid] value");
	GMT_Message (API, GMT_TIME_NONE, "\n  OPTIONAL ARGUMENTS:\n");
	GMT_Usage (API, 1, "\n-C<n/wavelength/mean_depth/tbw>");
	GMT_Usage (API, -2, "Compute admittance curves based on a theoretical model. "
		"Append <n/wavelength/mean_depth/tbw> as specified below. "
		"Total profile length in meters = <n> * <wavelength>. "
		"The remaining parameters are set by -T and -Z options. "
		"Append /dataflags (one or two) from tbw:");
	GMT_Usage (API, 3, "b: Write \"loading from below\" admittance.");
	GMT_Usage (API, 3, "t: Write \"elastic plate\" admittance.");
	GMT_Usage (API, 3, "w: Write wavelength instead of wavenumber.");
	GMT_Usage (API, 1, "\n-D<density>");
	GMT_Usage (API, -2, "Set density contrast across surface (used when not -T). "
		"Append a co-registered density grid for a variable density contrast or a constant <density>.");
	GMT_Usage (API, 1, "\n-E<n_terms>");
	GMT_Usage (API, -2, "Number of terms used in Parker's expansion [Default = 3].");
	GMT_Usage (API, 1, "\n-F[f[+s|z]|b|g|v|n|e]");
	GMT_Usage (API, -2, "Specify desired geopotential field:");
	GMT_Usage (API, 3, "b: Bouguer anomalies (mGal).");
	GMT_Usage (API, 3, "f: Free-air anomalies (mGal) [Default]. Optionally choose one modifier:");
	GMT_Usage (API, 4, "+s Adjust for implied slab correction [none].");
	GMT_Usage (API, 4, "+z Adjust the field so the far-field is exactly zero [no shift].");
	GMT_Usage (API, 3, "g: Geoid anomalies (m).");
	GMT_Usage (API, 3, "v: Vertical Gravity Gradient (VGG; 1 Eovtos = 0.1 mGal/km).");
	GMT_Usage (API, 3, "e: East deflections of the vertical (micro-radian).");
	GMT_Usage (API, 3, "n: North deflections of the vertical (micro-radian).");
	GMT_Usage (API, 1, "\n-I<cbktw>");
	GMT_Usage (API, -2, "Use <ingrid2> and <ingrid> to estimate admittance|coherence and write "
		"output to standard output (-G ignored if set). The <ingrid2>  should contain gravity or geoid anomalies "
		"for the same region of <ingrid>. Default computes admittance. Output "
		"contains 3 or 4 columns: Frequency (wavelength), admittance (coherence) "
		"one sigma error bar and, optionally, a theoretical admittance. "
		"Append dataflags (one to three) from cbktw:");
	GMT_Usage (API, 3, "c: Compute coherence instead of admittance.");
	GMT_Usage (API, 3, "b: Write a forth column with \"loading from below\" theoretical admittance.");
	GMT_Usage (API, 3, "k: Use km or wavelength unit [m].");
	GMT_Usage (API, 3, "t: Write a forth column with \"elastic plate\" theoretical admittance.");
	GMT_Usage (API, 3, "w: Write wavelength instead of wavenumber.");
	GMT_Usage (API, 1, "\n-M<mgal_at_45>");
	GMT_Usage (API, -2, "Value for gravity in mGal at 45 degrees latitude. Default is 980619.9203 mGal (Moritz's 1980 IGF value).");
	GMT_FFT_Option (API, 'N', GMT_FFT_DIM, "Choose or inquire about suitable grid dimensions for FFT, and set modifiers.");
	GMT_Usage (API, -2, "Warning: both -D -T...+m and -Q will implicitly set -N's +h.");
	GMT_Usage (API, 1, "\n-Q Writes out a grid with the flexural topography (with z positive up) "
		"whose average depth is set to the value given by -Z<zm>.");
	GMT_Usage (API, 1, "\n-S Computes predicted geopotential (see -F) grid due to a subplate load "
		"produced by the current bathymetry and the theoretical admittance. "
		"The necessary parameters are set within -T and -Z options.");
	GMT_Usage (API, 1, "\n-T<te/rl/rm/rw>[/<ri>][+m]");
	GMT_Usage (API, -2, "Computes the isostatic compensation. Input file is topo load. "
		"Append elastic thickness and densities of load, mantle, and "
		"water, all in SI units. Give average mantle depth via -Z. "
		"If the elastic thickness is > 1e10 it will be interpreted as the "
		"flexural rigidity [by default it is computed from Te and Young modulus]. "
		"If an optional infill density <ri> != <rl> is appended we find an approximate solution.");
	GMT_Usage (API, 3, "+m Write a grid with the Moho's geopotential effect "
		"(see -F) from model selected by -T.");
	GMT_Usage (API, 1, "\n-W<wd>[k]");
	GMT_Usage (API, -2, "Specify water depth (or observation level) in m; append k for km.  Must be positive.");
	GMT_Usage (API, 1, "\n-Z<zm>[/<zl>]");
	GMT_Usage (API, -2, "Specify Moho [and swell] average compensation depths.");
	GMT_Option (API, "V");
	GMT_Message (API, GMT_TIME_NONE, "\t-fg Convert geographic grids to meters using a \"Flat Earth\" approximation.\n");
	GMT_Option (API, ".");
	return (GMT_MODULE_USAGE);
}

#define bailout(code) {gmt_M_free_options (mode); return (code);}
#define Return(code) {Free_Ctrl (GMT, Ctrl); gmt_end_module (GMT, GMT_cpy); bailout (code);}

EXTERN_MSC int GMT_gravfft (void *V_API, int mode, void *args) {
	unsigned int k, n;
	int      error = 0;
	uint64_t m;
	gmt_grdfloat    slab_gravity = 0.0f, *topo = NULL, *raised = NULL;
	double   delta_pt, freq;

	struct GMT_GRID *Grid[2] = {NULL, NULL}, *Orig[2] = {NULL, NULL}, *Rho = NULL;
	struct GMT_GRID_HEADER_HIDDEN *HH = NULL;
	struct GMT_FFT_WAVENUMBER *FFT_info[2] = {NULL, NULL}, *K = NULL, *Rho_info = NULL;
	struct GRAVFFT_CTRL *Ctrl = NULL;
	struct GMT_CTRL *GMT = NULL, *GMT_cpy = NULL;
	struct GMT_OPTION *options = NULL;
	struct GMTAPI_CTRL *API = gmt_get_api_ptr (V_API);	/* Cast from void to GMTAPI_CTRL pointer */

	/*----------------------- Standard module initialization and parsing ----------------------*/

	if (API == NULL) return (GMT_NOT_A_SESSION);
	API->parker_fft_default = true;	/* So that GMT_FFT_Option can report +h as default */
	if (mode == GMT_MODULE_PURPOSE) return (usage (API, GMT_MODULE_PURPOSE));	/* Return the purpose of program */
	options = GMT_Create_Options (API, mode, args);
	if (API->error) return (API->error);	/* Set or get option list */

	if ((error = gmt_report_usage (API, options, 0, usage)) != GMT_NOERROR) bailout (error);	/* Give usage if requested */

	/* Parse the command-line arguments */

	if ((GMT = gmt_init_module (API, THIS_MODULE_LIB, THIS_MODULE_CLASSIC_NAME, THIS_MODULE_KEYS, THIS_MODULE_NEEDS, module_kw, &options, &GMT_cpy)) == NULL) bailout (API->error); /* Save current state */
	if (GMT_Parse_Common (API, THIS_MODULE_OPTIONS, options)) Return (API->error);
	Ctrl = New_Ctrl (GMT);	/* Allocate and initialize a new control structure */
	if ((error = parse (GMT, Ctrl, options)) != 0) Return (error);

	/*---------------------------- This is the grdfft main code ----------------------------*/

	/* -------------------- Compute only a theoretical model and exit -------------------- */
	if (Ctrl->C.active) {
		uint64_t dim[GMT_DIM_SIZE] = {1, 1, 0, 0};	/* One table and one segment */
		double *z_top_or_bot = NULL;
		struct GMT_DATASET *D = NULL;
		struct GMT_DATASEGMENT *S = NULL;
		struct GMT_FFT_WAVENUMBER *K = gmt_M_memory (GMT, NULL, 1, struct GMT_FFT_WAVENUMBER);

		z_top_or_bot = gmt_M_memory (GMT, NULL, (size_t)Ctrl->C.n_pt, double);

		delta_pt = 2 * M_PI / (Ctrl->C.n_pt * Ctrl->C.theor_inc);	/* Times 2PI because frequency will be used later */
		gravfft_compute_only_admitts (GMT, Ctrl, K, z_top_or_bot, delta_pt);
		delta_pt /= (2.0 * M_PI);			/* Write out frequency, not wavenumber  */
		if (Ctrl->misc.give_wavelength && Ctrl->misc.give_km) delta_pt *= 1000.0;	/* Wanted wavelength in km */

		gmt_set_cartesian (GMT, GMT_OUT);	/* To counter-act any -fg setting */

		dim[GMT_COL] = 2;
		dim[GMT_ROW] = Ctrl->C.n_pt;
		if ((D = GMT_Create_Data (GMT->parent, GMT_IS_DATASET, GMT_IS_NONE, 0, dim, NULL, NULL, 0, 0, NULL)) == NULL) {
			GMT_Report (GMT->parent, GMT_MSG_ERROR, "Unable to create a data set for spectral estimates\n");
			gmt_M_free (GMT, K);
			gmt_M_free (GMT, z_top_or_bot);
			Return (API->error);
		}
		S = D->table[0]->segment[0];	/* Only one table with one segment here */
		S->n_rows = Ctrl->C.n_pt;

		for (k = 0; k < Ctrl->C.n_pt; k++) {
			freq = (k + 1) * delta_pt;
			if (Ctrl->misc.give_wavelength) freq = 1. / freq;
			S->data[0][k] = freq;
			S->data[1][k] = z_top_or_bot[k];
		}
		if (GMT_Write_Data (GMT->parent, GMT_IS_DATASET, GMT_IS_FILE, GMT_IS_NONE, GMT_WRITE_SET, NULL, Ctrl->G.file, D) != GMT_NOERROR)
			Return (API->error);

		gmt_M_free (GMT, K);
		gmt_M_free (GMT, z_top_or_bot);
		Return (GMT_NOERROR);
	}
	/* ---------------------------------------------------------------------------------- */

	if (!Ctrl->Q.active) {
		if (((Ctrl->T.active && !Ctrl->T.moho) && Ctrl->E.n_terms > 1) || (Ctrl->S.active && Ctrl->E.n_terms > 1)) {
			GMT_Report (API, GMT_MSG_WARNING, "Due to a bug, or a method limitation (I didn't figure that yet)\n" \
				"with the selected options, the number of terms in Parker expansion is reset to one\n" \
				"See examples in the manual if you want to compute with higher order expansion\n\n");
			Ctrl->E.n_terms = 1;
		}
	}

	GMT_Report (API, GMT_MSG_INFORMATION, "Allocates memory and read data file\n");

	for (k = 0; k < Ctrl->In.n_grids; k++) {	/* First read the grid header(s) */
		if ((Orig[k] = GMT_Read_Data (API, GMT_IS_GRID, GMT_IS_FILE, GMT_IS_SURFACE, GMT_CONTAINER_ONLY, NULL, Ctrl->In.file[k], NULL)) == NULL)
			Return (API->error);
	}

	if (Ctrl->In.n_grids == 2) {	/* If given 2 grids, make sure they are co-registered and has same size, registration, etc. */
		if (!gmt_grd_domains_match (GMT, Orig[0], Orig[1], NULL)) {
			Return (GMT_RUNTIME_ERROR);
		}
	}

	if (Ctrl->D.variable) {	/* Read density contrast grid */
		if ((Rho = GMT_Read_Data (API, GMT_IS_GRID, GMT_IS_FILE, GMT_IS_SURFACE, GMT_CONTAINER_AND_DATA | GMT_GRID_IS_COMPLEX_REAL, NULL, Ctrl->D.file, NULL)) == NULL)
			Return (API->error);
		if (!gmt_grd_domains_match (GMT, Orig[0], Rho, "surface and density")) {
			Return (GMT_RUNTIME_ERROR);
		}
		HH = gmt_get_H_hidden (Rho->header);
		if (HH->has_NaNs == GMT_GRID_HAS_NANS) {
			uint64_t n_nan = 0;
			for (m = 0; m < Rho->header->size; m++) if (gmt_M_is_fnan (Rho->data[m])) Rho->data[m] = Rho->header->z_min, n_nan++;	/* Replace any NaNs with the minimum density */
			GMT_Report (API, GMT_MSG_INFORMATION, "Density grid %s had %" PRIu64 " NaNs; these have been replaced by the minimum density %lg\n", Ctrl->In.file[k], n_nan, Rho->header->z_min);
		}
	}

	/* Grids are compatible. Initialize FFT structs, grid headers, read data, and check for NaNs */

	for (k = 0; k < Ctrl->In.n_grids; k++) {	/* Read, and check that no NaNs are present in either grid */
		gmt_grd_init (GMT, Orig[k]->header, options, true);	/* Update the header */
		if ((Orig[k] = GMT_Read_Data (API, GMT_IS_GRID, GMT_IS_FILE, GMT_IS_SURFACE, GMT_DATA_ONLY |
                                      GMT_GRID_IS_COMPLEX_REAL, NULL, Ctrl->In.file[k], Orig[k])) == NULL)	/* Get data only */
			Return (API->error);
		HH = gmt_get_H_hidden (Orig[k]->header);
		/* Note: If input grid(s) are read-only then we must duplicate them; otherwise Grid[k] points to Orig[k] */
		(void) gmt_set_outgrid (GMT, Ctrl->In.file[k], false, 0, Orig[k], &Grid[k]);
		if (HH->has_NaNs == GMT_GRID_HAS_NANS) {
			uint64_t n_nan = 0;
			for (m = 0; m < Grid[k]->header->size; m++) if (gmt_M_is_fnan (Grid[k]->data[m])) Grid[k]->data[m] = 0.0, n_nan++;	/* Replace any NaNs with zero */
			GMT_Report (API, GMT_MSG_INFORMATION, "Input grid %s had %" PRIu64 " NaNs; these have been replaced with 0\n", Ctrl->In.file[k], n_nan);
		}
	}

	/* From here we address the first grid via Grid[0] and the 2nd grid (if given) as Grid[1];
	 * we are done with using the addresses Orig[k] directly. */

	if (Ctrl->W.active) {	/* Need to adjust for a different observation level relative to topo.grd */
		openmp_int row, col;
		GMT_Report (API, GMT_MSG_INFORMATION, "Remove %g m from topography grid %s\n", Ctrl->W.water_depth, Ctrl->In.file[0]);
		gmt_M_grd_loop (GMT, Grid[0], row, col, m)
			Grid[0]->data[m] -= (gmt_grdfloat)Ctrl->W.water_depth;
		Grid[0]->header->z_min -= (gmt_grdfloat)Ctrl->W.water_depth;
		Grid[0]->header->z_max -= (gmt_grdfloat)Ctrl->W.water_depth;
	}

	for (k = 0; k < Ctrl->In.n_grids; k++) {	/* Read, and check that no NaNs are present in either grid */
		FFT_info[k] = GMT_FFT_Create (API, Grid[k], GMT_FFT_DIM, GMT_GRID_IS_COMPLEX_REAL, Ctrl->N.info);	/* Also detrends, if requested */
	}

	if (Ctrl->D.variable) {	/* No detrending, please */
		int was = Ctrl->N.info->trend_mode;	/* Record what trendmode we had for topography */
		Ctrl->N.info->trend_mode = GMT_FFT_REMOVE_NOTHING;	/* Temporarily set to no removal */
		Rho_info = GMT_FFT_Create (API, Rho, GMT_FFT_DIM, GMT_GRID_IS_COMPLEX_REAL, Ctrl->N.info);
		Ctrl->N.info->trend_mode = was;	/* Restore what we had */
	}

	K = FFT_info[0];	/* We only need one of these anyway; K is a shorthand */

	Ctrl->misc.z_level = fabs (FFT_info[0]->coeff[0]);	/* Need absolute value or level removed for uppward continuation */
	GMT_Report (API, GMT_MSG_INFORMATION, "Level used for upward continuation: %g\n", Ctrl->misc.z_level);

	if (Ctrl->I.active) {		/* Compute admittance or coherence from data and exit */

		GMT_Report (API, GMT_MSG_INFORMATION, "Processing gravity file %s\n", Ctrl->In.file[1]);

		for (k = 0; k < Ctrl->In.n_grids; k++) {	/* Call the forward FFT, once per grid */
			GMT_Report (API, GMT_MSG_INFORMATION, "forward FFT...\n");
			if (GMT_FFT (API, Grid[k], GMT_FFT_FWD, GMT_FFT_COMPLEX, FFT_info[k]))
				Return (GMT_RUNTIME_ERROR);
		}

		error = gravfft_do_admittance (GMT, Grid[0], Grid[1], Ctrl, K);
		for (k = 0; k < Ctrl->In.n_grids; k++)
			GMT_FFT_Destroy (API, &(FFT_info[k]));

		if (!error) {
			Return (GMT_NOERROR);
		}
		else {
			Return (error);
		}
	}

	topo   = gmt_M_memory (GMT, NULL, Grid[0]->header->size, gmt_grdfloat);
	raised = gmt_M_memory (GMT, NULL, Grid[0]->header->size, gmt_grdfloat);

	if (Ctrl->Q.active || Ctrl->T.moho) {
		double coeff[3];
		GMT_Report (API, GMT_MSG_INFORMATION, "Forward FFT...\n");
		if (GMT_FFT (API, Grid[0], GMT_FFT_FWD, GMT_FFT_COMPLEX, FFT_info[0])) {
			Return (GMT_RUNTIME_ERROR);
		}

		gravfft_do_isostasy (GMT, Grid[0], Ctrl, K);

		GMT_Report (API, GMT_MSG_INFORMATION, "Inverse FFT...\n");
		if (GMT_FFT (API, Grid[0], GMT_FFT_INV, GMT_FFT_COMPLEX, K))
			Return (GMT_RUNTIME_ERROR);

		if (!doubleAlmostEqual (scale_out, 1.0))
			gmt_scale_and_offset_f (GMT, Grid[0]->data, Grid[0]->header->size, scale_out, 0);

		if (!Ctrl->T.moho) {

			if (false && Ctrl->N.info->trend_mode != GMT_FFT_REMOVE_MEAN) { /* Account also for the difference between detrend level and true mean  */
				gmt_grd_detrend (GMT, Grid[0], GMT_FFT_REMOVE_MEAN, coeff);
				gmt_scale_and_offset_f (GMT, Grid[0]->data, Grid[0]->header->size, 1.0, -Ctrl->Z.zm);
			}
			else
				gmt_scale_and_offset_f (GMT, Grid[0]->data, Grid[0]->header->size, 1.0, -Ctrl->Z.zm);

			/* The data are in the middle of the padded array; only the interior (original dimensions) will be written to file */
			if (GMT_Set_Comment (API, GMT_IS_GRID, GMT_COMMENT_IS_OPTION | GMT_COMMENT_IS_COMMAND, options, Grid[0]))
				Return (API->error);
			if (GMT_Write_Data (API, GMT_IS_GRID, GMT_IS_FILE, GMT_IS_SURFACE, GMT_DATA_ONLY,
                                NULL, Ctrl->G.file, Grid[0]) != GMT_NOERROR) {
				Return (API->error);
			}
			GMT_FFT_Destroy (API, &(FFT_info[0]));
			gmt_M_free (GMT, topo);
			gmt_M_free (GMT, raised);
			Return (GMT_NOERROR);
		}
		else {
			Ctrl->misc.z_level = Ctrl->Z.zm;
			Ctrl->misc.rho = Ctrl->T.rho_mc;
			scale_out = 1.0;
		}
	}

	gmt_M_memcpy (topo, Grid[0]->data, Grid[0]->header->size, gmt_grdfloat);
	/* Manually interleave this copy of topo [and hence raised] since we will call FFT repeatedly */
	gmt_grd_mux_demux (API->GMT, Grid[0]->header, topo, GMT_GRID_IS_INTERLEAVED);
	if (Ctrl->D.variable) {	/* Also interleave manually the rho grid */
		gmt_grd_mux_demux (API->GMT, Rho->header, Rho->data, GMT_GRID_IS_INTERLEAVED);
		for (m = 0; m < Grid[0]->header->size; m++)
			raised[m] = topo[m] * Rho->data[m];
	}
	else	/* Constant density contrast passed into gravfft_do_parker */
		gmt_M_memcpy (raised,  topo, Grid[0]->header->size, gmt_grdfloat);

	gmt_M_memset (Grid[0]->data, Grid[0]->header->size, gmt_grdfloat);

	for (n = 1; n <= Ctrl->E.n_terms; n++) {

		GMT_Report (API, GMT_MSG_INFORMATION, "Evaluating Parker for term = %d\n", n);

		if (n > 1)	/* n == 1 was initialized via the gmt_M_memcpy or loop above */
			for (m = 0; m < Grid[0]->header->size; m++) {
				raised[m] = (gmt_grdfloat)pow(topo[m], (double)n);
				if (Ctrl->D.variable) raised[m] *= Rho->data[m];
			}

		if (GMT_FFT_2D (API, raised, K->nx2, K->ny2, GMT_FFT_FWD, GMT_FFT_COMPLEX)) {
			gmt_M_free (GMT, raised);	gmt_M_free (GMT, topo);
			Return (GMT_RUNTIME_ERROR);
		}

		if (Ctrl->D.active || Ctrl->T.moho)	/* "classical" anomaly */
			gravfft_do_parker (GMT, Grid[0], Ctrl, K, raised, n, Ctrl->misc.rho);
		else if (Ctrl->T.active && !Ctrl->S.active)		/* Compute "loading from top" grav|geoid */
			gravfft_load_from_top_grid (GMT, Grid[0], Ctrl, K, raised, n);
		else if (Ctrl->S.active)				/* Compute "loading from below" grav|geoid */
			gravfft_load_from_below_grid (GMT, Grid[0], Ctrl, K, raised, n);
		else
			GMT_Report (API, GMT_MSG_ERROR, "It SHOULDN'T pass here\n");
	}

	if (GMT_FFT (API, Grid[0], GMT_FFT_INV, GMT_FFT_COMPLEX, K)) {
		gmt_M_free (GMT, raised);	gmt_M_free (GMT, topo);
		Return (GMT_RUNTIME_ERROR);
	}

	if (!doubleAlmostEqual (scale_out, 1.0))
		gmt_scale_and_offset_f (GMT, Grid[0]->data, Grid[0]->header->size, scale_out, 0);

	switch (Ctrl->F.mode) {
		case GRAVFFT_FAA:
			strcpy (Grid[0]->header->title, "Gravity anomalies");
			strcpy (Grid[0]->header->z_units, "mGal");
			if (Ctrl->F.slab) {	/* Do the slab adjustment */
				if (Ctrl->D.variable) Ctrl->misc.rho = Rho->header->z_min;
				slab_gravity = (gmt_grdfloat) (1.0e5 * 2 * M_PI * Ctrl->misc.rho * NEWTON_G *
				                        (Ctrl->W.water_depth - Ctrl->misc.z_level));
				GMT_Report (API, GMT_MSG_INFORMATION, "Add %g mGal to predicted FAA grid to account for implied slab\n", slab_gravity);
				if (Ctrl->F.bouguer)		/* The complete Bouguer contribution */
					for (m = 0; m < Grid[0]->header->size; m++) Grid[0]->data[m] = slab_gravity - Grid[0]->data[m];
				else
					for (m = 0; m < Grid[0]->header->size; m++) Grid[0]->data[m] += slab_gravity;
			}
			else if (Ctrl->F.zero) {	/* Shift so average FAA at the four corners is zero */
				double far_field = 0.25 * (Grid[0]->data[gmt_M_ijp(Grid[0]->header, 0, 0)] + Grid[0]->data[gmt_M_ijp(Grid[0]->header, Grid[0]->header->n_rows-1, 0)]
					+ Grid[0]->data[gmt_M_ijp(Grid[0]->header, 0, Grid[0]->header->n_columns-1)] + Grid[0]->data[gmt_M_ijp(Grid[0]->header, Grid[0]->header->n_rows-1, Grid[0]->header->n_columns-1)]);
				GMT_Report (API, GMT_MSG_INFORMATION, "Subtract %g mGal from predicted FAA grid to force far-field to be zero\n", far_field);
					for (m = 0; m < Grid[0]->header->size; m++) Grid[0]->data[m] -= far_field;
			}
			break;
		case GRAVFFT_GEOID:
			strcpy (Grid[0]->header->title, "Geoid anomalies");
			strcpy (Grid[0]->header->z_units, "meter");
			break;
		case GRAVFFT_VGG:
			strcpy (Grid[0]->header->title, "Vertical Gravity Gradient anomalies");
			strcpy (Grid[0]->header->z_units, "Eotvos");
			break;
		case GRAVFFT_DEFL_EAST:
			strcpy (Grid[0]->header->title, "Deflection of the vertical - East");
			strcpy (Grid[0]->header->z_units, "microradian");
			break;
		case GRAVFFT_DEFL_NORTH:
			strcpy (Grid[0]->header->title, "Deflection of the vertical - North");
			strcpy (Grid[0]->header->z_units, "microradian");
			break;
	}

	snprintf (Grid[0]->header->remark, GMT_GRID_REMARK_LEN160, "Parker expansion of order %d", Ctrl->E.n_terms);

	GMT_Report (API, GMT_MSG_INFORMATION, "Write Output...\n");

	for (k = 0; k < Ctrl->In.n_grids; k++)
		GMT_FFT_Destroy (API, &(FFT_info[k]));
	if (Ctrl->D.variable)
		GMT_FFT_Destroy (API, &Rho_info);
	gmt_M_free (GMT, raised);

	if (GMT_Set_Comment (API, GMT_IS_GRID, GMT_COMMENT_IS_OPTION | GMT_COMMENT_IS_COMMAND, options, Grid[0])) {
		gmt_M_free (GMT, topo);
		Return (API->error);
	}
	if (GMT_Write_Data (API, GMT_IS_GRID, GMT_IS_FILE, GMT_IS_SURFACE, GMT_DATA_ONLY,
                        NULL, Ctrl->G.file, Grid[0]) != GMT_NOERROR) {
		gmt_M_free (GMT, topo);
		Return (API->error);
	}

	gmt_M_free (GMT, topo);

	Return (GMT_NOERROR);
}

GMT_LOCAL void gravfft_do_isostasy (struct GMT_CTRL *GMT, struct GMT_GRID *Grid, struct GRAVFFT_CTRL *Ctrl, struct GMT_FFT_WAVENUMBER *K) {
	/* Do the isostatic response function convolution in the Freq domain.
	All units assumed to be in SI (that is kx, ky, modk wavenumbers in m**-1,
	densities in kg/m**3, Te in m, etc.
	rw, the water density, is used to set the Airy ratio and the restoring
	force on the plate (rm - ri)*gravity if ri = rw; so use zero for topo in air (ri changed to rl).  */
	uint64_t k;
	double  A = 1.0, rho_load, airy_ratio, rigidity_d, d_over_restoring_force, mk, k2, k4, transfer_fn;
	gmt_grdfloat *datac = Grid->data;
	gmt_M_unused(GMT);

	/*   te	 Elastic thickness, SI units (m)  */
	/*   rl	 Load density, SI units  */
	/*   rm	 Mantle density, SI units  */
	/*   rw	 Water density, SI units  */
	/*   [ri Infill density, SI units]  */
	/* If infill density was specified then Ctrl->T.approx will be true, in which case we will do the
	 * approximate FFT solution of Wessel [2001, JGR]: Use rhoi instead of rhol to determine flexural wavelength
	 * and and amplitudes but scale airy_ratio by A to compensate for the lower load weight */

	rho_load = Ctrl->T.rhol;
	if (Ctrl->T.approx) {	/* Do approximate calculation when both rhol and rhoi were set */
		char way = (Ctrl->T.rhoi < Ctrl->T.rhol) ? '<' : '>';
		GMT_Report (GMT->parent, GMT_MSG_INFORMATION, "Approximate FFT-solution to flexure since rho_i (%g) %c rho_l (%g)\n", Ctrl->T.rhoi, way, Ctrl->T.rhol);
		rho_load = Ctrl->T.rhoi;
		A = sqrt ((Ctrl->T.rhom - Ctrl->T.rhoi)/(Ctrl->T.rhom - Ctrl->T.rhol));
	}
	GMT_Report (GMT->parent, GMT_MSG_DEBUG, "Using effective load density rho_l = %g and Airy boost factor A = %g\n", rho_load, A);
	rigidity_d = (YOUNGS_MODULUS * Ctrl->T.te * Ctrl->T.te * Ctrl->T.te) / (12.0 * (1.0 - POISSONS_RATIO * POISSONS_RATIO));
	d_over_restoring_force = rigidity_d / ( (Ctrl->T.rhom - rho_load) * Ctrl->M.normal_gravity);
	airy_ratio = -A * (rho_load - Ctrl->T.rhow)/(Ctrl->T.rhom - rho_load);

	if (Ctrl->T.te == 0.0) {      /* Airy isostasy; scale global variable scale_out and return */
		scale_out *= airy_ratio;
		return;
	}

	for (k = 0; k < Grid->header->size; k+= 2) {
		mk = gmt_fft_get_wave (k, K);
		k2 = mk * mk;	k4 = k2 * k2;
		transfer_fn = airy_ratio / ( (d_over_restoring_force * k4) + 1.0);
		datac[k] *= (gmt_grdfloat)transfer_fn;
		datac[k+1] *= (gmt_grdfloat)transfer_fn;
	}
}

GMT_LOCAL void gravfft_do_parker (struct GMT_CTRL *GMT, struct GMT_GRID *Grid, struct GRAVFFT_CTRL *Ctrl, struct GMT_FFT_WAVENUMBER *K, gmt_grdfloat *raised, uint64_t n, double rho) {
	uint64_t i, k;
	double f, p, t, mk, kx, ky, v, c;
	gmt_grdfloat *datac = Grid->data;
	gmt_M_unused(GMT);

	f = 1.0;
	for (i = 2; i <= n; i++) f *= i;	/* n! */
	p = n - 1.0;

	c = 1.0e5 * 2.0 * M_PI * NEWTON_G * rho / f; /* Gives mGal */

	for (k = 0; k < Grid->header->size; k+= 2) {
		mk = gmt_fft_get_wave (k, K);
		if (p == 0.0)
			t = 1.0;
		else if (p == 1.0)
			t = mk;
		else
			t = pow (mk, p);

		v = c * exp (-mk * Ctrl->misc.z_level) * t;
		switch (Ctrl->F.mode) {
			case GRAVFFT_FAA:
				datac[k]   += (gmt_grdfloat) (v * raised[k]);
				datac[k+1] += (gmt_grdfloat) (v * raised[k+1]);
				break;
			case GRAVFFT_GEOID:
				if (mk > 0.0) v /= (Ctrl->M.mgal_at_45 * mk);
				datac[k]   += (gmt_grdfloat) (v * raised[k]);
				datac[k+1] += (gmt_grdfloat) (v * raised[k+1]);
				break;
			case GRAVFFT_VGG:
			 	v *= 1.0e4 * mk;	/* Scale mGal/m to 0.1 mGal/km = 1 Eotvos */
				datac[k]   += (gmt_grdfloat) (v * raised[k]);
				datac[k+1] += (gmt_grdfloat) (v * raised[k+1]);
				break;
			case GRAVFFT_DEFL_EAST:
				if (mk > 0.0) {	/* Scale tan (xslope) ~ slope to microradians */
					kx = gmt_fft_any_wave (k, GMT_FFT_K_IS_KX, K);
					v *= 1.e6 * (-kx / (Ctrl->M.mgal_at_45 * mk));
				}
				datac[k]   += (gmt_grdfloat) (-v * raised[k+1]);
				datac[k+1] += (gmt_grdfloat) ( v * raised[k]);
				break;
			case GRAVFFT_DEFL_NORTH:
				if (mk > 0.0) {	/* Scale tan (yslope) ~ slope to microradians */
					ky = gmt_fft_any_wave (k, GMT_FFT_K_IS_KY, K);
					v *= 1.e6 * (-ky / (Ctrl->M.mgal_at_45 * mk));
				}
				datac[k]   += (gmt_grdfloat) ( v * raised[k+1]);
				datac[k+1] += (gmt_grdfloat) (-v * raised[k]);
				break;
		}
	}
}

GMT_LOCAL int gravfft_do_admittance (struct GMT_CTRL *GMT, struct GMT_GRID *GridA, struct GMT_GRID *GridB, struct GRAVFFT_CTRL *Ctrl, struct GMT_FFT_WAVENUMBER *K) {
	/*	The following are the comments of the routine do_spectrum
	 *	of grdfft from which this code was adapted.
	 *
	 *	"This is modeled on the 1-D case, using the following ideas:
	 *	In 1-D, we ensemble average over samples of length L =
	 *	n * dt.  This gives n/2 power spectral estimates, at
	 *	frequencies i/L, where i = 1, n/2.  If we have a total
	 *	data set of ndata, we can make nest=ndata/n independent
	 *	estimates of the power.  Our standard error is then
	 *	1/sqrt(nest).
	 *	In making 1-D estimates from 2-D data, we may choose
	 *	n and L from nx2 or ny2 and delta_kx, delta_ky as
	 *	appropriate.  In this routine, we are giving the sum over
	 * 	all frequencies in the other dimension; that is, an
	 *	approximation of the integral."
	 */

	int      error = 0, n_col_out = 3, col;
	uint64_t dim[GMT_DIM_SIZE] = {1, 1, 0, 0};	/* One table and one segment, with either 1 + 1*2 = 3 or 1 + 8*2 = 17 columns and yet unknown rows */
	uint64_t k, k_0 = 0, nk, ifreq;
	unsigned int *nused = NULL;
	size_t   n_alloc;
	double   delta_k, r_delta_k, freq;
	double  *out = NULL, *err_bar = NULL, *coh = NULL, *b_pow = NULL, *g_pow = NULL, *co_spec = NULL, *quad = NULL;
	gmt_grdfloat   *datac = GridA->data;
	gmt_grdfloat   *in_grv = GridB->data;
	double  *z_from_below = NULL, *z_from_top = NULL;
	struct   GMT_DATASET *D = NULL;
	struct   GMT_DATASEGMENT *S = NULL;

	if (K->delta_kx < K->delta_ky)
		{delta_k = K->delta_kx;	nk = K->nx2/2;}
	else
		{delta_k = K->delta_ky;	nk = K->ny2/2;}
	n_alloc = nk;
	/* Get an array for summing stuff */
	b_pow   = gmt_M_memory (GMT, NULL, n_alloc, double );
	g_pow   = gmt_M_memory (GMT, NULL, n_alloc, double);
	err_bar = gmt_M_memory (GMT, NULL, n_alloc, double);
	co_spec = gmt_M_memory (GMT, NULL, n_alloc, double);
	quad    = gmt_M_memory (GMT, NULL, n_alloc, double);
	coh     = gmt_M_memory (GMT, NULL, n_alloc, double);
	out     = gmt_M_memory (GMT, NULL, n_alloc, double);
	if (Ctrl->misc.from_below)
		z_from_below = gmt_M_memory (GMT, NULL, n_alloc, double);
	if (Ctrl->misc.from_top)
		z_from_top = gmt_M_memory (GMT, NULL, n_alloc, double);
	n_alloc   = K->nx2 * K->ny2;
	nused   = gmt_M_memory (GMT, NULL, n_alloc, unsigned int);

	if (Ctrl->misc.coherence)
		Ctrl->I.active = false;

	/* Loop over it all, summing and storing, checking range for r */

	r_delta_k = 1.0 / delta_k;

	for (k = 2; k < GridA->header->size; k+= 2) {
		freq = gmt_fft_get_wave (k, K);
		ifreq = lrint (fabs(freq) * r_delta_k);	/* Might be zero when doing r average  */
		if (ifreq) ifreq--;
		if (ifreq >= nk) continue;	/* Might happen when doing r average  */
		b_pow[ifreq] += (datac[k]*datac[k] + datac[k+1]*datac[k+1]); /* B*B-conj = bat power */
		g_pow[ifreq] += (in_grv[k]*in_grv[k] + in_grv[k+1]*in_grv[k+1]); /* G*G-conj = grav power */
		co_spec[ifreq] += (in_grv[k]*datac[k] + in_grv[k+1]*datac[k+1]); /* keep only real of G*B-conj */
		quad[ifreq] += (datac[k+1]*in_grv[k] - in_grv[k+1]*datac[k]);
		nused[ifreq]++;
	}

	for (k = k_0; k < nk; k++)	/* Coherence is always needed for error bar computing */
		coh[k] = (co_spec[k]*co_spec[k] + quad[k]*quad[k]) / (b_pow[k]*g_pow[k]);

	for (k = k_0; k < nk; k++) {
		if (Ctrl->I.active) {
			out[k] = co_spec[k] / b_pow[k];
			/*err_bar[k] = out[k] * fabs (sqrt ((1.0 - coh[k]) / 2.0 * coh[k]) * out[k]) / sqrt(nused[k]); Versao do Smith*/
			err_bar[k] = out[k] * fabs (sqrt ((1.0 - coh[k]) / (2.0 * coh[k] * nused[k])) );
		}
		else if (Ctrl->misc.coherence) {
			out[k] = coh[k];
			err_bar[k] = out[k] * (1.0 - coh[k]) * sqrt(2.0 / coh[k]) / sqrt(nused[k]);
		}
	}

	/* Now get here when array is summed.  */
	delta_k /= (2.0*M_PI);				/* Write out frequency, not wavenumber  */
	if (Ctrl->misc.from_below) 		/* compute theoretical "load from below" admittance */
		gravfft_load_from_below_admitt(GMT, Ctrl, K, z_from_below);
	if (Ctrl->misc.from_top) 		/* compute theoretical "load from top" admittance */
		gravfft_load_from_top_admitt(GMT, Ctrl, K, z_from_top);

	gmt_set_cartesian (GMT, GMT_OUT);	/* To counter-act any -fg setting */

	n_col_out = (Ctrl->misc.from_below || Ctrl->misc.from_top) ? 4 : 3;
	dim[GMT_COL] = n_col_out;
	dim[GMT_ROW] = nk;
	if ((D = GMT_Create_Data (GMT->parent, GMT_IS_DATASET, GMT_IS_NONE, 0, dim, NULL, NULL, 0, 0, NULL)) == NULL) {
		GMT_Report (GMT->parent, GMT_MSG_ERROR, "Unable to create a data set for spectral estimates\n");
		error = GMT->parent->error;
		goto Lfree;		/* So that we can free what it need to be */
	}
	S = D->table[0]->segment[0];	/* Only one table with one segment here, with 17 cols and nk rows */
	S->n_rows = nk;

	if (Ctrl->misc.give_wavelength && Ctrl->misc.give_km) delta_k *= 1000.0;	/* Wanted wavelength in km */

	for (k = k_0; k < nk; k++) {
		freq = (k + 1) * delta_k;
		if (Ctrl->misc.give_wavelength) freq = 1.0/freq;

		col = 0;
		/* Col 0 is the frequency (or wavelength) */
		S->data[col++][k] = freq;
		/* Cols 1-2 are xpower and std.err estimate */
		S->data[col++][k] = out[k];
		S->data[col++][k] = err_bar[k];
		if (Ctrl->misc.from_below)
			S->data[col++][k] = z_from_below[k];
		else if (Ctrl->misc.from_top)
			S->data[col++][k] = z_from_top[k];
	}
	if (GMT_Write_Data (GMT->parent, GMT_IS_DATASET, GMT_IS_FILE, GMT_IS_NONE, GMT_WRITE_SET, NULL, Ctrl->G.file, D) != GMT_NOERROR)
		error = GMT->parent->error;

Lfree:
	gmt_M_free (GMT, out);
	gmt_M_free (GMT, b_pow);
	gmt_M_free (GMT, g_pow);
	gmt_M_free (GMT, err_bar);
	gmt_M_free (GMT, co_spec);
	gmt_M_free (GMT, coh);
	gmt_M_free (GMT, quad);
	gmt_M_free (GMT, nused);
	if (Ctrl->misc.from_below) gmt_M_free (GMT, z_from_below);
	if (Ctrl->misc.from_top) gmt_M_free (GMT, z_from_top);

	return error;
}

GMT_LOCAL void gravfft_compute_only_admitts(struct GMT_CTRL *GMT, struct GRAVFFT_CTRL *Ctrl, struct GMT_FFT_WAVENUMBER *K, double *z_top_or_bot, double delta_pt) {

	/* Calls the appropriate function to compute the theoretical admittance. */
	K->delta_kx = K->delta_ky = delta_pt;
	K->nx2 = K->ny2 = Ctrl->C.n_pt * 2;

	if (Ctrl->misc.from_top)
		gravfft_load_from_top_admitt(GMT, Ctrl, K, z_top_or_bot);
	else
		gravfft_load_from_below_admitt(GMT, Ctrl, K, z_top_or_bot);
}

GMT_LOCAL void gravfft_load_from_below_admitt(struct GMT_CTRL *GMT, struct GRAVFFT_CTRL *Ctrl, struct GMT_FFT_WAVENUMBER *K, double *z_from_below) {

	/* Compute theoretical admittance for the "loading from below" model
	   M. McNutt & Shure (1986) in same |k| of computed data admittance

	   The z_from_below is a vector that must have been previously allocated
	   with a size of "nk" like that variable is computed here.	*/

	unsigned int k, nk;
	double	earth_curvature, alfa, delta_k, freq, D, t1, t2, t3;
	gmt_M_unused(GMT);

	if (K->delta_kx < K->delta_ky)
		{delta_k = K->delta_kx;	 nk = K->nx2/2;}
	else
		{delta_k = K->delta_ky;	 nk = K->ny2/2;}

	delta_k /= TWO_PI;	/* Use frequency, not wavenumber  */
	D = (YOUNGS_MODULUS * Ctrl->T.te * Ctrl->T.te * Ctrl->T.te) / (12.0 * (1.0 - POISSONS_RATIO * POISSONS_RATIO));
	alfa = pow (TWO_PI, 4.) * D / (Ctrl->M.normal_gravity * Ctrl->T.rho_mc);

	for (k = 0; k < nk; k++) {
		freq = (k + 1) * delta_k;
		earth_curvature = (sphericity) ? (2 * earth_rad * freq) / (4 * M_PI * earth_rad * freq + 1) : 1.;
		t1 = earth_curvature * (TWO_PI * NEWTON_G);
		if (Ctrl->F.mode == GRAVFFT_FAA)
			t1 *= 1.0e5;     /* to have it in mGals */
		else                 /* Must be the GEOID case */
			t1 /= (Ctrl->M.normal_gravity * freq * TWO_PI);
		t2 = Ctrl->T.rho_cw * exp(-TWO_PI * freq * Ctrl->misc.z_level) +
			Ctrl->T.rho_mc * exp(-TWO_PI * freq * Ctrl->Z.zm);
		t3 = -(Ctrl->T.rho_mw + Ctrl->T.rho_mc * pow(freq,4.) * alfa) * exp(-TWO_PI * freq * Ctrl->Z.zl);
		z_from_below[k] = t1 * (t2 + t3);
	}
}

GMT_LOCAL void gravfft_load_from_top_admitt (struct GMT_CTRL *GMT, struct GRAVFFT_CTRL *Ctrl, struct GMT_FFT_WAVENUMBER *K, double *z_from_top) {

	/* Compute theoretical admittance for the "loading from top" model
	   M. McNutt & Shure (1986) in same |k| of computed data admittance

	   The z_from_top is a vector that must have been previously allocated
	   with a size of "nk" like that variable is computed here.	*/

	unsigned int k, nk;
	double	earth_curvature, alfa, delta_k, freq, D, t1, t2;
	gmt_M_unused(GMT);

	if (K->delta_kx < K->delta_ky)
		{delta_k = K->delta_kx;	 nk = K->nx2/2;}
	else
		{delta_k = K->delta_ky;	 nk = K->ny2/2;}

	delta_k /= TWO_PI;	/* Use frequency, not wavenumber  */
	D = (YOUNGS_MODULUS * Ctrl->T.te * Ctrl->T.te * Ctrl->T.te) / (12.0 * (1.0 - POISSONS_RATIO * POISSONS_RATIO));
	alfa = pow (TWO_PI,4.) * D / (Ctrl->M.normal_gravity * Ctrl->T.rho_mc);

	for (k = 0; k < nk; k++) {
		freq = (k + 1) * delta_k;
		earth_curvature = (sphericity) ? (2 * earth_rad * freq) / (4 * M_PI * earth_rad * freq + 1) : 1.;
		t1 = earth_curvature * (TWO_PI * NEWTON_G);
		if (Ctrl->F.mode == GRAVFFT_FAA)
			t1 *= 1.0e5;     /* to have it in mGals */
		else                 /* Must be the GEOID case */
			t1 /= (Ctrl->M.normal_gravity * freq * TWO_PI);
		t2 = exp(-TWO_PI * freq * Ctrl->misc.z_level) - exp(-TWO_PI * freq * Ctrl->Z.zm) / (1 + alfa*pow(freq,4.));
		z_from_top[k] = t1 * Ctrl->T.rho_cw * t2;
	}
}

GMT_LOCAL void gravfft_load_from_top_grid (struct GMT_CTRL *GMT, struct GMT_GRID *Grid, struct GRAVFFT_CTRL *Ctrl, struct GMT_FFT_WAVENUMBER *K, gmt_grdfloat *raised, unsigned int n) {

	/* Computes the gravity|geoid grid due to the effect of the bathymetry using the theoretical
	admittance for the "loading from top" model --  M. McNutt & Shure (1986)  */

	unsigned int i;
	uint64_t k;
	double	earth_curvature, alfa, D, t1, t2, f, p, t, mk;
	gmt_grdfloat *datac = Grid->data;
	gmt_M_unused(GMT);

	f = 1.0;
	for (i = 2; i <= n; i++)
		f *= i;	/* n! */
	p = n - 1.0;

	D = (YOUNGS_MODULUS * Ctrl->T.te * Ctrl->T.te * Ctrl->T.te) / (12.0 * (1.0 - POISSONS_RATIO * POISSONS_RATIO));
	alfa = pow(TWO_PI,4.) * D / (Ctrl->M.normal_gravity * Ctrl->T.rho_mc);
	raised[0] = 0.0f;		raised[1] = 0.0f;

	for (k = 0; k < Grid->header->size; k+= 2) {
		mk = gmt_fft_get_wave (k, K) / TWO_PI;
		if (p == 0.0)
			t = 1.0;
		else if (p == 1.0)
			t = mk;
		else
			t = pow (mk, p);
		earth_curvature = (sphericity) ? (2 * earth_rad * mk) / (4 * M_PI * earth_rad * mk + 1) : 1.;
		t1 = earth_curvature * (TWO_PI * NEWTON_G);
		t2 = exp(-TWO_PI * mk * Ctrl->misc.z_level) - exp(-TWO_PI * mk * Ctrl->Z.zm) / (1 + alfa*pow(mk,4.));

		/* The switch below is probably only correct for FAA and possibly GEOID.
		 * THose where the only two cases covered by JL's code */

		switch (Ctrl->F.mode) {
			case GRAVFFT_FAA:
				t1 *= 1.0e5;     /* To have it in mGals */
				datac[k]   += (gmt_grdfloat) ((Ctrl->T.rho_cw * t1 * t2) * t / f * raised[k]);
				datac[k+1] += (gmt_grdfloat) ((Ctrl->T.rho_cw * t1 * t2) * t / f * raised[k+1]);
				break;
			case GRAVFFT_GEOID:
				if (mk > 0.0) t1 /= (Ctrl->M.normal_gravity * mk * TWO_PI);
				datac[k]   += (gmt_grdfloat) ((Ctrl->T.rho_cw * t1 * t2) * t / f * raised[k]);
				datac[k+1] += (gmt_grdfloat) ((Ctrl->T.rho_cw * t1 * t2) * t / f * raised[k+1]);
				break;
			case GRAVFFT_VGG:
			 	t1 *= 1.0e4 * mk;	/* Scale mGal/m to 0.1 mGal/km = 1 Eotvos */
				datac[k]   += (gmt_grdfloat) ((Ctrl->T.rho_cw * t1 * t2) * t / f * raised[k]);
				datac[k+1] += (gmt_grdfloat) ((Ctrl->T.rho_cw * t1 * t2) * t / f * raised[k+1]);
				break;
			case GRAVFFT_DEFL_EAST:
				if (mk > 0.0) {	/* Scale tan (xslope) ~ slope to microradians */
					double kx = gmt_fft_any_wave (k, GMT_FFT_K_IS_KX, K);
					t1 *= 1.e6 * (-kx / (Ctrl->M.mgal_at_45 * mk));
				}
				datac[k]   += (gmt_grdfloat) (-(Ctrl->T.rho_cw * t1 * t2) * t / f * raised[k]);
				datac[k+1] += (gmt_grdfloat)  ((Ctrl->T.rho_cw * t1 * t2) * t / f * raised[k+1]);
				break;
			case GRAVFFT_DEFL_NORTH:
				if (mk > 0.0) {	/* Scale tan (yslope) ~ slope to microradians */
					double ky = gmt_fft_any_wave (k, GMT_FFT_K_IS_KY, K);
					t1 *= 1.e6 * (-ky / (Ctrl->M.mgal_at_45 * mk));
				}
				datac[k]   += (gmt_grdfloat)  ((Ctrl->T.rho_cw * t1 * t2) * t / f * raised[k]);
				datac[k+1] += (gmt_grdfloat) (-(Ctrl->T.rho_cw * t1 * t2) * t / f * raised[k+1]);
				break;
		}
	}
}

GMT_LOCAL void gravfft_load_from_below_grid (struct GMT_CTRL *GMT, struct GMT_GRID *Grid, struct GRAVFFT_CTRL *Ctrl, struct GMT_FFT_WAVENUMBER *K, gmt_grdfloat *raised, unsigned int n) {

	/* Computes the gravity|geoid grid due to the effect of the bathymetry using the theoretical
	admittance for the "loading from below" model --  M. McNutt & Shure (1986)  */

	unsigned int i;
	uint64_t k;
	double	earth_curvature, alfa, D, t1, t2, t3, f, p, t, mk;
	gmt_grdfloat *datac = Grid->data;
	gmt_M_unused(GMT);

	f = 1.0;
	for (i = 2; i <= n; i++)
		f *= i;	/* n! */
	p = n - 1.0;

	D = (YOUNGS_MODULUS * Ctrl->T.te * Ctrl->T.te * Ctrl->T.te) / (12.0 * (1.0 - POISSONS_RATIO * POISSONS_RATIO));
	alfa = pow(TWO_PI,4.) * D / (Ctrl->M.normal_gravity * Ctrl->T.rho_mc);
	raised[0] = 0.0f;		raised[1] = 0.0f;

	for (k = 0; k < Grid->header->size; k+= 2) {
		mk = gmt_fft_get_wave (k, K) / TWO_PI;
		if (p == 0.0)
			t = 1.0;
		else if (p == 1.0)
			t = mk;
		else
			t = pow (mk, p);
		earth_curvature = (sphericity) ? (2 * earth_rad * mk) / (4 * M_PI * earth_rad * mk + 1) : 1.;
		t1 = earth_curvature * (TWO_PI * NEWTON_G);
		if (Ctrl->F.mode == GRAVFFT_FAA)
			t1 *= 1.0e5;     /* to have it in mGals */
		else                 /* Must be the GEOID case */
			t1 /= (Ctrl->M.normal_gravity * mk * TWO_PI);
		t2 = Ctrl->T.rho_cw * exp(-TWO_PI * mk * Ctrl->misc.z_level) +
			 Ctrl->T.rho_mc * exp(-TWO_PI * mk * Ctrl->Z.zm);
		t3 = -(Ctrl->T.rho_mw + Ctrl->T.rho_mc * pow(mk,4.) * alfa) * exp(-TWO_PI * mk * Ctrl->Z.zl);
		datac[k] += (gmt_grdfloat) ((t1 * (t2 + t3)) * t / f * raised[k]);
		datac[k+1] += (gmt_grdfloat) ((t1 * (t2 + t3)) * t / f * raised[k+1]);
	}
}
