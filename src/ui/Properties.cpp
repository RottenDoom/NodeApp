// PropertiesPanel.cpp
#include "Properties.h"
#include <imgui.h>

void Properties::OnRender() {
    ImGui::Begin("Properties");
    ImGui::Text("Here go the node parameters...");
    // Add sliders, inputs, toggles etc.
    ImGui::SliderFloat("Brightness", &slider_power,  0.0f, 100.0f);
    ImGui::End();
}

void Properties::OnUpdate() {

}