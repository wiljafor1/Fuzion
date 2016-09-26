#include "esp.h"

/*---------------
	Defaults	
---------------*/
bool		Settings::ESP::enabled			= true;
bool		Settings::ESP::Walls::enabled	= true;
bool		Settings::ESP::Tracer::enabled	= true;
bool		Settings::ESP::Name::enabled	= true;
TracerType	Settings::ESP::Tracer::type		= BOTTOM;

bool WorldToScreen (const Vector &vOrigin, Vector &vScreen)
{
	return ( debugOverlay->ScreenPosition( vOrigin, vScreen ));
}

Vector2D WorldToScreen (const Vector &vOrigin)
{
	Vector vec;
	debugOverlay->ScreenPosition( vOrigin, vec );
	return LOC(vec.x, vec.y);
}

static wchar_t* cwConvert(const char* text)
{
	const size_t size = strlen(text) + 1;
	wchar_t* wText = new wchar_t[size];
	mbstowcs(wText, text, size);
	return wText;
}

void DrawESPBox (Vector vecOrigin, Vector vecViewOffset, Color color, int width, int additionalHeight)
{
	//SIDES

	Vector2D a = WorldToScreen (vecOrigin + Vector(width, width, additionalHeight));
	Vector2D b = WorldToScreen (vecOrigin + vecViewOffset + Vector (width, width, additionalHeight));

	Draw::DrawLine (a, b, color);


	a = WorldToScreen (vecOrigin + Vector(-width, width, additionalHeight));
	b = WorldToScreen (vecOrigin + vecViewOffset + Vector (-width, width, additionalHeight));

	Draw::DrawLine (a, b, color);


	a = WorldToScreen (vecOrigin + Vector(-width, -width, additionalHeight));
	b = WorldToScreen (vecOrigin + vecViewOffset + Vector (-width, -width, additionalHeight));

	Draw::DrawLine (a, b, color);

	a = WorldToScreen (vecOrigin + Vector(width, -width, additionalHeight));
	b = WorldToScreen (vecOrigin + vecViewOffset + Vector (width, -width, additionalHeight));

	Draw::DrawLine (a, b, color);


	//TOP

	a = WorldToScreen (vecOrigin + vecViewOffset + Vector(width, width, additionalHeight));
	b = WorldToScreen (vecOrigin + vecViewOffset + Vector (width, -width, additionalHeight));

	Draw::DrawLine (a, b, color);


	a = WorldToScreen (vecOrigin + vecViewOffset + Vector(width, width, additionalHeight));
	b = WorldToScreen (vecOrigin + vecViewOffset + Vector (-width, width, additionalHeight));

	Draw::DrawLine (a, b, color);

	a = WorldToScreen (vecOrigin + vecViewOffset + Vector(-width, -width, additionalHeight));
	b = WorldToScreen (vecOrigin + vecViewOffset + Vector (-width, width, additionalHeight));

	Draw::DrawLine (a, b, color);


	a = WorldToScreen (vecOrigin + vecViewOffset + Vector(width, -width, additionalHeight));
	b = WorldToScreen (vecOrigin + vecViewOffset + Vector (-width, -width, additionalHeight));

	Draw::DrawLine (a, b, color);


	//BOTTOM

	a = WorldToScreen (vecOrigin + Vector(width, width, additionalHeight));
	b = WorldToScreen (vecOrigin + Vector (width, -width, additionalHeight));

	Draw::DrawLine (a, b, color);


	a = WorldToScreen (vecOrigin + Vector(width, width, additionalHeight));
	b = WorldToScreen (vecOrigin + Vector (-width, width, additionalHeight));

	Draw::DrawLine (a, b, color);

	a = WorldToScreen (vecOrigin + Vector(-width, -width, additionalHeight));
	b = WorldToScreen (vecOrigin + Vector (-width, width, additionalHeight));

	Draw::DrawLine (a, b, color);


	a = WorldToScreen (vecOrigin + Vector(width, -width, additionalHeight));
	b = WorldToScreen (vecOrigin + Vector (-width, -width, additionalHeight));

	Draw::DrawLine (a, b, color);
}

