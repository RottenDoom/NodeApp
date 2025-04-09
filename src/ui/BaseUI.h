#pragma once

class BaseUI {
public:
    virtual ~BaseUI() = default;
    virtual void OnRender() = 0;
    virtual void OnUpdate() = 0;
};