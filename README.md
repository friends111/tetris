# Tetris
A Tetris implementation written in C++ using OpenGL.

## Description
A project used to learn more about OpenGL and C++. Because of the latter, there is a lot of gimmicks and seemingly unnecessary things used, as they served their learning purpose. Nevertheless, the code should not feel badly written, even though I'm not satisfied with the main Tetris class organization (or small lack thereof).

Tetris rules try to follow the official Tetris Guideline as much as possible. That includes SRS, 7-bag RNG, kicks, T-Spins, scoring system, lock delay, etc. There is only one game mode - endless.

There is no font cache, even though it could be used to optimize unnecessary recalculations of the same values when rendering text. The project is small enough that I skipped this feature.
There is no texture cache, because it uses only one texture. But again, if this was a bigger project that needed multiple textures, it would be wise to create a global texture cache (just for organization purposes).
Uniform Buffer Objects and Uniform Blocks could also be used if there was much more uniform values needed to be updated.

I haven't tried to compile the project on anything except Windows, even though there is some preprocessor code to potentially allow cross-platform compilation.

## Dependencies
The following versions were used during development, but higher, compatible ones should also work. 
* C++14
* GLEW 2.1.0
* GLFW 3.3.6 (OpenGL context (window) creation)
* GLM 0.9.9.8 (math)
* irrKlang 1.6.0 (sounds)

## Version History
* 1.0
    * Initial release
