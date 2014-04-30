/*
=================================================================================
This file is part of Cafu, the open-source game engine and graphics engine
for multiplayer, cross-platform, real-time 3D action.
Copyright (C) 2002-2013 Carsten Fuchs Software.

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

#ifndef CAFU_CW_GRENADE_HPP_INCLUDED
#define CAFU_CW_GRENADE_HPP_INCLUDED

#include "cw.hpp"


namespace cf
{
    namespace GameSys
    {
        class CarriedWeaponGrenadeT : public CarriedWeaponT
        {
            public:

            CarriedWeaponGrenadeT(ModelManagerT& ModelMan);
            ~CarriedWeaponGrenadeT();

            bool ServerSide_PickedUpByEntity(IntrusivePtrT<cf::GameSys::ComponentHumanPlayerT> HumanPlayer) const;
            void ServerSide_Think(IntrusivePtrT<cf::GameSys::ComponentHumanPlayerT> HumanPlayer, const PlayerCommandT& PlayerCommand, bool ThinkingOnServerSide, bool AnimSequenceWrap) const;

            void ClientSide_HandlePrimaryFireEvent(IntrusivePtrT<const cf::GameSys::ComponentHumanPlayerT> HumanPlayer, const VectorT& LastSeenAmbientColor) const;


            private:

            SoundI* FireSound;
        };
    }
}

#endif