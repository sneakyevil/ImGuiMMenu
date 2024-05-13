#pragma once

enum eImMMenuItemType : int
{
	eImMMenuItemType_Unknown = -1,
	eImMMenuItemType_Separator = 0,
	eImMMenuItemType_Text,
	eImMMenuItemType_TextUnselectable,
	eImMMenuItemType_Section,
	eImMMenuItemType_Checkbox,
	eImMMenuItemType_Combo,
	eImMMenuItemType_ComboCheckbox,
	eImMMenuItemType_Integer,
	eImMMenuItemType_Float,
	eImMMenuItemType_Keybind,
	eImMMenuItemType_InputText,
};

class C_ImMMenuItem
{
public:
	eImMMenuItemType m_Type = eImMMenuItemType_Unknown;
	std::string m_Name;
	std::string m_Description;

	virtual ~C_ImMMenuItem() { }
	virtual void Interaction() { }
	virtual bool SideInteraction(int p_Value) 
	{ 
		return false; 
	}
		
	C_ImMMenuItem() { }
	C_ImMMenuItem(eImMMenuItemType p_Type)
	{ 
		m_Type = p_Type; 
	}

	C_ImMMenuItem(eImMMenuItemType p_Type, std::string& p_Name)
	{ 
		m_Type = p_Type;
		m_Name = p_Name;
	}

	__inline C_ImMMenuTextMultiColor GetName()
	{ 
		return C_ImMMenuTextMultiColor(m_Name);
	}

	__inline C_ImMMenuTextMultiColor GetDescription()
	{ 
		return C_ImMMenuTextMultiColor(m_Description);
	}
};

class C_ImMMenuItemSeparator : public C_ImMMenuItem
{
public:
	C_ImMMenuItemSeparator(std::string& p_Name) : C_ImMMenuItem(eImMMenuItemType_Separator, p_Name) { }
};

class C_ImMMenuItemCheckbox : public C_ImMMenuItem
{
public:
	bool* m_Value;

	virtual void Interaction() 
	{
		*m_Value = !*m_Value;
	}

	C_ImMMenuItemCheckbox(std::string& p_Name, bool* p_Value)
		: C_ImMMenuItem(eImMMenuItemType_Checkbox, p_Name)
	{
		m_Value = p_Value;
	}

	__inline bool IsChecked() 
	{ 
		return *m_Value;
	}
};

class C_ImMMenuItemCombo : public C_ImMMenuItem
{
public:
	int* m_Value;
	std::vector<std::string> m_Items;
	bool m_Clamp;

	virtual bool SideInteraction(int p_Value)
	{
		int iSize = static_cast<int>(m_Items.size());

		*m_Value += p_Value;
		if (*m_Value >= iSize) {
			*m_Value = (m_Clamp ? iSize - 1 : 0);
		}
		else if (0 > *m_Value) {
			*m_Value = (m_Clamp ? 0 : iSize - 1);
		}

		return true;
	}

	C_ImMMenuItemCombo(std::string& p_Name, int* p_Value, std::vector<std::string>& p_Items, bool p_Clamp)
		: C_ImMMenuItem(eImMMenuItemType_Combo, p_Name)
	{
		m_Value		= p_Value;
		*m_Value	= ImClamp(*m_Value, 0, static_cast<int>(p_Items.size()));
		m_Items		= p_Items;
		m_Clamp		= p_Clamp;
	}

	__inline C_ImMMenuTextMultiColor GetPreview() 
	{ 
		return C_ImMMenuTextMultiColor(m_Items[*m_Value]); 
	}
};

class C_ImMMenuItemComboCheckbox : public C_ImMMenuItemCombo
{
public:
	std::vector<bool>* m_Values;

	virtual void Interaction()
	{
		m_Values->operator[](*m_Value) = !m_Values->operator[](*m_Value);
	}

	C_ImMMenuItemComboCheckbox(std::string& p_Name, int* p_Value, std::vector<bool>* p_Values, std::vector<std::string>& p_Items, bool p_Clamp)
		: C_ImMMenuItemCombo(p_Name, p_Value, p_Items, p_Clamp)
	{
		m_Type = eImMMenuItemType_ComboCheckbox;
		m_Values = p_Values;
	}

	__inline bool IsChecked() 
	{ 
		return m_Values->operator[](*m_Value);
	}
};

