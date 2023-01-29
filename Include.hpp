#pragma once
#include <math.h>
#include <string>
#include <vector>

#define IMMENU_TEXT_CENTER_VERTICAL 0.375f // You might need to change this later, but the font that is used by default has weird padding on top...

#include "Fonts/Primary.hpp"
#include "Fonts/Header.hpp"
#include "Fonts/Icons.hpp" // <, =, >

#include "TextMultiColor.hpp"
#include "Items.hpp"

class C_ImMMenu
{
public:
	// Members
	ImVec2 m_Pos;
	float m_FrameWidth;

	double m_BeginTime = 0.0;

	float m_DPIScale = 1.f;
	__inline float CalcDPI(float x) { return floorf(m_DPIScale * x); }

	struct Header_t
	{
		float m_Height;
		C_ImMMenuTextMultiColor Text;

		void* m_Image = nullptr;
		ImVec2 m_ImageSize;
	};
	Header_t Header;
	__inline void SetHeaderText(std::string m_Text) { Header.Text.Initialize(m_Text); }

	struct Title_t
	{
		C_ImMMenuTextMultiColor Text;
	};
	Title_t Title;
	__inline void SetTitleText(std::string m_Text) { Title.Text.Initialize(m_Text); }

	struct Footer_t
	{
		C_ImMMenuTextMultiColor Text;
	};
	Footer_t Footer;
	__inline void SetFooterText(std::string m_Text) { Footer.Text.Initialize(m_Text); }

	struct Item_t
	{
		std::vector<C_ImMMenuItem*> m_List;
		__inline int GetCount() { return static_cast<int>(m_List.size()); }
		__inline C_ImMMenuItem* Get(int m_Index) { return m_List[m_Index]; }

		std::vector<int> m_SelectableList;
		__inline int GetSelectableCount() { return static_cast<int>(m_SelectableList.size()); }
		__inline int GetSelectable(int m_Index) { return m_SelectableList[m_Index]; }
		__inline C_ImMMenuItem* GetSelectableItem(int m_Index) { return Get(m_SelectableList[m_Index]); }

		void CleanUp()
		{
			for (int i = 0; GetCount() > i; ++i)
				delete Get(i);

			m_List.clear();
			m_SelectableList.clear();
		}

		int AddNewItem(C_ImMMenuItem* m_Item)
		{
			m_List.emplace_back(m_Item);
			if (m_Item->IsSelectable())
			{
				m_SelectableList.emplace_back(GetCount() - 1);
				return (GetSelectableCount() - 1);
			}

			return -2;
		}

		int m_NumToShow		= 10;
		int m_Index			= 0; // From which item to start render
		int m_Selected		= 0; // Current selected item
		int m_Interacted	= -1; // If there was value change or pressed this value will be temporary set

		void ResetSelection()
		{
			m_Index			= 0;
			m_Selected		= 0;
			m_Interacted	= -1;
		}

		__inline bool IsSelectedValid()
		{
			int m_Count = GetSelectableCount();
			if (0 > m_Selected || m_Selected >= m_Count)
				return false;

			return true;
		}

		__inline bool IsAnyInteracted() { return (m_Interacted != -1); }
		__inline C_ImMMenuItem* GetInteracted() 
		{ 
			if (!IsAnyInteracted())
				return nullptr;

			return GetSelectableItem(m_Interacted);
		}

		void UpdateSideInteraction(int m_Value)
		{
			if (!IsSelectedValid())
				return;

			C_ImMMenuItem* m_Item = GetSelectableItem(m_Selected);
			if (m_Item)
				m_Item->SideInteraction(m_Value);
		}

		void UpdateInteraction()
		{
			if (!IsSelectedValid())
				return;

			C_ImMMenuItem* m_Item = GetInteracted();
			if (m_Item)
				m_Item->Interaction();
		}

		void AddSeparator(std::string m_Name)
		{
			C_ImMMenuItemSeparator* m_Item = new C_ImMMenuItemSeparator(m_Name);
			AddNewItem(m_Item);
		}

