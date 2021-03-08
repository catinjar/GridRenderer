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

    switch (node->Type)
    {
    case NodeType::VertexOutput:
        mainSource += "\tgl_Position = projection * view * model * inPosition;\r\n";
        break;

    case NodeType::FragmentOutput:
        mainSource += "\toutColor = " + GetPinVariableName(&node->Inputs[0], graph) + ";\r\n";
        break;

    case NodeType::Uniform:
        uniformsSource += "uniform ";

        if (node->Outputs[0].Type == PinType::Color)
            uniformsSource += "vec4 ";

        uniformsSource += GetPinVariableName(&node->Outputs[0], graph);
        uniformsSource += ";\r\n";
        break;

    case NodeType::Operation:
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
        break;
    }
}

void CompileVertexShader(Material* material, NodeGraph* graph)
{
    uniformsSource = "";
    mainSource = "";
    vertexShaderSource = "";

    auto it = std::find_if(graph->nodes.begin(), graph->nodes.end(), [](const Node& node) { return node.Type == NodeType::VertexOutput; });
    if (it != graph->nodes.end())
    {
        const auto& vertexOutputNode = *it;
        ResolveNode(&vertexOutputNode, graph);
    }

    vertexShaderSource += "#version 450 core\r\n";
    vertexShaderSource += "\r\n";
    vertexShaderSource += "layout(location = 0) in vec4 inPosition;\r\n";
    vertexShaderSource += "\r\n";
    vertexShaderSource += "uniform mat4 view;\r\n";
    vertexShaderSource += "uniform mat4 projection;\r\n";
    vertexShaderSource += "uniform mat4 model;\r\n";
    vertexShaderSource += uniformsSource;
    vertexShaderSource += "\r\n";
    vertexShaderSource += "void main(void)\r\n";
    vertexShaderSource += "{\r\n";
    vertexShaderSource += mainSource;
    vertexShaderSource += "\tgl_PointSize = 2.0f;\r\n";
    vertexShaderSource += "}\r\n";
}

void CompileFragmentShader(Material* material, NodeGraph* graph)
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
}

void CompileMaterial(Material* material, NodeGraph* graph)
{
    CompileVertexShader(material, graph);
    CompileFragmentShader(material, graph);

    material->SetSourceCode(vertexShaderSource, fragmentShaderSource);
}