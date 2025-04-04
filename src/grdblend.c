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
 * grdblend reads any number of grid files that may partly overlap and
 * creates a blend of all the files given certain criteria.  Each input
 * grid is considered to have an "outer" and "inner" region.  The outer
 * region is the extent of the grid; the inner region is provided as
 * input in the form of a -Rw/e/s/n statement.  Finally, each grid can
 * be assigned its separate weight.  This information is given to the
 * program in ASCII format, one line per grid file; each line looks like
 *
 * grdfile	-Rw/e/s/n	weight
 *
 * The blending will use a 2-D cosine taper between the inner and outer
 * regions.  The output at any node is thus a weighted average of the
 * values from any grid that occupies that grid node.  Because the out-
 * put grid can be really huge (say global grids at fine resolution),
 * all grid input/output is done row by row so memory should not be a
 * limiting factor in making large grid.
 *
 * Author:	Paul Wessel
 * Date:	1-JAN-2010
 * Version:	6 API
 */

#include "gmt_dev.h"
#include "longopt/grdblend_inc.h"

#define THIS_MODULE_CLASSIC_NAME	"grdblend"
#define THIS_MODULE_MODERN_NAME	"grdblend"
#define THIS_MODULE_LIB		"core"
#define THIS_MODULE_PURPOSE	"Blend several partially overlapping grids into one larger grid"
#define THIS_MODULE_KEYS	"<G{+,GG}"
#define THIS_MODULE_NEEDS	"R"
#define THIS_MODULE_OPTIONS "-:RVdfnr"

#define BLEND_UPPER	0
#define BLEND_LOWER	1
#define BLEND_FIRST	2
#define BLEND_LAST	3

struct GRDBLEND_CTRL {
	struct GRDBLEND_In {	/* Input files */
		bool active;
		char **file;
		unsigned int n;	/* If n > 1 we probably got *.grd or something */
	} In;
	struct GRDBLEND_G {	/* -G<grdfile> */
		bool active;
		char *file;
	} G;
	struct GRDBLEND_C {	/* -Cf|l|o|u[+n|p] */
		bool active;
		unsigned int mode;
		int sign;
	} C;
	struct GRDBLEND_I {	/* -I (for checking only) */
		bool active;
	} I;
	struct GRDBLEND_Q {	/* -Q */
		bool active;
	} Q;
	struct GRDBLEND_Z {	/* -Z<scale> */
		bool active;
		double scale;
	} Z;
	struct GRDBLEND_W {	/* -W[z] */
		bool active;
		unsigned int mode;	/* 1 if -Wz was given */
	} W;
};

struct GRDBLEND_INFO {	/* Structure with info about each input grid file */
	struct GMT_GRID *G;				/* I/O structure for grid files, including grd header */
	struct GMT_GRID_ROWBYROW *RbR;
	int in_i0, in_i1, out_i0, out_i1;		/* Indices of outer and inner x-coordinates (in output grid coordinates) */
	int in_j0, in_j1, out_j0, out_j1;		/* Indices of outer and inner y-coordinates (in output grid coordinates) */
	off_t offset;					/* grid offset when the grid extends beyond north */
	off_t skip;					/* Byte offset to skip in native binary files */
	off_t pos;					/* Current byte offset for native binary files */
	bool ignore;					/* true if the grid is entirely outside desired region */
	bool outside;				/* true if the current output row is outside the range of this grid */
	bool invert;					/* true if weight was given as negative and we want to taper to zero INSIDE the grid region */
	bool open;					/* true if file is currently open */
	bool delete;					/* true if file was produced by grdsample to deal with different registration/increments */
	bool memory;					/* true if grid is a in memory array */
	char file[PATH_MAX];			/* Name of grid file */
	double weight, wt_y, wxr, wxl, wyu, wyd;	/* Various weighting factors used for cosine-taper weights */
	double wesn[4];					/* Boundaries of inner region */
	gmt_grdfloat *z;					/* Row vector holding the current row from this file */
};

#define N_NOT_SUPPORTED	8

GMT_LOCAL int grdblend_found_unsupported_format (struct GMT_CTRL *GMT, struct GMT_GRID_HEADER *h, char *file) {
	/* Check that grid files are not among the unsupported formats that has no row-by-row io yet */
	unsigned int i, type;
	static char *not_supported[N_NOT_SUPPORTED] = {"rb", "rf", "sf", "sd", "af", "ei", "ef", "gd"};
	for (i = 0; i < N_NOT_SUPPORTED; i++) {	/* Only allow netcdf (both v3 and new) and native binary output */
		if (gmt_grd_format_decoder (GMT, not_supported[i], &type) != GMT_NOERROR) {
			/* no valid type id - which should not happen unless typo in not_supported array */
			GMT_Report (GMT->parent, GMT_MSG_INFORMATION, "Very odd - should not happen [format = %s]. Post a note on the gmt user forum\n", not_supported[i]);
			return (GMT_GRDIO_UNKNOWN_FORMAT);
		}
		if (h->type == type) {	/* Our file is in one of the unsupported formats that cannot do row-by-row i/o */
			GMT_Report (GMT->parent, GMT_MSG_INFORMATION, "Grid format type %s for file %s is not supported for row-by-row i/o.\n", not_supported[i], file);
			GMT_Report (GMT->parent, GMT_MSG_INFORMATION, "We will create a temporary output file which will be converted (via grdconvert) to your chosen format.\n");
			return (1);
		}
	}
	return (GMT_NOERROR);
}

GMT_LOCAL void grdblend_decode_R (struct GMT_CTRL *GMT, char *string, double wesn[]) {
	unsigned int i, pos, error = 0;
	char text[GMT_BUFSIZ];

	/* Needed to decode the inner region -Rw/e/s/n string */

	i = pos = 0;
	while (!error && (gmt_strtok (string, "/", &pos, text))) {
		error += gmt_verify_expectations (GMT, gmt_M_type (GMT, GMT_IN, i/2), gmt_scanf_arg (GMT, text, gmt_M_type (GMT, GMT_IN, i/2), true, &wesn[i]), text);
		i++;
	}
	if (error || (i != 4) || gmt_check_region (GMT, wesn)) {
		gmt_syntax (GMT, 'R');
	}
}

GMT_LOCAL bool grdblend_out_of_phase (struct GMT_GRID_HEADER *g, struct GMT_GRID_HEADER *h) {
	/* Look for phase shifts in w/e/s/n between the two grids */
	unsigned int way, side;
	double a;
	for (side = 0; side < 4; side++) {
		way = side / 2;
		a = fmod (fabs (((g->wesn[side] + g->xy_off * g->inc[way]) - (h->wesn[side] + h->xy_off * h->inc[way])) / h->inc[way]), 1.0);
		if (a < GMT_CONV8_LIMIT) continue;
		a = 1.0 - a;
		if (a < GMT_CONV8_LIMIT) continue;
		return true;
	}
	return false;
}

GMT_LOCAL bool grdblend_overlap_check (struct GMT_CTRL *GMT, struct GRDBLEND_INFO *B, struct GMT_GRID_HEADER *h, unsigned int mode) {
	double w, e, shift = 720.0;
	char *type[2] = {"grid", "inner grid"};

	if (gmt_grd_is_global (GMT, h)) return false;	/* Not possible to be outside the final grids longitude range if global */
	if (gmt_grd_is_global (GMT, B->G->header)) return false;	/* Not possible to overlap with the final grid in longitude range if your are a global grid */
	/* Here the grids are not global so we must carefully check for overlap while being aware of periodicity in 360 degrees */
	w = ((mode) ? B->wesn[XLO] : B->G->header->wesn[XLO]) - shift;	e = ((mode) ? B->wesn[XHI] : B->G->header->wesn[XHI]) - shift;
	while (e < h->wesn[XLO]) { w += 360.0; e += 360.0; shift -= 360.0; }
	if ((h->registration == GMT_GRID_NODE_REG && w > h->wesn[XHI]) || (h->registration == GMT_GRID_PIXEL_REG && w >= h->wesn[XHI]))  {
		GMT_Report (GMT->parent, GMT_MSG_WARNING, "File %s entirely outside longitude range of final grid region (skipped)\n", B->file);
		B->ignore = true;
		return true;
	}
	if (! (gmt_M_is_zero (shift))) {	/* Must modify region */
		if (mode) {
			B->wesn[XLO] = w;	B->wesn[XHI] = e;
		}
		else {
			B->G->header->wesn[XLO] = w;	B->G->header->wesn[XHI] = e;
		}
		GMT_Report (GMT->parent, GMT_MSG_INFORMATION, "File %s %s region needed longitude adjustment to fit final grid region\n", B->file, type[mode]);
	}
	return false;
}

