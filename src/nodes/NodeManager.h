#pragma once

#include "tinyfiledialogs.h"
#include "includes.h"
#include "ImageNode.h"
#include "Node.h"

class NodeManager {
public:
    NodeManager() = default;

    static NodeManager& GetInstance() {
        static NodeManager instance; // creates the instance
        return instance;
    }

    // prevent copies
    NodeManager(const NodeManager&) = delete;
    NodeManager& operator=(const NodeManager&) = delete;

    enum SocketType {
        Input = 0,
        Output = 1
    };

    int selectedNodeId = 0;

    //void ShowGUI();
    void AddImageNode(const char* path);
    // void ReplaceImageInNode(int nodeId, const char* newPath);
    void RenderNodes();
    // void Clear();

    void StartConnectionDrag(int id, SocketType type, ImVec2 startPos);
    void TryCreateConnection(int targetId, SocketType targetSocketType);
    void RenderPropertiesPanel();
    void SetSelectedNode(int nodeid);

    std::unordered_map<int, std::shared_ptr<Node>> nodeMap; // map of nodes against id.
    std::unordered_map<int, std::vector<int>> connections; // adjacency list that checks for connections


private:
    bool isDragging = false;
    int dragStartNode = 0;
    SocketType dragSocketType;
    ImVec2 dragSocketPos;
    std::vector<std::shared_ptr<Node>> nodes;

    int GenerateUniqueId();
    bool ConnectionExists(int fromId, int toId);
    bool CreateCycles(int fromId, int toId);
    bool dfs(int current, int target, std::unordered_set<int> visited);
    // std::unordered_map<int, size_t> nodeIdToIndex;
    int nextNodeId = 1;
};