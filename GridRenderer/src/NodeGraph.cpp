#include "NodeGraph.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

void NodeGraph::ApplyUniforms(const ShaderProgram& shaderProgram)
{
    for (const auto& node : nodes)
    {
        if (node.Type == NodeType::Uniform)
        {
            const auto& pin = node.Outputs[0];
            const auto uniformName = GetPinVariableName(node.Outputs[0].ID);
            const auto location = shaderProgram[uniformName];
            node.Outputs[0].Uniform.ApplyUniforms(location);
        }
    }
}

Node* NodeGraph::FindNode(ed::NodeId id)
{
    for (auto& node : nodes)
        if (node.ID == id)
            return &node;

    return nullptr;
}

Link* NodeGraph::FindLink(ed::LinkId id)
{
    for (auto& link : links)
        if (link.ID == id)
            return &link;

    return nullptr;
}

Pin* NodeGraph::FindPin(ed::PinId id)
{
    if (!id)
        return nullptr;

    for (auto& node : nodes)
    {
        for (auto& pin : node.Inputs)
            if (pin.ID == id)
                return &pin;

        for (auto& pin : node.Outputs)
            if (pin.ID == id)
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
        if (link.StartPinID == id || link.EndPinID == id)
            return &link;
    }

    return nullptr;
}

bool NodeGraph::IsPinLinked(ed::PinId id) const
{
    if (!id)
        return false;

    for (auto& link : links)
        if (link.StartPinID == id || link.EndPinID == id)
            return true;

    return false;
}

std::string NodeGraph::GetPinVariableName(ed::PinId id)
{
    const auto pin = FindPin(id);
    const auto link = FindLinkByPin(pin->ID);

    if (link != nullptr)
        return "var_" + std::to_string(link->ID.Get());
    else
        return "MISSED_LINK";
}