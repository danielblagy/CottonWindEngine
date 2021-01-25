# CottonWindEngine
C++ SDL2 OpenGL Header-Only Game Engine/Framework
## Notes
### Important Design Choice Notice
* At least for now, I want this engine to be header-only
* Support multiple graphics apis (because of the whole imgui thing)
### Problems
* entt doesn't compile with vs 2017 (with either c++ 17 or c++ latest, conformance mode switched off),
  * I've created a fresh empty project to test entt out, still doesn't compile
  * I've tried to include the header from the latest release, rather than the latest commit, still no luck :(