#include "noticeimage.h"
#include "wresrcmng.h"

void LoadNoticeImageFile(const char* filename,
	std::vector<sNoticeImage>& images, int maxNum, const char* lang)
{
	cFile* file = g_resrcmng->GetCFile(filename, -1);

	if (!file)
		return;

	int size = file->Length();
	char* buf = new char[size + 1];

	file->Read(buf, size);
	buf[size] = 0;
	CloseCFile(file);

	char* p = buf;

	images.clear();

	while (*p)
	{
		sNoticeImage info;
		std::string token;
		bool bSpace = true;

		token.clear();

		while (*p && *p != '\n')
		{
			if (*p == '(')
			{
				bSpace = false;

				if (!info.name[0])
					strcpy(info.name, token.c_str());
				token.clear();
			}
			else if (*p == ')')
			{
				sNoticeImage::sBn bn;
				bSpace = true;

				sscanf(token.c_str(), "%d %f %f %f %f", &bn.typeId, &bn.rect.x,
					&bn.rect.y, &bn.rect.w, &bn.rect.h);
				info.bnList.push_back(bn);
			}
			else if (*p == '\r')
				;
			else if (*p != ' ' || !bSpace)
				token += *p;

			++p;
		}

		if (!info.name[0])
			strcpy(info.name, token.c_str());

		if (strlen(info.name))
			images.push_back(info);

		if (images.size() == maxNum)
			break;

		if (*p)
			++p;
	}

	delete[] buf;
}
