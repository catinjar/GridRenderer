#include "MaterialCompiler.h"

#include <string>

static std::string uniformsSource;
static std::string mainSource;
static std::string vertexShaderSource;
static std::string fragmentShaderSource;

std::string GetPinVariableName(const Pin* pin, NodeGraph* graph)
{
    const auto link = graph->FindLinkByPin(pin->ID);
    if (link != nullptr)
        return "var_" + std::to_string(link->ID.Get());
    else
        return "MISSED_LINK";
}

void ResolveNode(const Node* node, NodeGraph* graph)
{
    for (const auto& input : node->Inputs)
    {
        const auto link = graph->FindLinkByPin(input.ID);

        if (link != nullptr)
        {
            const auto output = graph->FindPin(link->StartPinID);
            ResolveNode(output->Node, graph);
        }
    }

    if (node->Type == NodeType::FragmentOutput)
    {
        mainSource += "\toutColor = " + GetPinVariableName(&node->Inputs[0], graph) + ";\r\n";
    }

    if (node->Type == NodeType::Uniform)
    {
        uniformsSource += "uniform ";

        if (node->Outputs[0].Type == PinType::Color)
            uniformsSource += "vec4 ";

        uniformsSource += GetPinVariableName(&node->Outputs[0], graph);
        uniformsSource += ";\r\n";
    }

    if (node->Type == NodeType::Operation)
    {
        if (node->OpType == OperationType::MultiplyVec4)
        {
            mainSource += "\t" + GetPinVariableName(&node->Outputs[0], graph);
            mainSource += " = " + GetPinVariableName(&node->Inputs[0], graph) + " * " + GetPinVariableName(&node->Inputs[1], graph);
            mainSource += ";\r\n";
        }
        else if (node->OpType == OperationType::ColorToVec4)
        {
            mainSource += "\t" + GetPinVariableName(&node->Outputs[0], graph);
            mainSource += " = " + GetPinVariableName(&node->Inputs[0], graph);
            mainSource += ";\r\n";
        }
    }
}

void CompileMaterial(Material* material, NodeGraph* graph)
{
    uniformsSource = "";
    mainSource = "";
    fragmentShaderSource = "";

    auto it = std::find_if(graph->nodes.begin(), graph->nodes.end(), [](const Node& node) { return node.Type == NodeType::FragmentOutput; });
    if (it != graph->nodes.end())
    {
        const auto& fragmentOutputNode = *it;
        ResolveNode(&fragmentOutputNode, graph);
    }

    fragmentShaderSource += "#version 450 core\r\n";
    fragmentShaderSource += "\r\n";
    fragmentShaderSource += uniformsSource;
    fragmentShaderSource += "\r\n";
    fragmentShaderSource += "out vec4 outColor;\r\n";
    fragmentShaderSource += "\r\n";
    fragmentShaderSource += "void main(void)\r\n";
    fragmentShaderSource += "{\r\n";
    fragmentShaderSource += mainSource;
    fragmentShaderSource += "}\r\n";

    material->SetSourceCode(vertexShaderSource, fragmentShaderSource);
}