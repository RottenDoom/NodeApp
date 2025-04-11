#include "tinyfiledialogs.h"
#include "includes.h"
#include "ImageNode.h"

struct NodeSocket {
    ImVec2 position; // Position relative to node
    int id; // unique socket ID (optional)
};

struct Node {
    ImVec2 position;
    std::vector<NodeSocket> inputs;
    std::vector<NodeSocket> outputs;
};

struct NodeConnection {
    int fromNodeId;
    int fromSocketIndex;
    int toNodeId;
    int toSocketIndex;
};

class NodeManager {
public:
    void ShowGUI(); // Called in main ImGui loop
    void AddImageNode(const char* path);
    // void ReplaceImageInNode(int nodeId, const std::string& newPath);
    void RenderNodes();
    // void Clear();

private:

    int GenerateUniqueId();
    std::vector<std::unique_ptr<ImageNode>> nodes;
    std::unordered_map<int, size_t> nodeIdToIndex;
    int nextNodeId = 1;
};