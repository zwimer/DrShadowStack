#!/bin/bash

clang-format-7 -i -style=file ./src/*pp
github_changelog_generator -u zwimer -p DrShadowStack
