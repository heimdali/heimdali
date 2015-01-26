#!/bin/bash

# Set up environnment variables for a development mode, during which things
# are installed in heimdali/install.

# Check the file is sourced.
if [ "${BASH_SOURCE[0]}" == "${0}" ]
then 
    echo "Usage: source set_up_env_bash.sh"
    exit 1
fi

source activate heimdali

function pyrealpath ()
{
    python -c "import os; print os.path.realpath('$1')"
}

SCRIPT_DIR=$(pyrealpath $(dirname ${BASH_SOURCE[0]}))
ROOT_DIR=$(pyrealpath $SCRIPT_DIR/..)
INSTALL_DIR=$ROOT_DIR/install

CONDA_ENV_PATH=$(conda info -e | grep '*' | tr -s ' ' | cut -d" " -f3)

BIN_DIR=$INSTALL_DIR/bin
LIB_DIR=$CONDA_ENV_PATH/lib

export PATH=$BIN_DIR:$PATH
export LD_LIBRARY_PATH=$LIB_DIR:$LD_LIBRARY_PATH

echo "PATH has been prepended with               : $BIN_DIR"
echo "LD_LIBRARY_PATH has been prepended with   : $LIB_DIR"