GMT_LOCAL bool grdblend_outside_y_range (struct GMT_GRID_HEADER *h, double *wesn) {
	if (h->registration == GMT_GRID_NODE_REG) {
		if (h->wesn[YLO] > wesn[YHI] || h->wesn[YHI] < wesn[YLO]) return true;
	}
	else {	/* Pixel registration */
		if (h->wesn[YLO] >= wesn[YHI] || h->wesn[YHI] <= wesn[YLO]) return true;
	}
	return false;
}

GMT_LOCAL bool grdblend_outside_cartesian_x_range (struct GMT_GRID_HEADER *h, double *wesn) {
	if (h->registration == GMT_GRID_NODE_REG) {
		if (h->wesn[XLO] > wesn[XHI] || h->wesn[XHI] < wesn[XLO] || h->wesn[YLO] > wesn[YHI] || h->wesn[YHI] < wesn[YLO]) return true;
	}
	else{	/* Pixel registration */
		if (h->wesn[XLO] >= wesn[XHI] || h->wesn[XHI] <= wesn[XLO] || h->wesn[YLO] >= wesn[YHI] || h->wesn[YHI] <= wesn[YLO]) return true;
	}
	return false;
}

EXTERN_MSC void gmtlib_close_grd (struct GMT_CTRL *GMT, struct GMT_GRID *G);

