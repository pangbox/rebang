#include "classicserver.h"

namespace
{
	float l_LIMITANGLE = 10.0f;
	float l_WINDANGLE;
}

namespace S5
{
	namespace CLASSICSRV
	{
		enum
		{
			SERVERFLAG_CLASSIC = 0x80
		};

		int IsClassicServer(unsigned long serverFlag)
		{
			return (OnlinePlay() && (serverFlag & SERVERFLAG_CLASSIC)) ? TRUE
																	   : FALSE;
		}

		eWINDPOWERSTEP GetWindPowerStep(float windPower)
		{
			if (windPower < 4.0f)
				return WINDPOWERSTEP_WEAK;

			if (windPower >= 4.0f && windPower < 7.0f)
				return WINDPOWERSTEP_MIDDLE;

			return WINDPOWERSTEP_STRONG;
		}

		float WindRollSpeed(float windPower)
		{
			if (windPower < 4.0f)
				return g_PI * 0.45f;

			if (windPower >= 4.0f && windPower < 7.0f)
				return g_PI * 1.05f;

			if (windPower >= 7.0f)
				return g_PI * 2.25f;

			return 0.0f;
		}

		void WindAdditionalPitchAngleProcess(float elapsed)
		{
			static bool s_bIncrease = true;

			if (s_bIncrease)
			{
				if (l_LIMITANGLE <= 0.0f)
					l_LIMITANGLE = (float)(rand() % 12 + 3);

				l_WINDANGLE += l_LIMITANGLE * elapsed * 1.3f;
				if (l_WINDANGLE > l_LIMITANGLE)
					s_bIncrease = false;
			}
			else
			{
				if (l_LIMITANGLE >= 0.0f)
					l_LIMITANGLE = -(float)(rand() % 12 + 3);

				l_WINDANGLE += l_LIMITANGLE * elapsed * 1.3f;
				if (l_WINDANGLE < l_LIMITANGLE)
					s_bIncrease = true;
			}
		}

		float GetWindAdditionalPitchAngle()
		{
			return l_WINDANGLE;
		}
	}
}