		bool AddText(std::string m_Name)
		{
			C_ImMMenuItem* m_Item = new C_ImMMenuItem(ImMMenuItemType_Text, m_Name);
			return (m_Interacted == AddNewItem(m_Item));
		}

		void AddTextUnselectable(std::string m_Name)
		{
			C_ImMMenuItem* m_Item = new C_ImMMenuItem(ImMMenuItemType_TextUnselectable, m_Name);
			AddNewItem(m_Item);
		}

		bool AddSection(std::string m_Name)
		{
			C_ImMMenuItem* m_Item = new C_ImMMenuItem(ImMMenuItemType_Section, m_Name);
			return (m_Interacted == AddNewItem(m_Item));
		}

		bool AddCheckbox(std::string m_Name, bool* m_Value)
		{
			C_ImMMenuItemCheckbox* m_Item = new C_ImMMenuItemCheckbox(m_Name, m_Value);
			return (m_Interacted == AddNewItem(m_Item));
		}

		bool AddCombo(std::string m_Name, int* m_Value, std::vector<std::string>& m_Items)
		{
			C_ImMMenuItemCombo* m_Item = new C_ImMMenuItemCombo(m_Name, m_Value, m_Items);
			return (m_Interacted == AddNewItem(m_Item));
		}

		void AddComboCheckbox(std::string m_Name, int* m_Value, std::vector<bool>* m_Values, std::vector<std::string>& m_Items)
		{
			C_ImMMenuItemComboCheckbox* m_Item = new C_ImMMenuItemComboCheckbox(m_Name, m_Value, m_Values, m_Items);
			AddNewItem(m_Item);
		}

		void AddInteger(std::string m_Name, int* m_Value, int m_Min, int m_Max, int m_Power = 1)
		{
			C_ImMMenuItemInteger* m_Item = new C_ImMMenuItemInteger(m_Name, m_Value, m_Min, m_Max, m_Power);
			AddNewItem(m_Item);
		}

		void AddFloat(std::string m_Name, float* m_Value, float m_Min, float m_Max, float m_Power = 0.1f, const char* m_PreviewFormat = "%.3f")
		{
			C_ImMMenuItemFloat* m_Item = new C_ImMMenuItemFloat(m_Name, m_Value, m_Min, m_Max, m_Power, m_PreviewFormat);
			AddNewItem(m_Item);
		}

		void SetDescription(std::string m_Description)
		{
			C_ImMMenuItem* m_Item = Get(GetCount() - 1);
			m_Item->Description.Initialize(m_Description);
		}

		std::string GetSelectOfCountString()
		{
			int m_Count = GetSelectableCount();
			return (std::to_string(std::min(m_Count, m_Selected + 1)) + " of " + std::to_string(m_Count));
		}

		void Update(int m_SelectionChange)
		{
			int m_ItemCount = GetCount();
			int m_ItemSelectableCount = GetSelectableCount();

			int m_MaxIndex = (m_ItemCount - m_NumToShow);

			if (m_ItemSelectableCount > 0)
			{
				m_Selected += m_SelectionChange;
				if (0 > m_Selected)
				{
					m_Selected = (m_ItemSelectableCount - 1);
					m_Index = std::max(0, std::min(GetSelectable(m_Selected) - m_NumToShow + 1, std::max(m_MaxIndex, 0)));
				}
				else if (m_Selected >= m_ItemSelectableCount)
				{
					m_Selected = 0;
					m_Index = 0;
				}
				else
				{
					while (1)
					{
						int m_SelectedInList = GetSelectable(m_Selected);
						if (m_Index > m_SelectedInList)
							m_Index = std::max(m_Index - 1, 0);
						else if (m_SelectedInList >= (m_Index + m_NumToShow))
							m_Index = std::min(m_Index + 1, std::max(m_MaxIndex, 0));
						else
							break;
					}
				}
			}
			else
			{
				m_Index = 0;
				m_Selected = -1; // We got no item to select, disable selection...
			}
		}
	};
	Item_t Item;

