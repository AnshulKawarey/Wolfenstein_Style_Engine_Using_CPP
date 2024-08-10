#include <iostream>
#include <Windows.h>
#include <chrono>
#include <cmath>
using namespace std;

int nScreenWidth = 240;
int nScreenHeight = 80;
int nMapHeight = 32;
int nMapWidth = 32;

float fPlayerX = 10.0f;
float fPlayerY = 10.0f;
float fPlayerA = 0.0f;
float fFOV = 0.7853975f;
float fDepth = 32.0f;

int main()
{
	//Create screen buffer
	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	wstring map;
	map += L"################################";
	map += L"#..............##..............#";
	map += L"#.......#########.......########";
	map += L"#..............##.......#......#";
	map += L"#......##......##......##......#";
	map += L"#......##..............##......#";
	map += L"#..............................#";
	map += L"###............................#";
	map += L"##...........#####.............#";
	map += L"#............#####...........###";
	map += L"#...............##.............#";
	map += L"#..............................#";
	map += L"#..............................#";
	map += L"#...........................####";
	map += L"#..............................#";
	map += L"#########....########....#######";
	map += L"#########.......#####..........#";
	map += L"#...............#..............#";
	map += L"#.......#########.......########";
	map += L"#..............##..............#";
	map += L"#......##......##.......#......#";
	map += L"#......##......##......##......#";
	map += L"#..............##..............#";
	map += L"##.............##..............#";
	map += L"##.............##..............#";
	map += L"#............####..............#";
	map += L"#.......................########";
	map += L"#....................#.........#";
	map += L"#..............#######.........#";
	map += L"#..............##..............#";
	map += L"#..............##..............#";
	map += L"################################";

	auto tp1 = chrono::system_clock::now();
	auto tp2 = chrono::system_clock::now();

	//Game Loop
	while (1)
	{
		tp2 = chrono::system_clock::now();
		chrono::duration<float> elapsedTime = tp2 - tp1;
		tp1 = tp2;
		float fElapsedTime = elapsedTime.count();


		//Controls
		//Rotations
		if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
			fPlayerA -= (1.9f) * fElapsedTime;
		if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
			fPlayerA += (1.9f) * fElapsedTime;
		if (GetAsyncKeyState((unsigned short)'W') & 0x8000) {
			fPlayerX += sinf(fPlayerA) * 2.5f * fElapsedTime;
			fPlayerY += cosf(fPlayerA) * 2.5f * fElapsedTime;
		}
		if (GetAsyncKeyState((unsigned short)'S') & 0x8000) {
			fPlayerX -= sinf(fPlayerA) * 1.5f * fElapsedTime;
			fPlayerY -= cosf(fPlayerA) * 1.5f * fElapsedTime;
		}


		for (int x = 0; x < nScreenWidth; x++)
		{
			//calculating the projected ray angle for each column in the space
			float fRayAngle = (fPlayerA - fFOV / 2.0f) + ((float)x / (float)nScreenWidth) * fFOV;

			float fDistanceToWall = 0;
			bool bHitWall = false;

			float fEyeX = sinf(fRayAngle);
			float fEyeY = cosf(fRayAngle);

			while (!bHitWall && fDistanceToWall < fDepth)
			{

				fDistanceToWall += 0.1f;

				int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
				int nTestY = (int)(fPlayerY + fEyeY * fDistanceToWall);

				//to check if ray is out of bounds
				if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight)
				{
					bHitWall = true;
					fDistanceToWall = fDepth;			//sets distance to max depth as ray is out of bounds
				}
				else
				{
					if (map[nTestY * nMapWidth + nTestX] == '#')		//if y coord paired with the width and the x coord has # character on the map array made then wall hit is considered to be true
					{
						bHitWall = true;
					}
				}
			}

			//ceiling and floor distance
			int nCeiling = ((float)nScreenHeight / 2.0) - nScreenHeight / ((float)fDistanceToWall); //subtracting half of the screen with a proportionately sized subtraction, as distance changes subtraction will change too
			int nFloor = nScreenHeight - nCeiling; //same as ceiling

			short nShade = ' ';

			if (fDistanceToWall <= fDepth / 8.0f)		nShade = 0x2588;	//closest
			else if (fDistanceToWall <= fDepth / 5.0f)	nShade = 0x2593;
			else if (fDistanceToWall <= fDepth / 3.0f)	nShade = 0x2592;
			else if (fDistanceToWall <= fDepth)			nShade = 0x2591;	//farthest rendered
			else										nShade = ' ';		//too far so not rendered

			for (int y = 0; y < nScreenHeight; y++)
			{
				if (y <=nCeiling)
					screen[y * nScreenWidth + x] = ' ';
				else if(y>nCeiling && y<=nFloor)
					screen[y * nScreenWidth + x] = nShade;
				else
				{
					float b = 1.0f - (((float)y - nScreenHeight / 2.0f) / ((float)nScreenHeight / 2.0f));
					if (b < 0.25)		nShade = '#';
					else if (b < 0.5)	nShade = 'x';
					else if (b < 0.75)	nShade = '.';
					else if (b < 0.9)	nShade = '-';
					else				nShade = ' ';
					screen[y * nScreenWidth + x] = nShade;
				}
			}
		}

		for (int nx = 0; nx < nMapWidth; nx++)
			for (int ny = 0; ny < nMapWidth; ny++)
			{
				screen[(ny + 1) * nScreenWidth + nx] = map[ny * nMapWidth + nx];
			}

		screen[((int)fPlayerY + 1) * nScreenWidth + (int)fPlayerX] = '0';

		screen[nScreenWidth * nScreenHeight - 1] = '\0';
		WriteConsoleOutputCharacterW(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
	}

	return 0;
}
