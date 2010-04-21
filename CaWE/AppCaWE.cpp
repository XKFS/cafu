/*
=================================================================================
This file is part of Cafu, the open-source game and graphics engine for
multiplayer, cross-platform, real-time 3D action.
$Id$

Copyright (C) 2002-2010 Carsten Fuchs Software.

Cafu is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

Cafu is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Cafu. If not, see <http://www.gnu.org/licenses/>.

For support and more information about Cafu, visit us at <http://www.cafu.de>.
=================================================================================
*/

#include "wx/wx.h"
#include "wx/confbase.h"
#include "wx/dir.h"
#include "wx/fileconf.h"
#include "wx/filename.h"
#include "wx/image.h"
#include "wx/snglinst.h"
#include "wx/splash.h"
#include "wx/stdpaths.h"

#include "AppCaWE.hpp"
#include "ChildFrameViewWin2D.hpp"
#include "ChildFrameViewWin3D.hpp"
#include "CursorMan.hpp"
#include "GameConfig.hpp"
#include "ParentFrame.hpp"
#include "MapDocument.hpp"
#include "MapElement.hpp"
#include "Options.hpp"
#include "EditorMaterialManager.hpp"
#include "ToolbarMaterials.hpp"
#include "Tool.hpp"
#include "ToolManager.hpp"

#include "ClipSys/CollisionModelMan_impl.hpp"
#include "ConsoleCommands/Console.hpp"
#include "ConsoleCommands/ConsoleStdout.hpp"
#include "ConsoleCommands/ConsoleInterpreter.hpp"
#include "ConsoleCommands/ConsoleInterpreterImpl.hpp"
#include "ConsoleCommands/ConVar.hpp"
#include "ConsoleCommands/ConFunc.hpp"
#include "FileSys/FileManImpl.hpp"
#include "GuiSys/GuiMan.hpp"
#include "GuiSys/Window.hpp"
#include "MaterialSystem/MaterialManagerImpl.hpp"
#include "TypeSys.hpp"


static cf::ConsoleStdoutT ConsoleStdout;
cf::ConsoleI* Console=&ConsoleStdout;

//static cf::ConsoleFileT ConsoleFile("info.log");
//cf::ConsoleI* Console=&ConsoleFile;

static cf::FileSys::FileManImplT FileManImpl;
cf::FileSys::FileManI* cf::FileSys::FileMan=&FileManImpl;

static cf::ClipSys::CollModelManImplT CCM;
cf::ClipSys::CollModelManI* cf::ClipSys::CollModelMan=&CCM;

cf::GuiSys::GuiManI* cf::GuiSys::GuiMan=NULL;


OptionsT Options;


IMPLEMENT_APP(AppCaWE)


AppCaWE::AppCaWE()
    : wxApp(),
      m_FileConfig(NULL),
      m_ParentFrame(NULL)
{
    static ConsoleInterpreterImplT ConInterpreterImpl;
    ConsoleInterpreter=&ConInterpreterImpl;

    // All global convars and confuncs have registered themselves in linked lists.
    // Register them with the console interpreter now.
    ConFuncT::RegisterStaticList();
    ConVarT ::RegisterStaticList();
}


