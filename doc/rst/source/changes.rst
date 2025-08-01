:orphan:

.. _changelog:

=========
Changelog
=========

New Features in GMT 6.6
=======================

GMT 6.6 is our first release after Paul Wessel has left us. Though we are still in a kind of orphanage state,
the GMT development has not stopped. This release includes many bug fixes and the new and much requested 
Spilhaus map projection.

* ``psconvert`` has a new option **-!** that modifies the produced PostScript file in place thus avoiding
  making a copy of it before rasterization by ghostscript.
* Update for IGRF 14. (#8635)

New Features in GMT 6.5
=======================

GMT 6.5 includes multiple new supplement modules, many new features, general code and documentation improvements, and numerous
bug fixes! Here are the general updates included in 6.5:

* Add legend symbol for confidence lines (`#6890 <https://github.com/GenericMappingTools/gmt/pull/6890>`_)
* Allow format flags F and G in ``FORMAT_GEO_MAP`` to come first or last (`#7215 <https://github.com/GenericMappingTools/gmt/pull/7215>`_)
* Allow ``MAP_FRAME_TYPE=graph-origin`` (`#6824 <https://github.com/GenericMappingTools/gmt/pull/6824>`_)
* Enhance -o common option to accept the same transformations as -i (`#7887 <https://github.com/GenericMappingTools/gmt/pull/7887>`_)
* Improve support for GMT one-liners (`#7105 <https://github.com/GenericMappingTools/gmt/pull/7105>`_), (`#7099 <https://github.com/GenericMappingTools/gmt/pull/7099>`_)
* Improve support for guessing the date format (`#7901 <https://github.com/GenericMappingTools/gmt/pull/7901>`_)
* Let -U common option take +ttext to override time stamp (`#7127 <https://github.com/GenericMappingTools/gmt/pull/7127>`_)
* Support filling the area between two time- or space-series (`#7900 <https://github.com/GenericMappingTools/gmt/pull/7900>`_)
* Support leading zeros in the map frame(`#7655 <https://github.com/GenericMappingTools/gmt/pull/7655>`_)
* Update Scientific Color Maps to version 8 (`#7575 <https://github.com/GenericMappingTools/gmt/pull/7575>`_)

New Core Module Features in GMT 6.5
-----------------------------------

* :doc:`batch`: Allow -F to set a more specific naming template (`#6934 <https://github.com/GenericMappingTools/gmt/pull/6934>`_)
* :doc:`batch`: Let -D drop the mandatory moving of product files (`#6933 <https://github.com/GenericMappingTools/gmt/pull/6933>`_)
* :doc:`gmtspatial`: Augment with -N+i to treat each point individually (`#7747 <https://github.com/GenericMappingTools/gmt/pull/7747>`_)
* :doc:`grdcut`: Let -E extract a vertical slice from a 3-D cube (`#7961 <https://github.com/GenericMappingTools/gmt/pull/7961>`_)
* :doc:`movie`: Let -A handle addition of audio track (`#7461 <https://github.com/GenericMappingTools/gmt/pull/7461>`_)
* :doc:`psconvert`: Add option to transpose a dataset (`#7478 <https://github.com/GenericMappingTools/gmt/pull/7478>`_)
* :doc:`sample1d`: Add -Fe for step-up sampling (`#7488 <https://github.com/GenericMappingTools/gmt/pull/7488>`_)

Supplement updates in GMT 6.5
-----------------------------

* New supplement modules for the Global Seafloor Fabric and Magnetic Lineations (`<https://www.soest.hawaii.edu/PT/GSFML/>`_)
* New supplement to plot symbols for the wind direction and wind speed
* New supplement modules **grdshake** and **grdvs30** to build shake-maps

Enhancements in GMT 6.5
-----------------------

* :doc:`clear`: Allow removing any one dataset and listing directories to be deleted (`#7740 <https://github.com/GenericMappingTools/gmt/pull/7740>`_)
* :doc:`colorbar`: Add -LI to label first and last panel with < and > (`#8246 <https://github.com/GenericMappingTools/gmt/pull/8246>`_)
* :doc:`colorbar`: Add modifer +r to -S to only annotate the CPT limits (`#7728 <https://github.com/GenericMappingTools/gmt/pull/7728>`_)
* :doc:`colorbar`: Default to color bars with gaps for categorical CPTs (`#7831 <https://github.com/GenericMappingTools/gmt/pull/7831>`_)
* :doc:`colorbar`: Make sure only low and high contributes to decimal count in -S+r (`#7730 <https://github.com/GenericMappingTools/gmt/pull/7730>`_)
* :doc:`events`: Enhance the curves to do linear, quadratic or cosine ramp (`#7680 <https://github.com/GenericMappingTools/gmt/pull/7680>`_)
* :doc:`gmtmath`: Allow trailing text to be kept for output (`#7759 <https://github.com/GenericMappingTools/gmt/pull/7759>`_)
* :doc:`gmtlogo` Add modifier +hheight to set logo height instead of width (`#7458 <https://github.com/GenericMappingTools/gmt/pull/7458>`_)
* :doc:`gmtselect`: Add modifier +h[k|s] to -Z to pick z-value from segment header (`#7309 <https://github.com/GenericMappingTools/gmt/pull/7309>`_)
* :doc:`grdclip`: Enhance -Sa -Sb with modifier +e for equality (`#7858 <https://github.com/GenericMappingTools/gmt/pull/7858>`_)
* :doc:`grdmix`: Add -C[cpt] to build an image directly from a grid and a CPT (`#8193 <https://github.com/GenericMappingTools/gmt/pull/8193>`_)
* :doc:`grdimage`: Allow processing byte images without indexed colors, via CPT (`#7827 <https://github.com/GenericMappingTools/gmt/pull/7827>`_)
* :doc:`grdimage`: Handle mask grids with automatic CPT (`#6930 <https://github.com/GenericMappingTools/gmt/pull/6930>`_), (`#6931 <https://github.com/GenericMappingTools/gmt/pull/6931>`_)
* :doc:`grdimage`: Share -T with :doc:`grdview` (`#7315 <https://github.com/GenericMappingTools/gmt/pull/7315>`_)
* :doc:`grdinfo`: Add -Io for oblique format (`#7413 <https://github.com/GenericMappingTools/gmt/pull/7413>`_)
* :doc:`grdmath`: Control what happens to grid CPT settings (`#6916 <https://github.com/GenericMappingTools/gmt/pull/6916>`_)
* :doc:`grdpaste`: Add option to only report the "pasting" side. (`#7368 <https://github.com/GenericMappingTools/gmt/pull/7368>`_)
* :doc:`grd2cpt`: Support features from makecpt and improve docs (`#8138 <https://github.com/GenericMappingTools/gmt/pull/8138>`_)
* :doc:`grd2xyz`, :doc:`sample1d`: Add -Ccpt option to sample colors (`#8195 <https://github.com/GenericMappingTools/gmt/pull/8195>`_)
* :doc:`greenspline`: Add new option -K for statistics output to file or stdout (`#8162 <https://github.com/GenericMappingTools/gmt/pull/8162>`_)
* :doc:`greenspline`: Implement -Q for 1d (`#7155 <https://github.com/GenericMappingTools/gmt/pull/7155>`_)
* :doc:`image`: Allow -D+w to specify just height (`#7447 <https://github.com/GenericMappingTools/gmt/pull/7447>`_)
* :doc:`makecpt`: Add mechanism for passing -Tlist via a file (`#7569 <https://github.com/GenericMappingTools/gmt/pull/7569>`_)
* :doc:`makecpt`, :doc:`grd2cpt`: Let -Fx write hex CPT tables (`#7019 <https://github.com/GenericMappingTools/gmt/pull/7019>`_)
* :doc:`mapproject`: Add -Wm|M to get the rectangular region in projected coordinates (`#7611 <https://github.com/GenericMappingTools/gmt/pull/7611>`_)
* :doc:`mapproject`: Determine bounding box for oblique area (`#7657 <https://github.com/GenericMappingTools/gmt/pull/7657>`_)
* :doc:`movie`: Open and play movie if gmt end show is used (`#7456 <https://github.com/GenericMappingTools/gmt/pull/7456>`_)
* :doc:`plot`: Allow +apangle for decorated lines (`#8182 <https://github.com/GenericMappingTools/gmt/pull/8182>`_)
* :doc:`plot`, :doc:`plot3d`: Allow plot -Z to also control line/polygon transparency (`#7767 <https://github.com/GenericMappingTools/gmt/pull/7767>`_), (`#7765 <https://github.com/GenericMappingTools/gmt/pull/7765>`_)
* :doc:`plot`: Let box-and-whisker symbols take separate width and cap (`#7072 <https://github.com/GenericMappingTools/gmt/pull/7072>`_)
* :doc:`plot`: Treat stroke symbols like other symbols (`#7789 <https://github.com/GenericMappingTools/gmt/pull/7789>`_)
* :doc:`psconvert`: Let -N+kfill set alternate fade fill (`#7972 <https://github.com/GenericMappingTools/gmt/pull/7972>`_)
* :doc:`pslegend`: Let -D+w accept a percentage relative to map width (`#7909 <https://github.com/GenericMappingTools/gmt/pull/7909>`_)
* :doc:`sphdistance`: Check that -Q is given a file (`#7619 <https://github.com/GenericMappingTools/gmt/pull/7619>`_)
* :doc:`triangulate`: Enhance -S with modifier +z so we can write -Zvalue in headers (`#7815 <https://github.com/GenericMappingTools/gmt/pull/7815>`_)
* :doc:`/supplements/seis/meca`, :doc:`/supplements/seis/coupe`: Enhance meca and coupe w.r.t. adjusting positions and connecting with a line (`#7778 <https://github.com/GenericMappingTools/gmt/pull/7778>`_)
* :doc:`/supplements/potential/gravfft`: Clarify what happens  with NaNs (`#6937 <https://github.com/GenericMappingTools/gmt/pull/6937>`_)
* :doc:`/supplements/potential/gravprisms`: Enhance -D with +c for computing contrasts (`#6894 <https://github.com/GenericMappingTools/gmt/pull/6894>`_)
* :doc:`/supplements/potential/gravprisms`: Add +b modifier to -H to boost the seamount height (`#6875 <https://github.com/GenericMappingTools/gmt/pull/6875>`_)
* :doc:`/supplements/potential/gravprisms`: Allow -Drho to be used with -H for density contrasts (`#6837 <https://github.com/GenericMappingTools/gmt/pull/6837>`_)
* :doc:`/supplements/potential/gravprisms`: Handle interfaces (`#6829 <https://github.com/GenericMappingTools/gmt/pull/6829>`_)
* :doc:`/supplements/potential/grdflexure`: Better handle combination of -H and -W (`#6809 <https://github.com/GenericMappingTools/gmt/pull/6809>`_)


Maintenance updates in GMT 6.5
------------------------------

* Allow xxx.yyy.zzz as dataserver format (`#7753 <https://github.com/GenericMappingTools/gmt/pull/7753>`_)
* Simplify handling of ``USERDIR`` and ``CACHEDIR`` (`#7879 <https://github.com/GenericMappingTools/gmt/pull/7879>`_)

Deprecations in GMT 6.5
-----------------------

* :doc:`grdinterpolate`: Fix mistake in using +1|2 instead of +d1|2 (`#6958 <https://github.com/GenericMappingTools/gmt/pull/6958>`_)
* :doc:`psconvert`: Deprecates -N+i for ancient ghostscript versions (`7972 <https://github.com/GenericMappingTools/gmt/pull/7972>`_)

New Features in GMT 6.4
=======================

GMT 6.4 includes a new supplement module, many new features, general code and documentation improvements, and numerous
bug fixes! Here are the general updates included in 6.4:

Colormaps:

* Add :doc:`cmocean colour maps </reference/cpts>` (`#6446 <https://github.com/GenericMappingTools/gmt/pull/6446>`_)
* Let one-liners with CPTs add colorbar (`#6427 <https://github.com/GenericMappingTools/gmt/pull/6427>`_)
* Save any default CPT for remote datasets in hidden grid header (`#6178 <https://github.com/GenericMappingTools/gmt/pull/6178>`_)
* :doc:`grdedit`: Let -D+c allow setting/removing default CPT for grid (`#6223 <https://github.com/GenericMappingTools/gmt/pull/6223>`_)
* :doc:`grdinfo`: Report default CPT for a grid (`#6220 <https://github.com/GenericMappingTools/gmt/pull/6220>`_)

Vector plotting:

* Add optional fraction for terminal shrinking value (`#6115 <https://github.com/GenericMappingTools/gmt/pull/6115>`_)
* Enable plain geovector heads (`#6113 <https://github.com/GenericMappingTools/gmt/pull/6113>`_)
* Further vector scale improvements and enhancements (`#6194 <https://github.com/GenericMappingTools/gmt/pull/6194>`_, `#6197 <https://github.com/GenericMappingTools/gmt/pull/6197>`_)
* Let shrink length for geovector be accepted in any valid map unit (`#6155 <https://github.com/GenericMappingTools/gmt/pull/6155>`_)

Auto-legend:

* New default legend symbol dimensions when no details are provided (`#6165 <https://github.com/GenericMappingTools/gmt/pull/6165>`_)
* :doc:`events`, :doc:`grdvector`: Allow -l for auto-legends (`#6182 <https://github.com/GenericMappingTools/gmt/pull/6182>`_, `#6199 <https://github.com/GenericMappingTools/gmt/pull/6199>`_)

DCW/GSHHG:

* Add --show-gshhg and --show-dcw to gmt (`#6042 <https://github.com/GenericMappingTools/gmt/pull/6042>`_)
* Implement DCW collections and allow full continent, country, and state names in -R -E (`#6064 <https://github.com/GenericMappingTools/gmt/pull/6064>`_)

New Modules in GMT 6.4
----------------------
* :doc:`supplements/potential/gravprisms`: New module for computing geopotential anomalies (`#6445 <https://github.com/GenericMappingTools/gmt/pull/6445>`_)

New Core Module Features in GMT 6.4
-----------------------------------

* :doc:`gmtspatial`: Let -W extend segments from one of both ends (`#6255 <https://github.com/GenericMappingTools/gmt/pull/6255>`_)
* :doc:`grdfill`: Add grid sampling as new algorithm (`#6678 <https://github.com/GenericMappingTools/gmt/pull/6678>`_)
* :doc:`grdmask`: Enhanced node determination for polygon input (`#6289 <https://github.com/GenericMappingTools/gmt/pull/6289>`_)

Supplement updates in GMT 6.4
-----------------------------

* :doc:`supplements/potential/gravprisms`: Add option to save mean prism densities (`#6453 <https://github.com/GenericMappingTools/gmt/pull/6453>`_)
* :doc:`supplements/potential/grdseamount`: Add landslide option -S (`#6548 <https://github.com/GenericMappingTools/gmt/pull/6548>`_)
* :doc:`supplements/potential/grdseamount`, :doc:`supplements/potential/grdflexure`: Seamount density enhancements (`#6430 <https://github.com/GenericMappingTools/gmt/pull/6430>`_)
* :doc:`supplements/x2sys/x2sys_cross`: Add option to exclude crossings from acutely intersection lines (`#6346 <https://github.com/GenericMappingTools/gmt/pull/6346>`_)

Enhancements in GMT 6.4
-----------------------

* :doc:`contour`, :doc:`grdcontour`: Allow contour files to only list contour level (`#6522 <https://github.com/GenericMappingTools/gmt/pull/6522>`_)
* :doc:`gmtmath`: Add operators DEG2KM and KM2DEG (`#6177 <https://github.com/GenericMappingTools/gmt/pull/6177>`_)
* :doc:`grdinfo`: Introduce directives -Mc and -Mf (`#6767 <https://github.com/GenericMappingTools/gmt/pull/6767>`_)
* :doc:`grdimage`: Let -Q+zvalue set the transparent pixel indirectly (`#6232 <https://github.com/GenericMappingTools/gmt/pull/6232>`_)
* :doc:`mapproject`: Add -Wo for oblique domain in degrees (`#6474 <https://github.com/GenericMappingTools/gmt/pull/6474>`_)
* :doc:`mapproject`: Find encompassing rectangular regions for non-oblique projections (`#6669 <https://github.com/GenericMappingTools/gmt/pull/6669>`_)
* :doc:`plot`, :doc:`plot3d`: Allow -SE, -SJ, -SW options to parse appended map units (`#6282 <https://github.com/GenericMappingTools/gmt/pull/6282>`_)
* :doc:`plot`, :doc:`plot3d`: Allow -Sl to plot angled text symbol (`#6290 <https://github.com/GenericMappingTools/gmt/pull/6290>`_)
* :doc:`sample1d`: Better handling of trailing text (`#6098 <https://github.com/GenericMappingTools/gmt/pull/6098>`_)
* :doc:`surface`: Use optimal region under the hood (`#6537 <https://github.com/GenericMappingTools/gmt/pull/6537>`_)
* :doc:`triangulate`: Add ability to read prior triangulation (`#6701 <https://github.com/GenericMappingTools/gmt/pull/6701>`_)
* :doc:`triangulate`: Report triangle areas in -S with new -A (`#6691 <https://github.com/GenericMappingTools/gmt/pull/6691>`_)
* Add two -R shorthands for projected coordinates (`#6094 <https://github.com/GenericMappingTools/gmt/pull/6094>`_)
* Allow use of GMT_GRAPHICS_FORMAT for one-liners (`#6520 <https://github.com/GenericMappingTools/gmt/pull/6520>`_)
* Handle longer strings for netCDF history, remark and title (`#6084 <https://github.com/GenericMappingTools/gmt/pull/6084>`_)
* Let -X -Y also handle multipliers and not just divisors (`#6305 <https://github.com/GenericMappingTools/gmt/pull/6305>`_)

Maintenance updates in GMT 6.4
------------------------------

* Initialize data version control for managing test images (`#5888 <https://github.com/GenericMappingTools/gmt/pull/5888>`_)

Deprecations in GMT 6.4
-----------------------

* :doc:`inset`: Let inset take -C instead of -M (`#6297 <https://github.com/GenericMappingTools/gmt/pull/6297>`_)
* Ensure consistent use of table, t, +t when discussing tables (`#6237 <https://github.com/GenericMappingTools/gmt/pull/6237>`_)
* Switch to using q instead of u for user unit (`#6196 <https://github.com/GenericMappingTools/gmt/pull/6196>`_)

New Features in GMT 6.3
=======================

GMT 6.3 includes a new module, many new features, general code and documentation improvements, and numerous
bug fixes! Here are the general updates included in 6.3:

* Add new -B axis modifier +e to skip annotation(s) at end of an axis (`#5873 <https://github.com/GenericMappingTools/gmt/pull/5873>`_)
* Allow for a fixed label offset (`#5320 <https://github.com/GenericMappingTools/gmt/pull/5320>`_)
* Implement auto resolution for remote grids for plotting modules (`#5753 <https://github.com/GenericMappingTools/gmt/pull/5753>`_)
* Simplify conversions between km and degrees (`#5824 <https://github.com/GenericMappingTools/gmt/pull/5824>`_)

New Modules in GMT 6.3
----------------------

* :doc:`grdselect`: Make selections or determine common regions from 2-D grids, images or 3-D cubes (`#5929 <https://github.com/GenericMappingTools/gmt/pull/5929>`_)

New Core Module Features in GMT 6.3
-----------------------------------

* :doc:`begin`, :doc:`figure`, and :doc:`psconvert`: JPEG quality and recognition of modifiers (`#5574 <https://github.com/GenericMappingTools/gmt/pull/5574>`_)
* :doc:`coast`: Enable country-based clipping in -E (`#5334 <https://github.com/GenericMappingTools/gmt/pull/5334>`_)
* :doc:`coast`: Allow -Ecode+L to list for specific country (`#5744 <https://github.com/GenericMappingTools/gmt/pull/5744>`_)
* :doc:`colorbar`: Add modifier +r to reverse direction of scalebar (`#5230 <https://github.com/GenericMappingTools/gmt/pull/5230>`_)
* :doc:`colorbar`: Allow colorbar length and/or width to be given in percentages (`#5775 <https://github.com/GenericMappingTools/gmt/pull/5775>`_)
* :doc:`colorbar`: Handle CPTs with transparency (`#5328 <https://github.com/GenericMappingTools/gmt/pull/5328>`_)
* :doc:`events`: Support temporal color changes (`#5695 <https://github.com/GenericMappingTools/gmt/pull/5695>`_)
* :doc:`gmtmath`: Improve -Q behavior (`#5362 <https://github.com/GenericMappingTools/gmt/pull/5362>`_)
* :doc:`grd2cpt` and :doc:`makecpt`: Add suitable shorthands for creating month and day labels (`#5201 <https://github.com/GenericMappingTools/gmt/pull/5201>`_)
* :doc:`grd2cpt`: Let -L have ability to only set one limit (`#5317 <https://github.com/GenericMappingTools/gmt/pull/5317>`_)
* :doc:`grd2xyz`: Add option -L to limit output to a single vector (`#5705 <https://github.com/GenericMappingTools/gmt/pull/5705>`_)
* :doc:`grd2xyz`: Let option -T write STL output file (`#5955 <https://github.com/GenericMappingTools/gmt/pull/5955>`_)
* :doc:`grdclip`: Handle polar resampling via -Ar|t (`#5264 <https://github.com/GenericMappingTools/gmt/pull/5264>`_)
* :doc:`grdgradient`: Add +f modifier to -Q to specify file (`#5866 <https://github.com/GenericMappingTools/gmt/pull/5866>`_)
* :doc:`grdimage`: Resample 2nd grid used for intensities (`#5363 <https://github.com/GenericMappingTools/gmt/pull/5363>`_)
* :doc:`grdinfo`: Add ability to specify the two tails separately in -T+a (`#5924 <https://github.com/GenericMappingTools/gmt/pull/5924>`_)
* :doc:`grdmath`: Add CUMSUM operator (`#5239 <https://github.com/GenericMappingTools/gmt/pull/5239>`_)
* :doc:`grdtrack`: Let -C also accept a fixed azimuth for all profiles (`#5849 <https://github.com/GenericMappingTools/gmt/pull/5849>`_)
* :doc:`grdtrend`: Fit model along xx or yy only. (`#5496 <https://github.com/GenericMappingTools/gmt/pull/5496>`_)
* :doc:`inset`: Let begin optionally take -R -J to determine inset size instead of via -D (`#5903 <https://github.com/GenericMappingTools/gmt/pull/5903>`_)
* :doc:`plot`: Add inverted triangle to front symbol choices (`#5983 <https://github.com/GenericMappingTools/gmt/pull/5983>`_)
* :doc:`plot`: For error bars, allow asymmetrical bounds as well (`#5844 <https://github.com/GenericMappingTools/gmt/pull/5844>`_)
* :doc:`plot`, :doc:`plot3d`, and :doc:`clip`: Add polar resampling via -Ar|t (`#5263 <https://github.com/GenericMappingTools/gmt/pull/5263>`_)
* :doc:`project`: Let -G accept increment unit and +n modifier (`#5984 <https://github.com/GenericMappingTools/gmt/pull/5984>`_)
* :doc:`mapproject`: Report rectangular region that covers an obliquely defined region (`#5782 <https://github.com/GenericMappingTools/gmt/pull/5782>`_)
* :doc:`movie`: Let master frame accept a different dpu than the movie frames (`#5198 <https://github.com/GenericMappingTools/gmt/pull/5198>`_)
* :doc:`subplot`: Allow drop-down shade modifier in tag option (`#5333 <https://github.com/GenericMappingTools/gmt/pull/5333>`_)
* :doc:`text`: Implement smart justify -Dj for -M (`#5995 <https://github.com/GenericMappingTools/gmt/pull/5995>`_)

Supplement updates in GMT 6.3
-----------------------------

* :doc:`/supplements/seis/coupe` and :doc:`/supplements/seis/meca`: Add linear moment scaling (`#5649 <https://github.com/GenericMappingTools/gmt/pull/5649>`_)
* :doc:`/supplements/potential/grdflexure`: Let input list of grids and load times optionally also have load density (`#5249 <https://github.com/GenericMappingTools/gmt/pull/5249>`_)
* :doc:`greenspline` and :doc:`/supplements/geodesy/gpsgridder`: Improve histories, update and add animations (`#5734 <https://github.com/GenericMappingTools/gmt/pull/5734>`_)

Enhancements in GMT 6.3
-----------------------

* Add v (vector) to custom symbol kit, plus handle azimuth vs angle (`#5606 <https://github.com/GenericMappingTools/gmt/pull/5606>`_)
* Allow trailing comments in custom symbol files (`#5610 <https://github.com/GenericMappingTools/gmt/pull/5610>`_)
* Let -d set starting column via modifier (`#5966 <https://github.com/GenericMappingTools/gmt/pull/5966>`_)
* Let -f\ *col* explicitly set start of input trailing text (`#5370 <https://github.com/GenericMappingTools/gmt/pull/5370>`_)
* Let multi-layer GeoTIFF files be cut via *gdal_translate* (`#5819 <https://github.com/GenericMappingTools/gmt/pull/5819>`_)

Documentation updates in GMT 6.3
--------------------------------

* Add a GMT team page and update contributor recognition model (`#5589 <https://github.com/GenericMappingTools/gmt/pull/5589>`_)
* Add deprecations section to ReST documentation (`#5847 <https://github.com/GenericMappingTools/gmt/pull/5847>`_)
* Migrate remote dataset documentation to external site (`#6021 <https://github.com/GenericMappingTools/gmt/pull/6021>`_)
* New gallery example 53 showing subplots and shading (`#5336 <https://github.com/GenericMappingTools/gmt/pull/5336>`_)
* Improved animations in the gallery
* Update formatting of usage messages across all modules

Deprecations in GMT 6.3
-----------------------

* Clean up aspects of SVD function usage in GMT (`#5725 <https://github.com/GenericMappingTools/gmt/pull/5725>`_)
* Complete -D parsing update (`#5827 <https://github.com/GenericMappingTools/gmt/pull/5827>`_)
* Consolidate -A into -F for a more unified option (`#5613 <https://github.com/GenericMappingTools/gmt/pull/5613>`_)
* Deprecate the ugly perspective -JG syntax and introduce a modern form (`#5780 <https://github.com/GenericMappingTools/gmt/pull/5780>`_)
* :doc:`greenspline`: Update -C for writing intermediate grids (`#5714 <https://github.com/GenericMappingTools/gmt/pull/5714>`_)
* Let gmt end better handle -A args (`#5583 <https://github.com/GenericMappingTools/gmt/pull/5583>`_)
* Update -g synopsis, syntax, usage and docs (`#5617 <https://github.com/GenericMappingTools/gmt/pull/5617>`_)

New Features in GMT 6.2
=======================

GMT 6.2 includes a new module, new common option, general code and documentation improvements, and numerous
bug fixes! Here are the general updates included in 6.2:

#. Addition of :doc:`theme-settings` (sets of GMT defaults), with a default modern theme for modern mode, and
   :ref:`auto scaling options <reference/features:Automatic GMT settings>` for many GMT defaults.
#. New :doc:`animation 13 <animations/anim13>` of seismic waveforms.
#. New :doc:`animation 14 <animations/anim14>` of earthquake focal mechanisms.
#. Support for **+a**\ *angle* for y-axis as well as x-axis with the :ref:`-B axes settings <gmt:Axes settings>`.
#. General improvements to the automatic determination of frame attributes.
#. New **+d**\ *divisor* modifier to the :ref:`-i option <gmt:The **-i** option>` to simplify scaling of input values.
#. Allow parsing of **-Jz**\ *1:zzzzzz* for vertical scale.
#. New GMT configuration parameters :term:`MAP_FRAME_PERCENT`, :term:`COLOR_SET`, and :term:`COLOR_CPT`.
#. Add support for reading variable in NetCDF-4 groups.
#. Allow specifying the reciprocal increment for generating 1d arrays.
#. Allow LaTeX expressions in single-line titles and Cartesian axes labels, add support for multi-line plot titles, and
   add support for subtitles.
#. Many general documentation improvements.
#. Various improvements to the API in support of developments taking place in the external wrappers (Python, Julia, Matlab).

New Common Options in GMT 6.2:
------------------------------

#. :ref:`-w <gmt:The **-w** option>`: Convert selected coordinate to repeating cycles.

New Modules in GMT 6.2:
-----------------------

#. :doc:`gmtbinstats`: Bin spatial data and determine statistics per bin, with support for both hexagonal and rectangular tiling.

New Core Module Features in GMT 6.2:
------------------------------------

#. :doc:`colorbar`: New **+x** and **+y** modifiers to the **-S** option for setting axis label and unit; Support
   slanted annotations with **-S**.
#. :doc:`events`: New **-Z** option to animate geodesy and seismology symbols; New **-H** option to enable text label
   boxes; Support plotting lines as series of closely spaced circles.
#. :doc:`gmtmath`: New **VPDF** operator.
#. :doc:`gmtsplit`: New name for previous module splitxyz.
#. :doc:`grd2xyz`: New modifier for the **-W** option to set the length unit used.
#. :doc:`grdcut`: New **-F** option to clip a grid based on a polygon.
#. :doc:`grdfft`: New **-Q** option for no wavenumber operations.
#. :doc:`grdmath`: New **FISHER** and **VPDF** operators.
#. :doc:`greenspline`, :doc:`grdinterpolate`: Enable writing of 3-D netCDF data cubes.
#. :doc:`histogram`: New **-E** option for custom bar widths and optional shift; New **+b** modifier to **-C** to set
   color based on the bin value.
#. :doc:`legend`: New **-M** option to handle both hidden and given information.
#. :doc:`makecpt`, :doc:`grd2cpt`: Simplify the addition of category labels to CPT files with **-F**.
#. :doc:`movie`: New modifiers to **-L** and **-P** to enable drop-shadow and rounded rectangular boxes.
#. :doc:`plot`, :doc:`plot3d`: New **-H** option to scale the symbol size as well as the symbol pen outline attributes;
   Support sequential auto-colors for polygon fills or line pens.
#. :doc:`rose`: New **-N** option to draw the equivalent circular normal distribution.
#. :doc:`subplot`: New **-D** option to accept previous default plot settings.
#. :doc:`ternary`: Add support for drawing lines and polygons.
#. :doc:`text`: New **-S** option to cast shade beneath a text box.

Supplement updates in GMT 6.2:
------------------------------

#. :doc:`coupe </supplements/seis/coupe>`: Updated syntax for the **-A** option.
#. :doc:`coupe </supplements/seis/coupe>`, :doc:`meca </supplements/seis/meca>`,
   :doc:`velo </supplements/geodesy/velo>`: New scaling option **-H**; allow variable transparency; allow adjusting
   symbol color via intensity; allow setting symbol color using colormaps.
#. :doc:`gmtgravmag3d </supplements/potential/gmtgravmag3d>`: Add option to create geometric bodies (spheres, prisms,
   ellipsoids, etc.) and compute their effect.