GMT_LOCAL int grdblend_init_blend_job(struct GMT_CTRL *GMT, char **files, unsigned int n_files, struct GMT_GRID_HEADER **h_ptr,
                                      struct GRDBLEND_INFO **blend, bool delayed, struct GMT_GRID *Grid) {
	/* Returns how many blend files or a negative error value if something went wrong */
	int type, status, not_supported = 0, t_data, k_data = GMT_NOTSET;
	unsigned int one_or_zero, n = 0, n_scanned, do_sample, n_download = 0, down = 0, srtm_res = 0;
	bool srtm_job = false, common_inc = true, common_reg = true;
	struct GRDBLEND_INFO *B = NULL;
	struct GMT_GRID_HEADER *h = *h_ptr;	/* Input header may be NULL or preset */
	struct GMT_GRID_HIDDEN *GH = NULL;
	struct GMT_GRID_HEADER_HIDDEN *HH = NULL;
	struct GMT_GRID_HEADER_HIDDEN *HHG = NULL;

	char *sense[2] = {"normal", "inverse"}, buffer[GMT_BUFSIZ] = {""}, res[4] = {""};
	static char *V_level = GMT_VERBOSE_CODES;
	char Iargs[GMT_LEN256] = {""}, Rargs[GMT_LEN256] = {""}, cmd[GMT_LEN256] = {""};
	double wesn[4], sub = 0.0;
	struct BLEND_LIST {
		char *file;
		char *region;
		double weight;
		bool download;
	} *L = NULL;

	if (n_files > 1) {	/* Got a bunch of grid files */
		L = gmt_M_memory (GMT, NULL, n_files, struct BLEND_LIST);
		for (n = 0; n < n_files; n++) {
			L[n].file = strdup (files[n]);
			L[n].region = strdup ("-");	/* inner == outer region */
			L[n].weight = 1.0;		/* Default weight */
		}
	}
	else {	/* Must read blend file */
		bool got_region;
		unsigned int nr;
		size_t n_alloc = 0;
		struct GMT_RECORD *In = NULL;
		char r_in[GMT_LEN256] = {""}, file[PATH_MAX] = {""};
		double weight;
		gmt_set_meminc (GMT, GMT_SMALL_CHUNK);

		do {	/* Keep returning records until we reach EOF */
			if ((In = GMT_Get_Record (GMT->parent, GMT_READ_TEXT, NULL)) == NULL) {	/* Read next record, get NULL if special case */
				if (gmt_M_rec_is_error (GMT)) 		/* Bail if there are any read errors */
					return (-GMT_DATA_READ_ERROR);
				else if (gmt_M_rec_is_eof (GMT)) 		/* Reached end of file */
					break;
				continue;							/* Go back and read the next record */
			}
			/* Data record to process */

			/* Data record to process.  We permit this kind of records:
			 * file [[-R]inner_region ] [weight]
			 * i.e., file is required but region [grid extent] and/or weight [1] are optional
			 */

			n_scanned = sscanf (In->text, "%s %s %lf", file, r_in, &weight);
			if (n_scanned < 1) {
				GMT_Report (GMT->parent, GMT_MSG_ERROR, "Read error for blending parameters near row %d\n", n);
				gmt_M_free (GMT, L);
				return (-GMT_DATA_READ_ERROR);
			}
			if (n == n_alloc) L = gmt_M_malloc (GMT, L, n, &n_alloc, struct BLEND_LIST);
			L[n].file = strdup (file);
			nr = (n_scanned > 1) ? gmt_char_count (r_in, '/') : 0;	/* Count 3 slashes means we got w/e/s/n */
			if (n_scanned > 1 && nr == 3) {	/* Got a region specification */
				if (strncmp (r_in, "-R", 2U)) {	/* No -R present, add it */
					L[n].region = calloc (strlen (r_in)+3, sizeof (char));	/* Allocate enough space for -R and region */
					sprintf (L[n].region, "-R%s", r_in);
				}
				else	/* Got full -Rregion setting */
					L[n].region = strdup (r_in);
				got_region = true;
			}
			else {	/* Flag there was no inner region specified (or - given) */
				L[n].region = strdup ("-");
				got_region = false;
			}
			if (n_scanned == 2 && !got_region) weight = atof (r_in);	/* Got "file weight" record with no region present */
			L[n].weight = (n_scanned == 1 || (n_scanned == 2 && got_region)) ? 1.0 : weight;	/* Default weight is 1 if none or - were given */
			if ((t_data = gmt_file_is_a_tile (GMT->parent, L[n].file, GMT_LOCAL_DIR)) != GMT_NOTSET) {
				srtm_job = gmt_use_srtm_coverage (GMT->parent, &(L[n].file), &t_data, &srtm_res);	/* true if this dataset uses SRTM for 1s and 3s resolutions */
				if (gmt_access (GMT, &L[n].file[1], F_OK)) {	/* Tile must be downloaded */
					L[n].download = true;
					n_download++;
				}
				if (k_data == GMT_NOTSET) k_data = t_data;
			}
			n++;
		} while (true);
		gmt_reset_meminc (GMT);
		n_files = n;
	}
	if (srtm_job) {	/* Signal default CPT for earth or srtm relief final grid */
		if (h) {
			if (h->wesn[XHI] > 180.0) {
				sub = 360.0;
				h->wesn[XLO] -= sub;
				h->wesn[XHI] -= sub;
			}
			else if (h->wesn[XLO] < -180.0) {
				sub = -360.0;
				h->wesn[XLO] -= sub;
				h->wesn[XHI] -= sub;
			}
		}
	}

	B = gmt_M_memory (GMT, NULL, n_files, struct GRDBLEND_INFO);
	wesn[XLO] = wesn[YLO] = DBL_MAX;	wesn[XHI] = wesn[YHI] = -DBL_MAX;
	if (!delayed) sprintf (res, "-r%c", (Grid->header->registration == GMT_GRID_PIXEL_REG) ? 'p' : 'g');	/* We know the required registration up front */

	for (n = 0; n < n_files; n++) {	/* Process each input grid */

		if (L[n].download) {	/* Report that we will be downloading this SRTM tile */
			if (k_data != GMT_NOTSET) {
				char tile[8] = {""};
				strncpy (tile, &(L[n].file[1]), 7U);
				GMT_Report (GMT->parent, GMT_MSG_INFORMATION, "Downloading %s tile %d of %d [%s]\n", GMT->parent->remote_info[k_data].file, ++down, n_download, tile);
			}
		}

		strncpy (B[n].file, L[n].file, PATH_MAX-1);
		B[n].memory = gmt_M_file_is_memory (B[n].file);	/* If grid in memory then we only read once and have everything at once */
		if ((B[n].G = GMT_Read_Data (GMT->parent, GMT_IS_GRID, GMT_IS_FILE, GMT_IS_SURFACE, GMT_CONTAINER_ONLY|GMT_GRID_ROW_BY_ROW, NULL, B[n].file, NULL)) == NULL) {
			/* Failure somehow, free all grids read so far and bail */
			for (n = 0; n < n_files; n++) {
				gmt_M_str_free (L[n].file);	gmt_M_str_free (L[n].region);
			}
			gmt_M_free (GMT, L);	gmt_M_free (GMT, B);
			return (-GMT_DATA_READ_ERROR);
		}

		if ((not_supported = grdblend_found_unsupported_format (GMT, B[n].G->header, B[n].file)) == GMT_GRDIO_UNKNOWN_FORMAT) {
			GMT_Report (GMT->parent, GMT_MSG_ERROR, "Internal snafu - please report the problem on the GMT issues page\n");
			return (-GMT_NOT_A_VALID_LOGMODE);
		}
		B[n].weight = L[n].weight;
		if (!strcmp (L[n].region, "-"))
			gmt_M_memcpy (B[n].wesn, B[n].G->header->wesn, 4, double);	/* Set inner = outer region */
		else
			grdblend_decode_R (GMT, &L[n].region[2], B[n].wesn);			/* Must decode the -R string */
		if (h == NULL) {	/* Was not given -R, determine it from the input grids */
			if (B[n].G->header->wesn[YLO] < wesn[YLO]) wesn[YLO] = B[n].G->header->wesn[YLO];
			if (B[n].G->header->wesn[YHI] > wesn[YHI]) wesn[YHI] = B[n].G->header->wesn[YHI];
			if (B[n].G->header->wesn[XLO] < wesn[XLO]) wesn[XLO] = B[n].G->header->wesn[XLO];
			if (B[n].G->header->wesn[XHI] > wesn[XHI]) wesn[XHI] = B[n].G->header->wesn[XHI];
			if (n > 0) {
				if (fabs((B[n].G->header->inc[GMT_X] - B[0].G->header->inc[GMT_X]) / B[0].G->header->inc[GMT_X]) > 0.002 ||
					fabs((B[n].G->header->inc[GMT_Y] - B[0].G->header->inc[GMT_Y]) / B[0].G->header->inc[GMT_Y]) > 0.002)
						common_inc = false;
			}
			if (B[n].G->header->registration != B[0].G->header->registration)
				common_reg = false;
		}
		gmt_M_str_free (L[n].file);	/* Done with these now */
		gmt_M_str_free (L[n].region);
	}
	gmt_M_free (GMT, L);	/* Done with this now */

	if (h == NULL) {	/* Must use the common region from the tiles and require -I -r if not common increments or registration */
		uint64_t pp;
		double *inc = NULL;
		if (!common_inc && !GMT->common.R.active[ISET])
			GMT_Report (GMT->parent, GMT_MSG_ERROR, "Must specify -I if input grids have different increments\n");
		if (!common_reg && !GMT->common.R.active[GSET])
			GMT_Report (GMT->parent, GMT_MSG_ERROR, "Must specify -r if input grids have different registrations\n");
		if ((!common_inc && !GMT->common.R.active[ISET]) || (!common_reg && !GMT->common.R.active[GSET]))
			return (-GMT_RUNTIME_ERROR);
		/* While the inc may be fixed, our wesn may not be in phase, so since gmt_set_grddim
		 * will plow through and modify inc if it does not fit, we don't want that here. */
		if (GMT->common.R.active[ISET])	/* Got -I [-R -r] */
			gmt_increment_adjust (GMT, wesn, GMT->common.R.inc, (GMT->common.R.active[GSET]) ? GMT->common.R.registration : B[0].G->header->registration);	/* In case user specified incs using distance units we must call this here before adjusting wesn */
		inc = (GMT->common.R.active[ISET]) ? GMT->common.R.inc : B[0].G->header->inc;	/* Either use -I if given else they all have the same increments */
		pp = (uint64_t)ceil ((wesn[XHI] - wesn[XLO])/inc[GMT_X] - GMT_CONV6_LIMIT);
		wesn[XHI] = wesn[XLO] + pp * *inc;
		pp = (uint64_t)ceil ((wesn[YHI] - wesn[YLO])/inc[GMT_Y] - GMT_CONV6_LIMIT);
		wesn[YHI] = wesn[YLO] + pp * *inc;
		/* Create the h structure and initialize it */
		h = gmt_get_header (GMT);
		gmt_M_memcpy (h->wesn, wesn, 4, double);
		gmt_M_memcpy (h->inc, inc, 2, double);
		h->registration = (GMT->common.R.active[GSET]) ? GMT->common.R.registration : B[0].G->header->registration;	/* Either use -r if given else they all have the same registration */
		gmt_M_grd_setpad (GMT, h, GMT->current.io.pad); /* Assign default pad */
		gmt_set_grddim (GMT, h);	/* Update dimensions */
		*h_ptr = h;			/* Pass out the updated settings */
		GMT_Report (GMT->parent, GMT_MSG_INFORMATION,
			"We determined the region %.12g/%.12g/%.12g/%.12g from the given grids\n", h->wesn[XLO], h->wesn[XHI], h->wesn[YLO], h->wesn[YHI]);
	}

	HH = gmt_get_H_hidden (h);
	one_or_zero = !h->registration;

	for (n = 0; n < n_files; n++) {	/* Process each input grid */
		struct GMT_GRID_HEADER *t = B[n].G->header;	/* Shortcut for this tile header */

		/* Skip the file if its outer region does not lie within the final grid region */
		if (grdblend_outside_y_range (h, B[n].wesn)) {
			GMT_Report (GMT->parent, GMT_MSG_WARNING,
			            "File %s entirely outside y-range of final grid region (skipped)\n", B[n].file);
			B[n].ignore = true;
			continue;
		}
		if (gmt_M_x_is_lon (GMT, GMT_IN)) {	/* Must carefully check the longitude overlap */
			if (grdblend_overlap_check (GMT, &B[n], h, 0)) continue;	/* Check header for -+360 issues and overlap */
			if (grdblend_overlap_check (GMT, &B[n], h, 1)) continue;	/* Check inner region for -+360 issues and overlap */
		}
		else if (grdblend_outside_cartesian_x_range (h, B[n].wesn)) {
			GMT_Report (GMT->parent, GMT_MSG_WARNING,
			            "File %s entirely outside x-range of final grid region (skipped)\n", B[n].file);
			B[n].ignore = true;
			continue;
		}

		/* If input grids have different spacing or registration we must resample */

		Iargs[0] = Rargs[0] = '\0';
		do_sample = 0;
		if (not_supported) {
			GMT_Report (GMT->parent, GMT_MSG_INFORMATION,
			            "File %s not supported via row-by-row read - must reformat first\n", B[n].file);
			do_sample |= 2;
		}
		if (fabs((t->inc[GMT_X] - h->inc[GMT_X]) / h->inc[GMT_X]) > 0.002 ||
			fabs((t->inc[GMT_Y] - h->inc[GMT_Y]) / h->inc[GMT_Y]) > 0.002) {
			sprintf (Iargs, "-I%.12g/%.12g", h->inc[GMT_X], h->inc[GMT_Y]);
			GMT_Report (GMT->parent, GMT_MSG_INFORMATION, "File %s has different increments (%.12g/%.12g) than the output grid (%.12g/%.12g) - must resample\n",
				B[n].file, t->inc[GMT_X], t->inc[GMT_Y], h->inc[GMT_X], h->inc[GMT_Y]);
			do_sample |= 1;
		}
		if (grdblend_out_of_phase (t, h)) {	/* Set explicit -R for resampling that is multiple of desired increments AND inside both original grid and desired grid */
			double wesn[4];	/* Make sure wesn is equal to or larger than B[n].G->header->wesn so all points are included */
			unsigned int k;
			/* The goal below is to come up with a wesn that a proper subset of the output grid while still not
			 * exceeding the bounds of the current tile grid. The wesn below will be compatible with the output grid
			 * but is not compatible with the input grid.  grdsample will make the adjustment */
			k = (unsigned int)rint ((MAX (h->wesn[XLO], t->wesn[XLO]) - h->wesn[XLO]) / h->inc[GMT_X] - h->xy_off);
			wesn[XLO] = h->wesn[XLO] + k * h->inc[GMT_X];
			while (wesn[XLO] < t->wesn[XLO]) wesn[XLO] += t->inc[GMT_X];	/* Make sure we are not outside this grid */
			k = (unsigned int)rint ((MIN (h->wesn[XHI], t->wesn[XHI]) - h->wesn[XLO]) / h->inc[GMT_X] - h->xy_off);
			wesn[XHI] = h->wesn[XLO] + k * h->inc[GMT_X];
			while (wesn[XHI] > t->wesn[XHI]) wesn[XHI] -= t->inc[GMT_X];	/* Make sure we are not outside this grid */
			k = (unsigned int)rint ((MAX (h->wesn[YLO], t->wesn[YLO]) - h->wesn[YLO]) / h->inc[GMT_Y] - h->xy_off);
			wesn[YLO] = h->wesn[YLO] + k * h->inc[GMT_Y];
			while (wesn[YLO] < t->wesn[YLO]) wesn[YLO] += t->inc[GMT_Y];	/* Make sure we are not outside this grid */
			k = (unsigned int)rint  ((MIN (h->wesn[YHI], t->wesn[YHI]) - h->wesn[YLO]) / h->inc[GMT_Y] - h->xy_off);
			wesn[YHI] = h->wesn[YLO] + k * h->inc[GMT_Y];
			while (wesn[YHI] > t->wesn[YHI]) wesn[YHI] -= t->inc[GMT_Y];	/* Make sure we are not outside this grid */
			sprintf (Rargs, "-R%.12g/%.12g/%.12g/%.12g", wesn[XLO], wesn[XHI], wesn[YLO], wesn[YHI]);
			GMT_Report (GMT->parent, GMT_MSG_INFORMATION, "File %s coordinates are phase-shifted w.r.t. the output grid - must resample\n", B[n].file);
			do_sample |= 1;
		}
		else if (do_sample) {	/* Set explicit -R to handle possible subsetting */
			double wesn[4];
			gmt_M_memcpy (wesn, h->wesn, 4, double);
			if (wesn[XLO] < t->wesn[XLO]) wesn[XLO] = t->wesn[XLO];
			if (wesn[XHI] > t->wesn[XHI]) wesn[XHI] = t->wesn[XHI];
			if (wesn[YLO] < t->wesn[YLO]) wesn[YLO] = t->wesn[YLO];
			if (wesn[YHI] > t->wesn[YHI]) wesn[YHI] = t->wesn[YHI];
			sprintf (Rargs, "-R%.12g/%.12g/%.12g/%.12g", wesn[XLO], wesn[XHI], wesn[YLO], wesn[YHI]);
			GMT_Report (GMT->parent, GMT_MSG_DEBUG, "File %s is sampled using region %s\n", B[n].file, Rargs);
		}
		if (do_sample) {	/* One or more reasons to call upon grdsample before using this grid */
			gmt_filename_set (B[n].file);	/* Replace any spaces in filename with ASCII 29 */
			GMT->common.V.active = false;	/* Since we will parse again below */
			if (do_sample & 1) {	/* Resampling of the grid into a netcdf grid */

				if (gmt_get_tempname (GMT->parent, "grdblend_resampled", ".nc", buffer))
					return GMT_RUNTIME_ERROR;
				snprintf (cmd, GMT_LEN256, "%s %s %s %s -G%s -V%c", B[n].file, res,
				         Iargs, Rargs, buffer, V_level[GMT->current.setting.verbose]);
				if (gmt_M_is_geographic (GMT, GMT_IN)) strcat (cmd, " -fg");
				strcat (cmd, " --GMT_HISTORY=readonly");
				GMT_Report (GMT->parent, GMT_MSG_INFORMATION, "Resample %s via grdsample %s\n", B[n].file, cmd);
				if ((status = GMT_Call_Module (GMT->parent, "grdsample", GMT_MODULE_CMD, cmd))) {	/* Resample the file */
					GMT_Report (GMT->parent, GMT_MSG_ERROR, "Unable to resample file %s - exiting\n", B[n].file);
					return (-GMT_RUNTIME_ERROR);
				}
			}
			else {	/* Just reformat to netCDF so this grid may be used as well */
				if (srtm_job)
					GMT_Report (GMT->parent, GMT_MSG_INFORMATION,
					            "Convert tile from JPEG2000 to netCDF grid [%s]\n", B[n].file);
				if (gmt_get_tempname (GMT->parent, "grdblend_reformatted", ".nc", buffer))
					return GMT_RUNTIME_ERROR;
				snprintf (cmd, GMT_LEN256, "%s %s %s -V%c", B[n].file, Rargs, buffer, V_level[GMT->current.setting.verbose]);
				if (gmt_M_is_geographic (GMT, GMT_IN)) strcat (cmd, " -fg");
				strcat (cmd, " --GMT_HISTORY=readonly");
				GMT_Report (GMT->parent, GMT_MSG_INFORMATION, "Reformat %s via grdconvert %s\n", B[n].file, cmd);
				if ((status = GMT_Call_Module (GMT->parent, "grdconvert", GMT_MODULE_CMD, cmd))) {	/* Resample the file */
					GMT_Report (GMT->parent, GMT_MSG_ERROR, "Unable to resample file %s - exiting\n", B[n].file);
					return (-GMT_RUNTIME_ERROR);
				}
			}
			strncpy (B[n].file, buffer, PATH_MAX-1);	/* Use the temporary file instead */
			B[n].delete = true;		/* Flag to delete this temporary file when done */
			if (GMT_Destroy_Data (GMT->parent, &B[n].G))
				return (-GMT_RUNTIME_ERROR);
			t = NULL;	/* To remind us that this is now gone */
			if ((B[n].G = GMT_Read_Data (GMT->parent, GMT_IS_GRID, GMT_IS_FILE, GMT_IS_SURFACE, GMT_CONTAINER_ONLY|GMT_GRID_ROW_BY_ROW, NULL, B[n].file, NULL)) == NULL) {
				return -GMT_DATA_READ_ERROR;
			}
			B[n].memory = false;		/* Since we read from file, even if it was a mem-grid before */
			t = B[n].G->header;	/* Since it got reallocated */
			if (grdblend_overlap_check (GMT, &B[n], h, 0)) continue;	/* In case grdconvert changed the region */
		}
		if (B[n].weight < 0.0) {	/* Negative weight means invert sense of taper */
			B[n].weight = fabs (B[n].weight);
			B[n].invert = true;
		}
		B[n].RbR = gmt_M_memory (GMT, NULL, 1, struct GMT_GRID_ROWBYROW);		/* Allocate structure */
		GH = gmt_get_G_hidden (B[n].G);
		if (GH->extra != NULL)	/* Only memory grids will fail this test */
			gmt_M_memcpy (B[n].RbR, GH->extra, 1, struct GMT_GRID_ROWBYROW);	/* Duplicate, since GMT_Destroy_Data will free the header->extra */

		/* Here, i0, j0 is the very first col, row to read, while i1, j1 is the very last col, row to read .
		 * Weights at the outside i,j should be 0, and reach 1 at the edge of the inside block */

		/* The following works for both pixel and grid-registered grids since we are here using the i,j to measure the width of the
		 * taper zone in units of dx, dy. */

		B[n].out_i0 = irint ((t->wesn[XLO] - h->wesn[XLO]) * HH->r_inc[GMT_X]);
		B[n].in_i0  = irint ((B[n].wesn[XLO] - h->wesn[XLO]) * HH->r_inc[GMT_X]) - 1;
		B[n].in_i1  = irint ((B[n].wesn[XHI] - h->wesn[XLO]) * HH->r_inc[GMT_X]) + one_or_zero;
		B[n].out_i1 = irint ((t->wesn[XHI] - h->wesn[XLO]) * HH->r_inc[GMT_X]) - t->registration;
		B[n].out_j0 = irint ((h->wesn[YHI] - t->wesn[YHI]) * HH->r_inc[GMT_Y]);
		B[n].in_j0  = irint ((h->wesn[YHI] - B[n].wesn[YHI]) * HH->r_inc[GMT_Y]) - 1;
		B[n].in_j1  = irint ((h->wesn[YHI] - B[n].wesn[YLO]) * HH->r_inc[GMT_Y]) + one_or_zero;
		B[n].out_j1 = irint ((h->wesn[YHI] - t->wesn[YLO]) * HH->r_inc[GMT_Y]) - t->registration;

		B[n].wxl = M_PI * h->inc[GMT_X] / (B[n].wesn[XLO] - t->wesn[XLO]);
		B[n].wxr = M_PI * h->inc[GMT_X] / (t->wesn[XHI] - B[n].wesn[XHI]);
		B[n].wyu = M_PI * h->inc[GMT_Y] / (t->wesn[YHI] - B[n].wesn[YHI]);
		B[n].wyd = M_PI * h->inc[GMT_Y] / (B[n].wesn[YLO] - t->wesn[YLO]);

		if (B[n].out_j0 < 0) {	/* Must skip to first row inside the present -R */
			type = GMT->session.grdformat[t->type][0];
			if (type == 'c')	/* Old-style, 1-D netcdf grid */
				B[n].offset = t->n_columns * abs (B[n].out_j0);
			else if (type == 'n')	/* New, 2-D netcdf grid */
				B[n].offset = B[n].out_j0;
			else
				B[n].skip = (off_t)(B[n].RbR->n_byte * abs (B[n].out_j0));	/* do the fseek when we are ready to read first row */
		}
		GMT_Report (GMT->parent, GMT_MSG_DEBUG, "Grid %s: out: %d/%d/%d/%d in: %d/%d/%d/%d skip: %d offset: %d\n",
			B[n].file, B[n].out_i0, B[n].out_i1, B[n].out_j1, B[n].out_j0, B[n].in_i0, B[n].in_i1, B[n].in_j1, B[n].in_j0, (int)B[n].skip, (int)B[n].offset);

		/* Allocate space for one entire row for this grid */

		B[n].z = gmt_M_memory (GMT, NULL, t->n_columns, gmt_grdfloat);
		HHG = gmt_get_H_hidden (t);

		GMT_Report (GMT->parent, GMT_MSG_INFORMATION, "Blend file %s in %.12g/%.12g/%.12g/%.12g with %s weight %g [%d-%d]\n",
			HHG->name, B[n].wesn[XLO], B[n].wesn[XHI], B[n].wesn[YLO], B[n].wesn[YHI], sense[B[n].invert], B[n].weight, B[n].out_j0, B[n].out_j1);

		if (!B[n].memory) {	/* Free grid unless it is a memory grid */
			gmtlib_close_grd (GMT, B[n].G);	/* Close the grid file so we don't have lots of them open */
			if (GMT_Destroy_Data (GMT->parent, &B[n].G)) return (-GMT_RUNTIME_ERROR);
		}
	}

	if (fabs (sub) > 0.0) {	/* Must undo shift earlier */
		h->wesn[XLO] += sub;
		h->wesn[XHI] += sub;
	}
	*blend = B;	/* Pass back array of structures with grid information */

	return (n_files);
}

