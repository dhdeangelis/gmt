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
 * gmt_constants.h contains definitions of constants used through GMT.
 *
 * Author:	Paul Wessel
 * Date:	01-OCT-2009
 * Version:	6 API
 */

/*!
 * \file gmt_constants.h
 * \brief Definitions of constants used through GMT.
 */

#ifndef GMT_CONSTANTS_H
#define GMT_CONSTANTS_H

/*=====================================================================================
 *	GMT API CONSTANTS DEFINITIONS
 *===================================================================================*/

#include "gmt_error_codes.h"			/* All API error codes are defined here */

/*--------------------------------------------------------------------
 *			GMT CONSTANTS MACRO DEFINITIONS
 *--------------------------------------------------------------------*/

#ifndef TWO_PI
#define TWO_PI        6.28318530717958647692
#endif
#ifndef M_PI
#define M_PI          3.14159265358979323846
#endif
#ifndef M_PI_2
#define M_PI_2          1.57079632679489661923
#endif
#ifndef M_PI_4
#define M_PI_4          0.78539816339744830962
#endif
#ifndef M_E
#define	M_E		2.7182818284590452354
#endif
#ifndef M_SQRT2
#define	M_SQRT2		1.41421356237309504880
#endif
#ifndef M_LN2_INV
#define	M_LN2_INV	(1.0 / 0.69314718055994530942)
#endif
#ifndef M_EULER
#define M_EULER		0.577215664901532860606512	/* Euler's constant (gamma) */
#endif
#ifndef M_PHI
#define M_PHI		1.618033988749894848204587	/* Golden ratio (phi) */
#endif
#define MAD_NORMALIZE	1.4826	/*  1/N^{-1}(0.75), where z = \N^{-1}(p) is the inverse cumulative normal distribution */

#define GMT_CONV15_LIMIT 1.0e-15	/* Very tight convergence limit or "close to zero" limit */
#define GMT_CONV12_LIMIT 1.0e-12	/* Tight limit for gaps/overlaps in CPT z-values */
#define GMT_CONV9_LIMIT	 1.0e-9		/* Fairly tight convergence limit or "close to zero" limit */
#define GMT_CONV8_LIMIT	 1.0e-8		/* Fairly tight convergence limit or "close to zero" limit */
#define GMT_CONV6_LIMIT	 1.0e-6		/* 1 ppm */
#define GMT_CONV5_LIMIT	 1.0e-5		/* 10 ppm */
#define GMT_CONV4_LIMIT	 1.0e-4		/* Less tight convergence limit or "close to zero" limit */

#define GMT_ASCII_ES	27	/* ASCII code for escape (used to prevent +? strings in plain text from being seen as modifiers) */
#define GMT_ASCII_GS	29	/* ASCII code for group separator (temporarily replacing tabs) */
#define GMT_ASCII_RS	30	/* ASCII code for record separator (temporarily replacing spaces in filenames) */
#define GMT_ASCII_US	31	/* ASCII code for unit separator (temporarily replacing spaces in quoted text) */

#define GMT_RENAME_FILE	0
#define GMT_COPY_FILE	1

#define GMT_CONTROLLER	0	/* func_level of controlling process (gmt.c or external API) */
#define GMT_TOP_MODULE	1	/* func_level of top-level module being called */

#define GMT_PAPER_DIM		32767	/* Upper limit on PostScript paper size under modern mode, in points (~11.6 meters) */
#define GMT_PAPER_MARGIN_AUTO	40	/* Default paper margin under modern mode, in inches (101.6 centimeter) for auto-size mode */
#define GMT_PAPER_MARGIN_FIXED	1	/* Default paper margin under modern mode, in inches (2.54 centimeter) for fixed-size mode */

#define GMT_JPEG_DEF_QUALITY	90	/* Default JPG quality value for psconvert -Tj */

/*! Constants for use with calls to gmt_solve_svd (used in greenspline and geodesy/gpsgridder) */
enum GMT_enum_svd {
	GMT_SVD_EIGEN_RATIO_CUTOFF		= 0,	/* Only use eigenvalues whose ratio to the first exceeds a cutoff [0 = all] */
	GMT_SVD_EIGEN_NUMBER_CUTOFF		= 1,	/* Only use the first N largest eigenvalues */
	GMT_SVD_EIGEN_PERCENT_CUTOFF	= 2,	/* Only use a percentage of the (sorted) eigenvalues */
	GMT_SVD_EIGEN_VARIANCE_CUTOFF	= 3,	/* Find N eigenvalues that equals given percentage of variance explained */
	GMT_SVD_NO_HISTORY				= 0,	/* Only obtain final solution for selected eigenvalues */
	GMT_SVD_INCREMENTAL				= 1,	/* Evaluate incremental solution due to next added eigenvalue */
	GMT_SVD_CUMULATIVE				= 2		/* Evaluate cumulative solution due to next added eigenvalue */
};

/*! whether to ignore/read/write history file gmt.history */
enum GMT_enum_history {
	/*! 0 */	GMT_HISTORY_OFF = 0,
	/*! 1 */	GMT_HISTORY_READ,
	/*! 2 */	GMT_HISTORY_WRITE
};

/* Specific feature geometries as obtained from OGR */
/* Note: As far as registering or reading data, GMT only needs to know if data type is POINT, LINE, or POLY */

