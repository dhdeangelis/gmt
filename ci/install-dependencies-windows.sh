#!/usr/bin/env bash
#
# Bash script to install GMT dependencies on Windows via vcpkg and conda
#
# Environmental variables that can control the installation:
#
# - BUILD_DOCS: Build GMT documentation  [false]
# - RUN_TESTS:  Run GMT tests            [false]
# - PACKAGE:    Create GMT packages      [false]
#

set -x -e

# set defaults to false
BUILD_DOCS="${BUILD_DOCS:-false}"
RUN_TESTS="${RUN_TESTS:-false}"
PACKAGE="${PACKAGE:-false}"

WIN_PLATFORM=x64-windows

# install libraries
vcpkg install --recurse netcdf-c gdal[core,tools,geos,netcdf,png,openjpeg,jpeg,gif] pcre2 fftw3[core,threads] clapack openblas --triplet ${WIN_PLATFORM}
# Executable files search for DLL files in the directories listed in the PATH environment variable.
echo "${VCPKG_INSTALLATION_ROOT}/installed/${WIN_PLATFORM}/bin" >> $GITHUB_PATH
# Tools like gdal_translate, ogr2ogr are located in tools/gdal
echo "${VCPKG_INSTALLATION_ROOT}/installed/${WIN_PLATFORM}/tools/gdal" >> $GITHUB_PATH

# list installed packages
vcpkg list

conda_packages="ninja ghostscript=10.03.0"
if [ "$BUILD_DOCS" = "true" ]; then
    conda_packages+=" sphinx dvc"
    # choco install pngquant
fi

if [ "$RUN_TESTS" = "true" ]; then
    conda_packages+=" dvc"

    # Install graphicsmagick via choco
    choco install graphicsmagick --version 1.3.42 --no-progress
    echo 'C:\Program Files\GraphicsMagick-1.3.42-Q8\' >> $GITHUB_PATH
fi

# we need the GNU tar for packaging
if [ "$PACKAGE" = "true" ]; then
    echo 'C:\Program Files\Git\usr\bin\' >> $GITHUB_PATH
fi

# install more packages using conda
$CONDA\\Scripts\\conda.exe install ${conda_packages} -c conda-forge
echo "$CONDA\\Library\\bin" >> $GITHUB_PATH
echo "$CONDA\\Scripts" >> $GITHUB_PATH

# Add the vcpkg path again so it's prepended before conda's path and cmake can find
# the vcpkg library correctly
echo "${VCPKG_INSTALLATION_ROOT}/installed/${WIN_PLATFORM}/bin" >> $GITHUB_PATH

# Install Sphinx extensions
if [ "$BUILD_DOCS" = "true" ]; then
    ${CONDA}/python -m pip install --user -r doc/rst/requirements.txt
fi

set +x +e
