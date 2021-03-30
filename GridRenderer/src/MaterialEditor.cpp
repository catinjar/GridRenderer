#include "MaterialEditor.h"

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <utility>
#include <iostream>

#include "third_party/imgui-node-editor/utilities/builders.h"
#include "third_party/imgui-node-editor/utilities/widgets.h"
#include "third_party/imgui-node-editor/imgui_node_editor.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "third_party/imgui/imgui.h"
#include "third_party/imgui/imgui_internal.h"

#include "NFDHelper.h"
#include "MaterialCompiler.h"

namespace ed = ax::NodeEditor;
namespace util = ax::NodeEditor::Utilities;

using namespace ax;

using ax::Widgets::IconType;

static ed::EditorContext* m_Editor = nullptr;

static const int s_PinIconSize = 24;
static ImTextureID s_HeaderBackground = nullptr;
static ImTextureID s_SaveIcon = nullptr;
static ImTextureID s_RestoreIcon = nullptr;

struct NodeIdLess
{
    bool operator()(const ed::NodeId& lhs, const ed::NodeId& rhs) const
    {
        return lhs.AsPointer() < rhs.AsPointer();
    }
};

static const float s_TouchTime = 1.0f;
static std::map<ed::NodeId, float, NodeIdLess> s_NodeTouchTime;

static inline ImRect ImGui_GetItemRect()
{
    return ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
}

static inline ImRect ImRect_Expanded(const ImRect& rect, float x, float y)
{
    auto result = rect;
    result.Min.x -= x;
    result.Min.y -= y;
    result.Max.x += x;
    result.Max.y += y;
    return result;
}

static void TouchNode(ed::NodeId id)
{
    s_NodeTouchTime[id] = s_TouchTime;
}

static float GetTouchProgress(ed::NodeId id)
{
    auto it = s_NodeTouchTime.find(id);
    if (it != s_NodeTouchTime.end() && it->second > 0.0f)
        return (s_TouchTime - it->second) / s_TouchTime;
    else
        return 0.0f;
}

static void UpdateTouch()
{
    const auto deltaTime = ImGui::GetIO().DeltaTime;
    for (auto& entry : s_NodeTouchTime)
    {
        if (entry.second > 0.0f)
            entry.second -= deltaTime;
    }
}

static bool CanCreateLink(Pin* a, Pin* b)
{
    if (!a || !b || a == b || a->kind == b->kind || a->uniform.dataType != b->uniform.dataType || a->node == b->node)
        return false;

    return true;
}

static void BuildNode(Node* node)
{
    for (auto& input : node->inputs)
    {
        input.node = node;
        input.kind = PinKind::Input;
    }

    for (auto& output : node->outputs)
    {
        output.node = node;
        output.kind = PinKind::Output;
    }
}

void MaterialEditor::BuildNodes()
{
    for (auto& node : graph->nodes)
        BuildNode(&node);
}

static bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f)
{
    using namespace ImGui;
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImGuiID id = window->GetID("##Splitter");
    ImRect bb;
    bb.Min = window->DC.CursorPos + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
    bb.Max = bb.Min + CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);
    return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
}

ImColor GetIconColor(NodeType type)
{
    switch (type)
    {
    case NodeType::Attribute:       return ImColor(220, 48, 48);
    case NodeType::Uniform:         return ImColor(68, 201, 156);
    case NodeType::Operation:       return ImColor(147, 226, 74);
    case NodeType::VertexOutput:    return ImColor(124, 21, 153);
    case NodeType::FragmentOutput:  return ImColor(51, 150, 215);
    default:                        return ImColor(220, 220, 220);
    }
};

void DrawPinIcon(const Pin& pin, bool connected, int alpha)
{
    ImColor color = GetIconColor(pin.node->type);
    color.Value.w = alpha / 255.0f;
    IconType iconType = IconType::Circle;

    ax::Widgets::Icon(ImVec2(s_PinIconSize, s_PinIconSize), iconType, connected, color, ImColor(32, 32, 32, alpha));
};

