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

/*********************************/
/*** Carried Weapon - CrossBow ***/
/*********************************/

#include "cw_CrossBow.hpp"
#include "HumanPlayer.hpp"
#include "Constants_AmmoSlots.hpp"
#include "Constants_WeaponSlots.hpp"
#include "PhysicsWorld.hpp"
#include "Libs/LookupTables.hpp"
#include "../../GameWorld.hpp"
#include "Models/Model_proxy.hpp"


ModelProxyT& CarriedWeaponCrossBowT::GetViewWeaponModel  () const { static ModelProxyT M("Games/DeathMatch/Models/Weapons/DartGun_v.mdl"); return M; }
ModelProxyT& CarriedWeaponCrossBowT::GetPlayerWeaponModel() const { static ModelProxyT M("Games/DeathMatch/Models/Weapons/DartGun_p.mdl"); return M; }


bool CarriedWeaponCrossBowT::ServerSide_PickedUpByEntity(BaseEntityT* Entity) const
{
    // Consider if the entity already has this weapon.
    if (Entity->State.HaveWeapons & (1 << WEAPON_SLOT_CROSSBOW))
    {
        // If it also has the max. amount of ammo of this type, ignore the touch.
        if (Entity->State.HaveAmmo[AMMO_SLOT_ARROWS]==30) return false;

        // Otherwise pick the weapon up and let it have the ammo.
        Entity->State.HaveAmmo[AMMO_SLOT_ARROWS]+=10;
    }
    else
    {
        // This weapon is picked up for the first time.
        Entity->State.HaveWeapons|=1 << WEAPON_SLOT_CROSSBOW;
        Entity->State.ActiveWeaponSlot   =WEAPON_SLOT_CROSSBOW;
        Entity->State.ActiveWeaponSequNr =5;    // Draw
        Entity->State.ActiveWeaponFrameNr=0.0;

        Entity->State.HaveAmmoInWeapons[WEAPON_SLOT_CROSSBOW] =5;
        Entity->State.HaveAmmo         [AMMO_SLOT_ARROWS    ]+=5;
    }

    // Limit the amount of carryable ammo.
    if (Entity->State.HaveAmmo[AMMO_SLOT_ARROWS]>30) Entity->State.HaveAmmo[AMMO_SLOT_ARROWS]=30;

    return true;
}


void CarriedWeaponCrossBowT::ServerSide_Think(EntHumanPlayerT* Player, const PlayerCommandT& PlayerCommand, bool ThinkingOnServerSide, unsigned long /*ServerFrameNr*/, bool AnimSequenceWrap) const
{
    EntityStateT& State=Player->State;

    switch (State.ActiveWeaponSequNr)
    {
        case 0: // Idle1
        case 1: // Idle2
        case 2: // Idle3
            if (PlayerCommand.Keys & PCK_Fire1)
            {
                State.ActiveWeaponSequNr =3;    // Fire
                State.ActiveWeaponFrameNr=0.0;

                if (ThinkingOnServerSide)
                {
                    // If we are on the server-side, find out what or who we hit.
                    const float ViewDirZ=-LookupTables::Angle16ToSin[State.Pitch];
                    const float ViewDirY= LookupTables::Angle16ToCos[State.Pitch];

                    const VectorT ViewDir(ViewDirY*LookupTables::Angle16ToSin[State.Heading], ViewDirY*LookupTables::Angle16ToCos[State.Heading], ViewDirZ);

                    RayResultT RayResult(Player->GetRigidBody());
                    Player->PhysicsWorld->TraceRay(State.Origin/1000.0, scale(ViewDir, 9999999.0/1000.0), RayResult);

                    if (RayResult.hasHit() && RayResult.GetHitEntity()!=NULL)
                        RayResult.GetHitEntity()->TakeDamage(Player, 20, ViewDir);
                }
                break;
            }

            if (AnimSequenceWrap)
            {
                if (State.ActiveWeaponSequNr==2)
                {
                    State.ActiveWeaponSequNr=LookupTables::RandomUShort[PlayerCommand.Nr & 0xFFF] % 3;
                }
                else State.ActiveWeaponSequNr=2;    // Idle3 is the "best-looking" sequence.
            }
            break;

        case 3: // Fire
            if (AnimSequenceWrap)
            {
                State.ActiveWeaponSequNr =4;
                State.ActiveWeaponFrameNr=0.0;
            }
            break;

        case 4: // Reload
            if (AnimSequenceWrap)
            {
                State.ActiveWeaponSequNr =2;
                State.ActiveWeaponFrameNr=0.0;
            }
            break;

        case 5: // Draw (TakeOut)
            if (AnimSequenceWrap)
            {
                State.ActiveWeaponSequNr =0;
                State.ActiveWeaponFrameNr=0.0;
            }
            break;

        case 6: // Holster (PutAway)
            break;
    }
}