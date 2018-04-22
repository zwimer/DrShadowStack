## Need to know
1. Travis-CI will automatically update the `gh-pages` branch on any merge into master, do not make a PR to this branch. If an error in the `Doxyfile` is spotted, please make a PR to master to fix it.
1. For information for developers, visit the developer wiki page [here](https://github.com/zwimer/DrShadowStack/wiki/Developers)


## Contribution Guidlines
1. Code must utilize the commenting style used by the rest of this repo- specifically, comments must be written with [Doxygen](http://www.stack.nl/~dimitri/doxygen/) in mind.
1. Multiple small PR's are easier to review. If you must give a larger one please give a large number of comments.
1. Before making any pull request, please run the `run-before-push.sh` script. This will automatically update the changelog and format all `C++` code.
   - If this requires a developer token, then you may ignore the [github_changelog_generator](https://github.com/github-changelog-generator/github-changelog-generator) section and only run [Clang Format](https://clang.llvm.org/docs/ClangFormat.html).
