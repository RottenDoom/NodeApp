#include "NodeManager.h"
#include "BlurNode.h"

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

void NodeManager::AddNode(const Node::Type type)
{
    const int id = GenerateUniqueId();
    std::shared_ptr<Node> node = nullptr;
    switch(type) {
        case Node::BLUR:
            node = std::make_shared<BlurNode>(id);
            break;
        case Node::CONTRAST_AND_BRIGHTNESS:
            // do nothing for now
            break;
        case Node::COLOR_SPLITTER:
            // do nothing for now
            break;
        default:
            std::cerr << "Unknown node type\n";
            return;
            
    }
    if (!node) {
        std::cerr << "Failed to create Node\n";
        return;
    }

    node->nodeId = id;
    nodeMap[node->nodeId] = node;
    nodes.push_back(node);
}

void NodeManager::RenderNodes()
{
    for (const auto& node : nodes) { // can't use const since I have to know about the nodes.
        node->OnRender();
    }

    // for (const auto& connection : connections) {
    //     int fromId = connection.fromNodeId;
    //     int toId = connection.toNodeId;
    //     int fromSocket = connection.fromSocketIndex;
    //     int toSocket = connection.toSocketIndex;
    
    //     // Do something with the connection
    //     // std::cout << "Connection: " << fromId << ":" << fromSocket
    //     //           << " -> " << toId << ":" << toSocket << "\n";
    // }

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

            const auto& inputSockets = node->GetInputSockets();
            for (int i = 0; i < inputSockets.size(); ++i) {
                ImVec2 inputPos = node->GetInputSocketPos();
                float radius = 25.0f;  // socket size / 2 or something similar

                float dx = endPos.x - inputPos.x;
                float dy = endPos.y - inputPos.y;
                float distSquared = dx * dx + dy * dy;

                if (distSquared < radius * radius) {
                    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                        // Create connection between dragStartNode -> node->nodeId
                        TryCreateConnection(node->nodeId, SocketType::Input, i);
                    }
                }
            }
        }

        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
            isDragging = false; // stop dragging if not connected
        }
    }

    // draw established connections
    for (const auto& conn : connections) {

        auto fromNode = nodeMap[conn.fromNodeId];
        auto toNode = nodeMap[conn.toNodeId];

        ImVec2 startPos = fromNode->GetOutputSocketPos();
        ImVec2 endPos = toNode->GetInputSocketPos();

        ImVec2 cp1 = startPos + ImVec2(50, 0);
        ImVec2 cp2 = endPos + ImVec2(-50, 0);

        ImGui::GetForegroundDrawList()->AddBezierCubic(
            startPos, cp1, cp2, endPos, IM_COL32(100, 255, 200, 255), 3.0f
        );
    }
}

void NodeManager::UpdateNodes()
{
    for (auto& node : nodes) {
        node->OnUpdate();
    }
}

// function that checks if we have started dragging and registers the values of dragging node.
void NodeManager::StartConnectionDrag(int nodeId, SocketType socketType, const ImVec2& socketPos, int socketIndex)
{
    isDragging = true;
    dragStartNode = nodeId;
    dragSocketType = socketType;
    dragSocketPos = socketPos;
    dragSocketIndex = socketIndex;
}

// function that registers connections if they are suitable to connect
void NodeManager::TryCreateConnection(int targetId, SocketType targetSocketType, int targetSocketIndex)
{
    if (!isDragging || dragSocketType != SocketType::Output || targetSocketType != SocketType::Input) return;
    if (targetSocketType == dragSocketType) return;

    if (ConnectionExists(dragStartNode, targetId, dragSocketIndex, targetSocketIndex)) return;
    if (CreateCycles(dragStartNode, targetId)) {
        std::cerr << "Connection would create a cycle.\n";
        return;
    }

    connections.push_back({ dragStartNode, targetId, dragSocketIndex, targetSocketIndex });
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

std::optional<NodeManager::Connection> NodeManager::GetInputConnection(int nodeid, int nodeindex)
{
    for (const auto& conn : connections) {
        if (conn.toNodeId == nodeid && conn.toSocketIndex == nodeindex) {
            return conn;
        }
    }
    return std::nullopt;
}

std::shared_ptr<Node> NodeManager::GetNodeById(int fromId)
{
    auto it = nodeMap.find(fromId);
    if (it != nodeMap.end()) {
        return it->second;
    }
    return nullptr;
}

int NodeManager::GenerateUniqueId()
{
    return nextNodeId++;
}

bool NodeManager::ConnectionExists(int fromId, int toId, int fromSocketIndex, int toSocketIndex)
{
    for (const auto& conn : connections) {
        if (conn.fromNodeId == fromId && conn.toNodeId == toId &&
            conn.fromSocketIndex == fromSocketIndex && conn.toSocketIndex == toSocketIndex) {
            return true;
        }
    }
    return false;
}

bool NodeManager::CreateCycles(int fromId, int toId)
{
    return dfs(toId, fromId, std::unordered_set<int>());
}

// dfs helper function
bool NodeManager::dfs(int current, int target, std::unordered_set<int> visited)
{
    // If we've visited the target node, we found a cycle
    if (current == target) return true;
    visited.insert(current);

    for (const auto& conn : connections) {
        if (conn.fromNodeId == current && visited.find(conn.toNodeId) == visited.end()) {
            if (dfs(conn.toNodeId, target, visited)) {
                return true;
            }
        }
    }

    return false;
}