#. :doc:`grdseamount </supplements/potential/grdseamount>`: Add polynomial seamount shape.

Release of GMT 6.1.1
====================

The GMT 6.1.1 release adds no new features but fixes a number of bugs that have been reported
since the release of 6.1. As such, it is a stable and recommended upgrade for all 6.1 users.
For new features in 6.1.x in general, please read the following sections.

New Features in GMT 6.1
=======================

GMT 6.1 may be a minor revision to 6.0 but packs quite a punch. For general
changes, we mention

 #. Updated remote global data sets: Earth reliefs, crustal ages, land/ocean masks, and day/night imagery.
    The larger grid files (5x5 arc minutes and smaller resolutions) are now tiled and faster to download.
 #. Let *gmt.history*, *gmt.conf*, and *gmt.cpt* be hierarchical and maintained
    separately for figures, subplot panels, and insets in modern mode.
 #. Use a list of keywords (*separate,anywhere,lon_horizontal,lat_horizontal,
    tick_extend,tick_normal,lat_parallel*) instead of bit-sum for **MAP_ANNOT_OBLIQUE**.
 #. Let the macOS bundle be built with OpenMP support to accelerate some computational modules.
 #. Let GMT recognize MATLAB headers/comments via multiple **IO_HEADER_MARKER** characters.
 #. Let an explicitly signed grid cross size in **GMTCASE_MAP_GRID_CROSS_SIZE_PRIMARY** or
    **MAP_GRID_CROSS_SIZE_SECONDARY** mean centered (if positive) or asymmetrical (if negative) grid ticks.
 #. Add modifier **+v** for a *vertical* oblique Equator in -JO [horizontal].
 #. New **-B** modifier **+i** for placing internal frame annotations
 #. New **-B** modifier **+f** to turn on fancy geographic annotations.
 #. New polar projection (**-JP**) modifiers  (**+f**\|\ **r**\|\ **t**\|\ **z**) adds new
    capabilities for annotating azimuths, depths or radii.
 #. Revise verbosity default levels and their names and abbreviations.
 #. Add Web-Mercator as new sphere that can be selected.
 #. Explore adding long-format GMT options (e.g., **--region**\ =\ *w/e/s/n*).
 #. Allow both **-i** and **-o** to specify an open-ended list of columns to end of record.
 #. API improvements to support the GMT/MEX, PyGMT, and GMT.jl environments.

