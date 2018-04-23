#!/bin/bash
set -e
cd $TRAVIS_BUILD_DIR

# Get current branch
if [ -z "$TRAVIS_PULL_REQUEST_BRANCH" ]; then 
	BRANCH="$TRAVIS_BRANCH"
else
	BRANCH="$TRAVIS_PULL_REQUEST_BRANCH"
fi
echo "$BRANCH"

# Clone the repo
git clone -b "$BRANCH" https://git@$GH_REPO_REF
cd $GH_REPO_NAME

# Update the change log
github_changelog_generator -u zwimer -p DrShadowStack
git add ./CHANGELOG.md

# Format the code
clang-format-7 -i -style=file ./src/*pp
git add ./src/

# Update the repo
git commit -m "Changelog updated [ci skip]" | true
git remote rm origin
git remote add origin https://zwimer:$GH_REPO_TOKEN@github.com/zwimer/$GH_REPO_NAME
git push -u origin "$BRANCH" | true
