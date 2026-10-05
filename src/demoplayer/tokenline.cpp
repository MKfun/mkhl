#include "tokenline.h"
#include <cstring>

TokenLine::TokenLine()
{
	m_tokenNumber = 0;
	memset(m_tokenBuffer, 0, sizeof(m_tokenBuffer));
	memset(m_fullLine, 0, sizeof(m_fullLine));
	memset(m_token, 0, sizeof(m_token));
}

TokenLine::TokenLine(char *string)
{
	m_tokenNumber = 0;
	memset(m_tokenBuffer, 0, sizeof(m_tokenBuffer));
	memset(m_fullLine, 0, sizeof(m_fullLine));
	memset(m_token, 0, sizeof(m_token));
	SetLine(string);
}

TokenLine::~TokenLine()
{
}

bool TokenLine::SetLine(const char *newLine)
{
	m_tokenNumber = 0;
	if (!newLine || strlen(newLine) > 2046)
	{
		memset(m_fullLine, 0, sizeof(m_fullLine));
		memset(m_tokenBuffer, 0, sizeof(m_tokenBuffer));
		return false;
	}

	strncpy(m_fullLine, newLine, sizeof(m_fullLine) - 1);
	m_fullLine[sizeof(m_fullLine) - 1] = '\0';
	strncpy(m_tokenBuffer, newLine, sizeof(m_tokenBuffer) - 1);
	m_tokenBuffer[sizeof(m_tokenBuffer) - 1] = '\0';

	char *p = m_tokenBuffer;
	while (*p && m_tokenNumber < 128)
	{
		while (*p && (unsigned char)(*p - 33) > 0x5D)
			p++;

		if (!*p)
			break;

		if (*p == '"')
		{
			p++;
			m_token[m_tokenNumber] = p;
			while (*p && *p != '"')
				p++;
		}
		else
		{
			m_token[m_tokenNumber] = p;
			while (*p && (unsigned char)(*p - 33) <= 0x5D)
				p++;
		}

		m_tokenNumber++;
		if (!*p)
			break;

		*p++ = '\0';
	}

	return (m_tokenNumber != 128);
}

char *TokenLine::GetLine()
{
	return m_fullLine;
}

char *TokenLine::GetToken(int i)
{
	if (m_tokenNumber > i && i >= 0)
		return m_token[i];
	return nullptr;
}

char *TokenLine::CheckToken(char *parm)
{
	if (m_tokenNumber > 0)
	{
		for (int i = 0; i < m_tokenNumber; i++)
		{
			if (m_token[i] && !strcmp(parm, m_token[i]))
			{
				if (i + 1 != m_tokenNumber)
					return m_token[i + 1];
				return (char *)"";
			}
		}
	}
	return nullptr;
}

int TokenLine::CountToken()
{
	int count = 0;
	for (int i = 0; i < m_tokenNumber; i++)
	{
		if (m_token[i])
			count++;
	}
	return count;
}

char *TokenLine::GetRestOfLine(int i)
{
	if (m_tokenNumber > i && i >= 0)
		return m_token[i] + sizeof(m_tokenBuffer);
	return nullptr;
}