enum GMT_enum_ogr {
	GMT_IS_LINESTRING	= 2U,
	GMT_IS_POLYGON		= 4U,
	GMT_IS_MULTI		= 64U,	/* Flag for multi-versions of geometry */
	GMT_IS_MULTIPOINT	= 65U,	/* 1 plus 64 for multi */
	GMT_IS_MULTILINESTRING	= 66U,	/* 2 plus 64 for multi */
	GMT_IS_MULTIPOLYGON	= 68U};	/* 4 plus 64 for multi */

/*! Various allocation-length parameters */
enum GMT_enum_length {
	GMT_DIM_SIZE	= 4U,          /* Length of dim array used in Read|Create Data */
	GMT_TINY_CHUNK  = 8U,
	GMT_SMALL_CHUNK = 64U,
	GMT_CHUNK       = 2048U,
	GMT_BIG_CHUNK   = 65536U,
	GMT_LEN8	= 8U,
	GMT_LEN16	= 16U,          /* All strings used to format date/clock output must be this length */
	GMT_LEN32       = 32U,          /* Small length of texts */
	GMT_LEN64       = 64U,          /* Intermediate length of texts */
	GMT_LEN128      = 128U,         /* Double of 64 */
	GMT_LEN256      = 256U,         /* Max size of some text items */
	GMT_LEN512      = 512U,         /* Max size of other text items */
	GMT_LEN1024     = 1024U,        /* For file names (anticipating web fnames) */
	GMT_MAX_RANGES  = 64U,          /* Limit on number of row ranges given in -q */
	GMT_MAX_COLUMNS = 4096U,        /* Limit on number of columns in data tables (not grids) */
	GMT_BUFSIZ      = 4096U,        /* Size of char record for i/o */
	GMT_MSGSIZ      = 16384U,       /* Size of char record for messages and report */
	GMT_MIN_MEMINC  = 1U,           /* E.g., 16 kb of 8-byte doubles */
	GMT_MAX_MEMINC  = 67108864U};   /* E.g., 512 Mb of 8-byte doubles */

/*! The four plot length units [m just used internally] */
enum GMT_enum_unit {
	GMT_CM = 0,
	GMT_INCH,
	GMT_M,
	GMT_PT};

/*! Flags for 1-D array arg parsing */
enum GMT_enum_array {
	GMT_ARRAY_RANGE = 1,
	GMT_ARRAY_TIME  = 2,
	GMT_ARRAY_DIST  = 4,
	GMT_ARRAY_NOINC = 8,
	GMT_ARRAY_SCALAR = 16,
	GMT_ARRAY_NOMINMAX = 32,
	GMT_ARRAY_ROUND = 64,
	GMT_ARRAY_UNIQUE = 128};

/*! Handling of swap/no swap in i/o */
enum GMT_swap_direction {
	k_swap_none = 0,
	k_swap_in,
	k_swap_out};

/*! Scripting languages */
enum GMT_enum_script {GMT_BASH_MODE = 0,	/* Write Bash script */
	GMT_CSH_MODE,			/* Write C-shell script */
	GMT_DOS_MODE};			/* Write DOS script */

/*! Various mode for basemap order */
enum GMT_enum_basemap {
	GMT_BASEMAP_BEFORE			= 0,
	GMT_BASEMAP_AFTER			= 1,
	GMT_BASEMAP_FRAME_BEFORE	= 0,
	GMT_BASEMAP_FRAME_AFTER		= 1,
	GMT_BASEMAP_GRID_BEFORE		= 0,
	GMT_BASEMAP_GRID_AFTER		= 2,
	GMT_BASEMAP_ANNOT_BEFORE	= 0,
	GMT_BASEMAP_ANNOT_AFTER		= 4};

/*! Handling of periodic data */
enum GMT_time_period {
	GMT_CYCLE_SEC = 1,
	GMT_CYCLE_MIN,
	GMT_CYCLE_HOUR,
	GMT_CYCLE_DAY,
	GMT_CYCLE_WEEK,
	GMT_CYCLE_YEAR,
	GMT_CYCLE_ANNUAL,
	GMT_CYCLE_CUSTOM};

/* The following long-option-related definitions specify the meanings
   of individual bit flags within the GMT_KEYWORD_DICTIONARY type's
   transproc_mask field.

   Note these definitions are required for the immdiately following block
   of definitions (e.g., GMT_I_INCREMENT_KW, etc.) and must therefore
   appear in this file prior to that block.
 */
#define GMT_TP_STANDARD		0	/* Standard translation processing */
#define GMT_TP_MULTIDIR		0x1	/* Multi-directive support enabled */
#define GMT_TP_MDCOMMA		0x2	/* Use commas in short-option multi-directives */

/* The following long-option-related definitions allow for specification
   of whether or how a long-option translation should support
   multi-directive translations, e.g., from --fields=low,high,weight
   to -Alhw.
 */
#define GMT_MULTIDIR_DISABLE	0	/* Multi-directives not supported */
#define GMT_MULTIDIR_NOCOMMA	1	/* Use no commas in short-option multi-directives, e.g., -Alhw */
#define GMT_MULTIDIR_COMMA	2	/* Use commas in short-option multi-directives, e.g., -Al,h,w */

