#pragma once

/*
*	Example: "{FF0000}Red {00FF00}Green {0000FF}Blue"
*/
class C_ImMMenuTextMultiColor
{
public:
	std::vector<std::string> String;
	std::vector<ImU32> Color;
	int Count = 0;

	std::string GetFullString()
	{
		std::string m_Return;

		for (std::string& m_Text : String)
			m_Return += m_Text;

		return m_Return;
	}

	__inline void Push(std::string m_Text, ImU32 m_Color)
	{
		String.emplace_back(m_Text);
		Color.emplace_back(m_Color);
		++Count;
	}

	void Initialize(std::string m_Text)
	{
		String.clear();
		Color.clear();
		Count = 0;

		const char* m_ColorFormatParse = "%02x%02x%02x";
		int m_Color[3] = { 255, 255, 255 };

		if (m_Text[0] == '{' && m_Text.size() > 7 && m_Text[7] == '}')
		{
			sscanf_s(&m_Text[1], m_ColorFormatParse, &m_Color[0], &m_Color[1], &m_Color[2]);
			m_Text.erase(0, 8);
		}

		while (1)
		{
			size_t m_Pos = m_Text.find_first_of('{');
			if (m_Pos == std::string::npos)
				break;

			if (7 >= m_Text.size() || m_Text[m_Pos + 7] != '}') // Parsing failed we don't bother with other colors...
				break;

			Push(m_Text.substr(0, m_Pos), IM_COL32(m_Color[0], m_Color[1], m_Color[2], 255));

			sscanf_s(&m_Text[m_Pos + 1], m_ColorFormatParse, &m_Color[0], &m_Color[1], &m_Color[2]);

			m_Text.erase(0, m_Pos + 8);
		}

		Push(m_Text, IM_COL32(m_Color[0], m_Color[1], m_Color[2], 255));
	}

	C_ImMMenuTextMultiColor() { }
	C_ImMMenuTextMultiColor(std::string m_Text) { Initialize(m_Text); }
};