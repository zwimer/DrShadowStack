# Change Log

## [Unreleased](https://github.com/zwimer/DrShadowStack/tree/HEAD)

**Implemented enhancements:**

- Convert IO to use streams [\#61](https://github.com/zwimer/DrShadowStack/issues/61)
- Do not generate unique path [\#58](https://github.com/zwimer/DrShadowStack/issues/58)
- Add default mode [\#47](https://github.com/zwimer/DrShadowStack/issues/47)
- Remove need for drrun flag [\#43](https://github.com/zwimer/DrShadowStack/issues/43)
- Handle signals [\#24](https://github.com/zwimer/DrShadowStack/issues/24)
- Symbol Resolution on error [\#20](https://github.com/zwimer/DrShadowStack/issues/20)
- Generate code dcumentation [\#18](https://github.com/zwimer/DrShadowStack/issues/18)
- Use actual arg parser [\#16](https://github.com/zwimer/DrShadowStack/issues/16)
- CMake should handle pointer size [\#14](https://github.com/zwimer/DrShadowStack/issues/14)
- README [\#10](https://github.com/zwimer/DrShadowStack/issues/10)

**Fixed bugs:**

- Message bug - Wrong size message sent [\#67](https://github.com/zwimer/DrShadowStack/issues/67)
- Process RC created twice [\#65](https://github.com/zwimer/DrShadowStack/issues/65)
- External logging failed [\#60](https://github.com/zwimer/DrShadowStack/issues/60)
- Signal exception errors for SIGCHILD [\#56](https://github.com/zwimer/DrShadowStack/issues/56)
- --version and --help ignored for sub commands [\#53](https://github.com/zwimer/DrShadowStack/issues/53)
- Log file bug for non-debug mode [\#48](https://github.com/zwimer/DrShadowStack/issues/48)
- Argument parser ignores flags of the sub shell [\#41](https://github.com/zwimer/DrShadowStack/issues/41)
- Log file gets overwritten [\#40](https://github.com/zwimer/DrShadowStack/issues/40)
- Wait function breaks SS [\#33](https://github.com/zwimer/DrShadowStack/issues/33)
- Main process receiving SigPipe [\#23](https://github.com/zwimer/DrShadowStack/issues/23)
- boost::interprocess::mutex does not work on dynamorio [\#17](https://github.com/zwimer/DrShadowStack/issues/17)
- Implement process reference counter for internal SS [\#13](https://github.com/zwimer/DrShadowStack/issues/13)

**Closed issues:**

- Add a version [\#39](https://github.com/zwimer/DrShadowStack/issues/39)
- Implement process reference counter for external SS [\#34](https://github.com/zwimer/DrShadowStack/issues/34)
- Enable basic forking / threading for internal SS [\#32](https://github.com/zwimer/DrShadowStack/issues/32)
- Remap signals [\#22](https://github.com/zwimer/DrShadowStack/issues/22)
- Setup a new process group [\#21](https://github.com/zwimer/DrShadowStack/issues/21)
- Create Utilities [\#19](https://github.com/zwimer/DrShadowStack/issues/19)

**Merged pull requests:**

- Remove rc [\#66](https://github.com/zwimer/DrShadowStack/pull/66) ([zwimer](https://github.com/zwimer))
- Docs [\#64](https://github.com/zwimer/DrShadowStack/pull/64) ([zwimer](https://github.com/zwimer))
- Bug fix [\#63](https://github.com/zwimer/DrShadowStack/pull/63) ([zwimer](https://github.com/zwimer))
- Improve logging [\#62](https://github.com/zwimer/DrShadowStack/pull/62) ([zwimer](https://github.com/zwimer))
- Now use boost filesystem to generate unique path [\#59](https://github.com/zwimer/DrShadowStack/pull/59) ([zwimer](https://github.com/zwimer))
- Internal SS now reference counts! [\#57](https://github.com/zwimer/DrShadowStack/pull/57) ([zwimer](https://github.com/zwimer))
- Bug fix [\#54](https://github.com/zwimer/DrShadowStack/pull/54) ([zwimer](https://github.com/zwimer))
- Docs [\#50](https://github.com/zwimer/DrShadowStack/pull/50) ([zwimer](https://github.com/zwimer))
- Default mode added and fixed logging bug on non-debug mode [\#49](https://github.com/zwimer/DrShadowStack/pull/49) ([zwimer](https://github.com/zwimer))
- Readme [\#46](https://github.com/zwimer/DrShadowStack/pull/46) ([zwimer](https://github.com/zwimer))
- Internal drrun [\#45](https://github.com/zwimer/DrShadowStack/pull/45) ([zwimer](https://github.com/zwimer))
- Version added [\#44](https://github.com/zwimer/DrShadowStack/pull/44) ([zwimer](https://github.com/zwimer))
- Argument parser [\#42](https://github.com/zwimer/DrShadowStack/pull/42) ([zwimer](https://github.com/zwimer))
- Updated readme [\#30](https://github.com/zwimer/DrShadowStack/pull/30) ([zwimer](https://github.com/zwimer))
- Update docs [\#29](https://github.com/zwimer/DrShadowStack/pull/29) ([zwimer](https://github.com/zwimer))
- Docs [\#28](https://github.com/zwimer/DrShadowStack/pull/28) ([zwimer](https://github.com/zwimer))
- Separate log file [\#9](https://github.com/zwimer/DrShadowStack/pull/9) ([zwimer](https://github.com/zwimer))
- Updated index.html [\#8](https://github.com/zwimer/DrShadowStack/pull/8) ([zwimer](https://github.com/zwimer))
- Updated cmake [\#7](https://github.com/zwimer/DrShadowStack/pull/7) ([zwimer](https://github.com/zwimer))
- Develop [\#6](https://github.com/zwimer/DrShadowStack/pull/6) ([zwimer](https://github.com/zwimer))
- Updated changelog [\#5](https://github.com/zwimer/DrShadowStack/pull/5) ([zwimer](https://github.com/zwimer))
- Develop [\#4](https://github.com/zwimer/DrShadowStack/pull/4) ([zwimer](https://github.com/zwimer))
- Added changelog [\#3](https://github.com/zwimer/DrShadowStack/pull/3) ([zwimer](https://github.com/zwimer))
- Made signals into c from cpp [\#2](https://github.com/zwimer/DrShadowStack/pull/2) ([zwimer](https://github.com/zwimer))
- Added another toy example [\#1](https://github.com/zwimer/DrShadowStack/pull/1) ([zwimer](https://github.com/zwimer))



\* *This Change Log was automatically generated by [github_changelog_generator](https://github.com/skywinder/Github-Changelog-Generator)*