#!/bin/bash

echo "Creating DFU keys..."
nrfutil keys generate dfu_private.key
nrfutil keys display \
        --key pk \
        --format code \
        dfu_private.key \
        --out_file dfu_public_key.c
