#pragma once

#include <string>
#include <vector>
#include <fstream>

#include "third_party/imgui-node-editor/imgui_node_editor.h"
#include "third_party/ajson/ajson.hpp"

#include "UniformParam.h"
#include "ShaderProgram.h"

namespace ed = ax::NodeEditor;

enum PinKind
{
    Output,
    Input
};

enum NodeType
{
    VertexOutput,
    FragmentOutput,
    Uniform,
    Attribute,
    Operation,
    Comment
};

enum AttributeType
{
    Vertex,
    IndexI,
    IndexJ,
    IndexK,
    TecplotParam
};

struct Node;
struct NodeData;

AJSON(ImVec4, x, y, z, w);
AJSON(ImVec2, x, y);
AJSON(ImColor, Value);

struct Pin
{
    ed::PinId id;
    Node* node = nullptr;
    
    std::string name;
    PinKind kind = PinKind::Input;
    UniformParam uniform;

    int32_t serializedId = -1;

    Pin() { }

    Pin(int id, const char* name, ShaderDataType dataType) :
        id(id), node(nullptr), name(name), kind(PinKind::Input)
    {
        uniform.dataType = dataType;
    }
};

AJSON(Pin, name, kind, uniform, serializedId)

struct Node
{
    ed::NodeId id;
    NodeData* data = nullptr;
    
    std::string name;
    NodeType type = NodeType::Uniform;

    std::vector<Pin> inputs;
    std::vector<Pin> outputs;

    AttributeType attributeType = AttributeType::TecplotParam;
    int32_t attributeParamIndex = 1;

    ImColor color;
    ImVec2 size;

    int32_t serializedId = -1;

    Node() { }

    Node(int id, const char* name, ImColor color = ImColor(255, 255, 255)) :
        id(id), name(name), color(color), type(NodeType::Operation), size(0, 0)
    {
    }
};

AJSON(Node, name, type, inputs, outputs, attributeType, attributeParamIndex, color, size, serializedId)

struct Link
{
    ed::LinkId id;

    ed::PinId startPinID;
    ed::PinId endPinID;

    ImColor color;
    
    int32_t serializedId = -1;
    int32_t serializedStartPinId = -1;
    int32_t serializedEndPinId = -1;

    Link() { }

    Link(ed::LinkId id, ed::PinId startPinId, ed::PinId endPinId) :
        id(id), startPinID(startPinId), endPinID(endPinId), color(255, 255, 255)
    {
    }
};

AJSON(Link, color, serializedId, serializedStartPinId, serializedEndPinId);

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

    void Save()
    {
        ajson::save_to_file(*this, "material.json");
    }

    void Load()
    {
        ajson::load_from_file(*this, "material.json");
    }
};

AJSON(NodeGraph, nodes, links);