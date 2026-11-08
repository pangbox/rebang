#include "newsreader.h"

NewsReader::NewsReader()
{
}

NewsReader::~NewsReader()
{
	Clear();
}

unsigned int NewsReader::Load(const char* filename, unsigned int maxArticles)
{
	Clear();
	TiXmlDocument doc;
	if (!doc.LoadFile(filename))
		return 0;

	unsigned int count = 0;
	TiXmlNode* root = doc.FirstChild("rss");
	if (!root)
		return 0;

	TiXmlNode* node = root->FirstChild("notice");
	while (node)
	{
		sArticle article;
		GetChildNodeText(node, "title", article.title);
		GetChildNodeText(node, "url", article.url);
		GetChildNodeText(node, "content", article.content);
		GetChildNodeText(node, "RegDate", article.regDate);
		GetChildNodeText(node, "Cate", article.cate);
		GetChildNodeText(node, "Userid", article.userId);
		m_articles.push_back(article);
		++count;
		if (maxArticles && count >= maxArticles)
			break;
		node = node->NextSibling();
	}
	return count;
}

unsigned int NewsReader::GetNumArticles() const
{
	return m_articles.size();
}

const NewsReader::sArticle* NewsReader::GetArticle(unsigned int index) const
{
	if (index < m_articles.size())
		return &m_articles[index];
	return NULL;
}

void NewsReader::Clear()
{
	m_articles.clear();
}

bool NewsReader::GetChildNodeText(const TiXmlNode* node, const char* name,
	std::string& text)
{
	if (!node || !name)
		return false;
	const TiXmlNode* child = node->FirstChild(name);
	if (!child)
		return false;
	child = child->FirstChild();
	if (!child)
		return false;
	text = child->Value();
	return true;
}
