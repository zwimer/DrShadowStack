#!/bin/sh
set -e

git config --global push.default simple
git config user.name "Travis CI"
git config user.email "travis@travis-ci.org"
