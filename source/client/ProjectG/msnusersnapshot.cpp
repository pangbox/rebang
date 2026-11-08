#include "msnusersnapshot.h"
#include "frstatic.h"

IMPLEMENT_OBJECT(ntMsnUserSnapshotDlg, FrForm)

BEGIN_FRESH_MSGMAP(ntMsnUserSnapshotDlg, FrForm)

ON_FRESH_VI("id", FRCMD_INIT, ntMsnUserSnapshotDlg::OnIdInit)
ON_FRESH_VI("gender", FRCMD_INIT, ntMsnUserSnapshotDlg::OnGenderInit)
ON_FRESH_VI("level", FRCMD_INIT, ntMsnUserSnapshotDlg::OnLevelInit)
ON_FRESH_VI("guild", FRCMD_INIT, ntMsnUserSnapshotDlg::OnGuildInit)
ON_FRESH_VI("school", FRCMD_INIT, ntMsnUserSnapshotDlg::OnSchoolInit)
ON_FRESH_VI("status", FRCMD_INIT, ntMsnUserSnapshotDlg::OnStatusInit)
ON_FRESH_VV("detail", FRCMD_LBUTTONUP, ntMsnUserSnapshotDlg::OnDetailClick)

END_FRESH_MSGMAP()

ntMsnUserSnapshotDlg::ntMsnUserSnapshotDlg()
{
	m_pId = NULL;
	m_pGender = NULL;
	m_pLevel = NULL;
	m_pGuild = NULL;
	m_pSchool = NULL;
	m_pStatus = NULL;
}

ntMsnUserSnapshotDlg::~ntMsnUserSnapshotDlg()
{
}

void ntMsnUserSnapshotDlg::OnIdInit(int param)
{
	m_pId = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void ntMsnUserSnapshotDlg::OnGenderInit(int param)
{
	m_pGender = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void ntMsnUserSnapshotDlg::OnLevelInit(int param)
{
	m_pLevel = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void ntMsnUserSnapshotDlg::OnGuildInit(int param)
{
	m_pGuild = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void ntMsnUserSnapshotDlg::OnSchoolInit(int param)
{
	m_pSchool = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void ntMsnUserSnapshotDlg::OnStatusInit(int param)
{
	m_pStatus = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void ntMsnUserSnapshotDlg::OnDetailClick()
{
}