	struct Icons_t
	{
		ImFont* Font = nullptr;

		char LeftArrow[2]	= { '\0' };
		char RightArrow[2]	= { '\0' };
		char UpDownArrow[2] = { '\0' };

		bool Initialize()
		{
			if (!Font)
				return false;

			LeftArrow[0]	= '<';
			RightArrow[0]	= '>';
			UpDownArrow[0]	= '=';

			return true;
		}
	};
	Icons_t Icons;

	struct Fonts_t
	{
		ImFont* Primary = nullptr;
		ImFont* Header = nullptr;

		bool AllLoaded() { return (Primary && Header); }

		__inline ImVec2 CalcTextSize(ImFont* m_Font, const char* m_Text) { return m_Font->CalcTextSizeA(m_Font->FontSize, FLT_MAX, 0.f, m_Text); }
	};
	Fonts_t Font;

	struct Colors_t
	{
		ImU32 Header				= IM_COL32(160, 0, 0, 255);
		ImU32 Title					= IM_COL32(0, 0, 0, 255);
		ImU32 Footer				= IM_COL32(0, 0, 0, 220);
		ImU32 Item					= IM_COL32(0, 0, 0, 180);
		ImU32 ItemSelected			= IM_COL32(160, 0, 0, 180);
		ImU32 Separator				= IM_COL32(160, 0, 0, 255);
		ImU32 Primary				= IM_COL32(140, 0, 0, 255);
		ImU32 Description			= IM_COL32(0, 0, 0, 200);

		ImU32 Header_Text			= IM_COL32(255, 255, 255, 255);
		ImU32 Title_Text			= IM_COL32(255, 255, 255, 255);
		ImU32 Footer_Text			= IM_COL32(255, 255, 255, 255);
		ImU32 Primary_Text			= IM_COL32(255, 255, 255, 255);
	};
	Colors_t Color;

	struct Draw_t
	{
		ImVec2 m_ScreenSize;
		ImVec2 m_Pos;
		ImDrawList* List = nullptr;

		__inline ImDrawList* Get() { return List; }

		__inline void AddMultiColorText(ImFont* m_Font, float m_FontSize, ImVec2 m_Pos, C_ImMMenuTextMultiColor* m_Text)
		{
			ImVec2 m_InitialPos = m_Pos;
			for (int t = 0; m_Text->Count > t; ++t)
			{
				Get()->AddText(m_Font, m_FontSize, m_Pos, m_Text->Color[t], &m_Text->String[t][0]);
				m_Pos.x += m_Font->CalcTextSizeA(m_FontSize, FLT_MAX, 0.f, &m_Text->String[t][0]).x;

				if (strchr(&m_Text->String[t][0], '\n'))
				{
					m_Pos.x = m_InitialPos.x;
					m_Pos.y += m_FontSize;
				}
			}
		}
	};
	Draw_t Draw;

	struct Input_t
	{
		char m_SelectUpDown = 0; // -1 = Up | 1 = Up
		__inline void SelectUp() { m_SelectUpDown -= 1; }
		__inline void SelectDown() { m_SelectUpDown += 1; }

		char m_SelectLeftRight = 0; // -1 = Left | 1 = Right
		__inline void SelectLeft() { m_SelectLeftRight -= 1; }
		__inline void SelectRight() { m_SelectLeftRight += 1; }

		bool m_SelectInteraction = false;
		__inline void SelectInteraction() { m_SelectInteraction = true; }

		bool m_EnableNative = true; // You can disable this if you wanna handle it different way...
		double m_NavigationRepeatDelta = 0.25;

		ImGuiKey m_NavigationKeys[4] =
		{
			ImGuiKey_UpArrow, ImGuiKey_DownArrow,
			ImGuiKey_LeftArrow, ImGuiKey_RightArrow
		};
		int m_NavigationEmulatedPressMaxCount = 50;
		int m_NagivationEmulatedPressCount[4] = { 0, 0, 0, 0 }; // This is used to make selection faster each time the key is repeated hold and emulated press
		double m_NavigationLastPress[4] = { 0.0, 0.0, 0.0, 0.0 };