New Common Options in GMT 6.1:
------------------------------
 #. **-l**: Add automatic legend entries from the modules :doc:`plot`, :doc:`plot3d`,
    :doc:`grdcontour` and :doc:`pscontour` in modern mode.
 #. **-q**\[**i**\|\ **o**\ ]: Select specific data rows to complement selection of data columns (via **-i**, **-o**).

New Modules in GMT 6.1:
-----------------------

#. :doc:`batch`: Automate batch job processing by replicating a master script with job-specific parameters.
#. :doc:`grdmix`: Blending and transforming grids and images, including manipulating transparency.
#. :doc:`grdinterpolate`: Interpolate new 2-D grids or 1-D data series from a 3-D data cube.
#. :doc:`grdgdal`: Execute `GDAL raster programs <https://gdal.org/programs/index.html#raster-programs>`_
   (such as *gdalinfo*, *gdaldem*, *gdal_grid*, *gdal_translate*, *gdal_rasterize* or *gdalwarp*), from GMT.

New Core Module Features in GMT 6.1:
------------------------------------

#. :doc:`begin`: Ignore the user's *gmt.conf* files normally included by using **-C**.
#. :doc:`colorbar`: Option **-S** has been enhanced to handle bar appearance when **-B** is not used.
#. :doc:`gmtget`: Options **-D**, **-I**, **-N**, and **-Q** handle download and query of remote data sets.
#. :doc:`gmtmath`: New operators **RGB2HSV** and **HSV2RGB** for color manipulation.
#. :doc:`gmtregress`: Let **-A** also be used to limit angles considered for LMS regressions.
#. :doc:`gmtspatial`: New directive **-Sb** computes buffers around lines (via the optional GEOS library).
#. :doc:`gmtvector`: Add vector operator **-Tt** that translates points by given distance in given direction.
#. :doc:`grd2kml`: New option **-W** for adding contour overlays. Also rebuilt for global grids as well as
   to write PNG or JPG directly (depending on transparency) without going via *PostScript* conversion (only
   required if **-W** is used).
#. :doc:`grdcontour`: Better handling of contour file that can now have unique angles and pens per contour.
#. :doc:`grdconvert`: Enable scaling/translation services on output with **-Z**.
#. :doc:`grdfill`: Implement minimum-curvature spline infill with **-As**.
#. :doc:`grdfilter`: Let filter width optionally be a grid with variable widths.
#. :doc:`grdgradient`: Add support for ambient light in **-N**, as in **-E**, and therefore via **-I**
   in :doc:`grdimage` and :doc:`grdview`.
#. :doc:`grdimage`: Now **-I** may take a filename in addition to requests to derive intensities from it.
#. :doc:`grdinfo`: Now **-C** also appends registration and grid type as last two output columns
   (0 = gridline, 1 = pixel registration; 0 = Cartesian, 1 = geographic).
#. :doc:`grdmath`: New operators **DAYNIGHT** (for day/night terminator), **BLEND** (blend two grids using the weights
   from a third), **DOT** (dot product), and **RGB2HSV**, and **HSV2RGB** for color manipulations.
#. :doc:`grdtrack`: Determine central peak in all crossections with **-F** (requires **-C**); let **-E+c** continue
   a track if next line is a direct continuation of previous line.
#. :doc:`grdview`: Now **-I** may take a filename in addition to requests to derive intensities from it.
#. :doc:`pscontour`: Better handling of contour file that can now have unique angles and pens per contour.
#. :doc:`movie`: Add **-E** for an optional title sequence (with or without fading in/out), **-K** for fade in and
   fade out for main animation sequence, **-Sb** and **-Sf** can now take a PostScript layer instead of a script,
   and **-P** for adding one of six progress indicators.
#. :doc:`nearneighbor`: Let **-Nn** call GDAL's nearest neighbor algorithm.
#. :doc:`sample1d`: Adds a smoothing cubic spline via **-Fs**\ *p* (for a fit parameter *p*), with optional weights (**-W**).
#. :doc:`surface`: Let **-D** take a modifier **+z**\ *value* to set a constant breakline level.

Supplement updates in GMT 6.1:
------------------------------
#. *seis*: Update all module syntax to GMT 6 standards and make their i/o more robust.
#. *potential*: :doc:`grdflexure </supplements/potential/grdflexure>` adds new transfer functions now documented with equations.

New Features in GMT 6.0
=======================

GMT 6.0 is a major revision of GMT and its eco-system.  At the top level,
there are numerous changes:

#. An entirely new and permanent address with a brand new website layout and
   organization: https://www.generic-mapping-tools.org/.
#. A new discussion forum at https://forum.generic-mapping-tools.org/.
#. A data server in Hawaii (oceania.generic-mapping-tools.org) with plans
   for new mirror servers around the world.  This is where the remote files
   that start with @ come from.
#. A new way to use GMT (*modern* mode) that eliminates many of the
   aspects of classic GMT that perplexes users.  In modern mode, PostSCript
   is no longer the default graphics output format and most modules that
   had names starting with **ps** have had that prefix removed.  In addition,
   a few modules have entirely different names in modern mode (*psxy* is *plot*,
   *psxyz* is *plot3d*, and *psscale* is *colorbar*).
#. The default mode remains *classic*, the only mode previously available.  All
   existing classic mode GMT 4 and 5 scripts will run as before.

Modern mode modules in GMT 6.0
------------------------------

GMT modern mode is supported by five new commands:

#. :doc:`begin` starts a new GMT modern mode session.
#. :doc:`figure` names a new GMT figure in the current session
#. :doc:`subplot` starts, manages, and ends subplots in a figure.
#. :doc:`inset` starts, manages and ends an inset in a figure or subplot.
#. :doc:`end` ends a GMT modern mode session.

Here, **gmt begin** and **gmt end** begins and ends a modern mode session, hence
it is not possible to get entangled in modern mode if you prefer to run classic
mode scripts.  There are three additional commands that are associated with modern
mode; the first two also work in classic mode since they are typically not useful in scripts:

#. :doc:`docs` gives browser access to any GMT module documentation.
#. :doc:`clear` removes various session files or cached data files.
#. :doc:`movie` simplifies the construction of animated sequences.

The entire GMT Technical Reference, tutorial and gallery examples all use modern mode. In modern mode,
the default graphics format is PDF and scripts can open up the plots in the default
viewer automatically.

New modules in GMT 6.0
----------------------

Apart from modern mode we have added a few modules that are accessible to all users:

#. :doc:`events` makes a snapshot of all time-dependent events.
#. :doc:`/supplements/geodesy/earthtide` (supplement) computes the solid Earth tides.

General improvements in GMT 6.0
-------------------------------

While our focus has been almost exclusively on GMT modern mode, there is a
range of new capabilities have been added to all of GMT; here is a
summary of these changes:

*  The :doc:`gmt` driver has several new options to display the latest GMT citation, DOI,
   the current data server, and the ability to create a blank modern mode shell script or
   DOS batch template.

*  A new common option **-l** lets some modules (currently, only :doc:`plot` and :doc:`plot3d`)
   build an automatic legend. Most legends are now perfectly dimensions and aligned using
   the PostScript language.

*  We now consider untouched pixels when rendering PostScript to be opaque, hence automatic
   cropping to tightest bounding box will recognize areas painted white as different from opaque.

*  We have a much improved scheme for distinguishing between minus-signs and hyphens when typesetting
   text since these are different glyphs in various character sets.

*  Modern mode can produce any of several graphics :ref:`formats <tbl-formats>`. While the default
   is PDF, this can be changed via a new GMT defaults :term:`GMT_GRAPHICS_FORMAT`.
   The conversion from PostScript to the desired format can be modified via another new GMT defaults
   setting :term:`PS_CONVERT`.

