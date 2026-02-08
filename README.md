# OpenGL Sandbox / Model viewer

![proba](https://github.com/user-attachments/assets/553807e0-4712-4377-851f-3b7199c0e2c7)

[Download the demo for Windows](https://github.com/akalicanin/opengl-project/releases)

This is an OpenGL/glfw sandbox. It has movement, camera controls, Phong lighting and model loading capabilities.  

This is an incomplete project I work on from time to time.   
This release however is a fully functional demo of everything i've so far learned about OpenGL and implemented.  
It comes with 3 scenes/maps, selectable with 1/2/3 on your keyboard, that showcase different things.  

Map 1 is a mathematically calculated helix with light beams between them. On this scene, if you enable Debug view (R), you will be able to see where exactly the "light points" are located within the beam. The colors of the cubes represent the color the light itself is giving off.  

Map 2 and map 3 are models with a circling light.

I plan to further work on this project and turn it into something big, for now this small sandbox is available.

## CONTROLS

- WASD - Movement
- Mouse movement - Camera controls
- Mouse scroll - Zoom in, zoom out
- 1 - Select map 1
- 2 - Select map 2
- 3 - Select map 3
- R - Debug mode (only works on map1)
- F - Turn on flashlight
- U/I/O/P - Change background colors
- ESC - Quit program

## DISCLAIMER

This project uses many libraries I do not own and have not contributed to. Assimp, stb, glm, KHR, glad, glfw.  
Also, the 3D model from scene 3 is from learnopengl.com's example.