/* The following long-option-related definitions are for quasi-global command
 * line short-options which are common to many but not all modules, i.e.,
 * (i) many modules will use exactly the same particular long-to-short-option
 * translation (e.g., --increment to -I) as provided here, but (ii) other
 * modules will use the same short-option for some entirely different purpose
 * with a different (or even no) long-to-short-option translation and should
 * thus most definitely NOT use such a quasi-global definition).
 *
 * Any particular module can simply incorporate or NOT incorporate any of the
 * following long-option translation definitions into its module_kw[] array
 * depending upon whether or not it respectively conforms to or departs from
 * the particular quasi-global usage as defined here.
 *
 * Note that certain quasi-global short-options (e.g., -G), while common to
 * many modules, differ in the availability of particular modifiers (e.g., +d)
 * across the set of modules which use that same quasi-global short-option.
 * We maximize the benefit of using the commonized translation definitions
 * below by including in these definitions all such non-universal modifiers,
 * using aliases where necessary in the case of conflicts of meaning for a
 * particular modifier between modules. These definitions are therefore not
 * intended as accurate and complete expressions of modifier capabilities
 * for all modules which employ them; rather, we rely on each module to
 * properly reject the use of any inappropriate modifier after the
 * long-to-short-option translation dictated by any of the below definitions
 * is performed, assuming that users will base their usage on officially
 * supported module documentation (e.g., manual pages) rather than these
 * non-user-facing translation entries.
 */
#define GMT_I_INCREMENT_KW { '/', 'I', "increment|inc|spacing", "", "", "e,n", "exact,number", GMT_TP_STANDARD }
#define GMT_C_CPT_KW	{ 0, 'C', "cpt|cmap", "", "", "h,i,s,u,U", "hinge,zinc,file,fromunit,tounit", GMT_TP_STANDARD }
#define GMT_G_OUTGRID_KW { 0, 'G', "outgrid", "", "", "d,n,o,s,c,l", "divide,nan,offset,scale,gdal,list", GMT_TP_STANDARD }
#define GMT_W_PEN_KW	{ 0, 'W', "pen", "", "", "c", "color", GMT_TP_STANDARD }

#define GMT_VERBOSE_CODES	"q ewticd"	/* List of valid codes to -V (the blank is for NOTICE which is not user selectable */
#define GMT_DIM_UNITS	"cip"		/* Plot dimensions in cm, inch, or point */
#define GMT_LEN_UNITS2	"efkMnu"	/* Distances in meter, foot, survey foot, km, Mile, nautical mile */
#define GMT_LEN_UNITS	"dmsefkMnu"	/* Distances in arc-{degree,minute,second} or meter, foot, km, Mile, nautical mile, survey foot */
#define GMT_ARC_UNITS	"dms"		/* Distances in arc-{degree,minute,second}t */
#define GMT_TIME_UNITS	"yowdhms"	/* Time increments in year, month, week, day, hour, min, sec */
#define GMT_TIME_FIX_UNITS	"wdhms"	/* Fixed time increment units */
#define GMT_TIME_VAR_UNITS	"yo"	/* Variable time increments in year or month*/
#define GMT_WESN_UNITS	"WESN"		/* Sign-letters for geographic coordinates */
#define GMT_DIM_UNITS_DISPLAY	"c|i|p"			/* Same, used to display as options */
#define GMT_LEN_UNITS_DISPLAY	"d|m|s|e|f|k|M|n|u"	/* Same, used to display as options */
#define GMT_LEN_UNITS2_DISPLAY	"e|f|k|M|n|u"		/* Same, used to display as options */
#define GMT_TIME_UNITS_DISPLAY	"y|o|w|d|h|m|s"		/* Same, used to display as options */
#define GMT_TIME_FIX_UNITS_DISPLAY	"w|d|h|m|s"		/* Same, used to display as options */
#define GMT_DEG2SEC_F	3600.0
#define GMT_DEG2SEC_I	3600
#define GMT_SEC2DEG	(1.0 / GMT_DEG2SEC_F)
#define GMT_DEG2MIN_F	60.0
#define GMT_DEG2MIN_I	60
#define GMT_MIN2DEG	(1.0 / GMT_DEG2MIN_F)
#define GMT_MIN2SEC_F	60.0
#define GMT_MIN2SEC_I	60
#define GMT_SEC2MIN	(1.0 / GMT_MIN2SEC_F)
#define GMT_DAY2HR_F	24.0
#define GMT_DAY2HR_I	24
#define GMT_HR2DAY	(1.0 / GMT_DAY2HR_F)
#define GMT_WEEK2DAY_F	7.0
#define GMT_WEEK2DAY_I	7
#define GMT_WEEK2SEC_F	604800.0
#define GMT_WEEK2SEC_I	604800
#define GMT_SEC2WEEK	(1.0 / GMT_WEEK2SEC_F)
#define GMT_DAY2WEEK	(1.0 / GMT_WEEK2DAY_F)
#define GMT_DAY2MIN_F	1440.0
#define GMT_DAY2MIN_I	1440
#define GMT_MIN2DAY	(1.0 / GMT_DAY2MIN_F)
#define GMT_DAY2SEC_F	86400.0
#define GMT_DAY2SEC_I	86400
#define GMT_SEC2DAY	(1.0 / GMT_DAY2SEC_F)
#define GMT_HR2SEC_F	3600.0
#define GMT_HR2SEC_I	3600
#define GMT_SEC2HR	(1.0 / GMT_HR2SEC_F)
#define GMT_HR2MIN_F	60.0
#define GMT_HR2MIN_I	60
#define GMT_MIN2HR	(1.0 / GMT_HR2MIN_F)

#define GMT_YR2SEC_F	(365.2425 * 86400.0)
#define GMT_MON2SEC_F	(365.2425 * 86400.0 / 12.0)

#define GMT_LET_HEIGHT	0.728	/* Height of an "N" compared to point size */
#define GMT_LET_WIDTH	0.564	/* Width of an "N" compared to point size */
#define GMT_DEC_WIDTH	0.54	/* Width of a decimal number compared to point size */
#define GMT_PER_WIDTH	0.30	/* Width of a decimal point compared to point size */