void ESP::Tick ()
{
	C_BasePlayer* localPlayer = (C_BasePlayer*)entitylist->GetClientEntity(engine->GetLocalPlayer());

	if (!localPlayer)
		return;

	for (int i = 0; i < 64; ++i)
	{
		C_BaseEntity* entity = entitylist->GetClientEntity(i);
		
		if
		(
			   !entity
			|| entity == (C_BaseEntity*)localPlayer
			|| entity->GetDormant()
			|| entity->GetLifeState() != 0
			|| entity->GetHealth() <= 0
		)
			continue;
		
		
		
		if (Settings::ESP::Walls::enabled)
			ESP::DrawPlayerBox	(localPlayer, entity);
		
		if (Settings::ESP::Tracer::enabled)
			ESP::DrawTracer		(localPlayer, entity);
		
		if (Settings::ESP::Name::enabled)
			ESP::DrawPlayerName	(localPlayer, entity, i);
	}
}

void ESP::DrawTracer (C_BasePlayer* localPlayer, C_BaseEntity* entity)
{
	Color color;
	
	int playerTeam = localPlayer->GetTeam();
	int entityTeam = reinterpret_cast<C_BasePlayer*>(entity)->GetTeam();
	
	if (playerTeam != entityTeam)
	{
		color.r = 200;
		color.b = 50;
	}
	else
	{
		color.b = 200;
		color.g = 50;
	}
	
	int width;
	int height;
	engine->GetScreenSize (width, height);
	
	Vector2D tracerLocation;
	
	switch (Settings::ESP::Tracer::type)
	{
		case BOTTOM:
			tracerLocation = LOC(width / 2, height);
			break;
		case CURSOR:
			tracerLocation = LOC(width / 2, height / 2);
			break;
	}
	
	Vector s_vecEntity_s;
	if (!WorldToScreen(entity->GetVecOrigin(), s_vecEntity_s) && localPlayer->GetHealth() > 0)
	{
		Draw::DrawLine (tracerLocation, LOC(s_vecEntity_s.x, s_vecEntity_s.y), color);
	}
}

void ESP::DrawPlayerBox (C_BasePlayer* localPlayer, C_BaseEntity* entity)
{
	Color color;

	int playerTeam = localPlayer->GetTeam();
	int entityTeam = reinterpret_cast<C_BasePlayer*>(entity)->GetTeam();
	
	if (playerTeam != entityTeam)
	{
		color.r = 200;
		color.b = 50;
	}
	else
	{
		color.b = 200;
		color.g = 50;
	}
	
	int width = 14;
	int additionalHeight = 6;
	
	Vector vecOrigin = entity->GetVecOrigin();
	Vector vecViewOffset = localPlayer->GetVecViewOffset();
	
	Vector s_vecLocalPlayer_s;
	if (!WorldToScreen(vecOrigin, s_vecLocalPlayer_s))
		DrawESPBox (vecOrigin, vecViewOffset, color, width, additionalHeight);
}

void ESP::DrawPlayerName (C_BasePlayer* localPlayer, C_BaseEntity* entity, int entityIndex)
{
	Color color;

	int playerTeam = localPlayer->GetTeam();
	int entityTeam = reinterpret_cast<C_BasePlayer*>(entity)->GetTeam();
	
	if (playerTeam != entityTeam)
	{
		color.r = 150;
		color.b = 10;
	}
	else
	{
		color.b = 200;
		color.g = 100;
	}
	
	CEngineClient::player_info_t entityInformation;
	engine->GetPlayerInfo(entityIndex, &entityInformation);
	
	Vector vecOrigin = entity->GetVecOrigin ();
	
	Vector s_vecEntity_s;
	if (!WorldToScreen(vecOrigin, s_vecEntity_s))
		Draw::DrawString (CONV(entityInformation.name), LOC(s_vecEntity_s.x, s_vecEntity_s.y), color, 33, true);
}

