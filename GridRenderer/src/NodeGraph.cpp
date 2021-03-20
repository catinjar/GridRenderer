#include "NodeGraph.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

void NodeGraph::ApplyUniforms(const ShaderProgram& shaderProgram)
{
    for (const auto& node : nodes)
    {
        if (node.type == NodeType::Uniform)
        {
            const auto& pin = node.outputs[0];
            const auto uniformName = GetPinVariableName(node.outputs[0].id);
            const auto location = shaderProgram[uniformName];
            node.outputs[0].uniform.ApplyUniforms(location);
        }
    }
}

Node* NodeGraph::FindNode(ed::NodeId id)
{
    for (auto& node : nodes)
        if (node.id == id)
            return &node;

    return nullptr;
}

Link* NodeGraph::FindLink(ed::LinkId id)
{
    for (auto& link : links)
        if (link.id == id)
            return &link;

    return nullptr;
}

Pin* NodeGraph::FindPin(ed::PinId id)
{
    if (!id)
        return nullptr;

    for (auto& node : nodes)
    {
        for (auto& pin : node.inputs)
            if (pin.id == id)
                return &pin;

        for (auto& pin : node.outputs)
            if (pin.id == id)
                return &pin;
    }

    return nullptr;
}

Link* NodeGraph::FindLinkByPin(ed::PinId id)
{
    if (!id)
        return nullptr;

    for (auto& link : links)
    {
        if (link.startPinID == id || link.endPinID == id)
            return &link;
    }

    return nullptr;
}

bool NodeGraph::IsPinLinked(ed::PinId id) const
{
    if (!id)
        return false;

    for (auto& link : links)
        if (link.startPinID == id || link.endPinID == id)
            return true;

    return false;
}

std::string NodeGraph::GetPinVariableName(ed::PinId id)
{
    const auto pin = FindPin(id);
    const auto link = FindLinkByPin(pin->id);

    if (link != nullptr)
        return "var_" + std::to_string(link->id.Get());
    else
        return "MISSED_LINK";
}