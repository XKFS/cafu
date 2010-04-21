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

#include "ChildFrame.hpp"
#include "Document.hpp"
#include "ModelSetup.hpp"
#include "SceneSetup.hpp"
#include "SceneView.hpp"
#include "../ParentFrame.hpp"

#include "wx/wx.h"
#include "wx/confbase.h"
//#include "wx/settings.h"
//#include "wx/file.h"
//#include "wx/filename.h"
//#include "wx/numdlg.h"


namespace ModelEditor
{
    // Default perspective set by the first childframe instance and used to restore default settings later.
    static wxString AUIDefaultPerspective;
}


BEGIN_EVENT_TABLE(ModelEditor::ChildFrameT, wxMDIChildFrame)
    EVT_MENU_RANGE(ID_MENU_FILE_CLOSE, ID_MENU_FILE_SAVEAS, ModelEditor::ChildFrameT::OnMenuFile)
    EVT_CLOSE     (ModelEditor::ChildFrameT::OnClose)
END_EVENT_TABLE()


ModelEditor::ChildFrameT::ChildFrameT(ParentFrameT* Parent, const wxString& Title, ModelDocumentT* ModelDoc)
    : wxMDIChildFrame(Parent, wxID_ANY, Title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER | wxMAXIMIZE),
      m_Parent(Parent),
      m_Title(Title),
      m_ModelDoc(ModelDoc),
      m_SceneView(NULL),
      m_ModelSetup(NULL),
      m_SceneSetup(NULL)
{
    // Register us with the parents list of children.
    m_Parent->m_MdlChildFrames.PushBack(this);

    // Set up menu.
    wxMenuBar *item0 = new wxMenuBar;


    wxMenu* NewMenu = new wxMenu;
    NewMenu->Append(ParentFrameT::ID_MENU_FILE_NEW_MAP,   wxT("New &Map\tCtrl+N"), wxT(""));
    NewMenu->Append(ParentFrameT::ID_MENU_FILE_NEW_MODEL, wxT("New M&odel\tCtrl+Shift+N"), wxT(""));
    NewMenu->Append(ParentFrameT::ID_MENU_FILE_NEW_GUI,   wxT("New &GUI\tCtrl+Alt+N"), wxT(""));
    NewMenu->Append(ParentFrameT::ID_MENU_FILE_NEW_FONT,  wxT("New &Font"), wxT(""));

    m_FileMenu=new wxMenu;
    m_FileMenu->AppendSubMenu(NewMenu, wxT("&New"));

    m_FileMenu->Append(ParentFrameT::ID_MENU_FILE_OPEN, wxT("&Open...\tCtrl+O"), wxT("") );
    m_FileMenu->Append(ID_MENU_FILE_CLOSE, wxT("&Close\tCtrl+F4"), wxT("") );
    m_FileMenu->Append(ID_MENU_FILE_SAVE, wxT("&Save\tCtrl+S"), wxT("") );
    m_FileMenu->Append(ID_MENU_FILE_SAVEAS, wxT("Save &As..."), wxT("") );
    m_FileMenu->AppendSeparator();
    m_FileMenu->Append(ParentFrameT::ID_MENU_FILE_CONFIGURE, wxT("&Configure CaWE..."), wxT("") );
    m_FileMenu->Append(ParentFrameT::ID_MENU_FILE_EXIT, wxT("E&xit"), wxT("") );
    m_Parent->m_FileHistory.UseMenu(m_FileMenu);
    m_Parent->m_FileHistory.AddFilesToMenu(m_FileMenu);
    item0->Append( m_FileMenu, wxT("&File") );

    wxMenu* HelpMenu = new wxMenu;
    HelpMenu->Append(ParentFrameT::ID_MENU_HELP_CONTENTS, wxT("&CaWE Help\tF1"), wxT("") );
    HelpMenu->AppendSeparator();
    HelpMenu->Append(ParentFrameT::ID_MENU_HELP_CAFU_WEBSITE, wxT("Cafu &Website"), wxT("") );
    HelpMenu->Append(ParentFrameT::ID_MENU_HELP_CAFU_FORUM, wxT("Cafu &Forum"), wxT("") );
    HelpMenu->AppendSeparator();
    HelpMenu->Append(ParentFrameT::ID_MENU_HELP_ABOUT, wxT("&About..."), wxT("") );
    item0->Append(HelpMenu, wxT("&Help") );

    SetMenuBar(item0);


    // Setup wxAUI.
    m_AUIManager.SetManagedWindow(this);

    // Create model editor panes.
    m_ModelSetup=new ModelSetupT(this, wxSize(230, 500));
    m_SceneSetup=new SceneSetupT(this, wxSize(230, 500), m_ModelDoc->GetGameConfig());
    m_SceneView =new SceneViewT (this);     // Created after m_SceneSetup, so that its ctor can access the camera in m_SceneSetup.

    m_AUIManager.AddPane(m_SceneView, wxAuiPaneInfo().
                         Name("SceneView").Caption("Scene View").
                         CenterPane());

    m_AUIManager.AddPane(m_ModelSetup, wxAuiPaneInfo().
                         Name("ModelSetup").Caption("Model Setup").
                         Left());

    m_AUIManager.AddPane(m_SceneSetup, wxAuiPaneInfo().
                         Name("SceneSetup").Caption("Scene Setup").
                         Right());

    // Create AUI toolbars.
    // Note: Right now those toolbars don't look to well under Windows Vista because of the new windows toolbar style that is used
    // to render the toolbars but not the wxAUI handles. Insert this code to see the problem.
    wxToolBar* ToolbarDocument=new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
    ToolbarDocument->SetToolBitmapSize(wxSize(16,16));
    ToolbarDocument->AddTool(ParentFrameT::ID_MENU_FILE_NEW_MODEL, "New model", wxBitmap("CaWE/res/GuiEditor/page_white.png", wxBITMAP_TYPE_PNG), "New model");
    ToolbarDocument->AddTool(ID_MENU_FILE_SAVE,                    "Save", wxBitmap("CaWE/res/GuiEditor/disk.png", wxBITMAP_TYPE_PNG), "Save model");
    ToolbarDocument->Realize();

    m_AUIManager.AddPane(ToolbarDocument, wxAuiPaneInfo().Name("ToolbarDocument").
                         Caption("Toolbar Document").ToolbarPane().Top().Row(1).
                         LeftDockable(false).RightDockable(false));


    // Set default perspective if not yet set.
    if (AUIDefaultPerspective.IsEmpty()) AUIDefaultPerspective=m_AUIManager.SavePerspective();

    // Load user perspective (calls m_AUIManager.Update() automatically).
    m_AUIManager.LoadPerspective(wxConfigBase::Get()->Read("ModelEditor/AUI_UserLayout", m_AUIManager.SavePerspective()));

    if (!IsMaximized()) Maximize(true);     // Also have wxMAXIMIZE set as frame style.
    Show(true);

    // Initial update of the model editor panes.
    m_SceneView->Refresh(false);
    m_ModelSetup->RefreshPropGrid();
    m_SceneSetup->RefreshPropGrid();
}