		ImGuiKey m_InteractionKey = ImGuiKey_Enter;

		void Update(double m_CurrentTime)
		{
			for (int i = 0; 4 > i; ++i)
			{
				if (!ImGui::IsKeyDown(m_NavigationKeys[i]))
				{
					m_NagivationEmulatedPressCount[i] = 1;
					m_NavigationLastPress[i] = 0.0;
					continue;
				}

				if (m_NavigationLastPress[i] > m_CurrentTime)
					continue;

				if (m_NavigationEmulatedPressMaxCount > m_NagivationEmulatedPressCount[i])
					++m_NagivationEmulatedPressCount[i];

				m_NavigationLastPress[i] = (m_CurrentTime + (m_NavigationRepeatDelta / (static_cast<double>(m_NagivationEmulatedPressCount[i]) * 0.5)));

				switch (i)
				{
					case 0: SelectUp(); break;
					case 1: SelectDown(); break;
					case 2: SelectLeft(); break;
					case 3: SelectRight(); break;
				}
			}

			if (ImGui::IsKeyPressed(m_InteractionKey, false))
				SelectInteraction();
		}
	};
	Input_t Input;

	// Call this after ImGui_ImplWin32_Init & ImGui_[BackEnd_Renderer]_Init...
	bool Initialize()
	{
		ImGuiIO* m_IO = &ImGui::GetIO();

		// Icons
		Icons.Font = m_IO->Fonts->AddFontFromMemoryCompressedTTF(ImGuiMMenu::Font::Icons_Data, ImGuiMMenu::Font::Icons_SizeData, CalcDPI(16.f));
		if (!Icons.Initialize())
			return false;

		// Fonts
		Font.Primary	= m_IO->Fonts->AddFontFromMemoryCompressedTTF(ImGuiMMenu::Font::Primary_Data, ImGuiMMenu::Font::Primary_SizeData, CalcDPI(16.f));
		Font.Header		= m_IO->Fonts->AddFontFromMemoryCompressedTTF(ImGuiMMenu::Font::Header_Data, ImGuiMMenu::Font::Header_SizeData, CalcDPI(32.f));

		if (!Font.AllLoaded())
			return false;

		// Sizes
		m_Pos			= ImVec2(0.05f, 0.05f);
		m_FrameWidth	= CalcDPI(420.f);

		Header.m_Height		= CalcDPI(85.f);

		return true;
	}

	bool Begin()
	{
		ImGuiIO* m_IO = &ImGui::GetIO();

		m_BeginTime			= ImGui::GetTime();

		Draw.m_ScreenSize	= m_IO->DisplaySize;
		Draw.m_Pos			= ImVec2(fminf(Draw.m_ScreenSize.x - m_FrameWidth, floorf(m_Pos.x * Draw.m_ScreenSize.x)), floorf(m_Pos.y * Draw.m_ScreenSize.y));
		Draw.List			= ImGui::GetBackgroundDrawList();

		if (Input.m_EnableNative)
			Input.Update(m_BeginTime);

		return (Draw.List);
	}

