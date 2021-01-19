# CottonWindEngine
C++ SDL2 OpenGL Header-Only Game Engine/Framework
## Notes
### Important Design Choice Notice
Don't use SDL_Renderer, as it conflicts with ImGui, use modern OpenGL (3.2 core) directly instead