GMT_LOCAL int grdblend_sync_input_rows (struct GMT_CTRL *GMT, int row, struct GRDBLEND_INFO *B, unsigned int n_blend, double half) {
	unsigned int k;
	int G_row;
	uint64_t node;

	for (k = 0; k < n_blend; k++) {	/* Get every input grid ready for the new row */
		if (B[k].ignore) continue;	/* This grid is not even inside our area */
		if (row < B[k].out_j0 || row > B[k].out_j1) {	/* Either done with grid or haven't gotten to this range yet */
			B[k].outside = true;
			if (B[k].open) {	/* If an open file then we wipe */
				gmtlib_close_grd (GMT, B[k].G);	/* Close the grid file */
				if (GMT_Destroy_Data (GMT->parent, &B[k].G)) return GMT_NOERROR;
				B[k].open = false;
				gmt_M_free (GMT, B[k].z);
				gmt_M_free (GMT, B[k].RbR);
				if (B[k].delete && gmt_remove_file (GMT, B[k].file))	/* Delete the temporary resampled file, but it failed */
					GMT_Report (GMT->parent, GMT_MSG_ERROR, "Failed to delete file %s\n", B[k].file);
			}
			continue;
		}
		B[k].outside = false;		/* Here we know the row is inside this grid */
		if (row <= B[k].in_j0)		/* Top cosine taper weight */
			B[k].wt_y = 0.5 * (1.0 - cos ((row - B[k].out_j0 + half) * B[k].wyu));
		else if (row >= B[k].in_j1)	/* Bottom cosine taper weight */
			B[k].wt_y = 0.5 * (1.0 - cos ((B[k].out_j1 - row + half) * B[k].wyd));
		else				/* We are inside the inner region; y-weight = 1 */
			B[k].wt_y = 1.0;
		B[k].wt_y *= B[k].weight;

		if (B[k].memory) {	/* Grid already in memory, just copy the relevant row - no reading needed */
			G_row = row - B[k].out_j0;	/* The corresponding row number in the k'th grid */
			node = gmt_M_ijp (B[k].G->header, G_row, 0);	/* Start of our row at col = 0 */
			gmt_M_memcpy (B[k].z, &B[k].G->data[node], B[k].G->header->n_columns, gmt_grdfloat);	/* Copy that row */
		}
		else {	/* Deal with files that may need to be opened the first time we access it */
			if (!B[k].open) {
				struct GMT_GRID_HIDDEN *GH = NULL;
				if ((B[k].G = GMT_Read_Data (GMT->parent, GMT_IS_GRID, GMT_IS_FILE, GMT_IS_SURFACE, GMT_CONTAINER_ONLY|GMT_GRID_ROW_BY_ROW, NULL, B[k].file, NULL)) == NULL) {
					return GMT_GRID_READ_ERROR;
				}
				GH = gmt_get_G_hidden (B[k].G);
				gmt_M_memcpy (B[k].RbR, GH->extra, 1, struct GMT_GRID_ROWBYROW);	/* Duplicate, since GMT_Destroy_Data will free the header->extra */
				if (B[k].skip) {	/* Position for native binary files */
					if (fseek (B[k].RbR->fp, B[k].skip, SEEK_CUR)) {    /* Position for native binary files */
						GMT_Report (GMT->parent, GMT_MSG_ERROR, "Failed to seek to position in file %s\n", B[k].file);
						return GMT_GRDIO_SEEK_FAILED;
					}
#ifdef DEBUG
					B[k].RbR->pos = ftell (B[k].RbR->fp);
#endif
				}
				else {	/* Set offsets for netCDF files */
					B[k].RbR->start[0] += B[k].offset;					/* Start position for netCDF files */
					gmt_M_memcpy (GH->extra, B[k].RbR, 1, struct GMT_GRID_ROWBYROW);	/* Synchronize these two again */
				}
				B[k].open = true;
			}
			GMT_Get_Row (GMT->parent, 0, B[k].G, B[k].z);	/* Get one row from this file */
		}
	}
	return GMT_NOERROR;
}

