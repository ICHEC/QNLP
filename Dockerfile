FROM ubuntu:20.04
ENV DEBIAN_FRONTEND=noninteractive

#Combine many steps to make Docker image as small as possible
RUN apt-get update && apt-get install -y gcc g++ build-essential make mpich openssh-client openssh-server git
RUN apt-get update && apt-get install -y curl wget dvipng texlive-base texlive texlive-latex-extra cm-super cmake
WORKDIR /qnlp
COPY . /qnlp
RUN cd /qnlp && ls && ./setup_env.sh 
SHELL ["/bin/bash", "-c"]
RUN cd /qnlp && ls && source ./load_env.sh && cd ./build && cmake .. -DCMAKE_C_COMPILER=mpicc -DCMAKE_CXX_COMPILER=mpicxx -DENABLE_TESTS=1 -DENABLE_LOGGING=0 -DENABLE_PYTHON=1 -DENABLE_MPI=0 -DIqsMPI=OFF -DIqsMKL=OFF -DENABLE_NATIVE=on -DENABLE_RESOURCE_EST=0
RUN cd /qnlp/build && source ../load_env.sh && make
