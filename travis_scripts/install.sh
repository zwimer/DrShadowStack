#!/bin/bash
set -e


# Sources
sudo apt-add-repository 'deb http://apt.llvm.org/trusty/ llvm-toolchain-trusty main'
sudo apt-add-repository 'deb http://ppa.launchpad.net/ubuntu-toolchain-r/test/ubuntu trusty main'
sudo apt-get update

# Building
sudo apt-get install --yes libboost-program-options-dev libboost-filesystem-dev
sudo apt-get install --yes cmake

# Update current branch
sudo apt-get install --yes --force-yes clang-format-7
gem install github_changelog_generator

# Docs
sudo apt-get install --yes doxygen graphviz