bool AppCaWE::OnInit()
{
    cf::GuiSys::GetWindowTIM().Init();  // The one-time init of the Window type info manager.
    GetMapElemTIM().Init();             // The one-time init of the map elements type info manager.
    GetToolTIM().Init();                // The one-time init of the tools type info manager.

    const std::string AppDir="./CaWE";

    // This is for registering the CaWE.cmat file farther below.
    cf::FileSys::FileMan->MountFileSystem(cf::FileSys::FS_TYPE_LOCAL_PATH, AppDir+"/res/", AppDir+"/res/");
    cf::FileSys::FileMan->MountFileSystem(cf::FileSys::FS_TYPE_LOCAL_PATH, "./", "");


    const wxString UserDataDir=wxStandardPaths::Get().GetUserDataDir();

    if (!wxFileName::Mkdir(UserDataDir, 0777, wxPATH_MKDIR_FULL))
        wxMessageBox(wxString("Config file storage path \n")+UserDataDir+"\n doesn't exist, and I was not able to create it, either.", "Warning!");


    static wxSingleInstanceChecker SIC(GetAppName()+"-"+wxGetUserId(), UserDataDir);

    if (SIC.IsAnotherRunning())
    {
        wxMessageBox("An instance of CaWE is already running!");
        return false;
    }


    // Undo the wx locale initialization, as I want to be sure to use the same (default) locale "C" always and everywhere.
    // Using other locales introduces a lot of subtle errors. E.g. reading floating point numbers from anywhere
    // (like map files!) fails because e.g. "1.4" is no proper floating point string in the German locale (but "1,4" is).
    setlocale(LC_ALL, "C");

    // Activate a log console as the wx debug target.
    // wxLog::SetActiveTarget(new wxLogWindow(NULL, "wx Debug Console", true, false));

    wxInitAllImageHandlers();   // Needed e.g. for cursor initialization under GTK.


    // Set the globally used configuration storage object for easy access via wxConfigBase::Get().
    if (wxFileExists(UserDataDir+"/CaWE_2.config"))
    {
        if (wxRenameFile(UserDataDir+"/CaWE_2.config", UserDataDir+"/CaWE.config"))
            wxMessageBox("Your old config file at\n"+UserDataDir+"/CaWE_2.config"+
                         "\nwas successfully renamed to\n"+UserDataDir+"/CaWE.config"+"\n\nThis was a one-time update.\n :O)",
                         "Config update successful");
    }

    m_FileConfig=new wxFileConfig("CaWE", "Carsten Fuchs Software", UserDataDir+"/CaWE.config");
    wxConfigBase::Set(m_FileConfig);


    // Setup the global Material Manager pointer.
    static MaterialManagerImplT MatManImpl;

    MaterialManager=&MatManImpl;

    // Register the material script with the CaWE materials definitions.
    if (MaterialManager->RegisterMaterialScript(AppDir+"/res/CaWE.cmat", AppDir+"/res/").Size()==0)
        wxMessageBox("CaWE.cmat not found in \""+AppDir+"\".", "WARNING");


    // Create the MDI parent frame.
    m_ParentFrame=new ParentFrameT();

    SetTopWindow(m_ParentFrame);


    // Check for autosave files.
    wxDir AutoSaveDir(UserDataDir);

    if (AutoSaveDir.IsOpened())
    {
        ArrayT<wxString> AutoSavedFileNames;
        wxString         AutoSavedFileName;

        for (bool more=AutoSaveDir.GetFirst(&AutoSavedFileName, "autosave*.cmap", wxDIR_FILES); more; more=AutoSaveDir.GetNext(&AutoSavedFileName))
            AutoSavedFileNames.PushBack(AutoSavedFileName);

        if (AutoSavedFileNames.Size()>0)
        {
            const unsigned long ID=wxGetLocalTime();
            wxString RecoveredList;

            for (unsigned long FileNr=0; FileNr<AutoSavedFileNames.Size(); FileNr++)
            {
                const wxString NewName =wxString::Format("recovered_%lu_%lu.cmap", FileNr, ID);
                const bool     RenameOK=wxRenameFile(UserDataDir+"/"+AutoSavedFileNames[FileNr], UserDataDir+"/"+NewName);

                RecoveredList+=RenameOK ? NewName : AutoSavedFileNames[FileNr] + "(WARNING: renaming to "+NewName+" failed! Back it up immediately yourself!)";
                RecoveredList+="\n";
            }

            wxMessageBox(wxString("A previously crashed or otherwise prematurely aborted instance of CaWE left auto-saved files.\n")+
                         "They are kept in "+UserDataDir+" under the name(s)\n\n"+RecoveredList+"\n"+
                         "You may open them as usual, and save, move, delete or rename them at your convenience.",
                         "Crash recovery: auto-save files found and recovered!", wxOK | wxICON_EXCLAMATION);
        }
    }
    else
    {
        wxMessageBox(wxString("Could not open auto-save directory ")+UserDataDir, "WARNING", wxOK | wxICON_ERROR);
    }


    // Show the splash screen.
    // printf("%s   Before Splash\n", wxNow().c_str());
    wxBusyCursor BusyCursor;
#ifndef __WXGTK__
    wxSplashScreen SplashScreen(wxBitmap(AppDir+"/res/CaWE-logo.png", wxBITMAP_TYPE_PNG),
                                wxSPLASH_CENTRE_ON_PARENT | wxSPLASH_NO_TIMEOUT, 250, m_ParentFrame, -1);
#endif
    // printf("%s   After Splash\n", wxNow().c_str());


    srand(time(NULL));


    /**********************************************/
    /*** Load and init the Cafu Material System ***/
    /**********************************************/

    // Initialize the global options from the CaWE config files.
    Options.Init();

    // Initialize the global cursor manager instance.
    wxASSERT(CursorMan==NULL);
    CursorMan=new CursorManT;

    return wxApp::OnInit();
}


int AppCaWE::OnExit()
{
    // TODO: delete Options; Options=NULL;

    wxConfigBase::Set(NULL);

    delete m_FileConfig;
    m_FileConfig=NULL;

    // TODO: delete cf::FileSys::FileMan;   // Shoud have   cf::FileSys::FileMan=new cf::FileSys::FileManImplT;   in OnInit().
    cf::FileSys::FileMan=NULL;

    ConsoleInterpreter=NULL;     // This is very important, to make sure that no ConFuncT or ConVarT dtor accesses the ConsoleInterpreter that might already have been destroyed then.

    // Shutdown the global cursor manager instance.
    delete CursorMan;
    CursorMan=NULL;

    return wxApp::OnExit();
}