	void End()
	{
		// Input
		{
			if (Input.m_SelectUpDown != 0)
			{
				Item.Update(static_cast<int>(Input.m_SelectUpDown));
				Input.m_SelectUpDown = 0;
			}

			if (Input.m_SelectLeftRight != 0)
			{
				Item.UpdateSideInteraction(static_cast<int>(Input.m_SelectLeftRight));
				Input.m_SelectLeftRight = 0;
			}

			if (Input.m_SelectInteraction)
			{
				Item.m_Interacted = Item.m_Selected;
				Item.UpdateInteraction();
				Input.m_SelectInteraction = false;
			}
			else
				Item.m_Interacted = -1;
		}

		// Header
		{
			Draw.Get()->AddRectFilled(Draw.m_Pos, Draw.m_Pos + ImVec2(m_FrameWidth, Header.m_Height), Color.Header);

			if (Header.m_Image)
			{
				float m_WidthDiff	= (Header.m_ImageSize.x - m_FrameWidth);
				float m_HeightDiff	= (Header.m_ImageSize.y - Header.m_Height);

				if (m_WidthDiff > 0.f || m_HeightDiff > 0.f) // Need rescale
				{
					if (m_WidthDiff > m_HeightDiff)
					{
						float m_HeightCalc = (Header.m_ImageSize.y / Header.m_ImageSize.x);

						Header.m_ImageSize.x = m_FrameWidth;
						Header.m_ImageSize.y = floorf(m_HeightCalc * m_FrameWidth);
					}
					else
					{
						float m_WidthCalc = (Header.m_ImageSize.x / Header.m_ImageSize.y);

						Header.m_ImageSize.x = floorf(m_WidthCalc * Header.m_Height);
						Header.m_ImageSize.y = Header.m_Height;
					}
				}

				ImVec2 m_ImagePos(Draw.m_Pos + ImVec2(floorf(m_FrameWidth * 0.5f) - floorf(Header.m_ImageSize.x * 0.5f), floorf(Header.m_Height * 0.5f) - floorf(Header.m_ImageSize.y * 0.5f)));
				Draw.Get()->AddImage(Header.m_Image, m_ImagePos, m_ImagePos + Header.m_ImageSize, ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), IM_COL32(0, 0, 0, 100));
			}

			if (Header.Text.Count)
			{
				ImVec2 m_TextSize = Font.CalcTextSize(Font.Header, &Header.Text.GetFullString()[0]);
				ImVec2 m_TextPos(Draw.m_Pos + ImVec2(floorf((m_FrameWidth * 0.5f) - (m_TextSize.x * 0.5f)), floorf((Header.m_Height * 0.5f) - (m_TextSize.y * IMMENU_TEXT_CENTER_VERTICAL))));
				Draw.AddMultiColorText(Font.Header, Font.Header->FontSize, m_TextPos, &Header.Text);
			}

			Draw.m_Pos.y += Header.m_Height;
		}

		// Title
		if (Title.Text.Count)
		{
			float m_TitleHeight = floorf(Font.Primary->FontSize * 2.f);
			Draw.Get()->AddRectFilled(Draw.m_Pos, Draw.m_Pos + ImVec2(m_FrameWidth, m_TitleHeight), Color.Title);

			ImVec2 m_TextSize = Font.CalcTextSize(Font.Primary, &Title.Text.GetFullString()[0]);
			ImVec2 m_TextPos(Draw.m_Pos + ImVec2(floorf((m_FrameWidth * 0.5f) - (m_TextSize.x * 0.5f)), floorf((m_TitleHeight * 0.5f) - (m_TextSize.y * IMMENU_TEXT_CENTER_VERTICAL))));
			Draw.AddMultiColorText(Font.Primary, Font.Primary->FontSize, m_TextPos, &Title.Text);

			Draw.m_Pos.y += m_TitleHeight;
		}

