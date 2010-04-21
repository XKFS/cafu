import os, sys
import CompilerSetup

Import('env', 'buildMode', 'compiler')


CommonWorldObject = env.StaticObject("Common/World.cpp")

env.Program('CaBSP/CaBSP',   # I had preferred writing 'CaBSP' instead of 'CaBSP/CaBSP' here, but then under Linux we would get both a directory *and* an executeable with name 'CaBSP' in the build directory, which is not allowed/possible.
    Split("CaBSP/CaBSP.cpp CaBSP/BspTreeBuilder/BspTreeBuilder.cpp") + CommonWorldObject,
    LIBS=Split("SceneGraph MatSys cfsLib cfsCoreLib cfsLib ClipSys cfs_png cfs_jpeg bulletcollision lua minizip lightwave z"))   # cfsCoreLib is because ConVarTs are used in the SceneGraph. lua is because...?

env.Program('CaPVS/CaPVS',
    Split("CaPVS/CaPVS.cpp CaPVS/CaPVSWorld.cpp") + CommonWorldObject,
    LIBS=Split("SceneGraph MatSys cfsLib cfsCoreLib cfsLib ClipSys cfs_png cfs_jpeg bulletcollision lua minizip lightwave z"))   # cfsCoreLib is because ConVarTs are used in the SceneGraph. lua is because...?

env.Program('CaLight/CaLight',
    Split("CaLight/CaLight.cpp CaLight/CaLightWorld.cpp") + CommonWorldObject,
    LIBS=Split("SceneGraph MatSys cfsLib cfsCoreLib cfsLib ClipSys cfs_png cfs_jpeg bulletcollision lua minizip lightwave z"))   # cfsCoreLib is because ConVarTs are used in the SceneGraph. lua is because...?

env.Program('CaSHL/CaSHL',
    Split("CaSHL/CaSHL.cpp CaSHL/CaSHLWorld.cpp") + CommonWorldObject,
    LIBS=Split("SceneGraph MatSys cfsLib cfsCoreLib cfsLib ClipSys cfs_png cfs_jpeg bulletcollision lua minizip lightwave z"))   # cfsCoreLib is because ConVarTs are used in the SceneGraph. lua is because...?

# env.Program('CaCook/CaCook',
#     Split("""CaCook/CaCook.cpp
#              CaCook/NXU_Asc2Bin.cpp CaCook/NXU_ColladaExport.cpp CaCook/NXU_ColladaImport.cpp CaCook/NXU_cooking.cpp
#              CaCook/NXU_customcopy.cpp CaCook/NXU_File.cpp CaCook/NXU_Geometry.cpp CaCook/NXU_GraphicsMesh.cpp
#              CaCook/NXU_helper.cpp CaCook/NXU_hull.cpp CaCook/NXU_PhysicsExport.cpp CaCook/NXU_PhysicsInstantiator.cpp
#              CaCook/NXU_ScaledCopy.cpp CaCook/NXU_schema.cpp CaCook/NXU_SchemaStream.cpp CaCook/NXU_Streaming.cpp
#              CaCook/NXU_string.cpp CaCook/NXU_tinystr.cpp CaCook/NXU_tinyxml.cpp CaCook/NXU_tinyxmlerror.cpp
#              CaCook/NXU_tinyxmlparser.cpp"""),
#     CPPDEFINES=env["CPPDEFINES"]+["WIN32"],
#     CPPPATH=env["CPPPATH"]+["C:\\Programme\\NVIDIA Corporation\\NVIDIA PhysX SDK\\v2.8.1\\SDKs\\Cooking\\include",
#                             "C:\\Programme\\NVIDIA Corporation\\NVIDIA PhysX SDK\\v2.8.1\\SDKs\\Foundation\\include",
#                             "C:\\Programme\\NVIDIA Corporation\\NVIDIA PhysX SDK\\v2.8.1\\SDKs\\Physics\\include",
#                             "C:\\Programme\\NVIDIA Corporation\\NVIDIA PhysX SDK\\v2.8.1\\SDKs\\PhysXLoader\\include"],
#     LIBPATH=env["LIBPATH"]+["C:\\Programme\\NVIDIA Corporation\\NVIDIA PhysX SDK\\v2.8.1\\SDKs\\lib\\Win32"],
#     LIBS=Split("SceneGraph ClipSys MatSys cfsLib cfsCoreLib cfsLib cfs_png cfs_jpeg lua lightwave z")
#         +Split("NxCooking PhysXLoader"))



