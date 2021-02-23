#include "NodeGraph.h"

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

bool NodeGraph::IsPinLinked(ed::PinId id)
{
    if (!id)
        return false;

    for (auto& link : links)
        if (link.StartPinID == id || link.EndPinID == id)
            return true;

    return false;
}