#define GMT_PEN_LEN		128
#define GMT_PENWIDTH		0.25	/* Default pen width in points */
#define GMT_TEXT_CLEARANCE	15	/* Clearance around text in textboxes, in percent */
#define GMT_TEXT_OFFSET		20	/* Offset of text from refpoint, in percent */

/* Conversion from symbol size to pen width for stroke-only symbols x,y,+, - */
#define GMT_SYMBOL_SIZE_TO_PEN_WIDTH	15	/* 15% */

#define GMT_N_MAX_MODEL	20	/* No more than 20 basis functions in a trend model */

#define GMT_PAIR_COORD		0	/* Tell gmt_get_pair to get both x and y as coordinates */
#define GMT_PAIR_DIM_EXACT	1	/* Tell gmt_get_pair to get BOTH x and y as separate dimensions */
#define GMT_PAIR_DIM_DUP	2	/* Tell gmt_get_pair to get both x and y as dimensions, and if only x then set y = x */
#define GMT_PAIR_DIM_NODUP	3	/* Tell gmt_get_pair to get both x and y as dimensions, and if only x then leave y alone */

#define GMT_GRID_LAYOUT		"TRS"	/* Standard GMT scanline single-item grid */
#define GMT_IMAGE_LAYOUT	"TRBa"	/* Standard GMT scanline band-interleaved image */

#define GMT_CPT_CONTINUOUS	8	/* Final CPT should be continuous */
#define GMT_CPT_TEMPORARY	1024	/* CPT was built from list of colors, e.g., red,green,255,blue,... */
#define GMT_CPT_C_REVERSE	1	/* Reverse CPT colors */
#define GMT_CPT_Z_REVERSE	2	/* Reverse CPT z-values */
#define GMT_CPT_L_ANNOT		1	/* Annotate lower slice boundary */
#define GMT_CPT_U_ANNOT		2	/* Annotate upper slice boundary */
#define GMT_CPT_B_ANNOT		3	/* Annotate lower and upper slice boundary */
#define GMT_CPT_CATEGORICAL_VAL		1	/* Categorical CPT with numerical value */
#define GMT_CPT_CATEGORICAL_KEY		2	/* Categorical CPT with text key */
#define GMT_COLOR_AUTO_TABLE		1	/* Flag in rgb for auto-color changing per table */
#define GMT_COLOR_AUTO_SEGMENT		2	/* Flag in rgb for auto-color changing per segment */
#define GMT_CPT_INDEX_LBL		0	/* Index into hidden alloc_mode_text[] for labels */
#define GMT_CPT_INDEX_KEY		1	/* Index into hidden alloc_mode_text[] for keys */

/* Ticked contour settings */
#define GMT_TICKED_SPACING	15.0		/* Spacing between ticked contour ticks (in points) */
#define GMT_TICKED_LENGTH	3.0		/* Length of ticked contour ticks (in points) */

/* Default CPT if nothing specified or overruled by remote dataset preferences */
#define GMT_DEFAULT_CPT_NAME	"turbo"
/* Default color list (or cpt) for automatic, sequential color choices */
#define GMT_DEFAULT_COLOR_SET	"#0072BD,#D95319,#EDB120,#7E2F8E,#77AC30,#4DBEEE,#A2142F"

/* CPT extension is pretty fixed */
#define GMT_CPT_EXTENSION	".cpt"
#define GMT_CPT_EXTENSION_LEN	4U

/* -C (cpt) args for grd*.c plotters: */
#define CPT_OPT_ARGS "[<section>/]<master>|<cpt>|<*color1,color2[,color3,...][+h[<hinge>]][+i<dz>][+u|U<unit>][+s<fname>]"

#define GMT_IS_ROMAN_LCASE	1	/* For converting Arabic numerals to Roman */
#define GMT_IS_ROMAN_UCASE	2

/* Settings for the MAP_FRAME_TYPE = graph */
#define GMT_GRAPH_EXTENSION		7.5	/* In percent */
#define GMT_GRAPH_EXTENSION_UNIT	'%'	/* In percent */

/* Modes for subplot status */
#define GMT_SUBPLOT_ACTIVE	1
#define GMT_PANEL_NOTSET	2

/* Fraction of increment to force outward region expansion */
#define GMT_REGION_INCFACTOR 0.25

/* Allowable refpoint codes */
#define GMT_REFPOINT_CODES "gjJnx"

/* Modifiers for contour -A option */
#define GMT_CONTSPEC_MODS "acdefghijklLnNoprstuvwxX="

/* Directives and modifiers for -F interpolant option */
#define GMT_INTERPOLANT_OPT "a|c|e|l|n|s<p>[+d1|2]"

/* Directives and modifiers for -F cpt output option in makecpt/gdr2cpt */
#define GMT_COLORMODES_OPT "[R|c|g|h|r|x]][+c[<label>]][+k<keys>]"

/* Valid modifiers for various input files */

/* Valid modifiers for -Tmin/max/inc array creator */
#define GMT_ARRAY_MODIFIERS "abeilntu"

/* Modifiers for grid files:
 * +d<divisor> divides all grid values by this divisor
 * +o<offset>  adds this offset to all grid values
 * +n<nodata> sets what the no-data value is
 * +s<scl> scales all grid values by this scale
 * +u<unit> converts Cartesian x/y coordinates from given unit to meters
 * +U<unit> converts Cartesian x/y coordinates from meter to given unit
 */
