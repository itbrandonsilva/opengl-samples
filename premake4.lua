print("\nRequired libraries: GLEW, GLFW, GLM");
-- Find LIB directories.
glewDir = os.matchdirs("./glew*")[1]
glfwDir = os.matchdirs("./glfw*")[1]
glmDir  = os.matchdirs("./glm*")[1]

if ( glewDir and glfwDir and glmDir ) then
    print("All libraries found.");
else 
    print("\nLibraries not found: ");
    if not glewDir then print("GLEW") end;
    if not glfwDir then print("GLFW") end;
    if not glmDir  then print("GLM") end;
    print("");
    os.exit();
end

glewDir = "./" .. glewDir;
glfwDir = "./" .. glfwDir;
glmDir  = "./" ..  glmDir;

solution "OpenGL Samples"
    configurations { "Debug", "Release" }
    includedirs { glewDir .. "/include", glfwDir .. "/include", glmDir }
    libdirs     { glewDir .. "/lib", glfwDir .. "/lib-msvc100" }
    links       { "opengl32", "GLFW", "glew32" }
 
    configuration "Debug"
        defines { "DEBUG" }
        flags { "Symbols" }

    configuration "Release"
        defines { "NDEBUG" }
        flags { "Optimize" } 

    project "Particles"
        kind "WindowedApp"
        location "./particles"
        language "C++"
        files { "particles/**.cpp", "particles/**.h" }

    project "BinTriTree"
        kind "WindowedApp"
        location "./bintritree"
        language "C++"
        files { "bintritree/**.cpp", "bintritree/**.h" }

    project "AStar"
        kind "WindowedApp"
        location "./astar"
        language "C++"
        files { "astar/**.cpp", "astar/**.h" }

    project "Lighting"
        kind "WindowedApp"
        location "./lighting"
        language "C++"
        files { "lighting/**.cpp", "lighting/**.h", "lighting/**.gls" }
