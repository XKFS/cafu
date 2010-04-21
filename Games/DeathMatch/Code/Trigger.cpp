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

#include "Trigger.hpp"
#include "EntityCreateParams.hpp"
#include "GameImpl.hpp"
#include "ScriptState.hpp"
#include "TypeSys.hpp"
#include "ClipSys/ClipModel.hpp"
#include "MaterialSystem/Renderer.hpp"
#include "SceneGraph/Node.hpp"

extern "C"
{
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
}


// Implement the type info related code.
const cf::TypeSys::TypeInfoT* EntTriggerT::GetType() const
{
    return &TypeInfo;
 // return &EntTriggerT::TypeInfo;
}

void* EntTriggerT::CreateInstance(const cf::TypeSys::CreateParamsT& Params)
{
    return new EntTriggerT(*static_cast<const EntityCreateParamsT*>(&Params));
}

const luaL_Reg EntTriggerT::MethodsList[]=
{
    { "Activate",   EntTriggerT::Activate    },
    { "Deactivate", EntTriggerT::Deactivate  },
    { "IsActive",   EntTriggerT::GetIsActive },
 // { "__tostring", toString },
    { NULL, NULL }
};

const cf::TypeSys::TypeInfoT EntTriggerT::TypeInfo(GetBaseEntTIM(), "EntTriggerT", "BaseEntityT", CreateInstance, MethodsList);


EntTriggerT::EntTriggerT(const EntityCreateParamsT& Params)
    : BaseEntityT(Params,
                  EntityStateT(VectorT(),   // Origin
                               VectorT(),   // Velocity
                               BoundingBox3T<double>(Vector3dT()),
                               0,           // Heading
                               0,           // Pitch
                               0,           // Bank
                               0,
                               0,
                               0,           // ModelIndex
                               0,           // ModelSequNr
                               0.0,         // ModelFrameNr
                               0,           // Health
                               0,           // Armor
                               0,           // HaveItems
                               0,           // HaveWeapons
                               0,           // ActiveWeaponSlot
                               0,           // ActiveWeaponSequNr
                               0.0)),       // ActiveWeaponFrameNr
      IsActive(true)
{
    ClipModel.Register();
}


void EntTriggerT::OnTrigger(BaseEntityT* Activator)
{
    if (!IsActive) return;

    const cf::GameSys::GameImplT& GameImpl   =cf::GameSys::GameImplT::GetInstance();
    cf::GameSys::ScriptStateT*    ScriptState=GameImpl.GetScriptState();

    assert(GameImpl.IsSvThinking());
    assert(ScriptState!=NULL);

    ScriptState->CallEntityMethod(this, "OnTrigger", "E", Activator);
}


int EntTriggerT::Activate(lua_State* LuaState)
{
    EntTriggerT* Ent=(EntTriggerT*)cf::GameSys::ScriptStateT::GetCheckedObjectParam(LuaState, 1, TypeInfo);

    if (lua_gettop(LuaState)<2)
    {
        Ent->IsActive=true;
        return 0;
    }

    if (lua_isboolean(LuaState, 2))
    {
        Ent->IsActive=lua_toboolean(LuaState, 2)!=0;
        return 0;
    }

    Ent->IsActive=lua_tonumber(LuaState, 2)!=0;
    return 0;
}


int EntTriggerT::Deactivate(lua_State* LuaState)
{
    Activate(LuaState);

    EntTriggerT* Ent=(EntTriggerT*)cf::GameSys::ScriptStateT::GetCheckedObjectParam(LuaState, 1, TypeInfo);

    Ent->IsActive=!Ent->IsActive;
    return 0;
}


int EntTriggerT::GetIsActive(lua_State* LuaState)
{
    EntTriggerT* Ent=(EntTriggerT*)cf::GameSys::ScriptStateT::GetCheckedObjectParam(LuaState, 1, TypeInfo);

    lua_pushboolean(LuaState, Ent->IsActive);
    return 1;
}