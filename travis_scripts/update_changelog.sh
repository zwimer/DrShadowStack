#!/bin/bash
set -e

github_changelog_generator -u zwimer -p DrShadowStack
git add ./CHANGELOG.md

git remote rm origin
git remote add origin https://zwimer:$GH_REPO_TOKEN@github.com/zwimer/DrShadowStack

git commit -m "Travis-CI github changelog generator" | true
git push -u origin HEAD