static void *New_Ctrl (struct GMT_CTRL *GMT) {	/* Allocate and initialize a new control structure */
	struct GRDBLEND_CTRL *C = NULL;

	C = gmt_M_memory (GMT, NULL, 1, struct GRDBLEND_CTRL);

	/* Initialize values whose defaults are not 0/false/NULL */

	C->Z.scale = 1.0;

	return (C);
}

static void Free_Ctrl (struct GMT_CTRL *GMT, struct GRDBLEND_CTRL *C) {	/* Deallocate control structure */
	unsigned int k;
	if (!C) return;
	for (k = 0; k < C->In.n; k++) gmt_M_str_free (C->In.file[k]);
	gmt_M_free (GMT, C->In.file);
	gmt_M_str_free (C->G.file);
	gmt_M_free (GMT, C);
}

static int usage (struct GMTAPI_CTRL *API, int level) {
	const char *name = gmt_show_name_and_purpose (API, THIS_MODULE_LIB, THIS_MODULE_CLASSIC_NAME, THIS_MODULE_PURPOSE);
	if (level == GMT_MODULE_PURPOSE) return (GMT_NOERROR);
	GMT_Usage (API, 0, "usage: %s [<blendfile> | <grid1> <grid2> ...] -G<outgrid> "
		"%s %s [-Cf|l|o|u[+n|p]] [-Q] [%s] [-W[z]] [-Z<scale>] [%s] [%s] [%s] [%s] [%s]\n",
		name, GMT_I_OPT, GMT_Rgeo_OPT, GMT_V_OPT, GMT_di_OPT, GMT_f_OPT, GMT_n_OPT, GMT_r_OPT, GMT_PAR_OPT);

	if (level == GMT_SYNOPSIS) return (GMT_MODULE_SYNOPSIS);

	GMT_Message (API, GMT_TIME_NONE, "  REQUIRED ARGUMENTS:\n");
	GMT_Usage (API, 1, "\n<blendfile> | <grid1> <grid2> ...");
	GMT_Usage (API, -2, "<blendfile> is an ASCII file (or standard input) with blending parameters for each input grid. "
		"Each record has 1-3 items: filename [<inner_reg>] [<weight>]. "
		"Relative weights per grid are <weight> [1] inside the given inner region and cosine taper to 0 "
		"at actual grid limits. Skip <inner_reg> if inner region should equal the actual grid region "
		"and the relative weights for the entire grid is set to 1. "
		"Give a negative weight to invert the sense of the taper (i.e., |<weight>| outside the inner region.) "
		"If <weight> is not specified we default to 1. "
		"Grids not in netCDF or native binary format will be converted first. "
		"Grids not co-registered with the output -R -I will be resampled first.");
	GMT_Usage (API, -2, "Alternatively, if all grids have the same weight (1) and inner region == outer region, "
		"then you may instead list all the grid files on the command line (e.g., patches_*.nc). "
		"Note: You must have at least 2 input grids for this mechanism to work.");
	GMT_Usage (API, 1, "\n-G<outgrid>");
	GMT_Usage (API, -2, "Set the name of the final 2-D grid. "
		"Only netCDF and native binary grid formats are directly supported; "
		"other grid formats will be converted via grdconvert when blending is complete.");
	GMT_Option (API, "I,R");
	GMT_Message (API, GMT_TIME_NONE, "\n  OPTIONAL ARGUMENTS:\n");
	GMT_Usage (API, 1, "\n-Cf|l|o|u[+n|p]");
	GMT_Usage (API, -2, "Clobber modes; no blending takes places as output node value is determined by the mode:");
	GMT_Usage (API, 3, "f: First input grid determines the final value.");
	GMT_Usage (API, 3, "l: Lowest input grid value determines the final value.");
	GMT_Usage (API, 3, "o: Last input grid overrides any previous value.");
	GMT_Usage (API, 3, "u: Highest input grid value determines the final value.");
	GMT_Usage (API, -2, "Clobbering affects any value. Optionally, append one of two modifiers to change this:");
	GMT_Usage (API, 3, "+n Only consider clobbering if grid value is <= 0).");
	GMT_Usage (API, 3, "+p Only consider clobbering if grid value is >= 0.0).");
	GMT_Usage (API, 1, "\n-Q Raster output without a leading grid header [Default writes GMT grid file]. "
		"Output grid must be in one of the native binary formats.");
	GMT_Option (API, "V");
	GMT_Usage (API, 1, "\n-W[z]");
	GMT_Usage (API, -2, "Write out weight-sum only [make blend grid]. "
		"Append z to write weight-sum w times z instead.");
	GMT_Usage (API, 1, "\n-Z<scale>");
	GMT_Usage (API, -2, "Multiply z-values by this scale before writing to file [1].");
	GMT_Option (API, "di");
	if (gmt_M_showusage (API)) GMT_Usage (API, -2, "Also sets value for nodes without constraints [Default is NaN].");
	GMT_Option (API, "f,n");
	if (gmt_M_showusage (API)) GMT_Usage (API, -2, "(-n is passed to grdsample if grids are not co-registered).");
	GMT_Option (API, "r,.");

	return (GMT_MODULE_USAGE);
}

