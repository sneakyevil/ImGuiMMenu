#pragma once

enum E_ImMMenuItemType : int
{
	ImMMenuItemType_Unknown = -1,
	ImMMenuItemType_Separator = 0,
	ImMMenuItemType_Text,
	ImMMenuItemType_TextUnselectable,
	ImMMenuItemType_Section,
	ImMMenuItemType_Checkbox,
	ImMMenuItemType_Combo,
	ImMMenuItemType_ComboCheckbox,
	ImMMenuItemType_Integer,
	ImMMenuItemType_Float,
	ImMMenuItemType_Keybind,
	ImMMenuItemType_InputText,
};

class C_ImMMenuItem
{
public:
	E_ImMMenuItemType Type;
	std::string Name;
	std::string Description;

	virtual ~C_ImMMenuItem() { }
	virtual void Interaction() { }
	virtual bool SideInteraction(int m_Value) { return false; }

	C_ImMMenuItem() { }
	C_ImMMenuItem(E_ImMMenuItemType m_Type, std::string m_Name)
	{
		Type = m_Type;
		Name = m_Name;
	}

	C_ImMMenuTextMultiColor GetName() { return C_ImMMenuTextMultiColor(Name); }
	C_ImMMenuTextMultiColor GetDescription() { return C_ImMMenuTextMultiColor(Description); }

	bool IsSelectable()
	{
		if (Type == ImMMenuItemType_Unknown || Type == ImMMenuItemType_Separator ||
			Type == ImMMenuItemType_TextUnselectable)
			return false;

		return true;
	}

	bool HasLeftRightOption()
	{
		if (Type == ImMMenuItemType_Combo || Type == ImMMenuItemType_ComboCheckbox ||
			Type == ImMMenuItemType_Integer || Type == ImMMenuItemType_Float)
			return true;

		return false;
	}
};

class C_ImMMenuItemSeparator : public C_ImMMenuItem
{
public:
	C_ImMMenuItemSeparator(std::string m_Name) : C_ImMMenuItem(ImMMenuItemType_Separator, m_Name) { }
};

class C_ImMMenuItemCheckbox : public C_ImMMenuItem
{
public:
	bool* Value;

	virtual void Interaction()
	{
		*Value = !*Value;
	}

	C_ImMMenuItemCheckbox(std::string m_Name, bool* m_Value)
		: C_ImMMenuItem(ImMMenuItemType_Checkbox, m_Name)
	{
		Value = m_Value;
	}

	__inline bool IsChecked() { return *Value; }
};

class C_ImMMenuItemCombo : public C_ImMMenuItem
{
public:
	int* Value;
	std::vector<std::string> Items;

	virtual bool SideInteraction(int m_Value)
	{
		int m_Size = static_cast<int>(Items.size());

		*Value += m_Value;
		if (*Value >= m_Size)
			*Value = 0;
		else if (0 > *Value)
			*Value = m_Size - 1;

		return true;
	}

	C_ImMMenuItemCombo(std::string m_Name, int* m_Value, std::vector<std::string>& m_Items)
		: C_ImMMenuItem(ImMMenuItemType_Combo, m_Name)
	{
		Value	= m_Value;
		*Value	= std::max(0, std::min(*Value, static_cast<int>(m_Items.size())));
		Items	= m_Items;
	}

	C_ImMMenuTextMultiColor GetPreview() { return C_ImMMenuTextMultiColor(Items[*Value]); }
};

class C_ImMMenuItemComboCheckbox : public C_ImMMenuItemCombo
{
public:
	std::vector<bool>* Values;

	virtual void Interaction()
	{
		Values->operator[](*Value) = !Values->operator[](*Value);
	}

	C_ImMMenuItemComboCheckbox(std::string m_Name, int* m_Value, std::vector<bool>* m_Values, std::vector<std::string>& m_Items)
		: C_ImMMenuItemCombo(m_Name, m_Value, m_Items)
	{
		Type = ImMMenuItemType_ComboCheckbox;

		Values = m_Values;
	}

	__inline bool IsChecked() { return Values->operator[](*Value); }
};

class C_ImMMenuItemInteger : public C_ImMMenuItem
{
public:
	int* Value;
	int Min, Max;
	int Power;

	virtual bool SideInteraction(int m_Value)
	{
		*Value += (m_Value * Power);
		if (*Value > Max)
			*Value = Min;
		else if (Min > *Value)
			*Value = Max;

		return true;
	}

	C_ImMMenuItemInteger(std::string m_Name, int* m_Value, int m_Min, int m_Max, int m_Power)
		: C_ImMMenuItem(ImMMenuItemType_Integer, m_Name)
	{
		Value = m_Value;
		*Value = std::max(m_Min, std::min(*Value, m_Max));

		Min = m_Min;
		Max = m_Max;
		Power = m_Power;
	}

	__inline std::string GetPreview() { return std::to_string(*Value); }
};

class C_ImMMenuItemFloat : public C_ImMMenuItem
{
public:
	float* Value;
	float Min, Max;
	float Power;

	const char* PreviewFormat;

	virtual bool SideInteraction(int m_Value)
	{
		*Value += (static_cast<float>(m_Value) * Power);
		if (*Value > Max)
			*Value = Min;
		else if (Min > *Value)
			*Value = Max;

		return true;
	}

	C_ImMMenuItemFloat(std::string m_Name, float* m_Value, float m_Min, float m_Max, float m_Power, const char* m_PreviewFormat)
		: C_ImMMenuItem(ImMMenuItemType_Float, m_Name)
	{
		Value = m_Value;
		*Value = std::max(m_Min, std::min(*Value, m_Max));

		Min = m_Min;
		Max = m_Max;
		Power = m_Power;

		PreviewFormat = m_PreviewFormat;
	}

	__inline std::string GetPreview()
	{
		std::string m_Preview(32, '\0');
		sprintf_s(&m_Preview[0], m_Preview.size(), PreviewFormat, *Value);

		return m_Preview;
	}
};

class C_ImMMenuItemKeybind : public C_ImMMenuItem
{
public:
	ImGuiKey* Value;

	C_ImMMenuItemKeybind(std::string m_Name, ImGuiKey* m_Value)
		: C_ImMMenuItem(ImMMenuItemType_Keybind, m_Name)
	{
		Value = m_Value;
	}

	__inline ImGuiKey GetKey() { return *Value; }
};

class C_ImMMenuItemInputText : public C_ImMMenuItem
{
public:
	std::string PopupText;
	char* Buffer;
	size_t BufferSize;
	ImGuiInputTextFlags Flags;

	C_ImMMenuItemInputText(std::string m_Name, std::string m_PopupText, char* m_Buffer, size_t m_BufferSize, ImGuiInputTextFlags m_Flags)
		: C_ImMMenuItem(ImMMenuItemType_InputText, m_Name)
	{
		PopupText	= m_PopupText;
		Buffer		= m_Buffer;
		BufferSize	= m_BufferSize;
		Flags		= m_Flags;
	}

	C_ImMMenuTextMultiColor GetPopupText() { return C_ImMMenuTextMultiColor(PopupText); }
};