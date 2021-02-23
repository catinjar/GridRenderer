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

namespace ed = ax::NodeEditor;
namespace util = ax::NodeEditor::Utilities;

using namespace ax;

using ax::Widgets::IconType;

static ed::EditorContext* m_Editor = nullptr;

static const int s_PinIconSize = 24;
static ImTextureID s_HeaderBackground = nullptr;
static ImTextureID s_SaveIcon = nullptr;
static ImTextureID s_RestoreIcon = nullptr;

static std::string s_UniformsCode;
static std::string s_MainCode;
static std::string s_MaterialCode;

struct NodeIdLess
{
    bool operator()(const ed::NodeId& lhs, const ed::NodeId& rhs) const
    {
        return lhs.AsPointer() < rhs.AsPointer();
    }
};

static const float s_TouchTime = 1.0f;
static std::map<ed::NodeId, float, NodeIdLess> s_NodeTouchTime;

static int s_NextId = 1;
static int GetNextId()
{
    return s_NextId++;
}

static ed::LinkId GetNextLinkId()
{
    return ed::LinkId(GetNextId());
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
    if (!a || !b || a == b || a->Kind == b->Kind || a->Type != b->Type || a->Node == b->Node)
        return false;

    return true;
}

static void BuildNode(Node* node)
{
    for (auto& input : node->Inputs)
    {
        input.Node = node;
        input.Kind = PinKind::Input;
    }

    for (auto& output : node->Outputs)
    {
        output.Node = node;
        output.Kind = PinKind::Output;
    }
}

Node* MaterialEditor::SpawnFragmentShaderOutputNode()
{
    graph->nodes.emplace_back(GetNextId(), "Fragment Shader Output", ImColor(255, 128, 128));
    graph->nodes.back().Type = NodeType::FragmentOutput;
    graph->nodes.back().Inputs.emplace_back(GetNextId(), "Vec4", PinType::Vec4);

    BuildNode(&graph->nodes.back());

    return &graph->nodes.back();
}

Node* MaterialEditor::SpawnColorNode()
{
    graph->nodes.emplace_back(GetNextId(), "Color", ImColor(255, 128, 128));
    graph->nodes.back().Type = NodeType::Uniform;
    graph->nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Color);

    BuildNode(&graph->nodes.back());

    return &graph->nodes.back();
}

Node* MaterialEditor::SpawnMultiplyVec4()
{
    graph->nodes.emplace_back(GetNextId(), "Multiply Vec4", ImColor(255, 128, 128));
    graph->nodes.back().Type = NodeType::Operation;
    graph->nodes.back().OpType = OperationType::MultiplyVec4;
    graph->nodes.back().Inputs.emplace_back(GetNextId(), "Vec4", PinType::Vec4);
    graph->nodes.back().Inputs.emplace_back(GetNextId(), "Value", PinType::Float);
    graph->nodes.back().Outputs.emplace_back(GetNextId(), "Vec4", PinType::Vec4);

    BuildNode(&graph->nodes.back());

    return &graph->nodes.back();
}

Node* MaterialEditor::SpawnColorToVec4()
{
    graph->nodes.emplace_back(GetNextId(), "Color to Vec4", ImColor(255, 128, 128));
    graph->nodes.back().Type = NodeType::Operation;
    graph->nodes.back().OpType = OperationType::ColorToVec4;
    graph->nodes.back().Inputs.emplace_back(GetNextId(), "Color", PinType::Color);
    graph->nodes.back().Outputs.emplace_back(GetNextId(), "Vec4", PinType::Vec4);

    BuildNode(&graph->nodes.back());

    return &graph->nodes.back();
}

Node* MaterialEditor::SpawnComment()
{
    graph->nodes.emplace_back(GetNextId(), "Test Comment");
    graph->nodes.back().Type = NodeType::Comment;
    graph->nodes.back().Size = ImVec2(300, 200);

    return &graph->nodes.back();
}

void MaterialEditor::BuildNodes()
{
    for (auto& node : graph->nodes)
        BuildNode(&node);
}

Node* MaterialEditor::GetNodeByInput(const Pin* input)
{
    const auto link = FindLinkByPin(input->ID);

    if (link != nullptr)
    {
        const auto output = FindPin(link->StartPinID);
        return output->Node;
    }

    return nullptr;
}

std::string MaterialEditor::GetPinVariableName(const Pin* pin)
{
    const auto link = FindLinkByPin(pin->ID);
    if (link != nullptr)
        return "var_" + std::to_string(link->ID.Get());
    else
        return "MISSED_LINK";
}

