#!/bin/bash


# Format the code
clang-format-7 -i -style=file ./src/*pp

# Then update the docs
doxygen &
github_changelog_generator -u zwimer -p DrShadowStack &
