#!/bin/bash
set -e
cd $TRAVIS_BUILD_DIR

# Make a working directory for this script
mkdir update_current_commit
cd update_current_commit

# Get current branch
echo 'Getting branch...'
if [ -z "$TRAVIS_PULL_REQUEST_BRANCH" ]; then 
	BRANCH="$TRAVIS_BRANCH"
else
	BRANCH="$TRAVIS_PULL_REQUEST_BRANCH"
fi
echo "$BRANCH"

# Clone the repo
echo 'Cloning current branch...'
git clone -b "$BRANCH" https://git@$GH_REPO_REF
cd $GH_REPO_NAME

# Format the code
echo 'Formatting code...'
clang-format-7 -i -style=file ./src/*pp
git add ./src/

# Update the change log
echo 'Updating changelog...'
github_changelog_generator -u zwimer -p DrShadowStack
git add ./CHANGELOG.md

# Update the repo
echo 'Updating repo...'
git commit -m "Changelog updated [ci skip]" | true
git remote rm origin
git remote add origin https://zwimer:$GH_REPO_TOKEN@github.com/zwimer/$GH_REPO_NAME
git push -u origin "$BRANCH" | true
