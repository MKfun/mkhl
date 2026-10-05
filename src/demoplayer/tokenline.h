#ifndef DEMOPLAYER_TOKENLINE_H
#define DEMOPLAYER_TOKENLINE_H

class TokenLine
{
public:
	TokenLine();
	TokenLine(char *string);
	virtual ~TokenLine();

	bool SetLine(const char *newLine);
	char *GetLine();
	char *GetToken(int i);
	char *CheckToken(char *parm);
	int CountToken();
	char *GetRestOfLine(int i);

private:
	char m_tokenBuffer[2048];
	char m_fullLine[2048];
	char *m_token[128];
	int m_tokenNumber;
};

#endif // DEMOPLAYER_TOKENLINE_H
