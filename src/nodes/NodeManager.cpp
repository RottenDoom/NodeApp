#include "NodeManager.h"

void NodeManager::AddImageNode(const char *path)
{
    const int id = GenerateUniqueId();
    auto node = std::make_shared<ImageNode>(id, path);
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
    for (const auto& node : nodes) { // can't use const since I have to know about the nodes.
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
                    TryCreateConnection(node->nodeId, SocketType::Input);
                }
            }
        }

        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
            isDragging = false; // stop dragging if not connected
        }
    }

    // draw established connections
    for (auto& [fromId, neighbours] : connections) {
        auto fromNode = nodeMap[fromId];
        for (int to : neighbours) {
            auto& toNode = nodeMap[to];
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

    // if (!nodeMap.contains(dragStartNode) || !nodeMap.contains(targetId)) {
    //     std::cerr << "Invalid node ID in connection attempt\n";
    //     return;
    // }

    if (ConnectionExists(dragStartNode, targetId)) return;
    if (CreateCycles(dragStartNode, targetId)) {
        // to do (ImGui warn)
        std::cerr << "Connection would create a cycle.\n";
        return;
    }
    connections[dragStartNode].push_back(targetId);
    isDragging = false;
}

void NodeManager::RenderPropertiesPanel()
{
    ImGui::Begin("Properties");
    auto it = nodeMap.find(selectedNodeId);
    if (it != nodeMap.end()) {
        auto node = it->second;
        node->RenderProperties();
    } else {
        ImGui::Text("No node selected!");
    }

    ImGui::End();
}

void NodeManager::SetSelectedNode(int nodeid)
{
    selectedNodeId = nodeid;
}

int NodeManager::GenerateUniqueId()
{
    return nextNodeId++;
}

bool NodeManager::ConnectionExists(int fromId, int toId)
{
    auto it = connections.find(fromId);
    if (it != connections.end()) {
        const auto& neighbors = it->second;
        return std::find(neighbors.begin(), neighbors.end(), toId) != neighbors.end();
    }
    return false;
}

bool NodeManager::CreateCycles(int fromId, int toId)
{
    if (dfs(toId, fromId, std::unordered_set<int>())) {
        return true;  // cycle would be created
    }

    return false;  // safe to add connection
}

// dfs helper function
bool NodeManager::dfs(int current, int target, std::unordered_set<int> visited)
{
    // If we've visited the target node, we found a cycle
    if (current == target) {
        return true;
    }

    visited.insert(current);

    // Check all neighbors to see if there's a cycle
    for (const auto& neighbor : connections[current]) {
        if (visited.find(neighbor) == visited.end()) {
            if (dfs(neighbor, target, visited)) {
                return true;  // Found a cycle
            }
        }
    }

    return false;  // No cycle found
}