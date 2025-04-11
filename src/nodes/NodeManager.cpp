#include "NodeManager.h"

void NodeManager::AddImageNode(const char* path)
{
    int id = GenerateUniqueId();
    auto node = std::make_unique<ImageNode>(path);
    node->nodeId = id;
    nodes.push_back(std::move(node));
    nodeIdToIndex[id] = nodes.size() - 1;
}

void NodeManager::RenderNodes()
{
    for (auto& node : nodes) { // can't use const since I have to know about the nodes.
        node->OnRender();
    }
}

int NodeManager::GenerateUniqueId()
{
    return nextNodeId++;
}
