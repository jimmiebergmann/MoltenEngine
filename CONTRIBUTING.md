## How to contribute
1.  Fork this repository.
2.  Create a new branch with any of the following naming conventions:
    -   topic-*something* E.g. `topic-renderer`.
    -   feature-*something* E.g. `feature-my-new-cool-thingie`.
    -   fix-*something* E.g. `fix-broken-thing`.
    -   Feel free to reference to an issue. I.e. `fix-issue-123`.
3.  Commit changes.
4.  Send pull request at Github.

## Styleguides
-   Use 4 spaces for indention.
-   Put curly braces on their own line.
-   Class and function names are written in PascalCase, e.g. `class Object`.
-   Prefix template argument with T and use PascalCase for the rest, e.g. `template<typename TMyArg>`.
-   Public class variables and local variables are written in camelCase, e.g. `int32_t localVar = 0;`.
-   Private class variables are prefixed with "m_", followed with camelCase names, e.g. `float m_var;`.
-   Do not use global variables, other than constants, i.e. `constexpr` or `static const`.
-   Put output reference arguments at the beginning of any function's argument list.
-   Use fixed width integer types, e.g. `uint8_t`, `int32_t`, `uint32_t`.
-   Document public members of classes.

## Tests
Make sure to add new unit tests for new classes, functions or features. Unit tests are written using the `googletest` framework, which is available as a submodule.

## License
This project is licensed under MIT license. Put license notice on top of all header and source files.