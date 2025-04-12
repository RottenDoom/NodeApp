#pragma once

#include "tinyfiledialogs.h"
#include "includes.h"
#include "ImageNode.h"
#include "Node.h"

class NodeManager {
public:
    NodeManager() = default;
    static NodeManager& GetInstance();

    // prevent copies
    NodeManager(const NodeManager&) = delete;
    NodeManager& operator=(const NodeManager&) = delete;

    enum SocketType {
        Input = 0,
        Output = 1
    };

    //void ShowGUI();
    void AddImageNode(const char* path);
    // void ReplaceImageInNode(int nodeId, const char* newPath);
    void RenderNodes();
    // void Clear();

    void StartConnectionDrag(int id, SocketType type, ImVec2 startPos);
    void TryCreateConnection(int targetId, SocketType targetSocketType);

    std::unordered_map<int, std::shared_ptr<Node>> nodeMap;
    std::vector<std::pair<int, int>> connections;


private:
    bool isDragging = false;
    int dragStartNode = 0;
    SocketType dragSocketType;
    ImVec2 dragSocketPos;

    int GenerateUniqueId();
    std::vector<std::shared_ptr<Node>> nodes;
    // std::unordered_map<int, size_t> nodeIdToIndex;
    int nextNodeId = 1;
};