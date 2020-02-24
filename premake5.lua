workspace "Poission"
	architecture "x64"
	
	startproject "Poission"

	configurations
	{
		"Debug",
		"Release"
		
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

--include directories relative to root folder(solution direction)
IncludeDir = {}

group "Dependencies"


group ""



project "Poission"
	location "Poission"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	--pchheader "hzpch.h"
	--pchsource "Hazel/src/hzpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
		
		
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/include"
	
	}

	libdirs
	{
		"%{prj.name}/vendor/build/x64/vc14/lib",
		"%{prj.name}/vendor/build/x86/vc14/lib"
		
	}

	links
	{
		"opencv_ml2413d.lib",
"opencv_calib3d2413d.lib",
"opencv_core2413d.lib",
"opencv_highgui2413d.lib",
"opencv_contrib2413d.lib",
"opencv_features2d2413d.lib",
"opencv_flann2413d.lib",
"opencv_gpu2413d.lib",
"opencv_imgproc2413d.lib",
"opencv_legacy2413d.lib",
"opencv_objdetect2413d.lib",
"opencv_ts2413d.lib",
"opencv_video2413d.lib",
"opencv_nonfree2413d.lib",
"opencv_ocl2413d.lib",
"opencv_photo2413d.lib",
"opencv_stitching2413d.lib",
"opencv_superres2413d.lib",
"opencv_videostab2413d.lib"
	   
	}
	filter "system:windows"
		systemversion "latest"

		defines
		{
			
		}

	

	filter "configurations:Debug"
		defines "HZ_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "HZ_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "HZ_DIST"
		runtime "Release"
		optimize "on"
