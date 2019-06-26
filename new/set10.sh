#!/bin/bin/bash

export PATH=$PATH_O
export PATH=/usr/local/cuda-10.1/bin:/usr/local/cuda-10.1/NsightCompute-2019.1${PATH:+:${PATH}}
unset LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/usr/local/cuda-10.1/lib64\ ${LD_LIBRARY_PATH:+: ${LD_LIBRARY_PATH}}
export CUDA_INSTALL_PATH=/usr/local/cuda-10.1/cuda

