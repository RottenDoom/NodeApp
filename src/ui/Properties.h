#pragma once
#include "BaseUI.h"

class Properties : public BaseUI {
public:
    void OnRender() override;
    void OnUpdate() override;

private:
    float slider_power = 1.0f;
};