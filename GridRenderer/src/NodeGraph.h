#pragma once


#include <string>
#include <vector>

#include "third_party/imgui-node-editor/imgui_node_editor.h"

namespace ed = ax::NodeEditor;

enum class PinType
{
    Bool,
    Int,
    Float,
    Color,
    Vec4
};

enum class PinKind
{
    Output,
    Input
};

enum class NodeType
{
    VertexOutput,
    FragmentOutput,
    Uniform,
    Attribute,
    Operation,
    Comment
};

enum class OperationType
{
    MultiplyVec4,
    ColorToVec4
};

struct Node;

struct Pin
{
    ed::PinId ID;
    Node* Node;
    std::string Name;
    PinType Type;
    PinKind Kind;

    ImColor Color;

    Pin(int id, const char* name, PinType type) :
        ID(id), Node(nullptr), Name(name), Type(type), Kind(PinKind::Input)
    {
    }
};

struct Node
{
    ed::NodeId ID;
    std::string Name;
    NodeType Type;

    std::vector<Pin> Inputs;
    std::vector<Pin> Outputs;

    ImColor Color;
    ImVec2 Size;

    std::string State;
    std::string SavedState;

    OperationType OpType;

    Node(int id, const char* name, ImColor color = ImColor(255, 255, 255)) :
        ID(id), Name(name), Color(color), Type(NodeType::Operation), Size(0, 0), OpType(OperationType::ColorToVec4)
    {
    }
};

struct Link
{
    ed::LinkId ID;

    ed::PinId StartPinID;
    ed::PinId EndPinID;

    ImColor Color;

    Link(ed::LinkId id, ed::PinId startPinId, ed::PinId endPinId) :
        ID(id), StartPinID(startPinId), EndPinID(endPinId), Color(255, 255, 255)
    {
    }
};