#define GMT_GRIDFILE_MODIFIERS "donsuU"

/* Modifiers for CPT files:
 * +h[<hinge>] to override soft-hinge value in CPT
 * +i<dz> is used to round auto-determined min/max range to a multiple of dz.
 * +u<unit> converts z-values from given unit to meters
 * +U<unit> converts z-values from meter to given unit
 */
#define GMT_CPTFILE_MODIFIERS "hiuU"

/* Valid options to psconvert from figure and begin */
#define GMT_PSCONVERT_LIST "ACDEHIMNQS"

/* Valid frame setting modifiers */
#define GMT_FRAME_MODIFIERS "bginotwxyz"
/* Valid axis setting modifiers */
#define GMT_AXIS_MODIFIERS "aefLlpsSu"

/* Valid decorated line symbols */
#define GMT_DECORATE_SYMBOLS "-+AaBbCcDdGgHhIikNnpSsTtxy"

/* Settings for usage message indents and break/continue characters */

#define GMT_LINE_BREAK	"\xe2\x8f\x8e"	/* Glyph for return symbol in UTF-8 */
#define GMT_LINE_CONT	"\xe2\x80\xa6"	/* Ellipsis for continue symbol in UTF-8 */
#define GMT_LINE_BULLET "\xe2\x80\xa2"	/* Bullet glyph */

/*! Codes for grdtrack */
enum GMT_enum_tracklayout {
	GMT_LEFT_RIGHT = 1,
	GMT_EW_SN = 2,
	GMT_LEFT_ONLY = 4,
	GMT_RIGHT_ONLY = 8,
	GMT_ALTERNATE = 16,
	GMT_FIXED_AZIM = 32};

/*! Codes for first segment header */
enum GMT_enum_firstseg {
	GMT_FIRST_SEGHEADER_MAYBE = 0,
	GMT_FIRST_SEGHEADER_ALWAYS,
	GMT_FIRST_SEGHEADER_NEVER};

/*! Return codes from gmt_inonout */
enum GMT_enum_inside {
	GMT_OUTSIDE = 0,
	GMT_ONEDGE,
	GMT_INSIDE};

/*! Return codes from gmt_polygon_orientation */
enum GMT_enum_polorient {
	GMT_POL_IS_CCW = 0,
	GMT_POL_IS_CW  = 1};

/*! Codes for -q selections */
enum GMT_enum_skiprows {
	GMT_RANGE_ROW_IN   = 1,
	GMT_RANGE_DATA_IN  = 2,
	GMT_RANGE_ROW_OUT  = 3,
	GMT_RANGE_DATA_OUT = 4};

/*! Return codes from parsing region modifiers +r,+R,+e */
enum GMT_enum_region {
	GMT_REGION_ADD = 1,
	GMT_REGION_ROUND,
	GMT_REGION_ROUND_EXTEND};

/*! Return codes from gmt_get_refpoint */
enum GMT_enum_refpoint {
	GMT_REFPOINT_NOTSET = -1,	/* -D */
	GMT_REFPOINT_MAP,		/* -Dg */
	GMT_REFPOINT_JUST,		/* -Dj */
	GMT_REFPOINT_JUST_FLIP,		/* -DJ */
	GMT_REFPOINT_NORM,		/* -Dn */
	GMT_REFPOINT_PLOT};		/* -Dx */

/* Various constants for map roses */
enum GMT_enum_rose {
	GMT_ROSE_DIR_PLAIN = 0,		/* Direction map rose [plain] */
	GMT_ROSE_DIR_FANCY = 1,		/* Direction map rose [fancy] */
	GMT_ROSE_MAG = 2,		/* Magnetic map rose */
	GMT_ROSE_PRIMARY = 0,		/* Primary [inner] annotation ring for magnetic rose  */
	GMT_ROSE_SECONDARY = 1};	/* Secondary [outer] annotation ring for magnetic rose  */

/*! Various types of trend model */
enum GMT_enum_model {
	GMT_POLYNOMIAL = 0, GMT_CHEBYSHEV, GMT_COSINE, GMT_SINE, GMT_FOURIER};

/*! Various array indices of fonts and pens */
enum GMT_enum_index {
	GMT_PRIMARY = 0, GMT_SECONDARY = 1};

/*! Various mode for auto-download */
enum GMT_enum_download {
	GMT_NO_DOWNLOAD = 0, GMT_YES_DOWNLOAD = 1};

/*! Various mode for auto-legend pens */
enum GMT_enum_autolegend {
	GMT_LEGEND_PEN_D  = 0, GMT_LEGEND_PEN_V  = 1, GMT_LEGEND_PEN_P  = 2,
	GMT_LEGEND_DRAW_D = 1, GMT_LEGEND_DRAW_V = 2, GMT_LEGEND_LABEL_FIXED = 0,
	GMT_LEGEND_LABEL_FORMAT = 1, GMT_LEGEND_LABEL_LIST = 2, GMT_LEGEND_LABEL_HEADER = 3};

/*! Various mode for label positioning */
enum GMT_enum_maplabel {
	GMT_LABEL_ANNOT = 0, GMT_LABEL_AXIS = 1};

/*! Various mode for custom symbols */
enum GMT_enum_customsymb {
	GMT_CUSTOM_DEF  = 1,
	GMT_CUSTOM_EPS  = 2};

#define GMT_LEGEND_DX1_MUL 0.5	/* Default offset from margin to center of symbol if given as '-' times max symbol size */
#define GMT_LEGEND_DX2_MUL 1.5	/* Default offset from margin to start of label if given as '-' times max symbol size */
#define GMT_LEGEND_DXL_MUL 1.25	/* Same as GMT_LEGEND_DX2_MUL but for line or vector symbols that typically are longer tgat circles etc */