		// Items
		if (Item.GetCount())
		{
			// Draw
			for (int i = Item.m_Index; std::min(Item.GetCount(), Item.m_Index + Item.m_NumToShow) > i; ++i)
			{
				C_ImMMenuItem* m_Item = Item.Get(i);

				ImVec2 m_TextSize = Font.CalcTextSize(Font.Primary, &m_Item->Name.GetFullString()[0]);
				float m_FrameHeight = floorf(m_TextSize.y * 2.f);

				// Pre Left-side Element
				if (m_Item->Type == ImMMenuItemType_Separator)
				{
					ImVec2 m_TextSize = Font.CalcTextSize(Font.Primary, &m_Item->Name.GetFullString()[0]);
					ImVec2 m_TextPos(Draw.m_Pos + ImVec2(floorf((m_FrameWidth * 0.5f) - (m_TextSize.x * 0.5f)), floorf((m_FrameHeight * 0.5f) - (m_TextSize.y * IMMENU_TEXT_CENTER_VERTICAL))));

					Draw.Get()->AddRectFilled(Draw.m_Pos, Draw.m_Pos + ImVec2(m_FrameWidth, m_FrameHeight), Color.Item);

					ImVec2 m_UnderlinePos(Draw.m_Pos + ImVec2(floorf(m_FrameWidth * 0.5f), floorf((m_FrameHeight * 0.5f) + (m_TextSize.y * 0.75f))));
					Draw.Get()->AddLine(m_UnderlinePos - ImVec2(floorf(m_TextSize.x * 0.75f), 0.f), m_UnderlinePos + ImVec2(floorf(m_TextSize.x * 0.75f), 0.f), Color.Separator, 2.f);

					Draw.AddMultiColorText(Font.Primary, Font.Primary->FontSize, m_TextPos, &m_Item->Name);

					Draw.m_Pos.y += m_FrameHeight;
					continue;
				}

				bool m_Selected = (Item.m_Selected >= 0 && i == Item.GetSelectable(Item.m_Selected));

				ImVec2 m_TextPos(Draw.m_Pos + ImVec2(10.f, floorf((m_FrameHeight * 0.5f) - (m_TextSize.y * IMMENU_TEXT_CENTER_VERTICAL))));

				Draw.Get()->AddRectFilled(Draw.m_Pos, Draw.m_Pos + ImVec2(m_FrameWidth, m_FrameHeight), (m_Selected ? Color.ItemSelected : Color.Item));
				Draw.AddMultiColorText(Font.Primary, Font.Primary->FontSize, m_TextPos, &m_Item->Name);

				// Right-side Element
				switch (m_Item->Type)
				{
					case ImMMenuItemType_Section:
					{
						ImVec2 m_IconSize = Font.CalcTextSize(Icons.Font, Icons.RightArrow);
						ImVec2 m_IconPos(Draw.m_Pos + ImVec2(m_FrameWidth - 10.f - m_IconSize.x, floorf((m_FrameHeight * 0.5f) - (m_IconSize.y * 0.5f))));

						Draw.Get()->AddText(Icons.Font, Icons.Font->FontSize, m_IconPos, Color.Primary_Text, Icons.RightArrow);
					}
					break;
					case ImMMenuItemType_Checkbox:
					{
						float m_BoxSize = floorf(m_FrameHeight * 0.25f);
						ImVec2 m_BoxPos(Draw.m_Pos + ImVec2(m_FrameWidth - 10.f - m_BoxSize, floorf(m_FrameHeight * 0.5f)));

						Draw.Get()->AddRect(m_BoxPos - ImVec2(m_BoxSize, m_BoxSize), m_BoxPos + ImVec2(m_BoxSize, m_BoxSize), IM_COL32_WHITE, 0.f, 0, 2.f);

						if (reinterpret_cast<C_ImMMenuItemCheckbox*>(m_Item)->IsChecked())
						{
							m_BoxSize -= 2.f;
							Draw.Get()->AddRectFilled(m_BoxPos - ImVec2(m_BoxSize, m_BoxSize), m_BoxPos + ImVec2(m_BoxSize, m_BoxSize), IM_COL32_WHITE);
						}
					}
					break;
					case ImMMenuItemType_Combo:
					case ImMMenuItemType_ComboCheckbox:
					{
						C_ImMMenuTextMultiColor* m_PreviewText = reinterpret_cast<C_ImMMenuItemCombo*>(m_Item)->GetPreview();

						ImVec2 m_PreviewSize = Font.CalcTextSize(Font.Primary, &m_PreviewText->GetFullString()[0]);
						ImVec2 m_PreviewPos(Draw.m_Pos + ImVec2(m_FrameWidth - 10.f - m_PreviewSize.x, floorf((m_FrameHeight * 0.5f) - (m_PreviewSize.y * IMMENU_TEXT_CENTER_VERTICAL))));

						if (m_Selected)
						{
							float m_CheckboxOffset = 0.f;
							if (m_Item->Type == ImMMenuItemType_ComboCheckbox)
							{
								float m_BoxSize = floorf(m_FrameHeight * 0.25f);
								m_CheckboxOffset = (m_BoxSize * 2.f) + 5.f;

								ImVec2 m_BoxPos(Draw.m_Pos + ImVec2(m_FrameWidth - 10.f - m_BoxSize, floorf(m_FrameHeight * 0.5f)));

								Draw.Get()->AddRect(m_BoxPos - ImVec2(m_BoxSize, m_BoxSize), m_BoxPos + ImVec2(m_BoxSize, m_BoxSize), IM_COL32_WHITE, 0.f, 0, 2.f);

								if (reinterpret_cast<C_ImMMenuItemComboCheckbox*>(m_Item)->IsChecked())
								{
									m_BoxSize -= 2.f;
									Draw.Get()->AddRectFilled(m_BoxPos - ImVec2(m_BoxSize, m_BoxSize), m_BoxPos + ImVec2(m_BoxSize, m_BoxSize), IM_COL32_WHITE);
								}
							}

							ImVec2 m_IconRightSize = Font.CalcTextSize(Icons.Font, Icons.RightArrow);
							ImVec2 m_IconRightPos(Draw.m_Pos + ImVec2(m_FrameWidth - 10.f - m_IconRightSize.x - m_CheckboxOffset, floorf((m_FrameHeight * 0.5f) - (m_IconRightSize.y * 0.5f))));
							Draw.Get()->AddText(Icons.Font, Icons.Font->FontSize, m_IconRightPos, Color.Primary_Text, Icons.RightArrow);
						
							m_PreviewPos.x = m_IconRightPos.x - m_PreviewSize.x - 5.f;

							ImVec2 m_IconLeftSize = Font.CalcTextSize(Icons.Font, Icons.LeftArrow);
							ImVec2 m_IconLeftPos(m_IconRightPos - ImVec2(m_PreviewSize.x + m_IconLeftSize.x + 10.f, 0.f));
							Draw.Get()->AddText(Icons.Font, Icons.Font->FontSize, m_IconLeftPos, Color.Primary_Text, Icons.LeftArrow);
						}

						Draw.AddMultiColorText(Font.Primary, Font.Primary->FontSize, m_PreviewPos, m_PreviewText);
					}
					break;
					case ImMMenuItemType_Integer:
					case ImMMenuItemType_Float:
					{
						bool m_IsFloat = (m_Item->Type == ImMMenuItemType_Float);

						std::string m_PreviewText = (m_IsFloat ? reinterpret_cast<C_ImMMenuItemFloat*>(m_Item)->GetPreview() : reinterpret_cast<C_ImMMenuItemInteger*>(m_Item)->GetPreview());

						ImVec2 m_PreviewSize = Font.CalcTextSize(Font.Primary, &m_PreviewText[0]);
						ImVec2 m_PreviewPos(Draw.m_Pos + ImVec2(m_FrameWidth - 10.f - m_PreviewSize.x, floorf((m_FrameHeight * 0.5f) - (m_PreviewSize.y * IMMENU_TEXT_CENTER_VERTICAL))));

						if (m_Selected)
						{
							ImVec2 m_IconRightSize = Font.CalcTextSize(Icons.Font, Icons.RightArrow);
							ImVec2 m_IconRightPos(Draw.m_Pos + ImVec2(m_FrameWidth - 10.f - m_IconRightSize.x, floorf((m_FrameHeight * 0.5f) - (m_IconRightSize.y * 0.5f))));
							Draw.Get()->AddText(Icons.Font, Icons.Font->FontSize, m_IconRightPos, Color.Primary_Text, Icons.RightArrow);

							m_PreviewPos.x = m_IconRightPos.x - m_PreviewSize.x - 5.f;

							ImVec2 m_IconLeftSize = Font.CalcTextSize(Icons.Font, Icons.LeftArrow);
							ImVec2 m_IconLeftPos(m_IconRightPos - ImVec2(m_PreviewSize.x + m_IconLeftSize.x + 10.f, 0.f));
							Draw.Get()->AddText(Icons.Font, Icons.Font->FontSize, m_IconLeftPos, Color.Primary_Text, Icons.LeftArrow);
						}

						Draw.Get()->AddText(Font.Primary, Font.Primary->FontSize, m_PreviewPos, Color.Primary_Text, &m_PreviewText[0]);
					}
					break;
				}

				Draw.m_Pos.y += m_FrameHeight;
			}
		}