void MaterialEditor::ResolveNode(const Node* node)
{
    for (const auto& input : node->Inputs)
    {
        const auto link = FindLinkByPin(input.ID);

        if (link != nullptr)
        {
            const auto output = FindPin(link->StartPinID);
            ResolveNode(output->Node);
        }
    }

    if (node->Type == NodeType::FragmentOutput)
    {
        s_MainCode += "\toutColor = " + GetPinVariableName(&node->Inputs[0]) + ";\r\n";
    }

    if (node->Type == NodeType::Uniform)
    {
        s_UniformsCode += "uniform ";
        
        if (node->Outputs[0].Type == PinType::Color)
            s_UniformsCode += "vec4 ";
        
        s_UniformsCode += GetPinVariableName(&node->Outputs[0]);
        s_UniformsCode += ";\r\n";
    }

    if (node->Type == NodeType::Operation)
    {
        if (node->OpType == OperationType::MultiplyVec4)
        {
            s_MainCode += "\t" + GetPinVariableName(&node->Outputs[0]);
            s_MainCode += " = " + GetPinVariableName(&node->Inputs[0]) + " * " + GetPinVariableName(&node->Inputs[1]);
            s_MainCode += ";\r\n";
        }
        else if (node->OpType == OperationType::ColorToVec4)
        {
            s_MainCode += "\t" + GetPinVariableName(&node->Outputs[0]);
            s_MainCode += " = " + GetPinVariableName(&node->Inputs[0]);
            s_MainCode += ";\r\n";
        }
    }
}

