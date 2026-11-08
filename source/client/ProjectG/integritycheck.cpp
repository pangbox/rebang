#include "stdafx.h"
#include "integritycheck.h"
#include "wbone.h"
#include <stdarg.h>

CIntegrityCheck::CIntegrityCheck()
	: m_fp(NULL)
{
	(void)&g_CM_TO_WU;
}

CIntegrityCheck::~CIntegrityCheck()
{
}

bool CIntegrityCheck::Report(const char* filename)
{
	m_fp = fopen(filename, "wt");
	ReportCharParts();
	fclose(m_fp);
	return true;
}

void CIntegrityCheck::Log(const char* format, ...)
{
	char buffer[2048];
	va_list args;
	va_start(args, format);
	if (_vsnprintf(buffer, sizeof(buffer) - 1, format, args) == -1)
		buffer[sizeof(buffer) - 1] = 0;
	va_end(args);
	fprintf(m_fp, buffer);
}

bool CompareByName(const sResName& left, const sResName& right)
{
	if (strcmp(left.name, right.name) < 0)
		return true;
	return false;
}

void CIntegrityCheck::PrintOkMissing(bool ok)
{
	if (ok)
		Log("ok!!\n");
	else
		Log("missing!!\n");
}

void CIntegrityCheck::PrintOkNg(bool ok)
{
	if (ok)
		Log("ok!!\n");
	else
		Log("ng!!\n");
}

bool CIntegrityCheck::ReportCharParts()
{
	std::map<unsigned int, IFF_STRUCT::sPart>& parts =
		CSharedDoc::Instance()->m_itemManager.m_PartMap;
	std::map<unsigned int, IFF_STRUCT::sChar>& characters =
		CSharedDoc::Instance()->m_itemManager.m_CharMap;
	std::map<unsigned long, WPuppet*> puppets;
	int maxNumVtx = 0;
	int maxNumIdx = 0;
	std::map<std::string, std::string> missing;
	std::map<std::string, std::string> failed;
	std::map<std::string, std::string> matched;
	std::map<std::string, std::string> mismatched;

	Log("* part check - total %d\n\n", parts.size());
	for (PartIterator it = parts.begin(); it != parts.end(); it++)
	{
		if (strlen((*it).second.Data))
		{
			char filename[MAX_PATH];
			sprintf(filename, "%s.mpet", (*it).second.Data);
			Log("\t* part : \"%s\"\n", (*it).second.c.Name);
			Log("\t\t* meshpet : %s - ", filename);
			cFile* file = g_resrcmng->GetCFile(filename, -1);
			PrintOkMissing(file != NULL);
			if (!file)
			{
				missing[filename] = filename;
			}
			else
			{
				CloseCFile(file);
				Log("\t\t* meshpet loading - ");
				WPuppet* puppet =
					g_resrcmng->GetPuppet(filename, false, true, false);
				PrintOkNg(puppet != NULL);
				if (!puppet)
				{
					failed[filename] = filename;
				}
				else
				{
					WBone* bone = puppet->FindMeshBone(NULL);
					for (; bone; bone = puppet->FindMeshBone(bone))
					{
						w_mesh* mesh = bone->GetMesh();
						for (int i = 0; mesh; ++i)
						{
							Log("\t\t* mesh%d : numVtx(%d), numIdx(%d), drawFlag(0x%08x), numMesh, drawFlag2(0x%08x)\n",
								i, mesh->vtxNum, mesh->indexNum,
								mesh->drawFlag & 0xfffc0000,
								mesh->xiDrawFlag2 & 0x3fff);
							if (mesh->vtxNum > maxNumVtx)
								maxNumVtx = mesh->vtxNum;
							if (mesh->indexNum > maxNumIdx)
								maxNumIdx = mesh->indexNum;
							mesh = mesh->next;
						}
					}
					unsigned long typeId =
						(((*it).second.c.TypeId >> 18) & 0xff) | 0x4000000;
					std::map<unsigned long, WPuppet*>::iterator found;
					found = puppets.find(typeId);
					WPuppet* original = NULL;
					if (found == puppets.end())
					{
						CharIterator character;
						character = characters.find(typeId);
						if (character != characters.end())
						{
							original = g_resrcmng->GetPuppet(
								MakeStr("%s.bpet", (*character).second.Data),
								false, true, false);
							if (original)
								puppets.insert(std::map<unsigned long,
									WPuppet*>::value_type(typeId, original));
						}
					}
					else
						original = (*found).second;
					Log("\t\t* compare original matrix - ");
					bool equal =
						original && puppet->CompareBasicMatrix(original);
					if (equal)
					{
						matched[filename] = filename;
					}
					else
					{
						mismatched[filename] = filename;
					}
					PrintOkNg(equal);
					g_resrcmng->Release(puppet, true);
				}
			}
		}
	}
	Log("\n");
	Log("\t* missing mpets - total %d\n", missing.size());
	for (std::map<std::string, std::string>::iterator missingIt =
			 missing.begin();
		missingIt != missing.end(); missingIt++)
		Log("\t\t%s\n", (*missingIt).second.c_str());
	Log("\n");
	Log("\t* loading failed mpets - total %d\n", failed.size());
	for (std::map<std::string, std::string>::iterator failedIt = failed.begin();
		failedIt != failed.end(); failedIt++)
		Log("\t\t%s\n", (*failedIt).second.c_str());
	Log("\n");
	Log("\t* original matrix comparison succeeded mpets - %d\n",
		matched.size());
	for (std::map<std::string, std::string>::iterator matchedIt =
			 matched.begin();
		matchedIt != matched.end(); matchedIt++)
		Log("\t\t%s\n", (*matchedIt).second.c_str());
	Log("\n");
	Log("\t* original matrix comparison failed mpets - %d\n",
		mismatched.size());
	for (std::map<std::string, std::string>::iterator mismatchedIt =
			 mismatched.begin();
		mismatchedIt != mismatched.end(); mismatchedIt++)
		Log("\t\t%s\n", (*mismatchedIt).second.c_str());
	Log("\n\n");
	Log("\t* maxNumVtx : %d\n", maxNumVtx);
	Log("\t* maxNumIdx : %d\n", maxNumIdx);
	std::map<unsigned long, WPuppet*>::iterator puppetIt;
	for (puppetIt = puppets.begin(); puppetIt != puppets.end(); puppetIt++)
		g_resrcmng->Release((*puppetIt).second, true);
	return true;
}
