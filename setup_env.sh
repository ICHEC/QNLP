#!/bin/bash

###############################################################################
# Define env variables for paths
###############################################################################
if [ "$#" -eq 1 ]; then
    QNLP_ROOT=$1
else
    QNLP_ROOT=$PWD
fi
NLTK_DATA=${QNLP_ROOT}/third_party/install/nltk_data

###############################################################################
# Declare associative arrays to hold external dependencies
###############################################################################
declare -a GITHUB_REPOS
declare -a PIP_PACKAGES
declare -a CONDA_PACKAGES
declare -a PYTHON_CMDS

GITHUB_REPOS=(  "intel/intel-qs"
                "catchorg/Catch2::v2.7.0"
                "CLIUtils/CLI11"
                "pybind/pybind11"
                "mpi4py/mpi4py"
             )
PIP_PACKAGES=(  "multimethod" #used for multiple dispatch of certain function
                "pyvis" # used to present interactive graph of token relationships
                "ortools" # used to solve TSP for basis token ordering
             )
CONDA_PACKAGES=("nltk::anaconda"
                "jupyter"
                "numpy"
                "scipy"
                "tabulate"
                "mkl-include"
                "networkx"
                "matplotlib"
                "tqdm::conda-forge"
               )
CMDS=("python -m nltk.downloader -d ${NLTK_DATA} all")

###############################################################################
# Create all directories required by QNLP project
# build, third_party, third_party/{downloads, install}
###############################################################################
function setupDirs(){
    declare -a dirs=(   "${QNLP_ROOT}/build"
                        "${QNLP_ROOT}/third_party"
                        "${QNLP_ROOT}/third_party/downloads"
                        "${QNLP_ROOT}/third_party/install"
                        "${QNLP_ROOT}/install"
                        "${NLTK_DATA}"
                    )
    for i in "${dirs[@]}";
    do
        if [ ! -d "${i}" ];then
            mkdir -p "${i}"
        fi
    done
}

###############################################################################
# Set Conda version based on current OS.
# Assumes Nix systems only (MacOS not officially supported)
###############################################################################
export SYS=$(uname -s)
if [[ "${SYS}" == "Linux" ]];then
    CONDA="Miniconda3-latest-Linux-x86_64.sh"
elif [[ "${SYS}" == "Darwin" ]];then
    CONDA="Miniconda3-latest-MacOSX-x86_64.sh"
else
    echo "Unsupported configuration."
    exit
fi

###############################################################################
# Fetch miniconda installer
###############################################################################
function fetchConda(){
    echo "### fetchConda() ###"
    cd ${QNLP_ROOT}/third_party/downloads
    if [ ! -e "${CONDA}" ]; then #if conda does not already exist, acquire.
        echo "Conda installer not found. Acquiring."
        #wget https://repo.continuum.io/miniconda/${CONDA} --no-check-certificate
        curl -OL https://repo.continuum.io/miniconda/${CONDA}
    fi
    cd -
}

