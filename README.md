# ImGuiMMenu
 Replica of GTA Popup Menu under ImGui
 
### Quick Example
```cpp
// Include ImGui stuff...
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"

// Include menu main header
#include "ImGuiMMenu/Include.hpp"

static C_ImMMenu MMenu; // Define Menu

void MenuRender()
{
    MMenu.SetHeaderText("sneakyevil's Menu");

    if (MMenu.Begin())
    {
        MMenu.SetTitleText("Home");
        MMenu.SetFooterText("v2.0");

        MMenu.Item.AddSection("Section{FF0000}0");
        MMenu.Item.AddSection("Section{00FF00}1");
        MMenu.Item.AddText("Item{0000FF}0");
        MMenu.Item.AddText("Item{FF00FF}1");

        static bool m_Checkbox0 = false;
        MMenu.Item.AddCheckbox("Checkbox{00FFFF}0", &m_Checkbox0);

        static bool m_Checkbox1 = false;
        MMenu.Item.AddCheckbox("Checkbox{FFFF00}1", &m_Checkbox1);

        MMenu.Item.AddSeparator("Separator");
        MMenu.Item.AddTextUnselectable("You can't select me!\nIm also bigger!");

        static std::vector<std::string> m_ComboList = { "Item{AAAA00}0", "Item{AAAA00}1", "Item{AAAA00}2", "Item{AAAA00}3", "Item{AAAA00}4" };
        static int m_Combo0 = 0;
        MMenu.Item.AddCombo("Combo{FF0000}0", &m_Combo0, m_ComboList);

        static int m_ComboCheckbox = 0;
        static std::vector<bool> m_ComboCheckboxes(5);
        MMenu.Item.AddComboCheckbox("Combo{00FF00}1", &m_ComboCheckbox, &m_ComboCheckboxes, m_ComboList);

        static int m_Integer = 0;
        MMenu.Item.AddInteger("Integer{0000FF}0", &m_Integer, 0, 100);

        static float m_Float = 0;
        MMenu.Item.AddFloat("Float{FF00FF}0", &m_Float, 0.f, 10.f);

        MMenu.End();
    }
}

int main()
{
    // Render API & ImGui Init...

    ImGui_ImplWin32_Init(m_Window);
    // Render Init

    MMenu.Initialize();
  
    // Render loop
  
    ImGui::NewFrame();
  
    MenuRender(); // Our callback
  
    ImGui::EndFrame();
  
    // ...
}
```