static int parse (struct GMT_CTRL *GMT, struct GRDBLEND_CTRL *Ctrl, struct GMT_OPTION *options) {
	/* This parses the options provided to grdblend and sets parameters in CTRL.
	 * Any GMT common options will override values set previously by other commands.
	 * It also replaces any file names specified as input or output with the data ID
	 * returned when registering these sources/destinations with the API.
	 */

 	unsigned int n_errors = 0;
	size_t n_alloc = 0;
	struct GMT_OPTION *opt = NULL;
	struct GMTAPI_CTRL *API = GMT->parent;

	for (opt = options; opt; opt = opt->next) {
		switch (opt->option) {

			case '<':	/* Collect input files */
				Ctrl->In.active = true;
				if (n_alloc <= Ctrl->In.n)
					Ctrl->In.file = gmt_M_memory (GMT, Ctrl->In.file, n_alloc += GMT_SMALL_CHUNK, char *);
				n_errors += gmt_get_required_file (GMT, opt->arg, opt->option, 0, GMT_IS_GRID, GMT_IN, GMT_FILE_REMOTE, &(Ctrl->In.file[Ctrl->In.n]));
				Ctrl->In.n++;
				break;

			/* Processes program-specific parameters */

			case 'C':	/* Clobber mode */
				n_errors += gmt_M_repeated_module_option (API, Ctrl->C.active);
				switch (opt->arg[0]) {
					case 'f': Ctrl->C.mode = BLEND_FIRST; break;
					case 'l': Ctrl->C.mode = BLEND_LOWER; break;
					case 'o': Ctrl->C.mode = BLEND_LAST;  break;
					case 'u': Ctrl->C.mode = BLEND_UPPER; break;
					default:
						GMT_Report (API, GMT_MSG_ERROR, "Option -C option: Modifiers are f|l|o|u only\n");
						n_errors++;
						break;
				}
				if (strstr (opt->arg, "+p"))		/* Only use nodes >= 0 in the updates */
					Ctrl->C.sign = +1;
				else if (strstr (opt->arg, "+n"))	/* Only use nodes <= 0 in the updates */
					Ctrl->C.sign = -1;
				else {	/* May be nothing of old-style trailing - or + */
					switch (opt->arg[1]) {	/* Any restriction due to sign */
						case '-':  Ctrl->C.sign = -1; break;
						case '+':  Ctrl->C.sign = +1; break;
						case '\0': Ctrl->C.sign =  0; break;
						default:
							GMT_Report (API, GMT_MSG_ERROR, "Option -C%c option: Sign modifiers are +n|p\n", opt->arg[0]);
							n_errors++;
							break;
					}
				}
				break;
			case 'G':	/* Output filename */
				n_errors += gmt_M_repeated_module_option (API, Ctrl->G.active);
				n_errors += gmt_get_required_file (GMT, opt->arg, opt->option, 0, GMT_IS_GRID, GMT_OUT, GMT_FILE_LOCAL, &(Ctrl->G.file));
				break;
			case 'I':	/* Grid spacings */
				n_errors += gmt_M_repeated_module_option (API, Ctrl->I.active);
				n_errors += gmt_parse_inc_option (GMT, 'I', opt->arg);
				break;
			case 'N':	/* NaN-value (deprecated 7.29.2021 PW, use -di) */
				if (gmt_M_compat_check (GMT, 6)) {	/* Honor old -N<value> option */
					GMT_Report (API, GMT_MSG_COMPAT, "Option -N is deprecated; use GMT common option -di<nodata> instead.\n");
					if (opt->arg[0]) {
						char arg[GMT_LEN64] = {""};
						sprintf (arg, "i%s", opt->arg);
						n_errors += gmt_parse_d_option (GMT, arg);
					}
					else {
						GMT_Report (API, GMT_MSG_ERROR, "Option -N: Must specify value or NaN\n");
						n_errors++;
					}
				}
				else
					n_errors += gmt_default_option_error (GMT, opt);
				break;
			case 'Q':	/* No header on output */
				n_errors += gmt_M_repeated_module_option (API, Ctrl->Q.active);
				n_errors += gmt_get_no_argument (GMT, opt->arg, opt->option, 0);
				break;
			case 'W':	/* Write weights instead */
				n_errors += gmt_M_repeated_module_option (API, Ctrl->W.active);
				if (opt->arg[0] == 'z') Ctrl->W.mode = 1;
				break;
			case 'Z':	/* z-multiplier */
				n_errors += gmt_M_repeated_module_option (API, Ctrl->Z.active);
				n_errors += gmt_get_required_double (GMT, opt->arg, opt->option, 0, &Ctrl->Z.scale);
				break;

			default:	/* Report bad options */
				n_errors += gmt_default_option_error (GMT, opt);
				break;
		}
	}

	n_errors += gmt_M_check_condition (GMT, GMT->common.R.active[ISET] && (GMT->common.R.inc[GMT_X] <= 0.0 || GMT->common.R.inc[GMT_Y] <= 0.0),
	            "Option -I: Must specify positive increments\n");
	n_errors += gmt_M_check_condition (GMT, !Ctrl->G.active,
	            "Option -G: Must specify output file\n");
	return (n_errors ? GMT_PARSE_ERROR : GMT_NOERROR);
}

GMT_LOCAL bool grdblend_got_plot_domain (struct GMTAPI_CTRL *API, const char *file, char *code) {
	/* If given an =tiled_<ID>_[P|G][L|O|X].xxxxxx list then we return true if the region given when the list was assembled
	 * was a plot domain (give to a PS producer) or a grid domain (given to a grid producer). */
	char region;
	*code = 0;
	if (!gmt_file_is_tiled_list (API, file, NULL, code, &region)) return false;	/* Not a valid tiled list file */

	if (region == 'P') {
		GMT_Report (API, GMT_MSG_DEBUG, "Got tiled list determined from a plot region\n");
		return true;
	}
	return false;
}

#define bailout(code) {gmt_M_free_options (mode); return (code);}
#define Return(code) {Free_Ctrl (GMT, Ctrl); gmt_end_module (GMT, GMT_cpy); bailout (code);}