void MaterialEditor::GenerateMaterialCode()
{
    s_UniformsCode = "";
    s_MainCode = "";
    s_MaterialCode = "";

    auto it = std::find_if(graph->nodes.begin(), graph->nodes.end(), [](const Node& node) { return node.Type == NodeType::FragmentOutput; });
    if (it != graph->nodes.end())
    {
        const auto& fragmentOutputNode = *it;
        ResolveNode(&fragmentOutputNode);
    }

    s_MaterialCode += "#version 450 core\r\n";
    s_MaterialCode += "\r\n";
    s_MaterialCode += s_UniformsCode;
    s_MaterialCode += "\r\n";
    s_MaterialCode += "out vec4 outColor;\r\n";
    s_MaterialCode += "\r\n";
    s_MaterialCode += "void main(void)\r\n";
    s_MaterialCode += "{\r\n";
    s_MaterialCode += s_MainCode;
    s_MaterialCode += "}\r\n";
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

ImColor GetIconColor(PinType type)
{
    switch (type)
    {
        default:
        case PinType::Bool:     return ImColor(220, 48, 48);
        case PinType::Int:      return ImColor(68, 201, 156);
        case PinType::Float:    return ImColor(147, 226, 74);
        case PinType::Color:    return ImColor(220, 48, 48);
    }
};

void DrawPinIcon(const Pin& pin, bool connected, int alpha)
{
    IconType iconType;
    ImColor  color = GetIconColor(pin.Type);
    color.Value.w = alpha / 255.0f;
    switch (pin.Type)
    {
        case PinType::Bool:     iconType = IconType::Circle; break;
        case PinType::Int:      iconType = IconType::Circle; break;
        case PinType::Float:    iconType = IconType::Circle; break;
        case PinType::Color:    iconType = IconType::Circle; break;
        case PinType::Vec4:    iconType = IconType::Circle; break;
        default:
            return;
    }

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

void MaterialEditor::Init(NodeGraph* graph)
{
    SetMaterial(graph);

    ed::Config config;
    config.SettingsFile = "Blueprints.json";

    config.LoadNodeSettings = [&](ed::NodeId nodeId, char* data, void* userPointer) -> size_t
    {
        auto node = FindNode(nodeId);
        if (!node)
            return 0;

        if (data != nullptr)
            memcpy(data, node->State.data(), node->State.size());
        return node->State.size();
    };

    config.SaveNodeSettings = [&](ed::NodeId nodeId, const char* data, size_t size, ed::SaveReasonFlags reason, void* userPointer) -> bool
    {
        auto node = FindNode(nodeId);
        if (!node)
            return false;

        node->State.assign(data, size);

        TouchNode(nodeId);

        return true;
    };

    m_Editor = ed::CreateEditor(&config);
    ed::SetCurrentEditor(m_Editor);

    Node* node;
    node = SpawnFragmentShaderOutputNode(); ed::SetNodePosition(node->ID, ImVec2(-252, 220));
    node = SpawnColorNode();                ed::SetNodePosition(node->ID, ImVec2(-500, 351));

    BuildNodes();

    this->graph->links.push_back(Link(GetNextLinkId(), graph->nodes[1].Outputs[0].ID, graph->nodes[0].Inputs[0].ID));

    ed::NavigateToContent();
    GenerateMaterialCode();
}

void MaterialEditor::SetMaterial(NodeGraph* graph)
{
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
            builder.Begin(node.ID);
            
            builder.Header(node.Color);
            ImGui::Spring(0);
            ImGui::TextUnformatted(node.Name.c_str());
            ImGui::Spring(1);
            ImGui::Dummy(ImVec2(0, 28));
            ImGui::Spring(0);
            builder.EndHeader();

            for (auto& input : node.Inputs)
            {
                auto alpha = ImGui::GetStyle().Alpha;
                if (newLinkPin && !CanCreateLink(newLinkPin, &input) && &input != newLinkPin)
                    alpha = alpha * (48.0f / 255.0f);

                builder.Input(input.ID);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
                DrawPinIcon(input, IsPinLinked(input.ID), (int)(alpha * 255));
                ImGui::Spring(0);
                if (!input.Name.empty())
                {
                    ImGui::TextUnformatted(input.Name.c_str());
                    ImGui::Spring(0);
                }
                if (input.Type == PinType::Bool)
                {
                    ImGui::Button("Hello");
                    ImGui::Spring(0);
                }

                ImGui::PopStyleVar();
                builder.EndInput();
            }

            for (auto& output : node.Outputs)
            {
                auto alpha = ImGui::GetStyle().Alpha;
                if (newLinkPin && !CanCreateLink(newLinkPin, &output) && &output != newLinkPin)
                    alpha = alpha * (48.0f / 255.0f);

                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
                builder.Output(output.ID);

                if (node.Type == NodeType::Uniform && output.Type == PinType::Color)
                {
                    ImGui::BeginVertical(output.ID.AsPointer());
                    ImGui::PushItemWidth(100.0f);
                    ImGui::ColorPicker3("##color", &output.Color.Value.x);
                    ImGui::PopItemWidth();
                    ImGui::EndVertical();
                    ImGui::Spring(0);
                }

                if (!output.Name.empty())
                {
                    ImGui::Spring(0);
                    ImGui::TextUnformatted(output.Name.c_str());
                }

                ImGui::Spring(0);
                DrawPinIcon(output, IsPinLinked(output.ID), (int)(alpha * 255));
                ImGui::PopStyleVar();
                builder.EndOutput();
            }

            builder.End();
        }

        for (auto& node : graph->nodes)
        {
            if (node.Type != NodeType::Comment)
                continue;

            const float commentAlpha = 0.75f;

            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, commentAlpha);
            ed::PushStyleColor(ed::StyleColor_NodeBg, ImColor(255, 255, 255, 64));
            ed::PushStyleColor(ed::StyleColor_NodeBorder, ImColor(255, 255, 255, 64));
            ed::BeginNode(node.ID);
            ImGui::PushID(node.ID.AsPointer());
            ImGui::BeginVertical("content");
            ImGui::BeginHorizontal("horizontal");
            ImGui::Spring(1);
            ImGui::TextUnformatted(node.Name.c_str());
            ImGui::Spring(1);
            ImGui::EndHorizontal();
            ed::Group(node.Size);
            ImGui::EndVertical();
            ImGui::PopID();
            ed::EndNode();
            ed::PopStyleColor(2);
            ImGui::PopStyleVar();

            if (ed::BeginGroupHint(node.ID))
            {
                auto bgAlpha = static_cast<int>(ImGui::GetStyle().Alpha * 255);
                auto min = ed::GetGroupMin();

                ImGui::SetCursorScreenPos(min - ImVec2(-8, ImGui::GetTextLineHeightWithSpacing() + 4));
                ImGui::BeginGroup();
                ImGui::TextUnformatted(node.Name.c_str());
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
            ed::Link(link.ID, link.StartPinID, link.EndPinID, link.Color, 2.0f);

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
                    auto startPin = FindPin(startPinId);
                    auto endPin = FindPin(endPinId);

                    newLinkPin = startPin ? startPin : endPin;

                    if (startPin->Kind == PinKind::Input)
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
                        else if (endPin->Kind == startPin->Kind)
                        {
                            showLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
                            ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                        }
                        else if (endPin->Node == startPin->Node)
                        {
                            showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
                            ed::RejectNewItem(ImColor(255, 0, 0), 1.0f);
                        }
                        else if (endPin->Type != startPin->Type)
                        {
                            showLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
                            ed::RejectNewItem(ImColor(255, 128, 128), 1.0f);
                        }
                        else
                        {
                            showLabel("+ Create Link", ImColor(32, 45, 32, 180));
                            if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
                            {
                                graph->links.emplace_back(Link(GetNextId(), startPinId, endPinId));
                                graph->links.back().Color = GetIconColor(startPin->Type);
                            }
                        }
                    }
                }

                ed::PinId pinId = 0;
                if (ed::QueryNewNode(&pinId))
                {
                    newLinkPin = FindPin(pinId);
                    if (newLinkPin)
                        showLabel("+ Create Node", ImColor(32, 45, 32, 180));

                    if (ed::AcceptNewItem())
                    {
                        createNewNode = true;
                        newNodeLinkPin = FindPin(pinId);
                        newLinkPin = nullptr;
                        ed::Suspend();
                        ImGui::OpenPopup("Create New Node");
                        ed::Resume();
                    }
                }
            }
            else
                newLinkPin = nullptr;

            ed::EndCreate();

            if (ed::BeginDelete())
            {
                ed::LinkId linkId = 0;
                while (ed::QueryDeletedLink(&linkId))
                {
                    if (ed::AcceptDeletedItem())
                    {
                        auto id = std::find_if(graph->links.begin(), graph->links.end(), [linkId](auto& link) { return link.ID == linkId; });
                        if (id != graph->links.end())
                            graph->links.erase(id);
                    }
                }

                ed::NodeId nodeId = 0;
                while (ed::QueryDeletedNode(&nodeId))
                {
                    if (ed::AcceptDeletedItem())
                    {
                        auto id = std::find_if(graph->nodes.begin(), graph->nodes.end(), [nodeId](auto& node) { return node.ID == nodeId; });
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
        auto node = FindNode(contextNodeId);

        ImGui::TextUnformatted("Node Context Menu");
        ImGui::Separator();
        if (node)
        {
            ImGui::Text("ID: %p", node->ID.AsPointer());
            ImGui::Text("Inputs: %d", (int)node->Inputs.size());
            ImGui::Text("Outputs: %d", (int)node->Outputs.size());
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
        auto pin = FindPin(contextPinId);

        ImGui::TextUnformatted("Pin Context Menu");
        ImGui::Separator();
        if (pin)
        {
            ImGui::Text("ID: %p", pin->ID.AsPointer());
            if (pin->Node)
                ImGui::Text("Node: %p", pin->Node->ID.AsPointer());
            else
                ImGui::Text("Node: %s", "<none>");
        }
        else
            ImGui::Text("Unknown pin: %p", contextPinId.AsPointer());

        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("Link Context Menu"))
    {
        auto link = FindLink(contextLinkId);

        ImGui::TextUnformatted("Link Context Menu");
        ImGui::Separator();
        if (link)
        {
            ImGui::Text("ID: %p", link->ID.AsPointer());
            ImGui::Text("From: %p", link->StartPinID.AsPointer());
            ImGui::Text("To: %p", link->EndPinID.AsPointer());
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
        
        if (ImGui::MenuItem("Fragment Shader Output"))
            node = SpawnFragmentShaderOutputNode();
        if (ImGui::MenuItem("Color"))
            node = SpawnColorNode();
        if (ImGui::MenuItem("Multiply Vec4"))
            node = SpawnMultiplyVec4();
        if (ImGui::MenuItem("Color to Vec4"))
            node = SpawnColorToVec4();

        ImGui::Separator();
        
        if (ImGui::MenuItem("Comment"))
            node = SpawnComment();

        ImGui::Separator();

        if (node)
        {
            BuildNodes();

            createNewNode = false;

            auto newNodePostion = openPopupPosition;
            ed::SetNodePosition(node->ID, newNodePostion);

            if (auto startPin = newNodeLinkPin)
            {
                auto& pins = startPin->Kind == PinKind::Input ? node->Outputs : node->Inputs;

                for (auto& pin : pins)
                {
                    if (CanCreateLink(startPin, &pin))
                    {
                        auto endPin = &pin;
                        if (startPin->Kind == PinKind::Input)
                            std::swap(startPin, endPin);

                        graph->links.emplace_back(Link(GetNextId(), startPin->ID, endPin->ID));
                        graph->links.back().Color = GetIconColor(startPin->Type);

                        break;
                    }
                }
            }
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
            ed::Flow(link.ID);
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
        GenerateMaterialCode();
    }

    ImGui::EndHorizontal();

    ImGui::Text(s_MaterialCode.c_str());

    ImGui::EndChild();
}

Node* MaterialEditor::FindNode(ed::NodeId id)
{
    for (auto& node : graph->nodes)
        if (node.ID == id)
            return &node;

    return nullptr;
}

Link* MaterialEditor::FindLink(ed::LinkId id)
{
    for (auto& link : graph->links)
        if (link.ID == id)
            return &link;

    return nullptr;
}

Pin* MaterialEditor::FindPin(ed::PinId id)
{
    if (!id)
        return nullptr;

    for (auto& node : graph->nodes)
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

Link* MaterialEditor::FindLinkByPin(ed::PinId id)
{
    if (!id)
        return nullptr;

    for (auto& link : graph->links)
    {
        if (link.StartPinID == id || link.EndPinID == id)
            return &link;
    }

    return nullptr;
}

bool MaterialEditor::IsPinLinked(ed::PinId id)
{
    if (!id)
        return false;

    for (auto& link : graph->links)
        if (link.StartPinID == id || link.EndPinID == id)
            return true;

    return false;
}