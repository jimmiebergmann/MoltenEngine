# Third Party Libraries
External libraries are not located directly under this folder, but instead in its child folder "ThirdParty". This makes the #include paths cleaner, since all paths will start with "ThirdParty/".

Submodules are preferred over subtrees.

## Get submodules
After cloning Molten:
```sh
$ git submodule update --init
```
or get them while cloning:
```sh
$ git clone --recurse-submodules https://github.com/jimmiebergmann/MoltenEngine.git
```

### Available submodules
* Google Test - https://github.com/google/googletest