void ShowStyleEditor(bool* show = nullptr)
{
    if (!ImGui::Begin("Style", show))
    {
        ImGui::End();
        return;
    }

    auto paneWidth = ImGui::GetContentRegionAvailWidth();

    auto& editorStyle = ed::GetStyle();
    ImGui::BeginHorizontal("Style buttons", ImVec2(paneWidth, 0), 1.0f);
    ImGui::TextUnformatted("Values");
    ImGui::Spring();
    if (ImGui::Button("Reset to defaults"))
        editorStyle = ed::Style();
    ImGui::EndHorizontal();
    ImGui::Spacing();

    ImGui::DragFloat4("Node Padding", &editorStyle.NodePadding.x, 0.1f, 0.0f, 40.0f);
    ImGui::DragFloat("Node Rounding", &editorStyle.NodeRounding, 0.1f, 0.0f, 40.0f);
    ImGui::DragFloat("Node Border Width", &editorStyle.NodeBorderWidth, 0.1f, 0.0f, 15.0f);
    ImGui::DragFloat("Hovered Node Border Width", &editorStyle.HoveredNodeBorderWidth, 0.1f, 0.0f, 15.0f);
    ImGui::DragFloat("Selected Node Border Width", &editorStyle.SelectedNodeBorderWidth, 0.1f, 0.0f, 15.0f);
    ImGui::DragFloat("Pin Rounding", &editorStyle.PinRounding, 0.1f, 0.0f, 40.0f);
    ImGui::DragFloat("Pin Border Width", &editorStyle.PinBorderWidth, 0.1f, 0.0f, 15.0f);
    ImGui::DragFloat("Link Strength", &editorStyle.LinkStrength, 1.0f, 0.0f, 500.0f);
    ImGui::DragFloat("Scroll Duration", &editorStyle.ScrollDuration, 0.001f, 0.0f, 2.0f);
    ImGui::DragFloat("Flow Marker Distance", &editorStyle.FlowMarkerDistance, 1.0f, 1.0f, 200.0f);
    ImGui::DragFloat("Flow Speed", &editorStyle.FlowSpeed, 1.0f, 1.0f, 2000.0f);
    ImGui::DragFloat("Flow Duration", &editorStyle.FlowDuration, 0.001f, 0.0f, 5.0f);
    ImGui::DragFloat("Group Rounding", &editorStyle.GroupRounding, 0.1f, 0.0f, 40.0f);
    ImGui::DragFloat("Group Border Width", &editorStyle.GroupBorderWidth, 0.1f, 0.0f, 15.0f);

    ImGui::Separator();

    static ImGuiColorEditFlags edit_mode = ImGuiColorEditFlags_RGB;
    ImGui::BeginHorizontal("Color Mode", ImVec2(paneWidth, 0), 1.0f);
    ImGui::TextUnformatted("Filter Colors");
    ImGui::Spring();
    ImGui::RadioButton("RGB", &edit_mode, ImGuiColorEditFlags_RGB);
    ImGui::Spring(0);
    ImGui::RadioButton("HSV", &edit_mode, ImGuiColorEditFlags_HSV);
    ImGui::Spring(0);
    ImGui::RadioButton("HEX", &edit_mode, ImGuiColorEditFlags_HEX);
    ImGui::EndHorizontal();

    static ImGuiTextFilter filter;
    filter.Draw("", paneWidth);

    ImGui::Spacing();

    ImGui::PushItemWidth(-160);
    for (int i = 0; i < ed::StyleColor_Count; ++i)
    {
        auto name = ed::GetStyleColorName((ed::StyleColor)i);
        if (!filter.PassFilter(name))
            continue;

        ImGui::ColorEdit4(name, &editorStyle.Colors[i].x, edit_mode);
    }
    ImGui::PopItemWidth();

    ImGui::End();
}