envTools = env.Clone()

if sys.platform=="win32":
    envTools.Append(CPPPATH=['ExtLibs/freetype/include'])       # Linux builds (must) use the systems freetype library instead.
    envTools.Append(LIBPATH=['ExtLibs/DirectX7/lib'])
    # glu32 is only needed for the TerrainViewerOld...
    envTools.Append(LIBS=Split("SceneGraph MatSys cfsCoreLib cfsLib ClipSys cfs_png cfs_jpeg bulletcollision lua minizip lightwave z")
                       + Split("gdi32 glu32 opengl32 user32") + ['dinput', 'dxguid'])
elif sys.platform=="linux2":
    # envTools.Append(LINKFLAGS = ['-Wl,--export-dynamic'])     # Not needed any more, .so libs now link to the required .a libs directly, just as under Windows.
    # GLU is needed for the TerrainViewerOld *and* for e.g. gluBuild2DMipmaps() in the renderers...
    envTools.Append(CPPPATH=['/usr/include/freetype2'])         # As of 2009-09-10, this line is to become unnecessary in the future, see /usr/include/ftbuild.h for details.
    envTools.Append(LIBS=Split("SceneGraph MatSys cfsLib cfsCoreLib cfsLib ClipSys cfs_png cfs_jpeg bulletcollision lua minizip lightwave z")
                       + Split("GL GLU"))

envTools.Program('CaSanity', ['CaTools/CaSanity.cpp'] + CommonWorldObject)

envTools.Program("MakeFont", "CaTools/MakeFont.cpp", LIBS=envTools["LIBS"]+["freetype"])

envTools.Program('MaterialViewer', "CaTools/MaterialViewer.cpp")

envTools.Program('ModelViewer', "CaTools/ModelViewer.cpp")

envTools.Program('TerrainViewer', "CaTools/TerrainViewer.cpp", CPPPATH=envTools["CPPPATH"]+["ExtLibs/zlib"])

envTools.Program('TerrainViewerOld', "CaTools/TerrainViewerOld.cpp")

if sys.platform=="win32":
    env.Program('ReadDump', "CaTools/ReadDump.cpp", LIBS="wsock32")
elif sys.platform=="linux2":
    env.Program('ReadDump', "CaTools/ReadDump.cpp")



envCafu = env.Clone()
envCafu.Append(CPPPATH=['ExtLibs/lua/src'])

CafuMainObj     = envCafu.StaticObject("Ca3DE/Ca3DE",      "Ca3DE/Cafu.cpp")
CafuMainDediObj = envCafu.StaticObject("Ca3DE/Ca3DE-dedi", "Ca3DE/Cafu.cpp", CPPDEFINES=env['CPPDEFINES']+['CA3DE_DEDICATED_SERVER'])

EngineCommonAndServerObjs = envCafu.StaticObject(Split("""
    Ca3DE/Both/Ca3DEWorld.cpp Ca3DE/Both/EntityManager.cpp Ca3DE/Both/EngineEntity.cpp
    Ca3DE/Server/Server.cpp Ca3DE/Server/ServerWorld.cpp Ca3DE/Server/ClientInfo.cpp"""))

if sys.platform=="win32":
    envCafu.Append(LIBPATH=['ExtLibs/DirectX7/lib'])
    envCafu.Append(LIBS=Split("SceneGraph MatSys SoundSys cfsLib cfsCoreLib cfs_png cfs_jpeg bulletcollision minizip z lua ClipSys GuiSysNullEditor"))
    envCafu.Append(LIBS=Split("lightwave"))    # For the GuiSys::ModelWindowT class.
    envCafu.Append(LIBS=Split("gdi32 opengl32 user32 wsock32") + ['dinput', 'dxguid'])
    WinResource=envCafu.RES("Ca3DE/Dialog1.rc") + envCafu.RES("Ca3DE/Cafu.rc", CPPPATH=[CompilerSetup.wxMSW_Path+'/include'])

