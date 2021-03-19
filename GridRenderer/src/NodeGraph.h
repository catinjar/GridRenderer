#pragma once

#include <string>
#include <vector>

#include "third_party/imgui-node-editor/imgui_node_editor.h"

#include "UniformParam.h"
#include "ShaderProgram.h"

namespace ed = ax::NodeEditor;

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

enum class AttributeType
{
    Vertex,
    IndexI,
    IndexJ,
    IndexK,
    TecplotParam
};

struct Node;
struct NodeData;

struct Pin
{
    ed::PinId ID;
    Node* Node;
    std::string Name;
    PinKind Kind;

    UniformParam Uniform;

    Pin(int id, const char* name, ShaderDataType dataType) :
        ID(id), Node(nullptr), Name(name), Kind(PinKind::Input)
    {
        Uniform.dataType = dataType;
    }
};

struct Node
{
    ed::NodeId ID;
    std::string Name;
    NodeType Type;

    std::vector<Pin> Inputs;
    std::vector<Pin> Outputs;

    NodeData* Data = nullptr;

    AttributeType AttributeType = AttributeType::TecplotParam;
    int32_t AttributeParamIndex = 1;

    ImColor Color;
    ImVec2 Size;

    std::string State;
    std::string SavedState;

    Node(int id, const char* name, ImColor color = ImColor(255, 255, 255)) :
        ID(id), Name(name), Color(color), Type(NodeType::Operation), Size(0, 0)
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

struct NodeGraph
{
    void ApplyUniforms(const ShaderProgram& shaderProgram);
    
    Node* FindNode(ed::NodeId id);
    Link* FindLink(ed::LinkId id);
    Pin* FindPin(ed::PinId id);
    Link* FindLinkByPin(ed::PinId id);
    bool IsPinLinked(ed::PinId id) const;

    std::string GetPinVariableName(ed::PinId id);

    std::vector<Node> nodes;
    std::vector<Link> links;
};