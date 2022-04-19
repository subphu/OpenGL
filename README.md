# OpenGL
 
 ## Xcode
 - Install GLEW and GLFW
 - Create new Command Line Tool project
 - In General, Frameworks and Libraries, add OpenGL.framework
 - In General, Frameworks and Libraries, add files, go to `/usr/local/Cellar`, search and add libGLEW.2.1.dylib and libglfw.3.dylib
 - In Build Settings, Header Search Paths, add `$(SRCROOT)/Sandbox` and `/usr/local/include`
 - in Build Phases, Copy Files, change Destination to Frameworks, clear Subpath, and add resource and shader folder
![Red Maple](https://user-images.githubusercontent.com/13620169/164016922-3d79dd60-4892-475c-9186-560a0123c935.png)
