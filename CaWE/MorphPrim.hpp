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

#ifndef _MORPH_PRIMITIVE_HPP_
#define _MORPH_PRIMITIVE_HPP_

#include "Math3D/Vector3.hpp"
#include "Templates/Array.hpp"


class MapBezierPatchT;
class MapBrushT;
class MapElementT;
class Renderer2DT;
class Renderer3DT;
class wxPoint;
class MP_FaceT;


struct MP_PartT
{
    enum TypeT
    {
        TYPE_VERTEX,
        TYPE_EDGE
    };


    MP_PartT() : m_Selected(false)
    {
    }

    virtual TypeT     GetType() const=0;
    virtual Vector3fT GetPos()  const=0;

    bool m_Selected;   ///< Is this part/handle selected by the user?
};


class MP_VertexT : public MP_PartT
{
    public:

    TypeT     GetType() const { return TYPE_VERTEX; }
    Vector3fT GetPos()  const { return pos; }

    Vector3fT pos;
};


class MP_EdgeT : public MP_PartT
{
    public:

    MP_EdgeT();

    TypeT     GetType() const { return TYPE_EDGE; }
    Vector3fT GetPos()  const { return (Vertices[0]->pos + Vertices[1]->pos)*0.5f; }

    MP_VertexT* Vertices[2];    ///< The vertices of this edge (pointing into the MorphPrimT::m_Vertices array).
    MP_FaceT*   Faces[2];       ///< The faces this edge belongs to (pointing into the MorphPrimT::m_Faces array).
};


class MP_FaceT
{
    public:

    ArrayT<MP_EdgeT*> Edges;    ///< The edges of this face (pointing into the MorphPrimT::m_Edges array).
};


/// This is a *helper* class (see [1]) for the "morph" / "edit vertices" tool,
/// representing a map primitive (a brush or a bezier patch) that is currently being morphed by the tool.
///
/// About [1]: This class is considered a "helper" class, because the user code (i.e. the morph tool)
/// (currently) needs knowledge about the implementation of this class whenever it keeps pointers to
/// parts (vertices, edges) of the object that this class represents. Such pointers into the internal
/// structures may become invalidated by certain methods, and thus great care is required.
class MorphPrimT
{
    public:

    /// The constructor.
    /// @param MapElem   The original brush or bezier patch that this MorphPrimT is associated with / attached to.
    /// Note that this MorphPrimT does not become the "owner" of the MapElem pointer, e.g. it does not attempt to delete it in its dtor.
    /// That also means that this MorphPrimT should not live longer than the MapElem object.
    MorphPrimT(MapElementT* MapElem);

    ~MorphPrimT();

    MapElementT* GetElem() const { return m_MapElem; }
    bool         IsModified() const { return m_Modified; }

    /// Updates the associated map element by applying the morphed geometry to it.
    /// @returns if the update was successful.
    bool ApplyMorphToMapElem();

    /// Moves the selected handles by Delta.
    void MoveSelectedHandles(const Vector3fT& Delta);

    void Render(Renderer2DT& Renderer, bool RenderVertexHandles, bool RenderEdgeHandles) const;
    void Render(Renderer3DT& Renderer, bool RenderVertexHandles, bool RenderEdgeHandles) const;


    // Must store MP_VertexT*, not MP_VertexT, or else array growing renders all external pointers obsolete...
    ArrayT<MP_VertexT*> m_Vertices;
    ArrayT<MP_EdgeT*  > m_Edges;
    ArrayT<MP_FaceT*  > m_Faces;


    private:

    MorphPrimT(const MorphPrimT&);          ///< Use of the Copy Constructor    is not allowed.
    void operator = (const MorphPrimT&);    ///< Use of the Assignment Operator is not allowed.

    /// After a change of (or in) the m_Vertices array, this method computes the convex hull over them
    /// and updates (or rather, recomputes) all other member variables (the m_Edges and m_Faces).
    /// This method should only be called if the m_MapElem member is of type MapBrushT,
    /// because it also modifies the m_Vertices array, which is not desired for the MapBezierPatchT type.
    void UpdateBrushFromVertices();
    void UpdatePatch();

    MP_VertexT* FindClosestVertex(const Vector3fT& Point) const;
    MP_EdgeT*   FindEdge(const MP_VertexT* v1, const MP_VertexT* v2) const;

    MP_VertexT* GetConnectionVertex(MP_EdgeT* Edge1, MP_EdgeT* Edge2) const;
    void RenderHandle(Renderer3DT& Renderer, const wxPoint& ClientPos, const float* color) const;


    MapElementT*        m_MapElem;  ///< The "attached" map brush / bezier patch.
    bool                m_Modified; ///< Whether the MorphPrimT contains any modifications to the "attached" map brush/bezier patch.
};

#endif