elif sys.platform=="linux2":
    WinResource=[]
    # -Wl,-rpath,.           is so that also the . directory is searched for dynamic libraries when they're opened.
    # -Wl,--export-dynamic   is so that the exe exports its symbols so that the MatSys, SoundSys and game .so libs can in turn resolve theirs.
    envCafu.Append(LINKFLAGS = ['-Wl,-rpath,.', '-Wl,--export-dynamic'])
    envCafu.Append(LIBS=Split("MatSys SoundSys SceneGraph cfsLib cfsCoreLib cfs_png cfs_jpeg bulletdynamics bulletcollision bulletmath openal alut mpg123 ogg vorbis vorbisfile minizip z lua lightwave ClipSys GuiSysNullEditor"))
    # We need GLU for e.g. gluBuild2DMipmaps() in the renderers.
    # pthread is needed because some libraries that we load (possibly indirectly), e.g. the libCg.so and libopenal.so, use functions
    # from the pthread library, but have not been linked themselves against it. They rely on the executable to be linked appropriately
    # in order to resolve the pthread symbols. Paul Pluzhnikov states in a newsgroup posting (see [1]) that even if the .so libs were
    # linked against libpthread.so, the main exe still *must* link with -lpthread, too, because:
    # "Note that dlopen()ing an MT library from non-MT executable is not supported on most platforms, certainly not on Linux."
    # [1] http://groups.google.de/group/gnu.gcc.help/browse_thread/thread/1e8f8dfd6027d7fa/
    # rt is required in order to resolve clock_gettime() in openal-soft.
    envCafu.Append(LIBS=Split("GL GLU rt pthread"))
    # Wrapping -lcfsLib in --whole-archive and --no-whole-archive is required so that the linker puts all symbols that are in libcfsLib.a
    # into the executable, because otherwise, it would omit e.g. some ParticleEngine-related stuff that is not referenced by the engine,
    # and when the game DLL later needs it, we get an "undefined symbol" error from dlopen().
    # See my post "Having the GNU linker *not* remove unused symbols..." to the gnu.g++.help newsgroup on 2006-04-07,
    # and the replies by Maett and Paul Pluzhnikov.
    # Implementing this by appending to LINKCOM and using --allow-multiple-definition is a SCons-specific hack though,
    # because SCons currently does not support such kind of "wrapping". See my post to the scons-users mailing list on 2006-04-09
    # at http://scons.tigris.org/servlets/BrowseList?list=users&by=thread&from=455553.
    # The "-llightwave, ..." are all needed as a direct consequence of the forced --whole-archive for cfsLib,
    # which in turn requires these...
    # Note that this (using --whole-archive) is actually the proper strategy under Linux (vs. Windows), because this is *the* way
    # in order to make sure that the -fPIC can be handled correctly - otherwise we had to link .so libs with non-fPIC object files...
    envCafu.Append(LINKCOM=" -Wl,--allow-multiple-definition -Wl,--whole-archive -lcfsLib -lbulletdynamics -lbulletcollision -lbulletmath -lopenal -lalut -lmpg123 -logg -lvorbis -lvorbisfile -Wl,--no-whole-archive -llightwave -lz")

envCafu.Program('Ca3DE/Cafu',
    CafuMainObj + EngineCommonAndServerObjs + CommonWorldObject + ["Common/WorldMan.cpp"] + WinResource +
    Glob("Ca3DE/Client/*.cpp"))

# Build an explicit dedicated server currently only under Linux.
# Build it with env.Program() instead of envCafu.Program()?  I used env before, and lazily fixed link problems by switching to envCafu...
if sys.platform=="linux2":
    envCafu.Program('Ca3DE/Cafu-dedicated', CafuMainDediObj + EngineCommonAndServerObjs + CommonWorldObject + ["Common/WorldMan.cpp"])



SourceFilesList=(Glob("CaWE/*.cpp")
    +Glob("CaWE/FontWizard/*.cpp")
    +Glob("CaWE/GuiEditor/*.cpp")+Glob("CaWE/GuiEditor/Commands/*.cpp")+Glob("CaWE/GuiEditor/EditorData/*.cpp")
    +Glob("CaWE/MapCommands/*.cpp")
    +Glob("CaWE/MaterialBrowser/*.cpp")
    +Glob("CaWE/ModelEditor/*.cpp")
    +Glob("CaWE/wxExt/*.cpp")
    +Glob("CaWE/wxFB/*.cpp"))

