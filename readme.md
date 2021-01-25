# CottonWindEngine
C++ SDL2 OpenGL Header-Only Game Engine/Framework
## To Do
### Core
- [x] Game application framework class
- [x] Delta time handling
- [x] VSync handling
- [x] Event system
- [x] Layers system
- [x] Input polling
- [ ] Scene abstaction (ecs)
- [ ] Audio
### Utility
- [x] Logger
- [x] Event dispatcher
- [x] ImGui Layer (mostly for debugging)
### SDL2 Rendering (2D)
- [x] Render functions for point, line, rect primitives
- [ ] Render functions for circle, triangle primitives
- [ ] Texture rendering
- [ ] Text rendering
### Modern OpenGL Rendering
- [ ] Abstractions for buffers
- [ ] Shader abstraction
- [ ] Render functions for 2D primitives
- [ ] 2D batch renderer
- [ ] 3D rendering
## Notes
### Important Design Choice Notice
* At least for now, I want this engine to be header-only
* Support multiple graphics apis (because of the whole imgui thing)
### Problems
* entt doesn't compile with vs 2017 (with either c++ 17 or c++ latest, conformance mode switched off),
  * I've created a fresh empty project to test entt out, still doesn't compile
  * I've tried to include the header from the latest release, rather than the latest commit, still no luck :(