*  We have relaxed the *style* syntax for pens so that the :*phase* part is optional, with a default of 0.

*  We have rearranged our supplements a bit: We have split meca to seis and geodesy and moved new module
   :doc:`/supplements/geodesy/earthtide` and existing module :doc:`/supplements/geodesy/gpsgridder` to
   the geodesy supplement.  Also, :doc:`dimfilter` has moved to the core and we have remove the empty misc supplement.

*  In most modules that need to set up an equidistant 1-D array we now use the same machinery to parse
   options and created the arrays through a redesigned **-T** option.  For details on array creation,
   see `Generate 1-D Array`.

*  We have a new GMT common option **-j** that clarifies how to select flat Earth, great circle,
   and geodesic calculations and thus eliminates awkward, sign-based increments.

*  The GMT common option **-r** used to always set pixel-registration for grids but it can now
   take the optional directives **g** or **p** to specify the desired registration.

*  We now offer slanted annotations via the **-B** option, using the modifier **+a**\ *angle*.
   We have added auto-computed annotation and tick intervals for time-axes.  There is also the
   frame specifications **lrbtu** that just draw the corresponding frames without ticking.

*  We offer a wide range of new color tables, including the scientific color maps from Fabio Crameri,
   and we now use Google's *turbo* as the default GMT color table, and *geo* for topographic DEMs.

*  Modules that read data tables can now be given an ESRI shapefile directly.

*  GMT common options **-X** and **-Y** may now be specified using fractions of current plot's
   dimensions.

*  When specifying master CPTs one can add the modifier **+i**\ *dz* to ensure any automatically computed
   range is rounded into multiples of *dz*.

*  Let common option **-a** with no arguments place add all aspatial items to the input record.

*  We have added *dashdot* as a new shorthand style name.

*  Map regions can now be specified via **-R**\ *ISOcode* using the 2-char ISO country codes, with modifiers
   to round the resulting exact regions into multiples of given increments.  Under modern mode, new shorthand
   options **-Re** and **-Ra** will examine the data files given and determine the exact or approximate region,
   respectively.

Module enhancements in GMT 6.0
------------------------------

Several modules have obtained new options to extend their capabilities:

*  :doc:`grdfilter` now accepts the **-r** option to set grid node registration.

*  :doc:`clip` has a new option **-W**\ *pen* to draw the clip path as well as
   setting up clipping.

*  :doc:`plot` takes a new modifier **+s** to **-Sr** to specify a rectangle via opposite
   diagonal corners.  Users can now also specify a color indirectly via a CPT (i.e., **-C**)
   and a new **-Z**\ *value* option (instead of directly via **-G**). The wedge symbol (**-Sw**) has been greatly upgraded to
   offer windshield and spider-graph symbols.  There is now also a new QR code symbol
   that will redirect to the GMT homepage.  We also added a **+h** modifier for quoted lines
   when the user wants to hide the line. Finally, symbols **-SE-**, **-SJ-** and **-SW** can
   now all handle geographic units.

*  :doc:`plot3d` also allows users to specify a color indirectly via a CPT (i.e., **-C**)
   and a new **-Z**\ *value* option.  The wedge symbol (**-Sw**) has been greatly upgraded to
   offer windshield symbols and spider-graph symbols. There is now also a new QR code symbol
   that will redirect to the GMT homepage.

*  :doc:`text` can now handle lack of input files when **-F+c+t** is used to give both a string and
   its placement.

*  In modern mode, both :doc:`makecpt` and :doc:`grd2cpt` require a new option **-H** to actually
   write the resulting CPT to standard output (by default they write a hidden CPT that modern mode
   modules know where to find automatically).  **makecpt** also has a new option **-S** to create a
   symmetric color table given the range in a data file given via **-T**.

*  :doc:`gmtmath` has a new operator **PHI** that computes the
   golden ratio.  We now allow **-Cx** and **-Cy** to represent **-C**\ 0 and **-C** \1.

*  :doc:`grdmath` also has a new operator **PHI** that computes the
   golden ratio, as well as **NODE** and **NODEP** operators, and added more
   OpenMP support for operators **LDISTG**, **PSI**, **TCRIT**, **PLM**, and **PLMg**.

*  :doc:`rose` can now take **-JX** instead of **-S** so all plot modules take **-J**.

*  :doc:`grdedit` can now take **-J** and add meta-data to the grid header.

*  :doc:`gmt2kml` takes new option **-E** to extract altitudes stored in the Extended data property.

*  :doc:`/supplements/seis/polar` and :doc:`/supplements/seis/meca` can let beachball size scale
   with magnitude.  These and other plotting tools in seis can now accept the 3-D projection setting via **-p**.

*  Both :doc:`grdcontour` and :doc:`contour` can now accept a list of comma-separated contours instead
   of always creating equidistant lists. Also, if no contours are specified we auto-compute a reasonable
   selection of 10 to 20 contours.  We also added **-Ln**\|\ **N**\|\ **p**\|\ **P** for selecting
   just negative or positive contours.  Finally, we added modifier **+z** to **-Q** to exclude the zero-contour
   entirely.

*  :doc:`mapproject` has an enhanced option **-W** that can return reference point coordinates.
   Also, either **-J+**\ *proj* or **-J**\ *EPSG*:n can now be given.

*  :doc:`grdproject` also takes **-J**\ +*proj* or **-J**\ *EPSG*:n.

*  :doc:`project` has a new option **-Z** for generating the path of a specified ellipse.

*  :doc:`dimfilter` now writes an error analysis template to standard output via the **-L** option.

*  :doc:`surface` can now apply a data mask computed from the data distribution directly rather than
   having to make separate calls to :doc:`grdmask` and :doc:`grdmath`.  Also, the **-A** option now
   has a directive **m** to select Flat Earth scaling via the mean latitude.

*  The block-modules :doc:`blockmean`, :doc:`blockmedian`, and :doc:`blockmode` have new options
   **-A** and **-G** which allow them to write one or more grids directly.

*  :doc:`gmtinfo` has a new option **-a** which allows it to report aspatial column names, and
   **-Ib** to output the boundary polygon for the data.

*  :doc:`/supplements/spotter/backtracker` can now do reconstruction given individual hotspot
   drift histories.  We also added **-M** for fractional stage rotations.

*  :doc:`/supplements/spotter/grdrotater` has an option **-A** to override region of output grid.

*  :doc:`/supplements/spotter/polespotter` has a new option **-Cx**\ *file*.

*  :doc:`psconvert` has a new option **-H** for automatic sub-pixel rendering and scaling. Under
   modern mode we also have option **-M** for sandwiching a PostScript plot between two other plots.

*  We added modifiers **+a** and **+i** to option **-Z** in :doc:`gmtselect`.

*  :doc:`grdcut` has new option **-ZN** to strip off outside rows and cols that are all NaN.

*  :doc:`grdinfo` now accepts **-o** when **-Cn** is in effect.

*  Enable :doc:`basemap` **-L** to do Cartesian projection scales, even vertical.

*  Improve the vertical scale bar for :doc:`wiggle` as well.

*  :doc:`gmtconvert` has new option **-W** that attempts to convert trailing text to numbers, if possible.
   Append modifier **+n** to suppress NaN columns.  We also added **-N**\ *column*\ [**+a**\|\ **d**] to
   sort a table based on specified *column*.  Finally, **-EM**\ *stride* is similar to **-Em** but it will
   always include the last point.

*  :doc:`grdlandmask` **-E** will trace nodes being positioned exactly on polygon border.

*  :doc:`histogram` can now run in reverse cumulative mode via **-Qr**.

New Features in GMT 5.4
=======================

Between 5.3 and 5.4 we continued to work on the underlying API
needed to support the modules and especially the external interfaces
we are building toward MATLAB, Julia and Python.  We have introduced the use of
static analyzers to fix any code irregularities and we continue to submit
our builds to Coverity for similar reasons.  We have also made an effort
to standardize GMT non-common option usage across the suite.
Nevertheless, there have been many user-level enhancements as well.
Here is a summary of these changes in three key areas:

New modules in GMT 5.4
----------------------

We have added a new module to the GMT core called
:doc:`ternary`.
This module allows for the construction of ternary diagram, currently
restricted to symbols (i.e., a plot-like experience but for ternary data).
The *mgd77* supplement has gained a new tool :doc:`mgd77header <supplements/mgd77/mgd77header>`
for creating a valid MGD77-format header from basic metadata and information
determined from the header-less data file.

General improvements in GMT 5.4
-------------------------------

A range of new capabilities have been added to all of GMT; here is a
summary of these changes:

*  We have added a new lower-case GMT common option.  Programs that read
   ASCII data can use **-e** to only select data records that match a
   specified pattern or regular expression.

*  All modules can now read data via external URL addresses.  This works
   by using libcurl to access an external file and save it to the users'
   GMT cache directory.  This directory can be specified via a new GMT
   defaults called :term:`DIR_CACHE` (and defaults to
   the sub-directory cache under the **$GMT_USERDIR** directory [~/.gmt]).
   Subsequent use of the same URL will be read from the cache (except
   if explicitly removed by the user).  An exception is CGI Get Commands
   which will be executed anew each time. Both the user directory and
   the cache directory will be created if they do not exist.

*  Any reference to Earth topographic/bathymetric relief files called
   **@earth_relief_**\ *res*\ **.grd** will automatically obtain the grid
   from the GMT data server.  The resolution *res* allows a choice among
   13 command grid spacings: 60m, 30m, 20m, 15m, 10m, 06m, 05m, 04m, 03m, 02m,
   01m, 30s, and 15s (with file sizes 111 kb, 376 kb, 782 kb, 1.3 Mb, 2.8 Mb,
   7.5 Mb, 11 Mb, 16 Mb, 27 Mb, 58 Mb, 214 Mb, 778 Mb, and 2.6 Gb respectively).
   Once one of these have been downloaded any future reference will simply
   obtain the file from **$GMT_USERDIR** (except if explicitly
   removed by the user).

*  We are laying the groundwork for more dynamic documentation.  At present,
   the examples on the man pages (with the exception of *basemap* and *coast*)
   cannot be run by cut and paste since they reference imaginary data sets.
   These will soon appear with filenames starting in @ (e.g., @hotspots.txt),
   and when such files are found on the command line it is interpreted to be
   a shorthand notation for the full URL to the GMT cache data server.

*  We have added four new color tables inspired by matplotlib to the collection.
   These CPTs are called plasma, magma, inferno, and viridis.

*  We have updated the online documentation of user-contributed custom symbols and
   restored the beautiful biological symbols for whales and dolphins created by
   Pablo Valdés during the GMT4 era. These are now complemented by new custom
   symbols for structural geology designed by José A. Álvarez-Gómez.

*  The :doc:`PSL </devdocs/postscriptlight>` library no longer needs run-time files to configure the
   list of standard fonts and character encodings, reducing the number of configure
   files required.

*  The :doc:`gmt.conf` files produced by gmt set will only write parameters that differ
   from the GMT SI Standard settings.  This means most gmt.conf files will just
   be a few lines.

*  We have deprecated the **-c**\ *copies* option whose purpose was to modify the
   number of copies a printer would issue give a PostScript file.  This is better
   controlled by your printer driver and most users now work with PDF files.

*  The **-p** option can now do a simple rotation about the z-axis (i.e., not a
   perspective view) for more advanced plotting.

*  The placement of color scales around a map can now be near-automatic, as
   the **-DJ** setting now has many default values (such as for bar length,
   width, offsets and orientation) based on which side you specified.  If you
   use this option in concert with **-B** to turn off frame annotation on the
   side you place the scale bar then justification works exactly.

*  The **-i** option to select input columns can now handle repeat entries,
   e.g., -i0,2,2,4, which is useful when a column is needed as a coordinate
   *and* for symbol color or size.

*  The vector specifications now take one more modifier: **+h**\ *shape*
   allows vectors to quickly set the head shape normally specified via
   :term:`MAP_VECTOR_SHAPE`.  This is particularly useful
   when the symbol types are given via the input file.

*  The custom symbol macro language has been strengthened and now allows all
   angular quantities to be variables (i.e., provided from your data file as
   extra columns), the pen thickness can be specified as relative (and thus
   scale with the symbol size at run-time), and a symbol can internally switch
   colors between the pen and fill colors given on the command line.

*  We have reintroduced the old GMT4 polygon-vector for those who fell so hard
   in love with that symbol.  By giving old-style vector specifications you
   will now get the old-style symbol.  The new and superior vector symbols
   will require the use of the new (and standard) syntax.

Module enhancements in GMT 5.4
------------------------------

Several modules have obtained new options to extend their capabilities:

*  :doc:`gmt` has new session management option that lets you clear various
   files and cache directories via the new commands
   **gmt clear** *all*\|\ *history*\|\ *conf*\|\ *cache*.

*  :doc:`gmt2kml` adds option **-Fw** for drawing wiggles along track.

*  :doc:`gmtinfo` adds option **-F** for reporting the number of tables,
   segments, records, headers, etc.

*  :doc:`gmtmath` will convert all plot dimensions given on the command line
   to the prevailing length unit set via :term:`PROJ_LENGTH_UNIT`.
   This allows you to combine measurements like 12c, 4i, and 72p. The module
   also has a new **SORT** operator for sorting columns and **RMSW** for weighted
   root-mean-square.

