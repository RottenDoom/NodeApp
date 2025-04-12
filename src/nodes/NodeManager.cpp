#include "NodeManager.h"

NodeManager &NodeManager::GetInstance()
{
    static NodeManager instance;
    return instance;
}

void NodeManager::AddImageNode(const char *path)
{
    int id = GenerateUniqueId();
    auto node = std::make_shared<ImageNode>(path);
    if (!node) {
        std::cerr << "Failed to create ImageNode\n";
        return;
    }
    node->nodeId = id;
    nodeMap[node->nodeId] = node;
    nodes.push_back(node);
    // nodeIdToIndex[id] = nodes.size() - 1;
}

void NodeManager::RenderNodes()
{
    for (auto& node : nodes) { // can't use const since I have to know about the nodes.
        node->OnRender();
    }

    // if we are currently draggin the button
    if (isDragging) {
        ImVec2 endPos = ImGui::GetIO().MousePos;
        ImVec2 cp1 = dragSocketPos + ImVec2(50, 0);
        ImVec2 cp2 = endPos + ImVec2(-50, 0);

        ImGui::GetForegroundDrawList()->AddBezierCubic(
            dragSocketPos, cp1, cp2, endPos, IM_COL32(200, 200, 100, 255), 3.0f
        );


        // check if hovering over a valid input socket
        for (auto& node : nodes) {
            if (node->nodeId == dragStartNode) continue; // if hovering over itself.

            ImVec2 inputPos = node->GetInputSocketPos();
            float radius = 25.0f;  // socket size / 2 or something similar

            float dx = endPos.x - inputPos.x;
            float dy = endPos.y - inputPos.y;
            float distSquared = dx * dx + dy * dy;

            if (distSquared < radius * radius) {
                if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                    // Create connection between dragStartNode -> node->nodeId
                    connections.push_back({ dragStartNode, node->nodeId });
                }
            }
        }

        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
            isDragging = false; // stop dragging if not connected
        }
    }

    // draw established connections
    for (auto& [fromId, toId] : connections) {
        auto fromNode = nodeMap[fromId];
        auto toNode = nodeMap[toId];

        if (!fromNode || !toNode) {
            continue;
        }
        ImVec2 startPos = fromNode->GetOutputSocketPos();
        ImVec2 endPos = toNode->GetInputSocketPos();

        ImVec2 cp1 = startPos + ImVec2(50, 0);
        ImVec2 cp2 = endPos + ImVec2(-50, 0);

        ImGui::GetForegroundDrawList()->AddBezierCubic(
            startPos, cp1, cp2, endPos, IM_COL32(100, 255, 200, 255), 3.0f
        );
    }
}

// function that checks if we have started dragging and registers the values of dragging node.
void NodeManager::StartConnectionDrag(int id, SocketType type, ImVec2 startPos)
{
    isDragging = true;
    dragStartNode = id;
    dragSocketType = type;
    dragSocketPos = startPos;
}

// function that registers connections if they are suitable to connect
void NodeManager::TryCreateConnection(int targetId, SocketType targetSocketType)
{
    if (!isDragging || dragSocketType != SocketType::Output || targetSocketType != SocketType::Input) return;
    if (targetSocketType == dragSocketType) return;

    connections.emplace_back(dragStartNode, targetId);
    std::cout << dragStartNode << "->" << targetId << "\n";
    isDragging = false;
}

int NodeManager::GenerateUniqueId()
{
    return nextNodeId++;
}