class C_ImMMenuItemInteger : public C_ImMMenuItem
{
public:
	int* m_Value;
	int m_Min, m_Max;
	int m_Power;
	bool m_Clamp;

	virtual bool SideInteraction(int p_Value)
	{
		bool bShouldClamp = (m_Clamp || (*m_Value != m_Min && *m_Value != m_Max));
		*m_Value += (p_Value * m_Power);

		if (bShouldClamp) {
			*m_Value = ImClamp(*m_Value, m_Min, m_Max);
		}
		else if (*m_Value > m_Max) {
			*m_Value = m_Min;
		}
		else if (m_Min > *m_Value) {
			*m_Value = m_Max;
		}

		return true;
	}

	C_ImMMenuItemInteger(std::string& p_Name, int* p_Value, int p_Min, int p_Max, int p_Power, bool p_Clamp)
		: C_ImMMenuItem(eImMMenuItemType_Integer, p_Name)
	{
		m_Value		= p_Value;
		*m_Value	= ImClamp(*p_Value, p_Min, p_Max);

		m_Min	= p_Min;
		m_Max	= p_Max;
		m_Power = p_Power;
		m_Clamp = p_Clamp;
	}

	__inline std::string GetPreview() 
	{ 
		return std::to_string(*m_Value); 
	}
};

class C_ImMMenuItemFloat : public C_ImMMenuItem
{
public:
	float* m_Value;
	float m_Min, m_Max;
	float m_Power;
	bool m_Clamp;

	const char* m_PreviewFormat;

	virtual bool SideInteraction(int p_Value)
	{
		bool bShouldClamp = (m_Clamp || (*m_Value != m_Min && *m_Value != m_Max));
		*m_Value += (static_cast<float>(p_Value) * m_Power);

		if (bShouldClamp) {
			*m_Value = ImClamp(*m_Value, m_Min, m_Max);
		}
		else if (*m_Value > m_Max) {
			*m_Value = m_Min;
		}
		else if (m_Min > *m_Value) {
			*m_Value = m_Max;
		}

		return true;
	}

	C_ImMMenuItemFloat(std::string& p_Name, float* p_Value, float p_Min, float p_Max, float p_Power, bool p_Clamp, const char* p_PreviewFormat)
		: C_ImMMenuItem(eImMMenuItemType_Float, p_Name)
	{
		m_Value		= p_Value;
		*m_Value	= ImClamp(*p_Value, p_Min, p_Max);

		m_Min	= p_Min;
		m_Max	= p_Max;
		m_Power = p_Power;
		m_Clamp = p_Clamp;

		m_PreviewFormat = p_PreviewFormat;
	}

	const char* GetPreview()
	{
		const char* pBuffer;
		ImFormatStringToTempBuffer(&pBuffer, 0, m_PreviewFormat, *m_Value);
		return pBuffer;
	}
};

class C_ImMMenuItemKeybind : public C_ImMMenuItem
{
public:
	ImGuiKey* m_Value;

	C_ImMMenuItemKeybind(std::string& p_Name, ImGuiKey* p_Value)
		: C_ImMMenuItem(eImMMenuItemType_Keybind, p_Name)
	{
		m_Value = p_Value;
	}

	__inline ImGuiKey GetKey() 
	{ 
		return *m_Value; 
	}
};

class C_ImMMenuItemInputText : public C_ImMMenuItem
{
public:
	std::string m_PopupText;
	char* m_Buffer;
	size_t m_BufferSize;
	ImGuiInputTextFlags m_Flags;
	bool m_HideBuffer;

	C_ImMMenuItemInputText(std::string& p_Name, std::string p_PopupText, char* p_Buffer, size_t p_BufferSize, ImGuiInputTextFlags p_Flags, bool p_HideBuffer)
		: C_ImMMenuItem(eImMMenuItemType_InputText, p_Name)
	{
		m_PopupText		= p_PopupText;
		m_Buffer		= p_Buffer;
		m_BufferSize	= p_BufferSize;
		m_Flags			= p_Flags;
		m_HideBuffer 	= p_HideBuffer;
	}

	__inline C_ImMMenuTextMultiColor GetPopupText() 
	{ 
		return C_ImMMenuTextMultiColor(m_PopupText);
	}
};