*  :doc:`gmtwhich` **-G** will download a file from the internet (as discussed
   above) before reporting the path to the file (which will then be in the
   user's cache directory).

*  :doc:`grd2xyz` can now write weights equal to the area each node represents
   via the **-Wa** option.

*  :doc:`grdgradient` can now take a grid of azimuths via the **-A** option.

*  :doc:`grdimage` and :doc:`grdview` can now auto-compute the intensities
   directly from the required input grid via **-I**, and this option now
   supports modifiers **+a** and **+n** for changing the options of the
   grdgradient call within the module.

*  :doc:`grdinfo` adds option **-D** to determine the regions of all the
   smaller-size grid tiles required to cover the larger area.  It also take
   a new argument **-Ii** for reporting the exact region of an img grid.
   Finally, we now report area-weighted statistics for geographic grids,
   added **-Lp** for mode (maximum-likelihood) estimate of location and scale,
   and **-La** for requesting all of the statistical estimates.

*  :doc:`grdmath` has new operators **TRIM**, which will set all grid values
   that fall in the specified tails of the data distribution to NaN, **NODE**,
   which will create a grid with node indices 0 to (nx*ny)-1, and **RMSW**,
   which will compute the weighted root-mean-square.

*  :doc:`makecpt` and :doc:`grd2cpt` add option **-Ws** for producing
   wrapped (cyclic) CPT tables that repeat endlessly.  New CPT keyword
   **CYCLIC** controls if the CPT is cyclic.

*  :doc:`mapproject` adds a new **-Z** option for temporal calculations based
   on distances and speeds, and has been redesigned to allow several outputs
   by combining the options **-A**, **-G**, **-L**, and **-Z**.

*  :doc:`basemap` has a new map-inset (**-D**) modifier **+t** that will
   translate the plot origin after determining the lower-left corner of the
   map inset.

*  :doc:`histogram` has a new **-Z** modifier **+w** that will
   accumulate weights provided in the 2nd input column instead of pure counts.

*  :doc:`rose` adds option **-Q** for setting the  confidence level used
   for a Rayleigh test for uniformity of direction.  The **-C** option also
   takes a new modifier **+w**\ *modfile* for storing mode direction to file.

*  :doc:`gmt_shell_functions.sh` adds numerous new functions to simplify the
   building of animation scripts, animated GIF and MP4 videos, launching
   groups of jobs across many cores, packaging KMLs into a single KMZ archive,
   and more.

API changes in GMT 5.4
----------------------

We have introduced one change that breaks backwards compatibility for users of
the API functions.  We don't do this lightly but given the API is still considered
beta it was the best solution.  Function GMT_Create_Data now requires the mode to
be **GMT_IS_OUTPUT** (an new constant) if a dummy (empty) container should be
created to hold the output of a module.  We also added two new API functions
GMT_Duplicate_Options and GMT_Free_Option to manage option lists, and added
the new constants **GMT_GRID_IS_CARTESIAN** and **GMT_GRID_IS_GEO** so that
external tools can communicate the nature of grid written in situations where there
are no projections involved (hence GMT does not know a grid is geographic).
Passing this constant will be required in MB-System.

Backwards-compatible syntax changes
-----------------------------------

We strive to keep the GMT user interface consistent.  The common options help
with that, but the module-specific options have often used very different
forms to achieve similar goals.  We have revised the syntax of numerous options
across the modules to use the common *modifier* method.  However, as no GMT
users would be happy that their
scripts no longer run, these changes are backwards compatible.  Only the new
syntax will be documented but old syntax will be accepted.  Some options are
used across GMT and will get a special mention first:

*  Many modules use **-G** to specify the fill (solid color or pattern).
   The pattern specification has now changed to be
   **-Gp**\|\ **P**\ *pattern*\ [**+b**\ *color*][**+f**\ *color*][**+r**\ *dpi*]

*  When specifying grids one can always add information such as grid type, scaling,
   offset, etc.  This is now done using a cleaner syntax for grids:
   gridfile[=\ *ID*\ [**+s**\ *scale*][**+o**\ *offset*][**+n**\ *invalid*]].

Here is a list of modules with revised options:

*  :doc:`grdcontour` now expects **-Z**\ [**+**\ *scale*][**+o**\ *offset*][**+p**].

*  In :doc:`grdedit` and :doc:`xyz2grd`, the mechanism to change a grid's
   metadata is now done via modifiers to the **-D** option, such as
   **+x**\ *xname*, **+t**\ *title*, etc.

*  :doc:`grdfft` has changed to **-E**\ [**+w**\ [**k**]][**+n**].

*  :doc:`grdgradient` modifies the syntax of **-E** and **-N** by introducing modifiers,
   i.e., **-E**\ [**m**\|\ **s**\|\ **p**]\ *azim/elev*\ [**+a**\ *ambient*][**+d**\ *diffuse*][**+p**\ *specular*][**+s**\ *shine*] and
   **-N**\ [**e**\|\ **t**][*amp*][**+s**\ *sigma*][**+o**\ *offset*].

*  :doc:`grdtrend` follows :doc:`trend1d` and now wants **-N**\ *model*\ [**+r**].

*  :doc:`mapproject` introduces new and consistent syntax for **-G** and **-L** as
   **-G**\ [*lon0*/*lat0*][**+a**][**+i**][**+u**\ [**+**\|\ **-**]\ *unit*][**+v**] and
   **-L**\ *line.xy*\ [**+u**\ [**+**\|\ **-**]\ *unit*][**+p**].

*  :doc:`project` expects **-G**\ *inc*\ [/*lat*][**+h**].

*  :doc:`rose` now wants **-L**\ [*wlabel*\ ,\ *elabel*\ ,\ *slabel*\ ,\ *nlabel*] to
   match the other labeling options.

*  :doc:`text` now expects **-D**\ [**j**\|\ **J**]\ *dx*\ [/*dy*][**+v**\ [*pen*]].

*  :doc:`plot` expects **-E**\ [**x**\|\ **y**\|\ **X**\|\ **Y**][**+a**][**+cl**\|\ **f**][**+n**][**+w**\ *cap*][**+p**\ *pen*].

*  :doc:`trend2d` follows :doc:`trend1d` and now wants **-N**\ *model*\ [**+r**].


New Features in GMT 5.3
=======================

Between 5.2 and 5.3 we spent much time working on the underlying API
needed to support the modules and especially the external interfaces
we are building toward MATLAB and Python.  Nevertheless, there have
been many user-level enhancements as well.
Here is a summary of these changes in three key areas:

New modules in GMT 5.3
----------------------

We have added a new module to the GMT core called
:doc:`solar`.
This module plots various day-light terminators and other sunlight parameters.

Two new modules have been added to the *spotter* supplement:
The first is :doc:`gmtpmodeler<supplements/spotter/gmtpmodeler>`.
Like :doc:`grdpmodeler<supplements/spotter/grdpmodeler>` it evaluates plate
tectonic model predictions but at given point locations locations instead of
on a grid.  The second is :doc:`rotsmoother<supplements/spotter/rotsmoother>`
which smooths estimated rotations using quaternions.

Also, the *meca* supplement has gained a new tool :doc:`sac <supplements/seis/sac>`
for the plotting of seismograms in SAC format.

Finally, we have added :doc:`gpsgridder<supplements/geodesy/gpsgridder>`
to the *potential* supplement.  This tool is a Green's function gridding module
that grids vector data assumed to be coupled via an elastic model.  The prime
usage is for gridding GPS velocity components.

General improvements in GMT 5.3
-------------------------------

There are many changes to GMT, mostly under the hood, but also changes that
affect users directly.  We have added four new examples and one new animation
to highlight recently added capabilities.  There have been many bug fixes
as well. For specific enhancements, we have:

*  All GMT-distributed color palette tables (CPTs, now a total of 44) are
   *dynamic* and many have a *hinge* and a default *range*.  What this means
   is that the range of all CPTs have been normalized to 0-1, expect that
   those with a hinge are normalized to ±1, with 0 being the normalized
   hinge location.  CPTs with a hinge are interpolated separately on either
   side of the hinge, since a hinge typically signifies a dramatic color
   change (e.g., at sea-level) and we do not want that color change to be
   shifted to some other *z*-value when an asymmetrical range is being
   requested.  In situations where no range is specified then some CPTs
   will have a default range and that will be substituted instead.  The
   tools :doc:`makecpt` and :doc:`grd2cpt` now displays more meta-data
   about the various CPTs, including values for hinge, range, and the
   color-model used.

*  We have consolidated how map embellishments are specified.  This group
   includes map scales, color bars, legends, map roses, map insets,
   image overlays, the GMT logo, and a background panel.  A new section in the Technical Reference is
   dedicated to these items and how they are specified.  Common to all is
   the concept of a *reference point* relative to which the item is
   *justified* and *offset*.

*  We continue to extend support for OpenMP in GMT.  New modules that are
   OpenMP-enabled are :doc:`grdgradient`, :doc:`grdlandmask`, and :doc:`sph2grd`.

*  :doc:`blockmean`, :doc:`blockmedian` and :doc:`blockmode` have a new
   modifier **+s** to the **-W** option.  When used we expect 1-sigma
   uncertainties instead of weights and compute weight = 1/sigma.

*  :doc:`filter1d`: can now compute high-pass filtered output via a new
   **+h** modifier to the filter settings, similar to existing capability
   in :doc:`grdfilter`.

*  :doc:`gmtconvert` has a new option (**-F**) for line segmentation and
   network configuration. Also, the **-D** option has a new modifier **+o**
   that sets the origin used for the numbering of tables and segments.

*  :doc:`gmtinfo` has a new option **-L** for finding the common bounds
   across multiple files or segments.  Also, the **-T** option has been
   modified (while still being backwards compatible) to allow *dz* to be
   optional, with modifiers **+s** forcing a symmetric range and **+a**
   offering *alpha*-trimming of the tails before estimating the range.

*  :doc:`gmtmath` has gained new operators **VAR**,
   **RMS**, **DENAN**, as well as the weighted statistical operators
   **LMSSCLW**, **MADW**, **MEANW**, **MEDIANW**, **MODEW**, **PQUANTW**,
   **STDW**, and **VARW**.  Finally, we added a **SORT** operator that lets
   you sort an entire table in ascending or descending order based on the
   values in a selected column.

*  :doc:`gmtselect` has a new option **-G** for selecting based on a mask grid.
   Points falling in bins whose grid nodes are non-zero are selected (or not if **-Ig**)

*  :doc:`gmtspatial` has two new modifiers for the **-Q** option that allow
   output segments to be limited based on the segment length (or area for
   polygons) as well as sorting the output in ascending or descending order.

*  :doc:`grd2cpt` existing **-F** option now takes a new modifier **+c**
   for writing a discrete palette using the categorical format.

*  :doc:`grdedit` can now reset text items in the header via **-D** by
   specifying '-'.  Also, new **-C** option can be used to reset the
   command history in the header.

*  :doc:`grdfft` has a new modifier to the **-E** option that allows for more
   control of the power normalization for radial spectra.

*  :doc:`grdmath` also has the new operators **VAR**,
   **RMS**, **DENAN**, as well as the weighted statistical operators
   **LMSSCLW**, **MADW**, **MEANW**, **MEDIANW**, **MODEW**, **PQUANTW**,
   **STDW**, and **VARW**.  In addition it gains a new
   **AREA** operator which computes the gridcell area (in km\ :sup:`2` if the
   grid is geographic).  Finally, operators **MEAN**, **MEDIAN**, etc.,
   when working on a geographic grid, will weight the result using the
   **AREA** function for proper spherical statistics.

*  :doc:`grdvolume` can now accept **-Cr**\ *cval* which will evaluate
   the volume between *cval* and the grid's minimum value.

*  :doc:`greenspline` now offers a new **-E** option that evaluates the
   model fit at the input data locations and optionally saves the model
   misfits to a secondary output file.

*  :doc:`makecpt` can also let you build either a discrete or continuous custom
   color palette table from a comma-separated list of colors and
   *z*-values provided via a file, an equidistant setup, or comma-separated list.
   The **-F** option now takes a new modifier **+c** for writing a discrete
   palette using the categorical format.

*  :doc:`text` has new modifiers to its **-F** option that allows users
   to generate automatic labels such as record numbers of formatting of a
   third data column into a textual representation.  We also allow any
   baseline angles to be interpreted as *orientations*, i.e., they will be
   modified to fall in the -90/+90 range when **-F**\ ...\ **+A** is set.

*  :doc:`rose` can now control the attributes of vectors in a windrose
   diagram via **-M**.

*  :doc:`plot` have seen numerous enhancements.  New features include
   *decorated* lines, which are similar to quoted lines except we place
   symbols rather than text along the line.  Users also gain new controls
   over the plotting of lines, including the ability to add vector heads
   to the line endings, to trim back lines by specified amounts, and to
   request a Bezier spline interpolation in PostScript (see enhanced
   **-W** option).  A new option (**-F**) for line segmentation and networks
   have also been added. Various geographic symbols (such as ellipses; **-SE**,
   rotatable rectangles **-SJ**; and geo-vectors **-S=**) can now take size in geographic
   dimensions, including a new geo-wedge symbol.  We also offer one more
   type of fault-slip symbol, using curved arrow heads.  Also the arrow
   head selections now include inward-pointing arrows.  Custom symbols
   may now simply be a preexisting EPS figure.  Many of these enhancements
   are also available in :doc:`plot3d`.

*  The spotter supplement now comes with the latest rotation files from
   EarthByte, U. of Sydney.


The API
-------

We have spend most of our time strengthening the API, in particular in support
of the GMT/MATLAB toolbox.  A few new API functions have been added since the
initial release, including GMT_Get_Pixel, GMT_Set_Index, GMT_Open_VirtualFile,
GMT_Close_VirtualFile, GMT_Read_VirtualFile, GMT_Read_Group, and GMT_Convert_Data;
see the API :ref:`api` for details.


New Features in GMT 5.2
=======================

While the GMT 5.1-series has seen bug-fixes since its release, new features were
only added to the 5.2-series.  All in all, almost 200 new features (a combination
of new programs, new options, and enhancements) have been implemented.
Here is a summary of these changes in six key areas:

New modules in GMT 5.2
----------------------

There are two new modules in the core system:

:doc:`gmtlogo` is modeled after the shell script with the same
name but is now a regular C module that can be used to add the
GMT logo to maps and posters.

:doc:`gmtregress` determines linear regressions for data sets using
a variety of misfit norms and regression modes.

Four new modules have also been added to the *potential* supplement:

:doc:`gmtflexure <supplements/potential/gmtflexure>`:
	Compute the elastic flexural response to a 2-D (line) load.

:doc:`grdflexure <supplements/potential/grdflexure>`:
	Compute the flexural response to a 3-D (grid) load, using a variety
	or rheological models (elastic, viscoelastic, firmoviscous).

:doc:`talwani2d <supplements/potential/talwani2d>`:
	Compute a profile of the free-air gravity, geoid or vertical gravity gradient anomaly
	over a 2-D body given as cross-sectional polygons.

:doc:`talwani3d <supplements/potential/talwani3d>`:
	Compute a grid or profile of the free-air gravity, geoid or vertical gravity gradient anomaly
	over a 3-D body given as horizontal polygonal slices.

In addition, two established modules have been given more suitable names
(however, the old names are still recognized):

:doc:`grdconvert`
    Converts between different grid formats.
    Previously known as grdreformat (this name is recognized
    when GMT is running in compatibility mode).

:doc:`psconvert`
    Converts from PostScript to PDF, SVG, or various raster image formats.
    Previously known as ps2raster (this name is recognized
    when GMT is running in compatibility mode).

Finally, we have renamed our PostScript Light (PSL) library from psl
to PostScriptLight to avoid package name conflicts.  This library will eventually
become decoupled from GMT and end up as a required prerequisite.

New common options in GMT 5.2
-----------------------------

We have added two new lower-case GMT common options:

*  Programs that need to specify which values should represent "no data"
   can now use **-d**\ [**i**\|\ **o**]\ *nodata*. For instance, this
   option replaces the old **-N** in :doc:`grd2xyz` and :doc:`xyz2grd`
   (but is backwards compatible).

*  Some modules are now using OpenMP to spread computations over all
   available cores (only available if compiled with OpenMP support).
   Those modules will offer the new option **-x**\ [[-]\ *n*] to reduce
   how many cores to assign to the task.  The modules that currently
   have this option are :doc:`greenspline`, :doc:`grdmask`, :doc:`grdmath`,
   :doc:`grdfilter`, :doc:`grdsample`, :doc:`sph2grd`, the potential supplement's
   :doc:`grdgravmag3d <supplements/potential/grdgravmag3d>`,
   :doc:`talwani2d <supplements/potential/talwani2d>` and
   :doc:`talwani3d <supplements/potential/talwani3d>`, and the x2sys
   supplement's :doc:`x2sys_solve <supplements/x2sys/x2sys_solve>`.
   This list will grow longer with time.

New default parameters in GMT 5.2
---------------------------------

There have been a few changes to the GMT Defaults parameters.  All changes
are backwards compatible:

*  :term:`FORMAT_FLOAT_MAP` now allows the use %'g to get comma-separated groupings
   when integer values are plotted.

*  :term:`FORMAT_FLOAT_OUT` can now accept a space-separated list of formats
   as shorthand for first few columns.  On output it will show the formats
   in effect for multiple columns.

*  :term:`GMT_LANGUAGE` has replaced the old parameter **TIME_LANGUAGE**.
   Related to this, the files share/time/\*.d have been moved and renamed to
   share/localization/\*.txt and now include a new section
   or cardinal points letter codes.

*  :term:`IO_SEGMENT_BINARY` is a new parameter that controls how binary records
   with just NaNs should or should not be interpreted as segment headers.

*  :term:`PROJ_GEODESIC` was added to control which geodesic calculation should be
   used.  Choose among Vincenty [Default], Andoyer (fast approximate geodesics),
   and Rudoe (from GMT4).

*  :term:`TIME_REPORT` now has defaults for absolute or elapsed time stamps.

Updated common options in GMT 5.2
---------------------------------

Two of the established GMT common options have seen minor improvements:

*  Implemented modifier **-B+n** to *not* draw the frame at all.

*  Allow oblique Mercator projections to select projection poles in the
   southern hemisphere by using upper-case selectors **A**\|\ **B**\|\ **C**.

*  Added a forth way to specify the region for a new grid via the new
   **-R**\ [**L**\|\ **C**\|\ **R**][**T**\|\ **M**\|\ **B**]\ *x0*/*y0*/*nx*/*ny*
   syntax where you specify an reference point and number of points in the two
   dimensions (requires **-I** to use the increments).  The optional justification
   keys specify how the reference point relate to the grid region.

General improvements in GMT 5.2
-------------------------------

Several changes have affects across GMT; these are:

*  Added optional multi-threading capabilities to several modules, such as
   :doc:`greenspline`, :doc:`grdfilter`, :doc:`grdmask`, :doc:`grdsample`,
   the potential supplement's :doc:`grdgravmag3d <supplements/potential/grdgravmag3d>`,
   :doc:`talwani2d <supplements/potential/talwani2d>` and
   :doc:`talwani3d <supplements/potential/talwani2d>` and x2sys's :doc:`x2sys_solve <supplements/x2sys/x2sys_solve>`.

*  Optional prerequisite LAPACK means SVD decomposition in :doc:`greenspline` is
   now very fast, as is true for the regular Gauss-Jordan solution via a
   new multi-processor enabled algorithm.

*  Allow comma-separated colors instead of CPTs in options that are
   used to pass a CPT (typically this means the **-C** option).

*  Faster netCDF reading for COARDS table data (i.e., not grids).

*  When importing grids via GDAL the projection info is preserved and stored as netCDF metadata.
   This will allow third party programs like GDAL and QGIS to recognize the projection info of
   GMT created grids. Same thing happens when creating grids with :doc:`grdproject`.

*  Tools using GSHHG can now access information for both Antarctica data
   sets (ice-front and grounding line).

*  Tools that specify pens may now explicitly choose "solid" as an attribute,
   and we added "dashed" and "dotted" as alternatives to the shorthands "-" and ".".

*  Added three alternative vector head choices (terminal, square and circle) in addition
   to the default "arrow" style. We have also added the option for trimming the
   beginning and/or end point location of a vector, and you may now place the
   vector head at the mid-point of the vector instead at the ends.

*  All eight map embellishment features (map scale, color bar, direction rose, magnetic
   rose, GMT logo, raster images, map insets, and map legends) now use a uniform
   mechanism for specifying placement, justification, and attributes and is supported
   by a new section in the documentation.

*  Typesetting simultaneous sub- and super-scripts has improved (i.e., when a symbol
   should have both a subscript and and a superscript).

*  The custom symbol macro codes now allow for an unspecified symbol code (**?**), which
   means the desired code will be given as last item on each data record.  Such custom
   symbols must be specified with uppercase **-SK**.

Program-specific improvements in GMT 5.2
----------------------------------------

Finally, here is a list of enhancements to individual modules.  Any
changes to existing syntax will be backwards compatible:

*  :doc:`fitcircle` now has a new option **-F** that allows output to be in the
   form of coordinates only (no text report) and you may choose which items to
   return by appending suitable flags.

*  :doc:`gmt` now has a --show-cores option that reports the available cores.

*  :doc:`gmtconvert` adds a **-C** option that can be used to eliminate
   segments on output based on the number of records it contains.  We also
   added a **-F** option to create line segments from an input data sets using
   a variety of connectivity modes.

*  :doc:`gmtmath` adds a long list of new operators.  We have the operator **BPDF** for binomial probability distribution and
   **BCDF** for the cumulative binomial distribution function.  Due to confusion with
   other probability distributions we have introduced a series of new operator names
   (but honor backwards compatibility).  Listing the pdf and cdf for each distribution,
   these are **TPDF** and **TCDF** for the Student t-distribution,
   **FPDF** and **FCDF** for the F-distribution,
   **CHI2PDF** and **CHI2CDF** for the Chi-squared distribution,
   **EPDF** and **ECDF** for the exponential distribution (as well as **ECRIT**),
   **PPDF** and **PCDF** for the Poisson distribution,
   **LPDF** and **LCDF** for the Laplace distribution (as well as **LCRIT**),
   **RPDF** and **RCDF** for the Rayleigh distribution (as well as **RCRIT**),
   **WPDF** and **WCDF** for the Weibull distribution (as well as **WCRIT**), and
   **ZPDF** and **ZCDF** for the Normal distribution. We added **ROLL** for cyclic shifts of the stack,
   and **DENAN** as a more intuitive operator for removing NaNs, as
   well as new constants **TRANGE**, **TROW**, **F_EPS** and **D_EPS**, and we have renamed the
   normalized coordinates from **Tn** to **TNORM** (but this is backwards compatible).  We added
   operator **POINT** which reads a data table and places the mean x and mean y on the stack.
   Finally, we added new hyperbolic and inverse hyperbolic functions **COTH** and **ACOTH**,
   **SECH** and **ASECH**, and **CSCH** and **ACSCH**.

*  :doc:`gmtspatial` now lets you specify Flat Earth or Geodesic distance calculations
   for line lengths via **-Q**.

*  :doc:`grdblend` relaxes the **-W** restriction on only one output grid
   and adds the new mode **-Wz** to write the weight*zsum grid.

*  :doc:`grdedit` enhances the **-E** option to allow for 90-degree rotations
   or flips of grid, as well as a new **-G** to enable writing of the result
   to a new output file [Default updates the existing file]. The **-J** option
   saves the georeferencing info as metadata in netCDF grids.

*  :doc:`grdfilter` now includes histogram mode filtering to complement mode
   (LMS) filtering.

*  :doc:`grdgradient` adds **-Da** to compute the aspect (down-slope) direction [up-slope].

*  :doc:`grdinfo` reports the projection info of netCDF grids when that is stored in
   a grid's metadata in WKT format.

*  :doc:`grdmath` adds numerous new operators, such as **ARC** and **WRAP** for
   angular operators, **BPDF** for binomial probability distribution and
   **BCDF** for the cumulative binomial distribution function.  Due to confusion with
   other probability distributions we have introduced a series of new operator names
   (but accept backwards compatibility).  Listing the pdf and cdf for each distribution,
   these are **TPDF** and **TCDF** for the Student t-distribution,
   **FPDF** and **FCDF** for the F-distribution,
   **CHI2PDF** and **CHI2CDF** for the Chi-squared distribution,
   **EPDF** and **ECDF** for the exponential distribution (as well as **ECRIT**),
   **PPDF** and **PCDF** for the Poisson distribution,
   **LPDF** and **LCDF** for the Laplace distribution (as well as **LCRIT**),
   **RPDF** and **RCDF** for the Rayleigh distribution (as well as **RCRIT**),
   **WPDF** and **WCDF** for the Weibull distribution (as well as **WCRIT**), and
   **ZPDF** and **ZCDF** for the Normal distribution.  We added **LDISTG** (for distances
   to GSHHG), **CDIST2** and **SDIST2**
   (to complement **LDIST2** and **PDIST2**), and **ROLL** for cyclic shifts of the stack,
   and **DENAN** as a more intuitive operator for removing NaNs,
   while **LDIST1** has been renamed
   to **LDISTC**.  We also add new constants **XRANGE**, **YRANGE**, **XCOL**,
   **YROW** and **F_EPS**, and we have renamed the normalized coordinates from **Xn** to **XNORM**
   and **Yn** to **YNORM** (but this is backwards compatible).
   Finally, we added new hyperbolic and inverse hyperbolic functions **COTH** and **ACOTH**,
   **SECH** and **ASECH**, and **CSCH** and **ACSCH**.

*  :doc:`grdtrack` add the modifier **-G+l**\ *list* to pass a list of grids.

*  :doc:`grdview` implements the Waterfall plot mode via **-Qmx**\|\ **y**.

*  :doc:`kml2gmt` acquires a **-F** option to control which geometry to output.

*  :doc:`makecpt` takes **-E** to determine range from an input data table.

*  :doc:`mapproject` can be used in conjunction with the 3-D projection options to
   compute 3-D projected coordinates.  We also added **-W** to simply output the
   projected dimensions of the plot without reading input data.

*  :doc:`basemap` now takes **-A** to save the plot domain polygon in geographical coordinates.
   The **-L** option for map scale and **-T** for map roses have been revised (backwards compatible) and a
   new uniform **-F** option to specify background panel and its many settings was added.

*  :doc:`coast` can accept multiple **-E** settings to color several features independently.
   We also have the modifiers **+AS** to *only* plot Antarctica, **+ag** to use
   shelf ice grounding line for Antarctica coastline, and **+ai** to use ice/water
   front for Antarctica coastline [Default].  As above, the **-L** option for map scale
   and **-T** option for map roses have been revised (backwards compatible) and a new uniform **-F** option to specify
   background panel and its many settings was added.

*  :doc:`psconvert` (apart from the name change) has several new features, such as
   reporting dimensions of the plot when **-A** and **-V** are used,
   scaling the output plots via **-A+s**\ [**m**]\ *width*\ [/*height*],
   paint and outline the bounding box via **-A** modifiers **g**\ *fill* and **+p**\ *pen*,
   and **-Z** for removing the PostScript file on exit.  In addition, we have
   added SVG as a new output vector graphics format and now handle transparency even if
   non-PDF output formats are requested.

*  :doc:`contour` adds a **-Q**\ *cut* option like :doc:`grdcontour` and consolidates the
   old **-T**, **-Q** options for an index file to a new **-E** option.

*  :doc:`histogram` added modifiers **-W**\ *width*\ [**+l**\|\ **h**\|\ **b**]
   to allow for more control on what happens to points falling in the tails.

*  :doc:`image` added a new uniform **-D** option to specify location of the image and new uniform
   **-F** option to specify background panel and its many settings.

*  :doc:`legend` has many enhancements for specifying varying cell widths and color, as
   well as a new uniform **-D** option to specify location of legend and new uniform
   **-F** option to specify background panel and its many settings.

*  :doc:`colorbar` new uniform **-D** option to specify location of the scale. We have
   retired the **-T** option in favor of the new uniform
   **-F** option to specify background panel and its many settings.

*  :doc:`plot` has seen considerable enhancements. We added two new quoted
   line (**-Sq**) modifiers: **S**\|\ **s** for treating input as consecutive
   two-point line segments that should be individually quoted,
   and **+x**\ [*first*\ ,\ *last*] for automating cross-section labeling.
   We added a new symbol (**-S~**) for *decorated lines*.  These are very similar
   to quoted lines but instead place specified symbols along lines.
   We expanded **-N** to handle periodic, repeating symbols near the boundary,
   added a new modifier **+** to **-E** for asymmetrical error bars, and provided the
   shorthand **-SE-**\ *diameter* for degenerated ellipses (i.e., circles).
   The **-L** option has been enhanced to create envelope polygons around y(x),
   say for confidence envelopes (modifiers **+b**\|\ **d**\|\ **D**), and to complete a closed
   polygon by adding selected corners (modifiers **+xl**\|\ **r**\|\ *x0* or **+yb**\|\ **t**\|\ *y0*).
   The **-A**\-option now has new modifiers **x**\|\ **y** for creating stair-case curves.
   The slip-vector symbol can now optionally accept a vector-head angle [30].
   The custom symbols definition tests can now compare two input variables.
   We also added a **-F** option to draw line segments from an input data sets using
   a variety of connectivity modes.  Finally, for drawing lines there are new line
   attribute modifiers available via the pen setting **-W** such as drawing with a
   Bezier spline (**+s**), trimming the segments from the ends before plotting (**+o**\ *offset*),
   or adding vector heads at the ends of the lines (**+v**).

*  :doc:`plot3d` also has the new **-SE-**\ *diameter* shorthand as well as the **-N**
   modifiers for handling periodic plot symbols.  Like, plot it gets the same improvements
   to quoted lines and adds decorated lines as a new symbol.  Likewise,
   the **-L** option has been enhanced to create envelope polygons around y(x),
   say for confidence envelopes (modifiers **+b**\|\ **d**\|\ **D**), and to complete a closed
   polygon by adding selected corners (modifiers **+xl**\|\ **r**\|\ *x0* or **+yb**\|\ **t**\|\ *y0*).
   The slip-vector symbol can now optionally accept a vector-head angle [30].
   Finally, to match :doc:`plot` we have added the option **-T** for specifying no data input.

*  :doc:`sample1d` spline selection option **-F** can now accept the optional
   modifiers **+1** or **+2** which will compute
   the first or second derivatives of the spline, respectively.

*  :doc:`spectrum1d` can now turn off single-output data to standard output via **-T**
   or turn off multi-file output via **-N**.

*  :doc:`sphdistance` can now also perform a nearest-neighbor gridding where
   all grid nodes inside a Voronoi polygon is set to the same value as the
   Voronoi node value, via **-Ez**.

*  :doc:`trend1d` can now fit mixed polynomial and Fourier series models,
   as well as allowing models with just some terms in a polynomial or a
   Fourier series, including plain cosine or sine series terms.  Modifiers
   have been added to specify data origin and fundamental period instead of
   defaulting to the data mid-point and data range, respectively.

A few supplement modules have new features as well:

*  :doc:`mgd77track <supplements/mgd77/mgd77track>` adds the **-Gn**\ *gap* option to
   decimate the trackline coordinates by only plotting every *gap* point.

*  :doc:`gravfft <supplements/potential/gravfft>` adds **-W**\ *wd* to change
   observation level.

*  :doc:`grdgravmag3d <supplements/potential/grdgravmag3d>` adds **-H** to compute magnetic anomaly.

*  :doc:`grdpmodeler <supplements/spotter/grdpmodeler>` can now output more than one model
   prediction into several grids or as a record written to standard output.  Also gains the **-N** option
   used by other spotter tools to extend the model duration.


New Features in GMT 5
=====================

GMT 5 represents a new branch of GMT development that mostly preserves the
capabilities of the previous versions while adding over 200 new features
to an already extensive bag of tricks.  Our PostScript library
:doc:`PSL </devdocs/postscriptlight>` has seen a complete rewrite as well
and produce shorter and more compact PostScript. However, the big news
is aimed for developers who wish to leverage GMT in their own applications.
We have completely revamped the code base so that high-level
GMT functionality is now accessible via GMT "modules". These are
high-level functions named after their corresponding programs (e.g.,
``GMT_grdimage``) that contains all of the functionality of that program
within the function. While currently callable from C/C++ only (with some
support for F77), we are making progress on the Matlab interface as well
and there are plans to start on the Python version. Developers should
consult the :ref:`GMT API <api>` documentation for more details.

We recommend that users of GMT 4 consider learning the new rules and defaults
since eventually (in some years) GMT 4 will be obsolete.
To ease the transition to GMT 5 you may run it in compatibility mode,
thus allowing the use of many obsolete default names and command
switches (you will receive a warning instead).  This is discussed below.

Below are six key areas of improvements in GMT 5.

New programs in GMT 5
---------------------

First, a few new programs have been added and some have been
promoted (and possibly renamed) from earlier supplements:

:doc:`gmt`
    This is the **only** program executable that is distributed with GMT 5. To avoid
    problems with namespace conflicts (e.g., there are other, non-GMT programs
    with generic names like triangulate, surface, etc.) all GMT 5 modules are
    launched from the gmt executable via "gmt module" calls (e.g, gmt coast).
    For backwards compatibility (see below) we also offer symbolic links with
    the old executable names that simply point to the gmt program, which then
    can start the correct module.  Any module whose name starts with "gmt" can
    be abbreviated, e.g., "gmt gmtconvert" may be called as "gmt convert".

:doc:`gmt2kml`
    A :doc:`plot` -like tool to produce KML overlays for Google Earth. Previously
    found in the misc supplement.

:doc:`gmtconnect`
    Connect individual lines whose end points match within given tolerance.
    Previously known as gmtstitch in the misc supplement (this name is recognized
    when GMT is running in compatibility mode).

:doc:`gmtget`
    Return the values of the specified GMT defaults.  Previously only
    implemented as a shell script and thus not available on all platforms.

:doc:`gmtinfo`
    Report information about data tables. Previously known by the name minmax
    (this name is still recognized when GMT is running in compatibility mode).

:doc:`gmtsimplify`
    A line-reduction tool for coastlines and similar lines. Previously found
    in the misc supplement under the name gmtdp (this name is recognized when
    GMT is running in compatibility mode).

:doc:`gmtspatial`
    Perform various geospatial operations on lines and polygons.

:doc:`gmtvector`
    Perform basic vector manipulation in 2-D and 3-D.

:doc:`gmtwhich`
    Return the full path to specified data files.

grdraster
    Extracts subsets from large global grids.  Previously
    found in the dbase supplement.

:doc:`kml2gmt`
    Extract GMT data tables from Google Earth KML files.  Previously
    found in the misc supplement.

:doc:`sph2grd`
    Compute grid from list of spherical harmonic coefficients [We will add its
    natural complement grd2sph at a later date].

:doc:`sphdistance`
    Make grid of distances to nearest points on a sphere.  Previously
    found in the sph supplement.

:doc:`sphinterpolate`
    Spherical gridding in tension of data on a sphere.  Previously
    found in the sph supplement.

:doc:`sphtriangulate`
    Delaunay or Voronoi construction of spherical (*lon, lat*) data.  Previously
    found in the sph supplement.

We have also added a new supplement called potential that contains these five modules:

:doc:`gmtgravmag3d <supplements/potential/gmtgravmag3d>`:
    Compute the gravity/magnetic anomaly of a body by the method of Okabe.

:doc:`gmtflexure <supplements/potential/gmtflexure>`:
    Compute the flexure of a 2-D load using variable plate thickness and restoring force.

:doc:`gravfft <supplements/potential/gravfft>`:
    Compute gravitational attraction of 3-D surfaces and a little more by the method of Parker.

:doc:`grdgravmag3d <supplements/potential/grdgravmag3d>`:
    Computes the gravity effect of one (or two) grids by the method of Okabe.

:doc:`grdredpol <supplements/potential/grdredpol>`:
    Compute the Continuous Reduction To the Pole, also known as differential RTP.

:doc:`grdseamount <supplements/potential/grdseamount>`:
    Compute synthetic seamount (Gaussian or cone, circular or elliptical) bathymetry.

Finally, the spotter supplement has added one new module:

:doc:`grdpmodeler <supplements/spotter/grdpmodeler>`:
    Evaluate a plate model on a geographic grid.

New common options in GMT 5
---------------------------

First we discuss changes that have been
implemented by a series of new lower-case GMT common options:

*  Programs that read data tables can now process the aspatial metadata
   in OGR/GMT files with the new **-a** option. These can be produced by
   *ogr2ogr* (a `GDAL tool <https://gdal.org/>`_) when selecting the -f "GMT" output
   format. See Chapter :ref:`OGR_compat` for an explanation of the OGR/GMT file format.
   Because all GIS information is encoded via GMT comment lines these
   files can also be used in GMT 4 (the GIS metadata is simply
   skipped).

*  Programs that read or write data tables can specify a custom binary format
   using the enhanced **-b** option.

*  Programs that read data tables can control which columns to read and
   in what order (and optionally supply scaling relations) with the new **-i** option

*  Programs that read grids can use new common option **-n** to control
   grid interpolation settings and boundary conditions.

*  Programs that write data tables can control which columns to write
   and in what order (and optionally supply scaling relations) with the new **-o** option.

*  All plot programs can take a new **-p** option for perspective view
   from infinity. In GMT 4, only some programs could do this (e.g.,
   :doc:`coast`) and it took a
   program-specific option, typically **-E** and sometimes an option
   **-Z** would be needed as well. This information is now all passed
   via **-p** and applies across all GMT plotting programs.

*  Programs that read data tables can control how records with NaNs are
   handled with the new **-s** option.

*  All plot programs can take a new **-t** option to modify the PDF
   transparency level for that layer. However, as PostScript has no provision for
   transparency you can only see the effect if you convert it to PDF.

Updated common options in GMT 5
-------------------------------

Some of the established GMT common options have seen significant
improvements; these include:

*  The completely revised **-B** option can now handle irregular and custom annotations
   (see Section :ref:`custom_axes`). It also has a new automatic mode which
   will select optimal intervals given data range and plot size. The 3-D base maps can now have
   horizontal gridlines on xz and yz back walls.

*  The **-R** option may now accept a leading unit which implies the
   given coordinates are projected map coordinates and should be
   replaced with the corresponding geographic coordinates given the
   specified map projection. For linear projections such units imply a
   simple unit conversion for the given coordinates (e.g., km to meter).

*  Introduced **-fp** which allows data input to be in
   projected values, e.g., UTM coordinates while **-Ju** is given.

While just giving - (the hyphen) as argument presents just the synopsis of the command
line arguments, we now also support giving + which in addition will list
the explanations for all options that are not among the GMT common set.

New default parameters in GMT 5
-------------------------------

Most of the GMT default parameters have changed names in order to
group parameters into logical groups and to use more consistent naming.
However, under compatibility mode (see below) the old names are still recognized.
New capabilities have been implemented by introducing new GMT default settings:

*  :term:`DIR_DCW` specifies where to look for the optional
   Digital Charts of the World database (for country coloring or selections).

*  :term:`DIR_GSHHG` specifies where to look for the required
   Global Self-consistent Hierarchical High-resolution Geography database.

*  :term:`GMT_COMPATIBILITY` can be set to 4 to allow
   backwards compatibility with GMT 4 command-line syntax or 5 to impose
   strict GMT5 syntax checking.

*  :term:`IO_NC4_CHUNK_SIZE` is used to set the default
   chunk size for the **lat** and **lon** dimension of the **z** variable of
   netCDF version 4 files.

*  :term:`IO_NC4_DEFLATION_LEVEL` is used to set
   the compression level for netCDF4 files upon output.

*  :term:`IO_SEGMENT_MARKER` can be used to change the
   character that GMT uses to identify new segment header records [>].

*  :term:`MAP_ANNOT_ORTHO` controls whether axes annotations
   for Cartesian plots are horizontal or orthogonal to the individual axes.

*  :term:`GMT_FFT` controls which algorithms to use for Fourier
   transforms.

*  :term:`GMT_TRIANGULATE` controls which algorithm to use
   for Delaunay triangulation.

*  Great circle distance approximations can now be fine-tuned via new GMT default parameters
   :term:`PROJ_MEAN_RADIUS` and :term:`PROJ_AUX_LATITUDE`.
   Geodesics are now even more accurate by using the Vincenty [1975] algorithm instead of
   Rudoe's method.

*  :term:`GMT_EXTRAPOLATE_VAL` controls what splines should do if
   requested to extrapolate beyond the given data domain.

*  :term:`PS_TRANSPARENCY` allows users to modify how transparency will be
   processed when converted to PDF [Normal].

A few parameters have been introduced in GMT 5 in the past and have been removed again.
Among these are:

*  *DIR_USER*: was supposed to set the directory in which the user configuration files, or data are stored, but
   this creates problems, because it needs to be known already before it is potentially set in *DIR_USER*/gmt.conf.
   The environment variable **$GMT_USERDIR** is used for this instead.

*  *DIR_TMP*: was supposed to indicate the directory in which to store temporary files. But needs to be known without
   gmt.conf file as well. So the environment variable **$GMT_TMPDIR** is used instead.

General improvements in GMT 5
-----------------------------

Other wide-ranging changes have been implemented in different
ways, such as

*  All programs now use consistent, standardized choices for plot
   dimension units (**c**\ m, **i**\ nch, or **p**\ oint; we no longer
   consider **m**\ eter a plot length unit), and actual distances
   (choose spherical arc lengths in **d**\ egree, **m**\ inute, and
   **s**\ econd [was **c**], or distances in m\ **e**\ ter [Default],
   **f**\ oot [new], **k**\ m, **M**\ ile [was sometimes **i** or
   **m**], **n**\ autical mile, and s\ **u**\ rvey foot [new]).

*  Programs that read data tables can now process multi-segment tables
   automatically. This means programs that did not have this capability
   (e.g., :doc:`filter1d`) can now all operate on the
   segments separately; consequently, there is no longer a **-m**
   option.

*  While we support the scaling of z-values in grids via the filename convention
   name[=\ *ID*\ [**+s**\ *scale*][**+o**\ *offset*][**+n**\ *nan*] mechanism, there are times
   when we wish to scale the *x, y* domain as well. Users can now
   append **+u**\ *unit* to their gridfile names, where *unit* is one of non-arc units listed
   in Table :ref:`distunits <tbl-distunits>`.  This will convert your Cartesian
   x and y coordinates *from* the given unit *to* meters.  We also support the inverse
   option **+U**\ *unit*, which can be used to convert your grid
   coordinates *from* meters *to* the specified unit.

*  CPTs also support the **+u**\|\ **U**\ *unit* mechanism.  Here, the scaling
   applies to the z values.  By appending these modifiers to your CPT names you
   can avoid having two CPTs (one for meter and one for km) since only one is needed.

*  Programs that read grids can now directly handle Arc/Info float binary
   files (GRIDFLOAT) and ESRI .hdr formats.

*  Programs that read grids now set boundary conditions to aid further
   processing. If a subset then the boundary conditions are taken from
   the surrounding grid values.

*  All text can now optionally be filled with patterns and/or drawn with
   outline pens. In the past, only :doc:`text` could plot outline fonts via
   **-S**\ *pen*. Now, any text can be an outline text by manipulating
   the corresponding FONT defaults (e.g., :term:`FONT_TITLE`).

*  All color or fill specifications may append @\ *transparency* to
   change the PDF transparency level for that item. See **-t** for
   limitations on how to visualize this transparency.

*  GMT now ships with 36 standard color palette tables (CPT), up from 24.

Program-specific improvements in GMT 5
--------------------------------------

Finally, here is a list of numerous enhancements to individual programs:

*  :doc:`blockmean` added **-Ep** for error propagation and
   **-Sn** to report the number of data points per block.

*  :doc:`blockmedian` added **-Er**\ [-]
   to return as last column the record number that gave the median
   value. For ties, we return the record number of the higher data value
   unless **-Er**- is given (return lower). Added **-Es** to read and
   output source id for median value.

*  :doc:`blockmode` added **-Er**\ [-] but
   for modal value. Added **-Es** to read and output source id for modal
   value.

*  :doc:`gmtconvert` now has optional PCRE (regular expression) support,
   as well as a new option to select a subset of segments specified by
   segment running numbers (**-Q**) and improved options to extract a
   subset of records (**-E**) and support for a list of search strings
   via **-S+f**\ *patternfile*.

*  :doc:`gmtinfo` has new option **-A** to
   select what group to report on (all input, per file, or per segment).
   Also, use **-If** to report an extended region optimized for fastest results in FFTs.
   and **-Is** to report an extended region optimized for fastest results in :doc:`surface`.
   Finally, new option **-D**\ [*inc*] to align regions found via **-I** with the center
   of the data.

*  :doc:`gmtmath` with **-N**\ *ncol* and input
   files will add extra blank columns, if needed.  A new option **-E**
   sets the minimum eigenvalue used by operators LSQFIT and SVDFIT.
   Option **-L** applies operators on a per-segment basis instead of
   accumulating operations across the entire file.  Many new
   operators have been added (BITAND, BITLEFT, BITNOT, BITOR, BITRIGHT,
   BITTEST, BITXOR, DIFF, IFELSE, ISFINITE, SVDFIT, TAPER).  Finally,
   we have implemented user macros for long or commonly used expressions,
   as well as ability to store and recall using named variables.

*  :doc:`gmtselect` Takes **-E** to indicate if points exactly on a polygon
   boundary are inside or outside, and **-Z** can now be extended to apply
   to other columns than the third.

*  :doc:`grd2cpt` takes **-F** to specify output color model and **-G** to
   truncate incoming CPT to be limited to a given range.

*  :doc:`grd2xyz` takes **-C** to write row, col instead of *x, y*.  Append **f**
   to start at 1, else start at 0.  Alternatively, use **-Ci** to write just
   the two columns *index* and *z*, where *index*
   is the 1-D indexing that GMT uses when referring to grid nodes.

*  :doc:`grdblend` can now take list of grids on
   the command line and blend, and now has more blend choices (see **-C**). Grids no
   longer have to have same registration or spacing.

*  :doc:`grdclip` has new option **-Si** to set all data >= low and <= high
   to the *between* value, and **-Sr** to set all data == old to the *new* value.

*  :doc:`grdcontour` can specify a single contour with **-C+**\ *contour* and
   **-A+**\ *contour*.

*  :doc:`grdcut` can use **-S** to specify an origin and radius and cut the
   corresponding rectangular area, and **-N** to extend the region if the new
   **-R** domain exceeds existing boundaries.

*  :doc:`grdfft` can now accept two grids and let **-E** compute the cross-spectra.
   The **-N** option allows for many new and special settings, including ability
   to control data mirroring, detrending, tapering, and output of intermediate
   results.

*  :doc:`grdfilter` can now do spherical
   filtering (with wrap around longitudes and over poles) for non-global
   grids. We have also begun implementing Open MP threads to speed up
   calculations on multi-core machines. We have added rectangular
   filtering and automatic resampling to input resolution for high-pass
   filters. There is also **-Ff**\ *weightgrd* which reads the gridfile
   *weightgrd* for a custom Cartesian grid convolution. The *weightgrd*
   must have odd dimensions. Similarly added **-Fo**\ *opgrd* for
   operators (via coefficients in the grdfile *opgrd*) whose weight sum
   is zero (hence we do not sum and divide the convolution by the weight
   sum).

*  :doc:`grdgradient` now has **-Em** that gives results close to ESRI's
   "hillshade"'" (but faster).

*  :doc:`grdinfo` now has modifier
   **-Ts**\ *dz* which returns a symmetrical range about zero.  Also,
   if **-Ib** is given then the grid's bounding box polygon is written.

*  :doc:`grdimage` with GDAL support can write a raster image directly to
   a raster file (**-A**) and may plot raster images as well (**-Dr**).
   It also automatically assigns a color table if none is given and can use
   any of the 36 GMT color tables and scale them to fit the grid range.

*  :doc:`grdmask` has new option
   **-Ni**\|\ I\|\ p\|\ P to set inside of
   polygons to the polygon IDs. These may come from OGR aspatial values,
   segment head **-L**\ ID, or a running number, starting at a specified
   origin [0]. Now correctly handles polygons with perimeters and holes.
   Added z as possible radius value in **-S** which means read radii
   from 3rd input column.

*  :doc:`grdmath` added many new operators such as BITAND, BITLEFT, BITNOT, BITOR, BITRIGHT,
   BITTEST, BITXOR, DEG2KM, IFELSE, ISFINITE, KM2DEG, and TAPER.  Finally,
   we have implemented user macros for long or commonly used expressions,
   as well as ability to store and recall using named variables.

*  :doc:`grdtrack` has many new options.  The **-A** option controls how the
   input track is resampled when **-C** is selected, the new **-C**, **-D**
   options automatically create an equidistant set of cross-sectional
   profiles given input line segments; one or more grids can then be
   sampled at these locations.  The **-E** option allows users to quickly specify
   tracks for sampling without needed input tracks.  Also added **-S** which stack
   cross-profiles generated with **-C**. The **-N** will not skip
   points that are outside the grid domain but return NaN as sampled
   value.  Finally, **-T** will return the nearest non-NaN node if the initial
   location only finds a NaN value.

*  :doc:`grdvector` can now take **-Si**\ *scale* to give the reciprocal scale,
   i.e., cm/ unit or km/unit.  Also, the vector heads in GMT have completely been overhauled
   and includes geo-vector heads that follow great or small circles.

*  :doc:`grdview` will automatically assigns a color table if none is given and can use
   any of the 36 GMT color tables and scale them to fit the grid range.

*  :doc:`grdvolume` can let **-S** accept more distance units than just km. It also
   has a modified **-T**\ [**c**\|\ **h**] for ORS estimates based on max
   curvature or height. **-Cr** to compute the *outside* volume between two contours
   (for instances, the volume of water from a bathymetry grid).

*  :doc:`greenspline` has an improved **-C** option to control how many eigenvalues are used
   in the fitting, and **-Sl** adds a linear (or bilinear) spline.

*  :doc:`makecpt` has a new **-F** option to
   specify output color representation, e.g., to output the CPT in
   h-s-v format despite originally being given in r/g/b, and **-G** to
   truncate incoming CPT to be limited to a given range.  It also adds **Di**
   to match the bottom/top values in the input CPT.

*  :doc:`mapproject` has a new **-N**
   option to do geodetic/geocentric conversions; it combines with **-I**
   for inverse conversions. Also, we have extended **-A** to accept
   **-A**\ **o**\| \ **O** to compute line orientations (-90/90).
   In **-G**, prepend - to the unit for (fast) flat Earth or + for (slow) geodesic calculations.

*  :doc:`project` has added **-G**...[+] so
   if + is appended we get a segment header with information about the
   pole for the circle. Optionally, append /*colat* in **-G** for a small circle path.

*  :doc:`psconvert` has added a **-TF** option to create multi-page PDF files. There is
   also modification to **-A** to add user-specified margins, and it automatically detects
   if transparent elements have been included (and a detour via PDF might be needed).

*  :doc:`basemap` has added a **-D** option to place a map-inset box.

*  :doc:`clip` has added an extended **-C** option to close different types of clip paths.

*  :doc:`coast` has added a new option **-E** which lets users specify one or more countries
   to paint, fill, extract, or use as plot domain (requires DCW to be installed).

*  :doc:`contour` is now similar to :doc:`grdcontour` in the options it
   takes, e.g., **-C** in particular. In GMT 4, the program could only
   read a CPT and not take a specific contour interval.

*  :doc:`histogram` now takes **-D** to place histogram count labels on top of each bar
   and **-N** to draw the equivalent normal distributions.

*  :doc:`legend` no longer uses system calls to do the plotting.  The modified **-D**
   allows for minor offsets, while **-F** offers more control over the frame and fill.

*  :doc:`rose` has added **-Wv**\ *pen* to
   specify pen for vector (specified in **-C**). Added **-Zu** to set all radii to
   unity (i.e., for analysis of angles only).

*  :doc:`colorbar` has a new option **-T** that paints a rectangle behind the color bar.
   The **+n** modifier to **-E** draws a rectangle with NaN color and adds a label.
   The **-G** option will truncate incoming CPT to be limited to the specified z-range.
   Modification **-Np** indicates a preference to use polygons to draw the color bar.

*  :doc:`text` can take simplified input
   via new option **-F** to set fixed font (including size), angle, and
   justification. If these parameters are fixed for all the text strings
   then the input can simply be *x y text*.  It also has enhanced **-DJ** option
   to shorten diagonal offsets by :math:`\sqrt{2}` to maintain the same
   radial distance from point to annotation. Change all text to upper or
   lower case with **-Q**.

*  :doc:`plot` and :doc:`plot3d` both support the revised custom symbol macro
   language which has been expanded considerably to allow for complicated,
   multi-parameter symbols; see Chapter :ref:`App-custom_symbols`
   for details. Finally, we allow the base for bars and columns optionally to be
   read from data file by not specifying the base value.

*  :doc:`sample1d` offers **-A** to control resampling of spatial curves (with **-I**).

*  :doc:`spectrum1d` has added **-L** to control removal of trend, mean value or mid value.

*  :doc:`surface` has added **-r** to create pixel-registered grids and knows about
   periodicity in longitude (given **-fg**).  There is also **-D** to supply a "sort" break line.

*  :doc:`triangulate` now offers **-S**
   to write triangle polygons and can handle 2-column input if **-Z** is given.
   Can also write triangle edges as line with **-M**.

*  :doc:`xyz2grd` now also offers **-Af** (first value encountered),
   **-Am** (mean, the default), **-Ar** (rms), and **-As** (last value encountered).

Several supplements have new features as well:

*  :doc:`img2grd <supplements/img/img2grd>`
   used to be a shell script but is now a C program and can be used on all platforms.

*  :doc:`mgd77convert <supplements/mgd77/mgd77convert>`
   added **-C** option to assemble \*.mgd77 files from \*.h77/\*.a77 pairs.

*  The spotter programs can now read GPlates rotation files directly as well
   as write this format. Also,
   :doc:`rotconverter <supplements/spotter/rotconverter>` can extract plate
   circuit rotations on-the-fly from the GPlates rotation file.

**Note**: GMT 5 only produces PostScript and no longer has a setting for
Encapsulated PostScript (EPS). We made this decision since (a) our EPS determination
was always very approximate (no consideration of font metrics, etc.) and quite often wrong,
and (b) :doc:`psconvert` handles it exactly.  Hence, users who need EPS plots should
simply process their PostScript files via :doc:`psconvert`.
