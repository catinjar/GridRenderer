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
    ed::PinId id;
    Node* node;
    std::string name;
    PinKind kind;

    UniformParam uniform;

    Pin(int id, const char* name, ShaderDataType dataType) :
        id(id), node(nullptr), name(name), kind(PinKind::Input)
    {
        uniform.dataType = dataType;
    }
};

struct Node
{
    ed::NodeId id;
    std::string name;
    NodeType type;

    std::vector<Pin> inputs;
    std::vector<Pin> outputs;

    NodeData* data = nullptr;

    AttributeType attributeType = AttributeType::TecplotParam;
    int32_t attributeParamIndex = 1;

    ImColor color;
    ImVec2 size;

    std::string state;
    std::string savedState;

    Node(int id, const char* name, ImColor color = ImColor(255, 255, 255)) :
        id(id), name(name), color(color), type(NodeType::Operation), size(0, 0)
    {
    }
};

struct Link
{
    ed::LinkId id;

    ed::PinId startPinID;
    ed::PinId endPinID;

    ImColor color;

    Link(ed::LinkId id, ed::PinId startPinId, ed::PinId endPinId) :
        id(id), startPinID(startPinId), endPinID(endPinId), color(255, 255, 255)
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