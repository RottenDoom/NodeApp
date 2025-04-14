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

    struct Connection {
        int fromNodeId;
        int toNodeId;
        int fromSocketIndex; // usually 0 unless multiple outputs
        int toSocketIndex;   // e.g. 0 or 1 for blend node inputs

        // constructor to get away from wierd error
        Connection(int from = -1, int to = -1, int fromIndex = -1, int toIndex = -1)
        : fromNodeId(from), toNodeId(to), fromSocketIndex(fromIndex), toSocketIndex(toIndex) {}
    };

    int selectedNodeId = 0;

    //void ShowGUI();
    void AddImageNode(const char* path);
    void AddNode(const Node::Type type);
    // void ReplaceImageInNode(int nodeId, const char* newPath);
    void RenderNodes();
    void UpdateNodes();
    // void Clear();

    void StartConnectionDrag(int nodeId, SocketType socketType, const ImVec2& socketPos, int socketIndex);
    void TryCreateConnection(int targetId, SocketType targetSocketType, int targetSocketIndex);
    void RenderPropertiesPanel();
    void SetSelectedNode(int nodeid);

    std::optional<Connection> GetInputConnection(int nodeid, int nodeindex);
    std::shared_ptr<Node> GetNodeById(int fromId);

    std::unordered_map<int, std::shared_ptr<Node>> nodeMap; // map of nodes against id.
    std::vector<Connection> connections; // adjacency list that checks for connections


private:
    bool isDragging = false;
    int dragStartNode = 0;
    int dragSocketIndex = 0;
    SocketType dragSocketType;
    ImVec2 dragSocketPos;
    std::vector<std::shared_ptr<Node>> nodes;

    int GenerateUniqueId();
    bool ConnectionExists(int fromId, int toId, int fromSocketIndex, int toSocketIndex);
    bool CreateCycles(int fromId, int toId);
    bool dfs(int current, int target, std::unordered_set<int> visited);
    // std::unordered_map<int, size_t> nodeIdToIndex;
    int nextNodeId = 1;
};