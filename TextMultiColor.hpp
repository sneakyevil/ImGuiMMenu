#pragma once

class C_ImMMenuTextMultiColorClip
{
public:
	ImVec2 m_Offset;
	ImVec4 m_Clip;

	C_ImMMenuTextMultiColorClip() { }
	C_ImMMenuTextMultiColorClip(ImVec2 p_Offset, ImVec4 p_Clip)
	{
		m_Offset = p_Offset;
		m_Clip = p_Clip;
	}
};

/*
*	Example: "{FF0000}Red {00FF00}Green {0000FF}Blue"
*/
class C_ImMMenuTextMultiColor
{
public:
	std::vector<std::string> m_Strings;
	std::vector<ImU32> m_Colors;

	C_ImMMenuTextMultiColor() { }
	C_ImMMenuTextMultiColor(std::string p_Text)
	{
		Initialize(p_Text);
	}

	ImVec2 CalcTextSize(ImFont* p_Font)
	{
		std::string sFullStr;
		for (auto& sStr : m_Strings) {
			sFullStr += sStr;
		}

		const char* szTextBegin = sFullStr.c_str();
		const char* szTextEnd = &szTextBegin[sFullStr.size()];
		return p_Font->CalcTextSizeA(p_Font->FontSize, FLT_MAX, 0.f, szTextBegin, szTextEnd);
	}

	size_t GetCount()
	{
		return ImMin(m_Strings.size(), m_Colors.size());
	}

	__inline void Push(std::string p_Text, ImU32 p_Color)
	{
		m_Strings.emplace_back(p_Text);
		m_Colors.emplace_back(p_Color);
	}

	void Initialize(std::string p_Text)
	{
		m_Strings.clear();
		m_Colors.clear();

		const char* szColorFormatParse = "%02x%02x%02x";
		int iColor[3] = { 255, 255, 255 };

		if (p_Text[0] == '{' && p_Text.size() > 7 && p_Text[7] == '}')
		{
			sscanf_s(&p_Text[1], szColorFormatParse, &iColor[0], &iColor[1], &iColor[2]);
			p_Text.erase(0, 8);
		}

		while (1)
		{
			size_t sPos = p_Text.find_first_of('{');
			if (sPos == std::string::npos) {
				break;
			}

			if (7 >= p_Text.size() || p_Text[sPos + 7] != '}') { // Parsing failed we don't bother with other colors...
				break;
			}

			Push(p_Text.substr(0, sPos), IM_COL32(iColor[0], iColor[1], iColor[2], 255));

			sscanf_s(&p_Text[sPos + 1], szColorFormatParse, &iColor[0], &iColor[1], &iColor[2]);

			p_Text.erase(0, sPos + 8);
		}

		Push(p_Text, IM_COL32(iColor[0], iColor[1], iColor[2], 255));
	}

	void Draw(ImDrawList* p_DrawList, ImFont* p_Font, float p_FontSize, ImVec2 p_Pos, C_ImMMenuTextMultiColorClip* p_TextClip = nullptr)
	{
		if (p_TextClip) {
			p_Pos += p_TextClip->m_Offset;
		}

		ImVec2 vInitialPos = p_Pos;
		ImVec4* pClip = (p_TextClip ? &p_TextClip->m_Clip : nullptr);
		size_t sCount = GetCount();

		for (size_t i = 0; sCount > i; ++i)
		{
			std::string sString = m_Strings[i];
			while (1)
			{
				size_t sNewlinePos = sString.find_first_of('\n');
				if (sNewlinePos == std::string::npos) {
					break;
				}

				p_DrawList->AddText(p_Font, p_FontSize, p_Pos, m_Colors[i], &sString.substr(0, sNewlinePos)[0], nullptr, 0.f, pClip);
				p_Pos.x = vInitialPos.x;
				p_Pos.y += p_FontSize;

				sString.erase(0, sNewlinePos + 1);
			}

			auto pCString = sString.c_str();
			{
				p_DrawList->AddText(p_Font, p_FontSize, p_Pos, m_Colors[i], pCString, nullptr, 0.f, pClip);
				p_Pos.x += p_Font->CalcTextSizeA(p_FontSize, FLT_MAX, 0.f, pCString).x;
			}
		}
	}
};