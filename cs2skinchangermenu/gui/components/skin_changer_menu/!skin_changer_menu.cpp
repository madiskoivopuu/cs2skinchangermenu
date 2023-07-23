#include "pch.h"
#include "gui/components/components.h"

#include "functions.h"

#include "gui/gui_setup.h"
#include "gui/gui_main.h"
#include "skin_changer/skins_cache.h"
#include "cache.h"

#include "nuklear-gui/nuklear.h"
#include "netvars/fnvhash.h"

#include <iostream>

char* stristr(const char* str1, const char* str2)
{
	const char* p1 = str1;
	const char* p2 = str2;
	const char* r = *p2 == 0 ? str1 : 0;

	while (*p1 != 0 && *p2 != 0)
	{
		if (tolower((unsigned char)*p1) == tolower((unsigned char)*p2))
		{
			if (r == 0)
			{
				r = p1;
			}

			p2++;
		}
		else
		{
			p2 = str2;
			if (r != 0)
			{
				p1 = r + 1;
			}

			if (tolower((unsigned char)*p1) == tolower((unsigned char)*p2))
			{
				r = p1;
				p2++;
			}
			else
			{
				r = 0;
			}
		}

		p1++;
	}

	return *p2 == 0 ? (char*)r : 0;
}

void DrawSkinChangerMenu() {
    switch (stateSkinsOverviewMenu.currActiveWindow) {
    case Window::SkinsList:
        DrawSavedSkinsList();
        break;
	case Window::SkinSearch:
		DrawSkinsSearch();
		break;
	case Window::StickerSearch:
		DrawStickerSearch();
		break;
    case Window::SingleSkinSettings:
		DrawSingleSkinSettings();
        break;
    }
}