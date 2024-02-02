

local _____OutputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
local _____ProjectRoot = os.realpath( "" )



workspace( "CPP_Delegate_Test" )
    architecture( "x86_64" )
    configurations({ "Debug", "Dev", "Release" })


-------------------------------------------------------------------------------
--                         FILTERS
-------------------------------------------------------------------------------


    filter({ "configurations:Debug" })
        runtime( "Debug" )
        optimize( "Off" )
        symbols( "On" )

        defines
        ({ 
            "PE_DEBUG",
            "PE_NDEV",
            "PE_NRELEASE"
        })

    filter({ "configurations:Dev" })
        runtime( "Release" )
        optimize( "Speed" )
        symbols( "Off" )

        defines
        ({
            "PE_NDEBUG",
            "PE_DEV",
            "PE_NRELEASE"
        })

    filter({ "configurations:Release" })
        runtime( "Release" )
        optimize( "Speed" )
        symbols( "Off" )

        defines
        ({
            "PE_NDEBUG",
            "PE_NDEV",
            "PE_RELEASE"
        })



    filter( "system:linux" )
        systemversion( "latest" )
        
        defines
        ({
            
        })
    
    filter({ "system:windows" })
        systemversion( "latest" )
    
        defines
        ({
            
        })
    
    filter({ "system:macosx" })
        systemversion( "latest" )
    
        defines
        ({
            
        })
    
        
    filter({ })


    defines
    ({
        "_CRT_SECURE_NO_WARNINGS"
    })


    flags
    ({
        "MultiProcessorCompile"
    })


-------------------------------------------------------------------------------
--                         PROJECT
-------------------------------------------------------------------------------


    project( "CPP_Delegate_Test" )
        kind( "ConsoleApp" )
        language( "C++" )
        cppdialect( "C++20" )
        staticruntime( "On" )

        targetdir( _____ProjectRoot ..  "/.GEN/Bin/" .. _____OutputDir .. "/%{prj.name}" )
        objdir( _____ProjectRoot ..  "/.GEN/Intermediate/" .. _____OutputDir .. "/%{prj.name}" )


        files
        ({
            _____ProjectRoot .. "/Src/**.h",
            _____ProjectRoot .. "/Src/**.cpp"
        })

        includedirs
        ({
            _____ProjectRoot .. "/Src/Public"
        })
