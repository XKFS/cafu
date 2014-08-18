/*
=================================================================================
This file is part of Cafu, the open-source game engine and graphics engine
for multiplayer, cross-platform, real-time 3D action.
Copyright (C) 2002-2014 Carsten Fuchs Software.

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

#ifndef CAFU_MAPEDITOR_COMMAND_DELETE_COMPONENT_HPP_INCLUDED
#define CAFU_MAPEDITOR_COMMAND_DELETE_COMPONENT_HPP_INCLUDED

#include "../../CommandPattern.hpp"
#include "Templates/Pointer.hpp"


namespace cf { namespace GameSys { class ComponentBaseT; } }
namespace cf { namespace GameSys { class EntityT; } }
class MapDocumentT;


namespace MapEditor
{
    class CommandDeleteComponentT : public CommandT
    {
        public:

        CommandDeleteComponentT(MapDocumentT* MapDocument, IntrusivePtrT<cf::GameSys::EntityT> Entity, unsigned long Index);

        // CommandT implementation.
        bool Do();
        void Undo();
        wxString GetName() const;


        private:

        MapDocumentT*                              m_MapDocument;
        IntrusivePtrT<cf::GameSys::EntityT>        m_Entity;
        IntrusivePtrT<cf::GameSys::ComponentBaseT> m_Component;
        const unsigned long                        m_Index;
    };
}

#endif