/*! Various mode for axes */
enum GMT_enum_oblique {
	GMT_OBL_ANNOT_LON_X_LAT_Y    =  0,
	GMT_OBL_ANNOT_ANYWHERE       =  1,
	GMT_OBL_ANNOT_LON_HORIZONTAL =  2,
	GMT_OBL_ANNOT_LAT_HORIZONTAL =  4,
	GMT_OBL_ANNOT_EXTEND_TICKS   =  8,
	GMT_OBL_ANNOT_NORMAL_TICKS   = 16,
	GMT_OBL_ANNOT_LAT_PARALLEL   = 32,
	GMT_OBL_ANNOT_FLAG_LIMIT     = 64};

/*! Various mode for axes */
enum GMT_enum_axes {
	GMT_AXIS_NONE  = 0,
	GMT_AXIS_DRAW  = 1,
	GMT_AXIS_TICK  = 2,
	GMT_AXIS_BARB  = 3,
	GMT_AXIS_ANNOT = 4,
	GMT_AXIS_ALL   = 7};

/*! Various options for FFT calculations [Default is 0] */
enum FFT_implementations {
	k_fft_auto = 0,    /* Automatically select best FFT algorithm */
	k_fft_accelerate,  /* Select Accelerate Framework vDSP FFT [OS X only] */
	k_fft_fftw,        /* Select FFTW */
	k_fft_kiss,        /* Select Kiss FFT (always available) */
	k_fft_brenner,     /* Select Brenner FFT (Legacy*/
	k_n_fft_algorithms /* Number of FFT implementations available in GMT */
};

/*! Selections for run mode */
enum GMT_enum_runmode {
	GMT_CLASSIC = 0, /* Select Classic GMT behavior with -O -K -R -J */
	GMT_MODERN};  /* Select Modern behavior where -O -K are disabled and -R -J optional if possible */

/*! Selections for workflow mode */
enum GMT_enum_workflowmode {
	GMT_USE_WORKFLOW = 0, 	/* Default is to use current workflow if initiated and ignore if otherwise */
	GMT_BEGIN_WORKFLOW = 1,	/* Initiate a new workflow via gmt begin */
	GMT_END_WORKFLOW = 2,  /* Terminate current workflow via gmt begin */
	GMT_CLEAN_WORKFLOW = 4};  /* If given with BEGIN we ignore any gmt.conf files */

/*! Selections for pen/fill color replacements in custom symbol macros */
enum GMT_enum_colorswap {
	GMT_USE_FILL_RGB  = 1,	/* Take pen color from that of the current fill */
	GMT_USE_PEN_RGB = 2};	/* Take fill color from that of the current pen */

/*! Index for fill/stroke transparency value */
enum GMT_enum_transp {
	GMT_FILL_TRANSP     = 0,	/* transp[GMT_FILL_TRANSP] is used for filling */
	GMT_PEN_TRANSP      = 1,	/* transp[GMT_PEN_TRANSP] is used for stroking */
	GMT_SET_FILL_TRANSP = 1,	/* Bit-flag for fill transparency */
	GMT_SET_PEN_TRANSP  = 2,	/* Bit-flag for stroke transparency */
	GMT_SET_ALL_TRANSP  = 3};	/* Bit-flag for both transparencies */

/*! Various algorithms for triangulations */
enum GMT_enum_tri {
	GMT_TRIANGLE_WATSON = 0, /* Select Watson's algorithm */
	GMT_TRIANGLE_SHEWCHUK};  /* Select Shewchuk's algorithm */

/*! Various 1-D interpolation modes */
enum GMT_enum_spline {
	GMT_SPLINE_LINEAR = 0, /* Linear spline */
	GMT_SPLINE_AKIMA,      /* Akima spline */
	GMT_SPLINE_CUBIC,      /* Cubic spline */
	GMT_SPLINE_SMOOTH,     /* Smooth cubic spline */
	GMT_SPLINE_NN,         /* Nearest neighbor */
	GMT_SPLINE_STEP,       /* Step up to next value */
	GMT_SPLINE_NONE};      /* No spline set */

/*! Various 1-D interpolation derivatives */
enum GMT_enum_derivative {
	GMT_SPLINE_SLOPE = 10,		 /* Spline 1st derivative*/
	GMT_SPLINE_CURVATURE = 20};   /* Spline 2nd derivative */

enum GMT_enum_extrap {
	GMT_EXTRAPOLATE_NONE = 0,   /* No extrapolation; set to NaN outside bounds */
	GMT_EXTRAPOLATE_SPLINE,     /* Let spline extrapolate beyond bounds */
	GMT_EXTRAPOLATE_CONSTANT};  /* Set extrapolation beyond bound to specific constant */

enum GMT_enum_cross {
	GMT_CROSS_NORMAL = 0,	/* Regular grid cross */
	GMT_CROSS_ASYMM,    	/* Asymmetrical tick away from Equator/Greenwich */
	GMT_CROSS_SYMM}; 	/* Symmetric grid ticks */

/*! Timer reporting modes */
enum GMT_enum_tictoc {
	GMT_NO_TIMER = 0,	/* No timer reported */
	GMT_ABS_TIMER,		/* Report absolute time */
	GMT_ELAPSED_TIMER};	/* Report elapsed time since start of session */