###############################################################################
# Acquire all requested TAR files, untar them into an OS independent named directory
###############################################################################
installTAR(){

    #Loop through arrays, acquire TAR, and install the packages
    for s in $(seq 0 $(( ${#TAR_FILES[@]} -1 )) ); do
        echo "Acquiring TAR files from ${TAR_FILES[${s}]}"
        if [[ "${TAR_FILES[${s}]}" =~ "::" ]]; then
            PC=${TAR_FILES[${s}]} #Package::channel

            #Split string and download package with specified name after ::
            URL="${PC%::*}"
            FILENAME="${PC#*::}"
            echo ${QNLP_ROOT}/third_party/downloads/${PC#*::}
            if [ ! -f "${QNLP_ROOT}/third_party/downloads/${FILENAME}" ]; then
                curl -o ${QNLP_ROOT}/third_party/downloads/${FILENAME} -J -L "${URL}"
            fi

            mkdir ${QNLP_ROOT}/third_party/install/${FILENAME%%.*}
            tar xvf ${QNLP_ROOT}/third_party/downloads/${FILENAME} -C \
                ${QNLP_ROOT}/third_party/install/${FILENAME%%.*} \
                --strip-components 1
        else
            FILENAME=$(basename ${TAR_FILES[${s}]})
            pushd . > /dev/null && cd ${QNLP_ROOT}/third_party/downloads/
            if [ ! -f ${FILENAME} ]; then
                curl -O -J -L ${TAR_FILES[${s}]}
            fi
            tar xvf ${FILENAME} -C ${QNLP_ROOT}/third_party/install/
            popd > /dev/null
        fi
    done
}

###############################################################################
# Install miniconda and necessary packages
###############################################################################
function condaEnvSetup(){
    echo "### condaEnvSetup() ###"
    chmod +x ${QNLP_ROOT}/third_party/downloads/${CONDA}
    ${QNLP_ROOT}/third_party/downloads/${CONDA} -b -p ${QNLP_ROOT}/third_party/install/intel-qnlp_conda;
    source ${QNLP_ROOT}/third_party/install/intel-qnlp_conda/bin/activate ;
    conda update -n base conda -y;
    conda create -n intel-qnlp -y python=3.7;
    conda activate intel-qnlp #Activate said environment
}

###############################################################################
# Fetch SDKs if they do not exist already in Python environment
###############################################################################
function fetchPackages(){
    echo "### fetchPackages() ###"
    source ${QNLP_ROOT}/third_party/install/intel-qnlp_conda/bin/activate ;
    conda activate intel-qnlp #Activate said environment

    # Loop through arrays and install the packages and repos
    if [ "${#GITHUB_REPOS[@]}" -gt 0 ]; then
        pushd . &> /dev/null
        cd $QNLP_ROOT/third_party
        for s in $(seq 0 $(( ${#GITHUB_REPOS[@]} -1 )) ); do
            echo ${GITHUB_REPOS[${s}]}
            PC=${GITHUB_REPOS[${s}]} #Package::channel

            if [[ "${GITHUB_REPOS[${s}]}" =~ "::" ]]; then
                git clone https://github.com/${PC%::*}
                PCC=${PC#*/}
                cd ${PCC%::*}
                git checkout ${PC#*::}
                cd -
            else
                git clone https://github.com/${GITHUB_REPOS[${s}]}
            fi

        done
        popd &> /dev/null
    fi

    # CONDA
    if [ "${#CONDA_PACKAGES[@]}" -gt 0 ]; then
        for s in $(seq 0 $(( ${#CONDA_PACKAGES[@]} -1 )) ); do
            echo "Installing ${CONDA_PACKAGES[${s}]}"

            if [[ "${CONDA_PACKAGES[${s}]}" =~ "::" ]]; then
                PC=${CONDA_PACKAGES[${s}]} #Package::channel
                #Split string and install package from specified channel
                conda install ${PC%::*} -y -c ${PC#*::}
            else
                conda install ${CONDA_PACKAGES[${s}]} -y
            fi
        done
    fi

    # PIP
    if [ "${#PIP_PACKAGES[@]}" -gt 0 ]; then
        for s in $(seq 0 $(( ${#PIP_PACKAGES[@]} -1 )) ); do
            echo ${PIP_PACKAGES[${s}]}
            python -m pip install --no-binary all --compile ${PIP_PACKAGES[${s}]}
        done
    fi

    # Bash commands
    if [ "${#CMDS[@]}" -gt 0 ]; then
        for s in $(seq 0 $(( ${#CMDS[@]} -1 )) ); do
            echo "\"${CMDS[${s}]}\""
            ${CMDS[${s}]}
        done
    fi
}

###############################################################################
#Update conda environment and all installed packages
###############################################################################
function condaUpdateAll(){
    source ${QNLP_ROOT}/third_party/install/intel-qnlp_conda/bin/activate ;
    conda update --all -y
}

###############################################################################
#                                   main
###############################################################################

LOG_NAME="SetupEnv"
setupDirs > >(tee -a ${LOG_NAME}_out.log) 2> >(tee -a ${LOG_NAME}_err.log >&2) &&
fetchConda > >(tee -a ${LOG_NAME}_out.log) 2> >(tee -a ${LOG_NAME}_err.log >&2) &&
condaEnvSetup > >(tee -a ${LOG_NAME}_out.log) 2> >(tee -a ${LOG_NAME}_err.log >&2) &&
fetchPackages > >(tee -a ${LOG_NAME}_out.log) 2> >(tee -a ${LOG_NAME}_err.log >&2) &&
condaUpdateAll > >(tee -a ${LOG_NAME}_out.log) 2> >(tee -a ${LOG_NAME}_err.log >&2) &&

if [ $? -ne 0 ]; then
    echo "Installation failed. Please check logs for further information." > >(tee -a ${LOG_NAME}_out.log) 2> >(tee -a ${LOG_NAME}_err.log >&2)
else
    echo "Installation succeeded. If you encounter any problems please report them to here, or the respective package authors." > >(tee -a ${LOG_NAME}_out.log) 2> >(tee -a ${LOG_NAME}_err.log >&2)
    echo "To load the environment run: source ${QNLP_ROOT}/load_env.sh" > >(tee -a ${LOG_NAME}_out.log) 2> >(tee -a ${LOG_NAME}_err.log >&2)
fi

cat > ${QNLP_ROOT}/load_env.sh << EOL
#!/bin/bash
source ${QNLP_ROOT}/third_party/install/intel-qnlp_conda/bin/activate ;
export PATH="${QNLP_ROOT}/install":"\${PATH}"
export NLTK_DATA="${NLTK_DATA}"
export QNLP_ROOT="${QNLP_ROOT}"
conda activate intel-qnlp

EOL
###############################################################################
