#pragma once

class Application {
public:
    void Run();

    static Application& GetInstance() { return *sInstance; }
    void Close();
    ~Application();
private:
    void Init();
    void Render();
    void Shutdown();

    static Application* sInstance;
    bool m_Running = false;
    
};