/*! Various line/grid/image interpolation modes */
enum GMT_enum_track {
	GMT_TRACK_FILL = 0,	/* Normal fix_up_path behavior: Keep all (x,y) points but add intermediate if gap > cutoff */
	GMT_TRACK_FILL_M,	/* Fill in, but navigate via meridians (y), then parallels (x) */
	GMT_TRACK_FILL_P,	/* Fill in, but navigate via parallels (x), then meridians (y) */
	GMT_TRACK_SAMPLE_FIX,	/* Resample the track at equidistant points; old points may be lost. Use given spacing */
	GMT_TRACK_SAMPLE_ADJ};	/* Resample the track at equidistant points; old points may be lost. Adjust spacing to fit length of track exactly */

enum GMT_enum_bcr {
	BCR_NEARNEIGHBOR = 0, /* Nearest neighbor algorithm */
	BCR_BILINEAR,         /* Bilinear spline */
	BCR_BSPLINE,          /* B-spline */
	BCR_BICUBIC};         /* Bicubic spline */

enum GMT_segmentation {
	SEGM_CONTINUOUS = 0,	/* The segmentation method */
	SEGM_VECTOR,
	SEGM_NETWORK,
	SEGM_REFPOINT,
	SEGM_SEGMENT = 0,	/* The segmentation method */
	SEGM_TABLE,
	SEGM_DATASET,
	SEGM_RECORD,
	SEGM_ORIGIN};		/* External reference point */

/*! Various grid/image boundary conditions */
enum GMT_enum_bc {
	GMT_BC_IS_NOTSET = 0, /* BC not yet set */
	GMT_BC_IS_NATURAL,    /* Use natural BC */
	GMT_BC_IS_PERIODIC,   /* Use periodic BC */
	GMT_BC_IS_GEO,        /* Geographic BC condition at N or S pole */
	GMT_BC_IS_DATA};      /* Fill in BC with actual data */

enum GMT_enum_anchors {	/* Various anchor strings */
	GMT_ANCHOR_LOGO = 0,	/* Anchor for logo */
	GMT_ANCHOR_IMAGE,	/* Anchor for image */
	GMT_ANCHOR_LEGEND,	/* Anchor for legend */
	GMT_ANCHOR_COLORBAR,	/* Anchor for colorbar */
	GMT_ANCHOR_INSET,	/* Anchor for inset */
	GMT_ANCHOR_MAPSCALE,	/* Anchor for map scale */
	GMT_ANCHOR_MAPROSE,	/* Anchor for map rose */
	GMT_ANCHOR_VSCALE,	/* Anchor for vertical scale */
	GMT_ANCHOR_NTYPES};	/* Number of such types */

enum GMT_enum_fillcurves {	/*! Various mode for accepting two segments for curve-filling */
	GMT_CURVES_SEPARATE = 0, GMT_CURVES_COREGISTERED = 1};

enum GMT_enum_scales {	/* Various scale issues */
	GMT_SCALE_MAP = 1,
	GMT_SCALE_FREE = 2};

enum GMT_enum_radius {	/* Various "average" radii for an ellipsoid with axes a,a,b */
	GMT_RADIUS_MEAN = 0,	/* Mean radius IUGG R_1 = (2*a+b)/3 = a (1 - f/3) */
	GMT_RADIUS_AUTHALIC,	/* Authalic radius 4*pi*r^2 = surface area of ellipsoid, R_2 = sqrt (0.5a^2 + 0.5b^2 (tanh^-1 e)/e) */
	GMT_RADIUS_VOLUMETRIC,	/* Volumetric radius 3/4*pi*r^3 = volume of ellipsoid, R_3 = (a*a*b)^(1/3) */
	GMT_RADIUS_MERIDIONAL,	/* Meridional radius, M_r = [(a^3/2 + b^3/2)/2]^2/3 */
	GMT_RADIUS_QUADRATIC};	/* Quadratic radius, Q_r = 1/2 sqrt (3a^2 + b^2) */

enum GMT_enum_latswap {GMT_LATSWAP_NONE = -1,	/* Deactivate latswapping */
	GMT_LATSWAP_G2A = 0,	/* input = geodetic;   output = authalic   */
	GMT_LATSWAP_A2G,	/* input = authalic;   output = geodetic   */
	GMT_LATSWAP_G2C,	/* input = geodetic;   output = conformal  */
	GMT_LATSWAP_C2G,	/* input = conformal;  output = geodetic   */
	GMT_LATSWAP_G2M,	/* input = geodetic;   output = meridional */
	GMT_LATSWAP_M2G,	/* input = meridional; output = geodetic   */
	GMT_LATSWAP_G2O,	/* input = geodetic;   output = geocentric */
	GMT_LATSWAP_O2G,	/* input = geocentric; output = geodetic   */
	GMT_LATSWAP_G2P,	/* input = geodetic;   output = parametric */
	GMT_LATSWAP_P2G,	/* input = parametric; output = geodetic   */
	GMT_LATSWAP_O2P,	/* input = geocentric; output = parametric */
	GMT_LATSWAP_P2O,	/* input = parametric; output = geocentric */
	GMT_LATSWAP_N};		/* number of defined swaps  */

enum GMT_enum_geodesic {	/* Various geodesic algorithms */
	GMT_GEODESIC_VINCENTY = 0,	/* Best possible, currently Vincenty */
	GMT_GEODESIC_ANDOYER,		/* Faster approximation, currently Andoyer */
	GMT_GEODESIC_RUDOE};		/* For legacy GMT4 calculations */

