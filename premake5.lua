-- premake5.lua
workspace "CapsLockNotifier"
   configurations { "Debug", "Release" }
   architecture "x86_64"
   location "build"

project "CapsLockNotifier"
   kind "WindowedApp"
   language "C++"
   cppdialect "C++20"
   targetdir "%{wks.location}/bin/%{cfg.buildcfg}"
   objdir "%{wks.location}/bin-int/%{cfg.buildcfg}"

   -- Include source and resource files
   files { "src/**.cpp", "resource/**.rc", "include/**.h" }
   includedirs { "." }
       
   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
   
   filter "system:windows"
      systemversion "latest"
      links { "user32", "gdi32", "shell32" }