#!/bin/bash

# go to user folder
cd ~

# create cosmos folder if it does not exits
mkdir -p cosmos cosmos/bin cosmos/nodes/ cosmos/resources

echo 'export PATH=/root/cosmos/bin:$PATH' >>~/.bashrc
source ~/.bashrc