#define METERS_IN_A_FOOT		0.3048			/* 2.54 * 12 / 100 */
#define METERS_IN_A_SURVEY_FOOT		(1200.0/3937.0)		/* ~0.3048006096 m */
#define METERS_IN_A_KM			1000.0
#define METERS_IN_A_MILE		1609.433		/* meters in statute mile */
#define METERS_IN_A_NAUTICAL_MILE	1852.0			/* meters in a nautical mile */
#define GMT_MAP_DIST_UNIT		'e'			/* Default distance is the meter */

enum GMT_enum_coord {GMT_GEOGRAPHIC = 0,	/* Means coordinates are lon,lat : compute spherical distances */
	GMT_CARTESIAN,	/* Means coordinates are Cartesian x,y : compute Cartesian distances */
	GMT_GEO2CART,	/* Means coordinates are lon,lat but must be mapped to (x,y) : compute Cartesian distances */
	GMT_CART2GEO};	/* Means coordinates are lon,lat but must be mapped to (x,y) : compute Cartesian distances */

enum GMT_enum_dist {GMT_MAP_DIST = 0,	/* Distance in the map */
	GMT_CONT_DIST,		/* Distance along a contour or line in dist units */
	GMT_LABEL_DIST};	/* Distance along a contour or line in dist label units */

enum GMT_enum_inonout {GMT_IOO_UNKNOWN = 0,	/* Decide based on range and type */
	GMT_IOO_CARTESIAN,			/* Use Cartesian inside function */
	GMT_IOO_SPHERICAL};			/* Use spherical inside function */

enum GMT_enum_path {GMT_RESAMPLE_PATH = 0,	/* Default: Resample geographic paths based in a max gap allowed (path_step) */
	GMT_LEAVE_PATH};	/* Options like -A can turn off this resampling, where available */

enum GMT_enum_stairpath {GMT_STAIRS_OFF = 0,	/* Default: No stairclimbing */
	GMT_STAIRS_Y,	/* Move vertically (meridian) to next point along y, then horizontally along x */
	GMT_STAIRS_X};	/* Move horizontally (parallel) to next point along x, then vertically along y */

enum GMT_enum_cdist {GMT_CARTESIAN_DIST	 = 0,	/* Cartesian 2-D x,y data, r = hypot */
	GMT_CARTESIAN_DIST_PERIODIC,		/* Cartesian 2-D x,y data,but with periodic BCs */
	GMT_CARTESIAN_DIST2,		/* Cartesian 2-D x,y data, return r^2 to avoid hypot */
	GMT_CARTESIAN_DIST_PROJ,	/* Project lon,lat to Cartesian 2-D x,y data, then get distance */
	GMT_CARTESIAN_DIST_PROJ2,	/* Same as --"-- but return r^2 to avoid hypot calls */
	GMT_CARTESIAN_DIST_PROJ_INV};	/* Project Cartesian 2-D x,y data to lon,lat, then get distance */
enum GMT_enum_mdist {
	GMT_NO_MODE = 0,
	GMT_FLATEARTH,		/* Compute Flat Earth distances */
	GMT_GREATCIRCLE,	/* Compute great circle distances */
	GMT_GEODESIC,		/* Compute geodesic distances */
	GMT_LOXODROME};		/* Compute loxodrome distances (otherwise same as great circle machinery) */
enum GMT_enum_sph {GMT_DIST_M = 10,	/* 2-D lon, lat data, convert distance to meter */
	GMT_DIST_DEG = 20,	/* 2-D lon, lat data, convert distance to spherical degree */
	GMT_DIST_COS = 30};	/* 2-D lon, lat data, convert distance to cos of spherical degree */

enum enum_item {MOVIE_ITEM_IS_LABEL = 0,	/* Item is a frame counter or time label in movie */
	MOVIE_ITEM_IS_PROG_INDICATOR};			/* Item is a progress indicator symbol in movie */

/* Help us with big and little endianness */
#ifdef WORDS_BIGENDIAN
#define GMT_BIGENDIAN	true
#define GMT_ENDIAN		'B'
#else
#define GMT_BIGENDIAN	false
#define GMT_ENDIAN		'L'
#endif

/* Constants used for automatic data download via curl */
enum GMT_enum_curl {GMT_REGULAR_FILE = 0,	/* Regular file the may or may not exist */
	GMT_CACHE_FILE = 1,	/* Temporary GMT test data file destined for the cache */
	GMT_DATA_FILE  = 2,	/* Official GMT data file destined for the user's user dir */
	GMT_URL_FILE   = 3,	/* Data given by an URL destined for the cache */
	GMT_URL_QUERY  = 4,	/* Data given by an URL CGI command destined for the cache */
	GMT_AUTO_DIR   = 0,	/* Use the directory given per internal rules */
	GMT_CACHE_DIR  = 1,	/* Use the cache directory */
	GMT_DATA_DIR   = 2,	/* Use the data directory */
	GMT_LOCAL_DIR  = 3,	/* Use the local (current) directory */
	GMT_REMOTE_DIR = 4}; /* File is on the remote server */

/* Constants for controlling the written grid history via grdedit and grdconvert */
enum GMT_grid_history {
	GMT_GRDHISTORY_NONE	= 0,	/* No output history at all */
	GMT_GRDHISTORY_OLD	= 1,	/* Only save the previous command history in the output [Default] */
	GMT_GRDHISTORY_NEW	= 2,	/* Only save the current module command history in the output  */
	GMT_GRDHISTORY_BOTH	= 3		/* Append current module history to previous history  */
};

#endif  /* GMT_CONSTANTS_H */