void MaterialEditor::Init(Material* material, NodeGraph* graph)
{
    SetMaterial(material, graph);

    m_Editor = ed::CreateEditor();
    ed::SetCurrentEditor(m_Editor);

    Node* node;
    node = SpawnVertexShaderOutputNode();                   ed::SetNodePosition(node->id, ImVec2(-252, 100));
    node = SpawnFragmentShaderOutputNode();                 ed::SetNodePosition(node->id, ImVec2(-252, 220));
    node = SpawnInputNode("Color", ShaderDataType::Color);  ed::SetNodePosition(node->id, ImVec2(-600, 351));

    BuildNodes();

    this->graph->links.push_back(Link(graph->GetNextLinkId(), graph->nodes[2].outputs[0].id, graph->nodes[1].inputs[0].id));

    CompileMaterial(material, graph);

    ed::NavigateToContent();
}

void MaterialEditor::SetMaterial(Material* material, NodeGraph* graph)
{
    this->material = material;
    this->graph = graph;
}

void MaterialEditor::Shutdown()
{
    if (m_Editor)
    {
        ed::DestroyEditor(m_Editor);
        m_Editor = nullptr;
    }
}

void MaterialEditor::Draw()
{
    UpdateTouch();

    auto& io = ImGui::GetIO();

    ed::SetCurrentEditor(m_Editor);

    static ed::NodeId contextNodeId = 0;
    static ed::LinkId contextLinkId = 0;
    static ed::PinId  contextPinId = 0;
    static bool createNewNode = false;
    static Pin* newNodeLinkPin = nullptr;
    static Pin* newLinkPin = nullptr;

    static float leftPaneWidth = 400.0f;
    static float rightPaneWidth = 800.0f;
    Splitter(true, 4.0f, &leftPaneWidth, &rightPaneWidth, 50.0f, 50.0f);

    ShowLeftPane(leftPaneWidth - 4.0f);

    ImGui::SameLine(0.0f, 12.0f);

    ed::Begin("Node editor");
    {
        auto cursorTopLeft = ImGui::GetCursorScreenPos();

        util::BlueprintNodeBuilder builder(s_HeaderBackground, 64, 64);

        for (auto& node : graph->nodes)
        {
            builder.Begin(node.id);
            
            builder.Header(node.color);
            ImGui::Spring(0);
            ImGui::TextUnformatted(node.name.c_str());
            ImGui::Spring(1);
            ImGui::Dummy(ImVec2(0, 28));
            ImGui::Spring(0);
            builder.EndHeader();

            for (auto& input : node.inputs)
            {
                auto alpha = ImGui::GetStyle().Alpha;
                if (newLinkPin && !CanCreateLink(newLinkPin, &input) && &input != newLinkPin)
                    alpha = alpha * (48.0f / 255.0f);

                builder.Input(input.id);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
                DrawPinIcon(input, graph->IsPinLinked(input.id), (int)(alpha * 255));
                
                ImGui::Spring(0);
                
                if (!input.name.empty())
                {
                    ImGui::TextUnformatted(input.name.c_str());
                    ImGui::Spring(0);
                }

                ImGui::PopStyleVar();
                builder.EndInput();
            }

            for (auto& output : node.outputs)
            {
                auto alpha = ImGui::GetStyle().Alpha;
                if (newLinkPin && !CanCreateLink(newLinkPin, &output) && &output != newLinkPin)
                    alpha = alpha * (48.0f / 255.0f);

                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
                builder.Output(output.id);

                if (node.type == NodeType::Uniform)
                {
                    ImGui::BeginVertical(output.id.AsPointer());
                    ImGui::PushItemWidth(100.0f);

                    output.uniform.DrawUI();
                    
                    ImGui::PopItemWidth();
                    ImGui::EndVertical();
                    ImGui::Spring(0);
                }
                
                if (node.type == NodeType::Attribute)
                {
                    if (node.attributeType == AttributeType::TecplotParam)
                    {
                        ImGui::BeginVertical(output.id.AsPointer());
                        ImGui::PushItemWidth(100.0f);

                        ImGui::DragInt("Index", &node.attributeParamIndex, 1, 1, 8);

                        ImGui::PopItemWidth();
                        ImGui::EndVertical();
                        ImGui::Spring(0);
                    }
                }

                if (!output.name.empty())
                {
                    ImGui::Spring(0);
                    ImGui::TextUnformatted(output.name.c_str());
                }

                ImGui::Spring(0);
                
                DrawPinIcon(output, graph->IsPinLinked(output.id), (int)(alpha * 255));
                
                ImGui::PopStyleVar();
                builder.EndOutput();
            }

            builder.End();
        }

        for (auto& node : graph->nodes)
        {
            if (node.type != NodeType::Comment)
                continue;

            const float commentAlpha = 0.75f;

            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, commentAlpha);
            ed::PushStyleColor(ed::StyleColor_NodeBg, ImColor(255, 255, 255, 64));
            ed::PushStyleColor(ed::StyleColor_NodeBorder, ImColor(255, 255, 255, 64));
            ed::BeginNode(node.id);
            ImGui::PushID(node.id.AsPointer());
            ImGui::BeginVertical("content");
            ImGui::BeginHorizontal("horizontal");
            ImGui::Spring(1);
            ImGui::TextUnformatted(node.name.c_str());
            ImGui::Spring(1);
            ImGui::EndHorizontal();
            ed::Group(node.size);
            ImGui::EndVertical();
            ImGui::PopID();
            ed::EndNode();
            ed::PopStyleColor(2);
            ImGui::PopStyleVar();

            if (ed::BeginGroupHint(node.id))
            {
                auto bgAlpha = static_cast<int>(ImGui::GetStyle().Alpha * 255);
                auto min = ed::GetGroupMin();

                ImGui::SetCursorScreenPos(min - ImVec2(-8, ImGui::GetTextLineHeightWithSpacing() + 4));
                ImGui::BeginGroup();
                ImGui::TextUnformatted(node.name.c_str());
                ImGui::EndGroup();

                auto drawList = ed::GetHintBackgroundDrawList();

                auto hintBounds = ImGui_GetItemRect();
                auto hintFrameBounds = ImRect_Expanded(hintBounds, 8, 4);

                drawList->AddRectFilled(
                    hintFrameBounds.GetTL(),
                    hintFrameBounds.GetBR(),
                    IM_COL32(255, 255, 255, 64 * bgAlpha / 255), 4.0f);

                drawList->AddRect(
                    hintFrameBounds.GetTL(),
                    hintFrameBounds.GetBR(),
                    IM_COL32(255, 255, 255, 128 * bgAlpha / 255), 4.0f);
            }
            ed::EndGroupHint();
        }

        for (auto& link : graph->links)
            ed::Link(link.id, link.startPinID, link.endPinID, link.color, 2.0f);

        if (!createNewNode)
        {
            if (ed::BeginCreate(ImColor(255, 255, 255), 2.0f))
            {
                auto showLabel = [](const char* label, ImColor color)
                {
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
                    auto size = ImGui::CalcTextSize(label);

                    auto padding = ImGui::GetStyle().FramePadding;
                    auto spacing = ImGui::GetStyle().ItemSpacing;

                    ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

                    auto rectMin = ImGui::GetCursorScreenPos() - padding;
                    auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

                    auto drawList = ImGui::GetWindowDrawList();
                    drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
                    ImGui::TextUnformatted(label);
                };

                ed::PinId startPinId = 0, endPinId = 0;
                if (ed::QueryNewLink(&startPinId, &endPinId))
                {
                    auto startPin = graph->FindPin(startPinId);
                    auto endPin = graph->FindPin(endPinId);

                    newLinkPin = startPin ? startPin : endPin;

                    if (startPin->kind == PinKind::Input)
                    {
                        std::swap(startPin, endPin);
                        std::swap(startPinId, endPinId);
                    }

                    if (startPin && endPin)
                    {
                        if (endPin == startPin)
                        {
                            ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                        }
                        else if (endPin->kind == startPin->kind)
                        {
                            showLabel("x Incompatible Pin kind", ImColor(45, 32, 32, 180));
                            ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                        }
                        else if (endPin->node == startPin->node)
                        {
                            showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
                            ed::RejectNewItem(ImColor(255, 0, 0), 1.0f);
                        }
                        else if (endPin->uniform.dataType != startPin->uniform.dataType)
                        {
                            showLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
                            ed::RejectNewItem(ImColor(255, 128, 128), 1.0f);
                        }
                        else
                        {
                            showLabel("+ Create Link", ImColor(32, 45, 32, 180));
                            if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
                            {
                                graph->links.emplace_back(Link(graph->GetNextId(), startPinId, endPinId));
                                graph->links.back().color = GetIconColor(startPin->node->type);
                            }
                        }
                    }
                }

                ed::PinId pinId = 0;
                if (ed::QueryNewNode(&pinId))
                {
                    newLinkPin = graph->FindPin(pinId);
                    if (newLinkPin)
                        showLabel("+ Create Node", ImColor(32, 45, 32, 180));

                    if (ed::AcceptNewItem())
                    {
                        createNewNode = true;
                        newNodeLinkPin = graph->FindPin(pinId);
                        newLinkPin = nullptr;
                        ed::Suspend();
                        ImGui::OpenPopup("Create New Node");
                        ed::Resume();

                        BuildNodes();
                    }
                }
            }
            else
            {
                newLinkPin = nullptr;
            }

            ed::EndCreate();

            if (ed::BeginDelete())
            {
                ed::LinkId linkId = 0;
                while (ed::QueryDeletedLink(&linkId))
                {
                    if (ed::AcceptDeletedItem())
                    {
                        auto id = std::find_if(graph->links.begin(), graph->links.end(), [linkId](auto& link) { return link.id == linkId; });
                        if (id != graph->links.end())
                            graph->links.erase(id);
                    }
                }

                ed::NodeId nodeId = 0;
                while (ed::QueryDeletedNode(&nodeId))
                {
                    if (ed::AcceptDeletedItem())
                    {
                        auto id = std::find_if(graph->nodes.begin(), graph->nodes.end(), [nodeId](auto& node) { return node.id == nodeId; });
                        if (id != graph->nodes.end())
                            graph->nodes.erase(id);
                    }
                }
            }
            ed::EndDelete();
        }

        ImGui::SetCursorScreenPos(cursorTopLeft);
    }

    auto openPopupPosition = ImGui::GetMousePos();
    ed::Suspend();
    if (ed::ShowNodeContextMenu(&contextNodeId))
        ImGui::OpenPopup("Node Context Menu");
    else if (ed::ShowPinContextMenu(&contextPinId))
        ImGui::OpenPopup("Pin Context Menu");
    else if (ed::ShowLinkContextMenu(&contextLinkId))
        ImGui::OpenPopup("Link Context Menu");
    else if (ed::ShowBackgroundContextMenu())
    {
        ImGui::OpenPopup("Create New Node");
        newNodeLinkPin = nullptr;
    }
    ed::Resume();

    ed::Suspend();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    if (ImGui::BeginPopup("Node Context Menu"))
    {
        auto node = graph->FindNode(contextNodeId);

        ImGui::TextUnformatted("Node Context Menu");
        ImGui::Separator();
        if (node)
        {
            ImGui::Text("ID: %p", node->id.AsPointer());
            ImGui::Text("Inputs: %d", (int)node->inputs.size());
            ImGui::Text("Outputs: %d", (int)node->outputs.size());
        }
        else
            ImGui::Text("Unknown node: %p", contextNodeId.AsPointer());
        ImGui::Separator();
        if (ImGui::MenuItem("Delete"))
            ed::DeleteNode(contextNodeId);
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("Pin Context Menu"))
    {
        auto pin = graph->FindPin(contextPinId);

        ImGui::TextUnformatted("Pin Context Menu");
        ImGui::Separator();
        if (pin)
        {
            ImGui::Text("ID: %p", pin->id.AsPointer());
            if (pin->node)
                ImGui::Text("Node: %p", pin->node->id.AsPointer());
            else
                ImGui::Text("Node: %s", "<none>");
        }
        else
            ImGui::Text("Unknown pin: %p", contextPinId.AsPointer());

        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("Link Context Menu"))
    {
        auto link = graph->FindLink(contextLinkId);

        ImGui::TextUnformatted("Link Context Menu");
        ImGui::Separator();
        if (link)
        {
            ImGui::Text("ID: %p", link->id.AsPointer());
            ImGui::Text("From: %p", link->startPinID.AsPointer());
            ImGui::Text("To: %p", link->endPinID.AsPointer());
        }
        else
            ImGui::Text("Unknown link: %p", contextLinkId.AsPointer());
        ImGui::Separator();
        if (ImGui::MenuItem("Delete"))
            ed::DeleteLink(contextLinkId);
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("Create New Node"))
    {
        Node* node = nullptr;
        
        if (ImGui::BeginMenu("Input"))
        {
            node = DrawInputNodesMenu();
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Output"))
        {
            node = DrawOuputNodesMenu();
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Operations"))
        {
            node = DrawOperationNodesMenu();
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Attributes"))
        {
            node = DrawAttributeNodesMenu();
            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Comment"))
            node = SpawnComment();

        ImGui::Separator();

        if (node)
        {
            BuildNodes();

            createNewNode = false;

            auto newNodePostion = openPopupPosition;
            ed::SetNodePosition(node->id, newNodePostion);

            if (auto startPin = newNodeLinkPin)
            {
                auto& pins = startPin->kind == PinKind::Input ? node->outputs : node->inputs;

                for (auto& pin : pins)
                {
                    if (CanCreateLink(startPin, &pin))
                    {
                        auto endPin = &pin;
                        if (startPin->kind == PinKind::Input)
                            std::swap(startPin, endPin);

                        graph->links.emplace_back(Link(graph->GetNextId(), startPin->id, endPin->id));
                        graph->links.back().color = GetIconColor(startPin->node->type);

                        break;
                    }
                }
            }

            BuildNodes();
        }

        ImGui::EndPopup();
    }
    else
    {
        createNewNode = false;
    }
   
    ImGui::PopStyleVar();
    ed::Resume();

    ed::End();

    BuildNodes();
}

void MaterialEditor::ShowLeftPane(float paneWidth)
{
    ImGui::BeginChild("Selection", ImVec2(paneWidth, 0));

    paneWidth = ImGui::GetContentRegionAvailWidth();

    ImGui::BeginHorizontal("Style Editor", ImVec2(paneWidth, 0));
    ImGui::Spring(0.0f, 0.0f);

    if (ImGui::Button("Zoom to Content"))
        ed::NavigateToContent();

    ImGui::Spring(0.0f);

    if (ImGui::Button("Show Flow"))
    {
        for (auto& link : graph->links)
            ed::Flow(link.id);
    }

    ImGui::Spring();

    static bool showStyleEditor = false;
    if (ImGui::Button("Edit Style"))
        showStyleEditor = true;

    ImGui::EndHorizontal();

    if (showStyleEditor)
        ShowStyleEditor(&showStyleEditor);

    ImGui::BeginHorizontal("Material Compile");

    if (ImGui::Button("Compile"))
    {
        BuildNodes();
        CompileMaterial(material, graph);
    }

    if (ImGui::Button("Save"))
    {
        std::string filePath;
        if (NFD_SaveFile(filePath))
        {
            BuildNodes();
            graph->Save(filePath);
        }
    }

    if (ImGui::Button("Load"))
    {
        std::string filePath;
        if (NFD_ChooseFile(filePath))
        {
            graph->Load(filePath);
            BuildNodes();

            for (auto& node : graph->nodes)
                ed::SetNodePosition(node.id, node.serializedPosition);
        }
    }

    ImGui::EndHorizontal();

    ImGui::Text(material->GetVertexSourceCode().c_str());
    ImGui::NewLine();
    ImGui::Text(material->GetFragmentSourceCode().c_str());

    ImGui::EndChild();
}

Node* MaterialEditor::DrawInputNodesMenu()
{
    Node* node = nullptr;

    node = DrawInputNodeMenuItem("Bool", ShaderDataType::Bool);
    node = DrawInputNodeMenuItem("Int", ShaderDataType::Int);
    node = DrawInputNodeMenuItem("Uint", ShaderDataType::Uint);
    node = DrawInputNodeMenuItem("Float", ShaderDataType::Float);
    node = DrawInputNodeMenuItem("Double", ShaderDataType::Double);
    node = DrawInputNodeMenuItem("bVec2", ShaderDataType::bVec2);
    node = DrawInputNodeMenuItem("bVec3", ShaderDataType::bVec3);
    node = DrawInputNodeMenuItem("bVec4", ShaderDataType::bVec4);
    node = DrawInputNodeMenuItem("iVec2", ShaderDataType::iVec2);
    node = DrawInputNodeMenuItem("iVec3", ShaderDataType::iVec3);
    node = DrawInputNodeMenuItem("iVec4", ShaderDataType::iVec4);
    node = DrawInputNodeMenuItem("uVec2", ShaderDataType::uVec2);
    node = DrawInputNodeMenuItem("uVec3", ShaderDataType::uVec3);
    node = DrawInputNodeMenuItem("uVec4", ShaderDataType::uVec4);
    node = DrawInputNodeMenuItem("Vec2", ShaderDataType::Vec2);
    node = DrawInputNodeMenuItem("Vec3", ShaderDataType::Vec3);
    node = DrawInputNodeMenuItem("Vec4", ShaderDataType::Vec4);
    node = DrawInputNodeMenuItem("dVec2", ShaderDataType::dVec2);
    node = DrawInputNodeMenuItem("dVec3", ShaderDataType::dVec3);
    node = DrawInputNodeMenuItem("dVec4", ShaderDataType::dVec4);
    node = DrawInputNodeMenuItem("Mat2x2", ShaderDataType::Mat2x2);
    node = DrawInputNodeMenuItem("Mat2x3", ShaderDataType::Mat2x3);
    node = DrawInputNodeMenuItem("Mat2x4", ShaderDataType::Mat2x4);
    node = DrawInputNodeMenuItem("Mat3x2", ShaderDataType::Mat3x2);
    node = DrawInputNodeMenuItem("Mat3x3", ShaderDataType::Mat3x3);
    node = DrawInputNodeMenuItem("Mat3x4", ShaderDataType::Mat3x4);
    node = DrawInputNodeMenuItem("Mat4x2", ShaderDataType::Mat4x2);
    node = DrawInputNodeMenuItem("Mat4x3", ShaderDataType::Mat4x3);
    node = DrawInputNodeMenuItem("Mat4x4", ShaderDataType::Mat4x4);
    node = DrawInputNodeMenuItem("Mat2", ShaderDataType::Mat2);
    node = DrawInputNodeMenuItem("Mat3", ShaderDataType::Mat3);
    node = DrawInputNodeMenuItem("Mat4", ShaderDataType::Mat4);
    node = DrawInputNodeMenuItem("Color", ShaderDataType::Color);

    return node;
}

Node* MaterialEditor::DrawInputNodeMenuItem(const char* name, ShaderDataType type)
{
    if (ImGui::MenuItem(name))
        return SpawnInputNode(name, type);

    return nullptr;
}

Node* MaterialEditor::DrawOuputNodesMenu()
{
    if (ImGui::MenuItem("Vertex Shader Output"))
        return SpawnVertexShaderOutputNode();

    if (ImGui::MenuItem("Fragment Shader Output"))
        return SpawnFragmentShaderOutputNode();

    return nullptr;
}

Node* MaterialEditor::DrawOperationNodesMenu()
{
    Node* node = nullptr;

    auto& nodes = NodeLibrary::GetInstance()->nodes;

    for (auto& nodeData : nodes)
    {
        if (ImGui::MenuItem(nodeData.name.c_str()))
            return SpawnOperationNode(&nodeData);
    }

    return node;
}

Node* MaterialEditor::DrawAttributeNodesMenu()
{
    Node* node = nullptr;

    node = DrawAttributeNodeMenuItem("Param", AttributeType::TecplotParam, ShaderDataType::Float);
    node = DrawAttributeNodeMenuItem("Vertex", AttributeType::Vertex, ShaderDataType::Vec4);
    node = DrawAttributeNodeMenuItem("Index I", AttributeType::IndexI, ShaderDataType::Int);
    node = DrawAttributeNodeMenuItem("Index J", AttributeType::IndexJ, ShaderDataType::Int);
    node = DrawAttributeNodeMenuItem("Index K", AttributeType::IndexK, ShaderDataType::Int);

    return node;
}

Node* MaterialEditor::DrawAttributeNodeMenuItem(const char* name, AttributeType type, ShaderDataType dataType)
{
    if (ImGui::MenuItem(name))
        return SpawnAttributeParamNode(name, type, dataType);

    return nullptr;
}

Node* MaterialEditor::SpawnVertexShaderOutputNode()
{
    graph->nodes.emplace_back(graph->GetNextId(), "Vertex Shader Output", ImColor(128, 255, 128));
    graph->nodes.back().type = NodeType::VertexOutput;
    graph->nodes.back().inputs.emplace_back(graph->GetNextId(), "Vec4", ShaderDataType::Vec4);

    BuildNode(&graph->nodes.back());

    return &graph->nodes.back();
}

Node* MaterialEditor::SpawnFragmentShaderOutputNode()
{
    graph->nodes.emplace_back(graph->GetNextId(), "Fragment Shader Output", ImColor(255, 128, 128));
    graph->nodes.back().type = NodeType::FragmentOutput;
    graph->nodes.back().inputs.emplace_back(graph->GetNextId(), "Vec4", ShaderDataType::Vec4);

    BuildNode(&graph->nodes.back());

    return &graph->nodes.back();
}

Node* MaterialEditor::SpawnInputNode(const char* name, ShaderDataType type)
{
    graph->nodes.emplace_back(graph->GetNextId(), name, ImColor(255, 128, 128));
    graph->nodes.back().type = NodeType::Uniform;
    graph->nodes.back().outputs.emplace_back(graph->GetNextId(), "", type);

    BuildNode(&graph->nodes.back());

    return &graph->nodes.back();
}

Node* MaterialEditor::SpawnOperationNode(NodeData* nodeData)
{
    graph->nodes.emplace_back(graph->GetNextId(), nodeData->name.c_str(), ImColor(255, 128, 128));
    graph->nodes.back().type = NodeType::Operation;
    graph->nodes.back().data = nodeData;

    for (const auto& input : nodeData->inputs)
        graph->nodes.back().inputs.emplace_back(graph->GetNextId(), input.name.c_str(), input.type);

    for (const auto& output : nodeData->outputs)
        graph->nodes.back().outputs.emplace_back(graph->GetNextId(), output.name.c_str(), output.type);

    BuildNode(&graph->nodes.back());

    return &graph->nodes.back();
}

Node* MaterialEditor::SpawnAttributeParamNode(const char* name, AttributeType type, ShaderDataType dataType)
{
    graph->nodes.emplace_back(graph->GetNextId(), name, ImColor(128, 255, 128));
    graph->nodes.back().type = NodeType::Attribute;
    graph->nodes.back().attributeType = type;
    graph->nodes.back().outputs.emplace_back(graph->GetNextId(), "Value", dataType);

    BuildNode(&graph->nodes.back());

    return &graph->nodes.back();
}

Node* MaterialEditor::SpawnComment()
{
    graph->nodes.emplace_back(graph->GetNextId(), "Test Comment");
    graph->nodes.back().type = NodeType::Comment;
    graph->nodes.back().size = ImVec2(300, 200);

    return &graph->nodes.back();
}