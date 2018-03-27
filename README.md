# DrShadowStack

TODO: put a description

## Requirements

TODO: 
dynamorio
```gem install github_changelog_generator```

## Installation Instructions

TODO:

## Usage

Requires boost: sudo apt-get install libboost-all-dev




```
#include<unistd.h>
#include<stdio.h>
#include <sys/syscall.h>
int main() {

    printf("%d\n", syscall(SYS_gettid));
    vfork();
    printf("%d\n", syscall(SYS_gettid));
    execve("/bin/true", NULL, NULL);
    printf("%d\n", syscall(SYS_gettid));

}
... vfork -> execve
```

Special fix : analysis, if mis-match use drsym



## Documentation

Additional documentation to each component of DrShadowStack is provided and can also be found TODO:

 This documentation was generated via Doxygen. If you wish to generate this documentation yourself, from the DrShadowStack directory, run the following
```bash
doxygen
```

The documentation 'index.html' is located within Documentation/html/