		// Footer
		{
			float m_FooterHeight = floorf(Font.Primary->FontSize * 2.f);
			Draw.Get()->AddRectFilled(Draw.m_Pos, Draw.m_Pos + ImVec2(m_FrameWidth, m_FooterHeight), Color.Footer);

			// Select Info
			{
				std::string m_Text = Item.GetSelectOfCountString();
				ImVec2 m_TextSize = Font.CalcTextSize(Font.Primary, &m_Text[0]);
				ImVec2 m_TextPos(Draw.m_Pos + ImVec2(10.f, floorf((m_FooterHeight * 0.5f) - (m_TextSize.y * IMMENU_TEXT_CENTER_VERTICAL))));
				Draw.Get()->AddText(Font.Primary, Font.Primary->FontSize, m_TextPos, Color.Title_Text, &m_Text[0]);
			}

			// Icon Center
			{
				ImVec2 m_IconSize = Font.CalcTextSize(Icons.Font, Icons.UpDownArrow);
				ImVec2 m_IconPos(Draw.m_Pos + ImVec2(floorf((m_FrameWidth * 0.5f) - (m_IconSize.x * 0.5f)), floorf((m_FooterHeight * 0.5f) - (m_IconSize.y * 0.5f))));

				Draw.Get()->AddText(Icons.Font, Icons.Font->FontSize, m_IconPos, Color.Primary_Text, Icons.UpDownArrow);
			}

			if (Footer.Text.Count)
			{
				ImVec2 m_TextSize = Font.CalcTextSize(Font.Primary, &Footer.Text.GetFullString()[0]);
				ImVec2 m_TextPos(Draw.m_Pos + ImVec2(m_FrameWidth - 10.f - m_TextSize.x, floorf((m_FooterHeight * 0.5f) - (m_TextSize.y * IMMENU_TEXT_CENTER_VERTICAL))));
				Draw.AddMultiColorText(Font.Primary, Font.Primary->FontSize, m_TextPos, &Footer.Text);
			}

			Draw.m_Pos.y += m_FooterHeight;
		}

		// Item Description
		if (Item.IsSelectedValid())
		{
			C_ImMMenuItem* m_Item = Item.GetSelectableItem(Item.m_Selected);
			if (m_Item && m_Item->Description.Count)
			{
				Draw.m_Pos.y += 5.f;

				ImVec2 m_TextSize			= Font.CalcTextSize(Font.Primary, &m_Item->Description.GetFullString()[0]);
				float m_DescriptionHeight	= floorf(m_TextSize.y * 0.85f) + 10.f;

				Draw.Get()->AddRectFilled(Draw.m_Pos, Draw.m_Pos + ImVec2(m_FrameWidth, m_DescriptionHeight), Color.Description);
				Draw.Get()->AddLine(Draw.m_Pos, Draw.m_Pos + ImVec2(m_FrameWidth, 0.f), Color.Primary, 3.f);

				ImVec2 m_TextPos(Draw.m_Pos + ImVec2(10.f, 8.f));
				Draw.AddMultiColorText(Font.Primary, floorf(Font.Primary->FontSize * 0.85f), m_TextPos, &m_Item->Description);
			}
		}

		Item.CleanUp();
	}
};