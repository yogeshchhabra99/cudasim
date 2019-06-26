#!/bin/bin/bash

export PATH=$PATH_O
export PATH=/usr/local/cuda-4.0/cuda/bin${PATH:+:${PATH}}
export LD_LIBRARY_PATH=/usr/local/cuda-4.0/cuda/lib64:/usr/local/cuda-4.0/cuda/lib
export CUDA_INSTALL_PATH=/usr/local/cuda-4.0/cuda