EXTERN_MSC int GMT_grdblend (void *V_API, int mode, void *args) {
	unsigned int col, row, nx_360 = 0, k, kk, m, n_blend, nx_final, ny_final, out_case;
	int status, pcol, err, error;
	bool reformat, wrap_x, write_all_at_once = false, first_grid, delayed = true, not_nan;

	uint64_t ij, n_fill, n_tot;
	double wt_x, w, wt;
	gmt_grdfloat *z = NULL, no_data_f;

	char type, zcase;
	char *outfile = NULL, outtemp[PATH_MAX];

	struct GRDBLEND_INFO *blend = NULL;
	struct GMT_GRID *Grid = NULL;
	struct GMT_GRID_HEADER_HIDDEN *HH = NULL;
	struct GMT_GRID_HEADER *h_region = NULL;
	struct GRDBLEND_CTRL *Ctrl = NULL;
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

	/*---------------------------- This is the grdblend main code ----------------------------*/

	gmt_grd_set_datapadding (GMT, true);	/* Turn on gridpadding when reading a subset */

	if (Ctrl->In.n <= 1) {	/* Got a blend file (or stdin) */
		if (Ctrl->In.n) {	/* Check if user mistakenly gave a single grid as input */
			char test_file[PATH_MAX] = {""};
			int ret_code;
			struct GMT_GRID_HEADER *h_test = gmt_get_header (GMT);
			strncpy (test_file, Ctrl->In.file[0], PATH_MAX);
			if ((ret_code = gmt_grd_get_format (GMT, test_file, h_test, true)) == GMT_NOERROR) {
				GMT_Report (API, GMT_MSG_ERROR, "Only a single grid found; no blending can take place\n");
				gmt_free_header (API->GMT, &h_test);
				Return (GMT_RUNTIME_ERROR);
			}
			gmt_free_header (API->GMT, &h_test);
		}
		if (GMT_Init_IO (API, GMT_IS_DATASET, GMT_IS_TEXT, GMT_IN, GMT_ADD_DEFAULT, 0, options) != GMT_NOERROR) {	/* Register data input */
			Return (API->error);
		}
		if (GMT_Begin_IO (API, GMT_IS_DATASET, GMT_IN, GMT_HEADER_ON) != GMT_NOERROR) {	/* Enables data input and sets access mode */
			Return (API->error);
		}
	}

	if (GMT->common.R.active[RSET] && GMT->common.R.active[ISET]) {	/* Set output grid via -R -I [-r] */
		double *region = NULL, wesn[4];
		if (Ctrl->In.n == 1 && grdblend_got_plot_domain (API, Ctrl->In.file[0], &zcase)) {	/* Must adjust -R to be exact multiple of desired grid inc */
			double inc15[2] = {15.0 / 3600.0, 15.0/ 3600.0};
			double *inc = (zcase == 'O') ? inc15 : GMT->common.R.inc;	/* Pointer to the grid increment */
			gmt_increment_adjust (GMT, GMT->common.R.wesn, inc, GMT_GRID_DEFAULT_REG);	/* In case user specified incs using distance units we must call this here before adjusting wesn */
			wesn[XLO] = floor ((GMT->common.R.wesn[XLO] / inc[GMT_X]) + GMT_CONV8_LIMIT) * inc[GMT_X];
			wesn[XHI] = ceil  ((GMT->common.R.wesn[XHI] / inc[GMT_X]) - GMT_CONV8_LIMIT) * inc[GMT_X];
			wesn[YLO] = floor ((GMT->common.R.wesn[YLO] / inc[GMT_Y]) + GMT_CONV8_LIMIT) * inc[GMT_Y];
			wesn[YHI] = ceil  ((GMT->common.R.wesn[YHI] / inc[GMT_Y]) - GMT_CONV8_LIMIT) * inc[GMT_Y];
			region = wesn;	/* Use this (possibly slightly adjusted) region */
		}
		/* Here, region is either pointing to specific entries or is NULL, meaning we default to given -R */
		if ((Grid = GMT_Create_Data (API, GMT_IS_GRID, GMT_IS_SURFACE, GMT_CONTAINER_ONLY, NULL, region, NULL,
			GMT_GRID_DEFAULT_REG, GMT_NOTSET, NULL)) == NULL)
				Return (API->error);
		h_region = Grid->header;
		delayed = false;	/* Was able to create the grid from command line options */
	}

	status = grdblend_init_blend_job (GMT, Ctrl->In.file, Ctrl->In.n, &h_region, &blend, delayed, Grid);
	if (status < 0) Return (-status);	/* Something went wrong in grdblend_init_blend_job */

	if (Ctrl->In.n <= 1 && GMT_End_IO (API, GMT_IN, 0) != GMT_NOERROR) {	/* Disables further data input */
		Return (API->error);
	}

	n_blend = status;
	if (!Ctrl->W.active && n_blend == 1) {
		GMT_Report (API, GMT_MSG_INFORMATION, "Only 1 grid found; no blending will take place\n");
	}

	if (delayed) {	/* Now we can create the output grid */
		if ((Grid = GMT_Create_Data (API, GMT_IS_GRID, GMT_IS_SURFACE, GMT_CONTAINER_ONLY, NULL, h_region->wesn, h_region->inc,
			h_region->registration, GMT_NOTSET, NULL)) == NULL)
				Return (API->error);
		gmt_free_header (API->GMT, &h_region);
	}

	if ((err = gmt_grd_get_format (GMT, Ctrl->G.file, Grid->header, false)) != GMT_NOERROR) {
		GMT_Report (API, GMT_MSG_ERROR, "%s [%s]\n", GMT_strerror(err), Ctrl->G.file);
		Return (GMT_RUNTIME_ERROR);
	}
	HH = gmt_get_H_hidden (Grid->header);

	GMT_Report (API, GMT_MSG_INFORMATION, "Processing input grids\n");

	/* Formats other than netcdf (both v3 and new) and native binary must be reformatted at the end */
	reformat = grdblend_found_unsupported_format (GMT, Grid->header, Ctrl->G.file);
	type = GMT->session.grdformat[Grid->header->type][0];
	if (Ctrl->Q.active && (reformat || (type == 'c' || type == 'n'))) {
		GMT_Report (API, GMT_MSG_ERROR, "Option -Q: Not supported for grid format %s\n", GMT->session.grdformat[Grid->header->type]);
		Return (GMT_RUNTIME_ERROR);
	}

	n_fill = n_tot = 0;

	/* Process blend parameters and populate blend structure and open input files and seek to first row inside the output grid */

	no_data_f = (GMT->common.d.active[GMT_IN]) ? (gmt_grdfloat)GMT->common.d.nan_proxy[GMT_IN] : GMT->session.f_NaN;

	/* Initialize header structure for output blend grid */

	n_tot = gmt_M_get_nm (GMT, Grid->header->n_columns, Grid->header->n_rows);

	z = gmt_M_memory (GMT, NULL, Grid->header->n_columns, gmt_grdfloat);	/* Memory for one output row */

	if (GMT_Set_Comment (API, GMT_IS_GRID, GMT_COMMENT_IS_OPTION | GMT_COMMENT_IS_COMMAND, options, Grid)) {
		gmt_M_free (GMT, z);
		Return (API->error);
	}

	if (gmt_M_file_is_memory (Ctrl->G.file)) {	/* GMT_grdblend is called by another module; must return as GMT_GRID */
		/* Allocate space for the entire output grid */
		if (GMT_Create_Data (API, GMT_IS_GRID, GMT_IS_GRID, GMT_DATA_ONLY, NULL, NULL, NULL, 0, 0, Grid) == NULL) {
			gmt_M_free (GMT, z);
			Return (API->error);
		}
		write_all_at_once = true;
	}
	else {
		unsigned int w_mode;
		if (reformat) {	/* Must use a temporary netCDF file then reformat it at the end */
			if (gmt_get_tempname (GMT->parent, "grdblend_temp", ".nc", outtemp))
				return GMT_RUNTIME_ERROR;
			outfile = outtemp;
		}
		else
			outfile = Ctrl->G.file;
		/* Write the grid header unless -Q */
		w_mode = GMT_CONTAINER_ONLY | GMT_GRID_ROW_BY_ROW;
		if (Ctrl->Q.active) w_mode |= GMT_GRID_NO_HEADER;
		if ((error = GMT_Write_Data (API, GMT_IS_GRID, GMT_IS_FILE, GMT_IS_SURFACE, w_mode, NULL, outfile, Grid))) {
			gmt_M_free (GMT, z);
			Return (error);
		}
	}

	if (Ctrl->Z.active) GMT_Report (API, GMT_MSG_WARNING, "Output data will be scaled by %g\n", Ctrl->Z.scale);

	out_case = (Ctrl->W.active) ? 1 + Ctrl->W.mode : 0;

	Grid->header->z_min = DBL_MAX;	Grid->header->z_max = -DBL_MAX;	/* These will be updated in the loop below */
	if (gmt_M_is_geographic (GMT, GMT_IN)) gmt_set_geographic (GMT, GMT_OUT);	/* Inherit the flavor of the input */
	wrap_x = (gmt_M_x_is_lon (GMT, GMT_OUT));	/* Periodic geographic grid */
	if (wrap_x) nx_360 = urint (360.0 * HH->r_inc[GMT_X]);

	for (row = 0; row < Grid->header->n_rows; row++) {	/* For every output row */

		gmt_M_memset (z, Grid->header->n_columns, gmt_grdfloat);	/* Start from scratch */

		status = grdblend_sync_input_rows (GMT, row, blend, n_blend, Grid->header->xy_off);	/* Wind each input file to current record and read each of the overlapping rows */
		if (status) {
			gmt_M_free (GMT, z);
			Return (status);
		}

		for (col = 0; col < Grid->header->n_columns; col++) {	/* For each output node on the current row */

			w = 0.0;	/* Reset weight */
			first_grid = true;	/* Since some grids do not contain this (row,col) we want to know when we are processing the first grid inside */
			not_nan = false;	/* Will be true once the first grid that has a non-NaN node is encountered for this (row,col); */
			for (k = m = 0; k < n_blend; k++) {	/* Loop over every input grid; m will be the number of contributing grids to this node  */
				if (blend[k].ignore) continue;					/* This grid is entirely outside the s/n range */
				if (blend[k].outside) continue;					/* This grid is currently outside the s/n range */
				if (wrap_x) {	/* Special testing for periodic x coordinates */
					pcol = col + nx_360;
					while (pcol > blend[k].out_i1) pcol -= nx_360;
					if (pcol < blend[k].out_i0) continue;	/* This grid is currently outside the w/e range */
				}
				else {	/* Not periodic */
					pcol = col;
					if (pcol < blend[k].out_i0 || pcol > blend[k].out_i1) continue;	/* This grid is currently outside the xmin/xmax range */
				}
				kk = pcol - blend[k].out_i0;					/* kk is the local column variable for this grid */
				if (gmt_M_is_fnan (blend[k].z[kk])) continue;			/* NaNs do not contribute */
				not_nan = true;	/* At least one non-NaN grid contributing */
				if (Ctrl->C.active) {	/* Clobber; update z[col] according to selected mode */
					switch (Ctrl->C.mode) {
						case BLEND_FIRST: if (m) continue; break;	/* Already set */
						case BLEND_UPPER: if (m && blend[k].z[kk] <= z[col]) continue; break;	/* Already has a higher value; else set below */
						case BLEND_LOWER: if (m && blend[k].z[kk] >= z[col]) continue; break;	/* Already has a lower value; else set below */
						/* Last case BLEND_LAST is always true in that we always update z[col] */
					}
					switch (Ctrl->C.sign) {	/* Check if sign of input grid should be considered in decision */
						case -1: if (first_grid) {z[col] = blend[k].z[kk]; first_grid = false; continue; break;}	/* Must initialize with first grid in case nothing passes */
							 else if (blend[k].z[kk] > 0.0) continue;	/* Only pick grids value if negative or zero */
							 break;
						case +1: if (first_grid) { z[col] = blend[k].z[kk]; first_grid = false; continue; break;}	/* Must initialize with first grid in case nothing passes */
							 else if (blend[k].z[kk] < 0.0) continue;	/* Only pick grids value if positive or zero */
							 break;
						default: break;						/* Always use the grid value */

					}
					z[col] = blend[k].z[kk];					/* Just pick this grid's value */
					w = 1.0;							/* Set weights to 1 */
					m = 1;								/* Pretend only one grid came here */
				}
				else {	/* Do the weighted blending */
					if (pcol <= blend[k].in_i0)					/* Left cosine-taper weight */
						wt_x = 0.5 * (1.0 - cos ((pcol - blend[k].out_i0 + Grid->header->xy_off) * blend[k].wxl));
					else if (pcol >= blend[k].in_i1)					/* Right cosine-taper weight */
						wt_x = 0.5 * (1.0 - cos ((blend[k].out_i1 - pcol + Grid->header->xy_off) * blend[k].wxr));
					else								/* Inside inner region, weight = 1 */
						wt_x = 1.0;
					wt = wt_x * blend[k].wt_y;					/* Actual weight is 2-D cosine taper */
					if (blend[k].invert) wt = blend[k].weight - wt;			/* Invert the sense of the tapering */
					z[col] += (gmt_grdfloat)(wt * blend[k].z[kk]);				/* Add up weighted z*w sum */
					w += wt;							/* Add up the weight sum */
					m++;								/* Add up the number of contributing grids */
				}
			}

			if (Ctrl->C.sign && m == 0 && not_nan) m = 1, w = 1.0;	/* Since we started off with the first grid and never set m,w at that time */

			if (m) {	/* OK, at least one grid contributed to an output value */
				switch (out_case) {
					case 0: /* Blended average */
						z[col] = (gmt_grdfloat)((w == 0.0) ? 0.0 : z[col] / w);	/* Get weighted average z */
						if (Ctrl->Z.active) z[col] *= (gmt_grdfloat)Ctrl->Z.scale;	/* Apply the global scale here */
						break;
					case 1:	/* Just weights */
						z[col] = (gmt_grdfloat)w;				/* Only interested in the weights */
						break;
					case 2:	/* w*z = sum of z */
						z[col] = (gmt_grdfloat)(z[col] * w);
						break;
				}
				n_fill++;						/* One more cell filled */
				if (z[col] < Grid->header->z_min) Grid->header->z_min = z[col];	/* Update the extrema for output grid */
				if (z[col] > Grid->header->z_max) Grid->header->z_max = z[col];
				if (gmt_M_is_zero (z[col]))
					m = 1;
			}
			else			/* No grids covered this node, defaults to the no_data value */
				z[col] = no_data_f;
		}
		if (write_all_at_once) {	/* Must copy entire row to grid */
			ij = gmt_M_ijp (Grid->header, row, 0);
			gmt_M_memcpy (&(Grid->data[ij]), z, Grid->header->n_columns, gmt_grdfloat);
		}
		else
			GMT_Put_Row (API, row, Grid, z);

		if (row%10 == 0)  GMT_Report (API, GMT_MSG_INFORMATION, "Processed row %7ld of %d\r", row, Grid->header->n_rows);

	}
	GMT_Report (API, GMT_MSG_INFORMATION, "Processed row %7ld\n", row);
	nx_final = Grid->header->n_columns;	ny_final = Grid->header->n_rows;

	if (write_all_at_once) {	/* Must write entire grid */
		if (GMT_Write_Data (API, GMT_IS_GRID, GMT_IS_FILE, GMT_IS_SURFACE, GMT_CONTAINER_AND_DATA, NULL, Ctrl->G.file, Grid) != GMT_NOERROR) {
			gmt_M_free (GMT, z);
			Return (API->error);
		}
	}
	else {	/* Finish the line-by-line writing */
		mode = GMT_CONTAINER_ONLY | GMT_GRID_ROW_BY_ROW;
		if (Ctrl->Q.active) mode |= GMT_GRID_NO_HEADER;
		if (!Ctrl->Q.active && (error = GMT_Write_Data (API, GMT_IS_GRID, GMT_IS_FILE, GMT_IS_SURFACE, mode, NULL, outfile, Grid))) {
			Return (error);
		}
		if ((error = GMT_Destroy_Data (API, &Grid)) != GMT_NOERROR) Return (error);
	}
	gmt_M_free (GMT, z);

	/* Free up the list with grid information, closing files as necessary */

	for (k = 0; k < n_blend; k++) {
		if (blend[k].open || blend[k].memory) {
			gmt_M_free (GMT, blend[k].z);
			gmt_M_free (GMT, blend[k].RbR);
		}
		if (blend[k].open) {
			gmtlib_close_grd (GMT, blend[k].G);	/* Close the grid file so we don't have lots of them open */
			if (blend[k].delete && gmt_remove_file (GMT, blend[k].file))	/* Delete the temporary resampled file */
				GMT_Report (GMT->parent, GMT_MSG_ERROR, "Failed to delete file %s\n", blend[k].file);
			if ((error = GMT_Destroy_Data (API, &blend[k].G)) != GMT_NOERROR) Return (error);
		}
	}

	if (gmt_M_is_verbose (GMT, GMT_MSG_INFORMATION)) {
		char empty[GMT_LEN64] = {""};
		GMT_Report (API, GMT_MSG_INFORMATION, "Blended grid size of %s is %d x %d\n", Ctrl->G.file, nx_final, ny_final);
		if (n_fill == n_tot)
			GMT_Report (API, GMT_MSG_INFORMATION, "All nodes assigned values\n");
		else {
			if (gmt_M_is_fnan (no_data_f))
				strcpy (empty, "NaN");
			else
				sprintf (empty, "%g", no_data_f);
			GMT_Report (API, GMT_MSG_INFORMATION, "%" PRIu64 " nodes assigned values, %" PRIu64 " set to %s\n", n_fill, n_tot - n_fill, empty);
		}
	}

	gmt_M_free (GMT, blend);

	if (reformat) {	/* Must reformat the output grid to the non-supported format */
		int status;
		char cmd[GMT_LEN256] = {""}, *V_level = GMT_VERBOSE_CODES;
		sprintf (cmd, "%s %s -V%c --GMT_HISTORY=readonly", outfile, Ctrl->G.file, V_level[GMT->current.setting.verbose]);
		GMT_Report (API, GMT_MSG_INFORMATION, "Reformat %s via grdconvert %s\n", outfile, cmd);
		if ((status = GMT_Call_Module (GMT->parent, "grdconvert", GMT_MODULE_CMD, cmd))) {	/* Resample the file */
			GMT_Report (API, GMT_MSG_ERROR, "Unable to resample file %s.\n", outfile);
		}
		if (gmt_remove_file (GMT, outfile))	/* Try half-heartedly to remove the temporary file */
			GMT_Report (GMT->parent, GMT_MSG_ERROR, "Failed to delete file %s\n", outfile);
	}

	/* These two were not copied back into GMT and would be lost in gmt_end_module. Needed to set the correct
	   @PROJ parameters in PS files. See issue #8438
	*/
	GMT_cpy->current.io.col_type[0][0] = GMT->current.io.col_type[0][0];
	GMT_cpy->current.io.col_type[0][1] = GMT->current.io.col_type[0][1];

	Return (GMT_NOERROR);
}
