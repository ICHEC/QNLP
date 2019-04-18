#!/bin/bash

# Pass download directory and install directory as arguments 1 and 2 respectively.
# Please specify full paths using $PWD. Install directory will be not relocatable.

DL_DIR=$1
INSTALL_DIR=$2

if [ $# -eq 0 ]; then
    DL_DIR=$PWD
    INSTALL_DIR=$PWD
    mkdir -p ${DL_DIR}/downloads
    mkdir -p ${INSTALL_DIR}/install
elif [ $# ! -eq 2 ]; then
    echo "Please specify download directory and install directory as:"
    echo "./CondInstall.sh $PWD/download $PWD/install"
    exit
else
    if [ ! -d "${DL_DIR}" ];then
        mkdir -p ${DL_DIR}/downloads
    fi
    if [ ! -d "${INSTALL_DIR}" ];then
        mkdir -p ${INSTALL_DIR}/install
    fi
fi

# Set Conda version based on current OS. Assumes Nix systems only
export SYS=$(uname -s)
if [[ "${SYS}" == "Linux" ]];then
    CONDA="Miniconda3-latest-Linux-x86_64.sh"
elif [[ "${SYS}" == "Darwin" ]];then
    CONDA="Miniconda3-latest-MacOSX-x86_64.sh"
else
    echo "Unsupported configuration."
    exit
fi

#Fetch miniconda installer
function fetchConda(){
    echo "### fetchConda() ###"
    cd ${DL_DIR}/downloads
    if [ ! -e "${CONDA}" ]; then #if conda does not already exist, acquire.
        echo "Conda installer not found. Acquiring."
        #wget https://repo.continuum.io/miniconda/${CONDA} --no-check-certificate
        curl -OL https://repo.continuum.io/miniconda/${CONDA}
    fi
    cd -
}

installTAR(){
    #Acquire all requested TAR files, untar them into an OS independent named directory

    #Loop through arrays, acquire TAR, and install the packages
    for s in $(seq 0 $(( ${#TAR_FILES[@]} -1 )) ); do
        echo "Acquiring TAR files from ${TAR_FILES[${s}]}"
        if [[ "${TAR_FILES[${s}]}" =~ "::" ]]; then
            PC=${TAR_FILES[${s}]} #Package::channel

            #Split string and download package with specified name after ::
            URL="${PC%::*}"
            FILENAME="${PC#*::}"
            echo ${DL_DIR}/downloads/${PC#*::}
            if [ ! -f "${DL_DIR}/downloads/${FILENAME}" ]; then
                curl -o ${DL_DIR}/downloads/${FILENAME} -J -L "${URL}"
            fi

            mkdir ${INSTALL_DIR}/install/${FILENAME%%.*}
            tar xvf ${DL_DIR}/downloads/${FILENAME} -C \
                ${INSTALL_DIR}/install/${FILENAME%%.*} \
                --strip-components 1
        else
            FILENAME=$(basename ${TAR_FILES[${s}]})
            pushd . > /dev/null && cd ${DL_DIR}/downloads/
            if [ ! -f ${FILENAME} ]; then
                curl -O -J -L ${TAR_FILES[${s}]}
            fi
            tar xvf ${FILENAME} -C ${INSTALL_DIR}/install/
            popd > /dev/null
        fi
    done
}

#install miniconda and necessary packages
function condaEnvSetup(){
    echo "### condaEnvSetup() ###"
    chmod +x ${DL_DIR}/downloads/${CONDA}
    ${DL_DIR}/downloads/${CONDA} -b -p ${INSTALL_DIR}/install/intel-qnlp_conda;
    source ${INSTALL_DIR}/install/intel-qnlp_conda/etc/profile.d/conda.sh ;
    conda update -n base conda -y;
    conda create -n intel-qnlp -y python;
    conda activate intel-qnlp #Activate said environment
}

#Fetch SDKs if they do not exist already in Python environment
function fetchSDKs(){
    echo "### fetchSDKs() ###"

    conda activate intel-qnlp #Activate said environment

    # Declare the SDK names and packages array
    declare -a PIP_PACKAGES
    declare -a CONDA_PACKAGES
    PIP_PACKAGES=()
    #If there are :: in the package name, read before delimiter as package and after as channel
    CONDA_PACKAGES=("nltk::anaconda" "numpy" "scipy" "jupyter")

    #Loop through arrays and install the packages
    if [ "${#CONDA_PACKAGES[@]}" -gt 0 ]; then
        for s in $(seq 0 $(( ${#CONDA_PACKAGES[@]} -1 )) ); do
            echo "Installing ${CONDA_PACKAGES[${s}]}"
    
            if [[ "${CONDA_PACKAGES[${s}]}" =~ "::" ]]; then
                PC=${CONDA_PACKAGES[${s}]} #Package::channel
                #Split string and install package in specified channel
                conda install ${PC%::*} -y -c ${PC#*::}
            else
                conda install ${CONDA_PACKAGES[${s}]} -y
            fi
        done
    fi
    if [ "${#PIP_PACKAGES[@]}" -gt 0 ]; then
        for s in $(seq 0 $(( ${#PIP_PACKAGES[@]} -1 )) ); do
            echo ${PIP_PACKAGES[${s}]}
            python -m pip install --no-binary all --compile ${PIP_PACKAGES[${s}]}
        done
    fi
    python -c "import nltk; nltk.dowdnload('wordnet')"
}

##########################################
#                 main
##########################################
#Redirect logs to output and error files
#installTAR > >(tee -a CondInstall_out.log) 2> >(tee -a CondInstall_err.log >&2) &&
fetchConda > >(tee -a CondInstall_out.log) 2> >(tee -a CondInstall_err.log >&2) &&
condaEnvSetup > >(tee -a CondInstall_out.log) 2> >(tee -a CondInstall_err.log >&2) &&
fetchSDKs > >(tee -a CondInstall_out.log) 2> >(tee -a CondInstall_err.log >&2) &&

if [ $? -ne 0 ]; then
    echo "Installation failed. Please check logs for further information."
else
    echo "Installation succeeded. If you encounter any problems please report them to here, or the respective package authors."
    echo "To load the environment run: source ${INSTALL_DIR}/load_env.sh"
fi

cat > ${INSTALL_DIR}/load_env.sh << EOL
#!/bin/bash
export PATH=${INSTALL_DIR}/install:$PATH
source ${INSTALL_DIR}/install/intel-qnlp_conda/etc/profile.d/conda.sh;
conda activate intel-qnlp 

EOL
