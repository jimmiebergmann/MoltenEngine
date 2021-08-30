# Third Party Libraries
External libraries are not located directly under this folder, but instead in its child folder "ThirdParty". This makes the #include paths cleaner, since all paths will start with "ThirdParty/".

Submodules are preferred over subtrees.

## Get submodules
After cloning Molten:
``` shell
$ git submodule update --init
```
or get them while cloning:
``` shell
$ git clone --recurse-submodules https://github.com/jimmiebergmann/MoltenEngine.git
```

### Available submodules
*   Google Test - <https://github.com/google/googletest> ([License](https://github.com/google/googletest/blob/master/LICENSE))
*   FreeType2 - <https://github.com/jimmiebergmann/FreeType2> ([License](https://github.com/jimmiebergmann/FreeType2/blob/master/docs/LICENSE.TXT))