ModelEditor::ChildFrameT::~ChildFrameT()
{
    m_Parent->m_FileHistory.RemoveMenu(m_FileMenu);

    // Unregister us from the parents list of children.
    const int Index=m_Parent->m_MdlChildFrames.Find(this);
    m_Parent->m_MdlChildFrames.RemoveAt(Index);

    m_AUIManager.UnInit();

    delete m_ModelDoc;
}


void ModelEditor::ChildFrameT::OnMenuFile(wxCommandEvent& CE)
{
    switch (CE.GetId())
    {
        case ID_MENU_FILE_CLOSE:
        {
            // Close() generates a EVT_CLOSE event which is handled by our OnClose() handler.
            // See wx Window Deletion Overview for more details.
            Close();
            break;
        }

        case ID_MENU_FILE_SAVE:
        {
            // TODO: Save();
            break;
        }

        case ID_MENU_FILE_SAVEAS:
        {
            // TODO: Save(true);
            break;
        }
    }
}


void ModelEditor::ChildFrameT::OnClose(wxCloseEvent& CE)
{
    if (!CE.CanVeto())
    {
        Destroy();
        return;
    }

    // TODO:  if (m_LastSavedAtCommandNr==m_History.GetLastSaveSuggestedCommandID())
    {
        // Our document has not been modified since the last save - close this window.
        Destroy();
        return;
    }

/*
    const int Answer=wxMessageBox("The GUI has been modified since it was last saved.\n"
        "Do you want to save it before closing?\n"
        "Note that when you select 'No', all changes since the last save will be LOST.",
        "Save GUI before closing?", wxYES_NO | wxCANCEL | wxICON_EXCLAMATION);

    switch (Answer)
    {
        case wxYES:
            if (!Save())
            {
                // The document could not be saved - keep the window open.
                CE.Veto();
                return;
            }

            // The GUI was successfully saved - close the window.
            Destroy();
            return;

        case wxNO:
            Destroy();
            return;

        default:    // Answer==wxCANCEL
            CE.Veto();
            return;
    }
*/
}