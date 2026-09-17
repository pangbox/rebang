// Minimalist stubbed ATL. Though there is no exact known header like this, it
// does seem to reproduce a pattern often seen in the executable, so presumably
// they did do something along these lines somehow.
namespace ATL
{
	class CAtlWinModule
	{
		unsigned char storage[44];

	public:
		CAtlWinModule();
		~CAtlWinModule();
	};
	__declspec(selectany) CAtlWinModule _AtlWinModule;
}