wxEnv = env.Clone()
wxEnv.Append(CPPPATH=['ExtLibs/lua/src',
                      'ExtLibs/noise/src'])

if sys.platform=="win32":
    wxPath=CompilerSetup.wxMSW_Path;

    wxEnv.Append(CPPPATH = [wxPath+'/include'])

    # TODO: Move this into the SConstruct file (including the wx include path above).
    #   Note that we only (want to) determine the right library path matching the used compiler here.
    #   The specific wx-version used (e.g. latest stable vs. trunk) is still determined locally (here),
    #   BUT if this is moved into the SConstruct file, also the wx-version (wxPath above) must be fixed there.
    LibPath="/lib/"+compiler+"_lib"

    wxEnv.Append(CPPPATH=['ExtLibs/freetype/include'])      # Linux builds (must) use the systems freetype library instead.
    wxEnv.Append(LIBPATH = [wxPath+LibPath])
    wxEnv.Append(LIBS = Split("SceneGraph MatSys cfsLib cfsCoreLib ClipSys cfs_png cfs_jpeg bulletcollision noise lua minizip lightwave z"))
    wxEnv.Append(LIBS = Split("freetype"))
    wxEnv.Append(LIBS = Split("advapi32 comctl32 comdlg32 gdi32 ole32 oleaut32 opengl32 rpcrt4 shell32 user32 wsock32"))

    if buildMode=="dbg":
        wxEnv.Append(CPPDEFINES = ['__WXDEBUG__'])
        wxEnv.Append(CPPPATH = [wxPath+LibPath+"/mswud"])
        wxEnv.Append(LIBS = Split("wxbase29ud wxbase29ud_net wxjpegd wxmsw29ud_adv wxmsw29ud_core wxmsw29ud_gl wxmsw29ud_aui wxmsw29ud_propgrid wxregexud"))
    else:
        wxEnv.Append(CPPPATH = [wxPath+LibPath+"/mswu"])
        wxEnv.Append(LIBS = Split("wxbase29u wxbase29u_net wxjpeg wxmsw29u_adv wxmsw29u_core wxmsw29u_gl wxmsw29u_aui wxmsw29u_propgrid wxregexu"))

    WinResource=wxEnv.RES("CaWE/CaWE.rc")

elif sys.platform=="linux2":
    wxPath=Dir(CompilerSetup.wxGTK_Path).abspath;

    wxEnv.Append(CPPPATH=['/usr/include/freetype2'])        # As of 2009-09-10, this line is to become unnecessary in the future, see /usr/include/ftbuild.h for details.
    wxEnv.Append(LINKFLAGS = ['-Wl,--export-dynamic'])      # Need this so that the Renderer DLLs can have their unresolved symbols dynamically resolved at load time.
    wxEnv.Append(LIBS = Split("SceneGraph MatSys cfsCoreLib cfsLib ClipSys cfs_png cfs_jpeg bulletcollision noise lua minizip lightwave z"))

    if buildMode=="dbg":
        wxEnv.Append(LIBS = Split("wx_gtk2ud_gl-2.9 wx_gtk2ud_aui-2.9 wx_gtk2ud_propgrid-2.9 wx_gtk2ud_xrc-2.9 wx_gtk2ud_qa-2.9 wx_gtk2ud_html-2.9 wx_gtk2ud_adv-2.9 wx_gtk2ud_core-2.9 wx_baseud_xml-2.9 wx_baseud_net-2.9 wx_baseud-2.9"))
        wxEnv.ParseConfig(wxPath + "/build-gtk_d/wx-config --cxxflags --libs std,gl")
    else:
        wxEnv.Append(LIBS = Split("wx_gtk2u_gl-2.9 wx_gtk2u_aui-2.9 wx_gtk2u_propgrid-2.9 wx_gtk2u_xrc-2.9 wx_gtk2u_qa-2.9 wx_gtk2u_html-2.9 wx_gtk2u_adv-2.9 wx_gtk2u_core-2.9 wx_baseu_xml-2.9 wx_baseu_net-2.9 wx_baseu-2.9"))
        wxEnv.ParseConfig(wxPath + "/build-gtk_r/wx-config --cxxflags --libs std,gl")

    WinResource=[]

wxEnv.Program('CaWE/CaWE', SourceFilesList + WinResource + CommonWorldObject)