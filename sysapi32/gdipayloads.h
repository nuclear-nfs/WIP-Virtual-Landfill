DWORD __stdcall Payload1(LPVOID lpvoid)
{
	HDC hdc;
	int sw, sh;
	while (1) {
		hdc = GetDC(HWND_DESKTOP);
		sw = GetSystemMetrics(0), sh = GetSystemMetrics(1);
		Rectangle(hdc, 50 + rand () % sw, 50 + rand () % sh, rand () % sw, rand () % sh);
		Ellipse(hdc, rand () % sw, rand () % sh, 50 + rand () % sw, 50 + rand () % sh);
		LineTo(hdc, sw, rand () % sh);
		BitBlt(hdc, rand () % 16, rand () % 16, sw, sh, hdc, rand () % 16, rand () % 16, SRCAND);
		ReleaseDC(NULL, hdc);
		Sleep(10);
	}
}

DWORD __stdcall Cursors(LPVOID lpvoid)
{
	HDC hdc;
	int sw, sh;
	while (1) {
		hdc = GetDC(0);
		sw = GetSystemMetrics(0), sh = GetSystemMetrics(1);
		DrawIconEx(hdc, rand () % sw, rand () % sh, LoadCursorA(NULL, IDC_ARROW), rand () % 5 * GetSystemMetrics(SM_CXICON), rand () % 5 * GetSystemMetrics(SM_CYICON), NULL, NULL, DI_NORMAL);
		ReleaseDC(NULL, hdc);
		Sleep(10);
	}
}

DWORD __stdcall Mirroring(LPVOID lpvoid)
{
	HDC hdc;
	int sw, sh;
	while (1) {
		hdc = GetDC(NULL);
		sw = GetSystemMetrics(0), sh = GetSystemMetrics(1);
		StretchBlt(hdc, 0, 0, sw, sh, hdc, sw, 0, -sw, sh, SRCCOPY);
		Sleep(10);
		StretchBlt(hdc, 0, 0, sw, sh, hdc, 0, sh, sw, -sh, SRCCOPY);
		Sleep(10);
		ReleaseDC(NULL, hdc);
		Sleep(10);
	}
}

DWORD __stdcall Payload2(LPVOID lpvoid)
{
	HDC hdc;
	int sw, sh;
	HBRUSH hb;
	HPEN hp;
	while (1) {
		hdc = GetDC(NULL);
		sw = GetSystemMetrics(0), sh = GetSystemMetrics(1);
		hb = CreateSolidBrush(GetRandomRainbowSpectrum());
		hp = CreatePen(PS_SOLID, 20, RGB(rand () % 255, rand () % 255, rand () % 255));
		SelectObject(hdc, hb);
		PatBlt(hdc, 0, 0, sw, sh, PATINVERT);
		SelectObject(hdc, hp);
		Arc(hdc, rand () % sw, rand () % sh, rand () % sw, rand () % sh, sw, sh, rand () % sw, sh);
		BitBlt(hdc, -2, -2, sw, sh, hdc, 0, 0, NOTSRCERASE);
		DeleteObject(hb); DeleteObject(hp);
		ReleaseDC(NULL, hdc);
		Sleep(10);
	}
}

DWORD __stdcall InvertEpilepsy(LPVOID lpvoid)
{
	HDC hdc;
	int sw, sh;
	int rw, rh;
	while (1) {
		hdc = GetDC(0);
		sw = GetSystemMetrics(0), sh = GetSystemMetrics(1);
		rw = rand () % sw, rh = rand () % sh;
		SelectObject(hdc, CreateSolidBrush(GetRandomRainbowSpectrum()));
		BitBlt(hdc, rw, 0, 150, sh, hdc, rw, 0, PATINVERT);
		InvertRgn(hdc, CreateEllipticRgn(rand () % sw, rand () % sh, rand () % sw, rand () % sh));
		ReleaseDC(NULL, hdc);
		Sleep(10);
	}
}

DWORD __stdcall QuadSrcShaking(LPVOID lpvoid)
{
	HDC hdc;
	int sw, sh;
	while (1) {
		hdc = GetDC(0);
		sw = GetSystemMetrics(0), sh = GetSystemMetrics(1);
		BitBlt(hdc, 50, 50, sw, sh, hdc, 0, 0, SRCCOPY);
		Sleep(10);
		BitBlt(hdc, 0, 0, sw, sh, hdc, 50, 0, SRCERASE);
		Sleep(10);
		BitBlt(hdc, 0, -50, sw, sh, hdc, 0, 0, SRCPAINT);
		Sleep(10);
		BitBlt(hdc, -50, 0, sw, sh, hdc, 0, 0, SRCINVERT);
		ReleaseDC(NULL, hdc);
		Sleep(10);
	}
}

DWORD __stdcall PRGBQUADShader(LPVOID lpvoid)
{
    HDC hdc = GetDC(NULL), hmem = CreateCompatibleDC(hdc);
    int sw = GetSystemMetrics(0), sh = GetSystemMetrics(1);
    BITMAPINFO bmp = { 0 };
    PRGBQUAD rgbquad = { 0 };
    bmp.bmiHeader.biSize = sizeof(BITMAPINFO);
    bmp.bmiHeader.biBitCount = 32;
    bmp.bmiHeader.biPlanes = 1;
    bmp.bmiHeader.biWidth = sw;
    bmp.bmiHeader.biHeight = sh;
    HBITMAP bmt = CreateDIBSection(hdc, &bmp, NULL, (void**)&rgbquad, NULL, NULL);
    SelectObject(hmem, bmt);
    for (;;) { //e
        BitBlt(hmem, 0, 5, sw, sh, hdc, 0, 0, SRCPAINT);
        for (int c = 0; c < sw * sh; c++) {
            int sx = c % sw, sy = c / sw;
            rgbquad[c].b -= sx & sy;
        }
        BitBlt(hdc, 0, 0, sw, sh, hmem, 0, 0, SRCINVERT);
        BitBlt(hdc, rand () % 5, rand () % 5, sw, sh, hdc, rand () % 5, rand () % 5, SRCCOPY);
        Sleep(10);
    }
}

DWORD __stdcall Text(LPVOID lpvoid)
{
	HDC hdc;
	int sw, sh;
	HFONT hfnt;
	while (1) {
		hdc = GetDC(0);
		sw = GetSystemMetrics(0), sh = GetSystemMetrics(1);
		hfnt = CreateFontA(40, 20, rand () % 360, 0, FW_BOLD, true, false, false, ANSI_CHARSET, 0, 0, 0, 0, "Arial");
		LPCSTR text[] = {
		"bcdedit", "net", "xcopy", "fdisk", "mountvol", "timeout", "reg", "start", "tree"
		};
		SelectObject(hdc, hfnt);
		SetTextColor(hdc, GetRandomRainbowSpectrum());
		SetBkColor(hdc, RGB(0, 0, 0));
		int textCount = rand () % 9;
		TextOutA(hdc, rand () % sw, rand () % sh, text[textCount], strlen(text[textCount]));
		DeleteObject(hfnt);
		ReleaseDC(NULL, hdc);
		Sleep(10);
	}
}

DWORD __stdcall Blue(LPVOID lpvoid)
{
	HDC hdc;
	int sw, sh;
	HBRUSH hb;
	while (1) {
		hdc = GetDC(0);
		sw = GetSystemMetrics(0), sh = GetSystemMetrics(1);
		hb = CreateSolidBrush(RGB(22, 7, 117));
		SelectObject(hdc, hb);
		PatBlt(hdc, 0, 0, sw, sh, PATCOPY);
		DeleteObject(hb);
		ReleaseDC(NULL, hdc);
		Sleep(10);
	}
}

DWORD __stdcall CursorPayload(LPVOID lpvoid)
{
	HDC desk;
	POINT cur;
	HBRUSH hbr;
	while (true) {
		desk = GetDC(NULL);
		GetCursorPos(&cur);
		hbr = CreateSolidBrush(GetRandomRainbowSpectrum());
		SelectObject(desk, hbr);
		Ellipse(desk, cur.x + 50, cur.y + 50, cur.x - 50, cur.y - 50);
		DeleteObject(hbr);
		ReleaseDC(NULL, desk);
		Sleep(10);
	}
}

// sounds

void Sound1() {
	HWAVEOUT wave = 0;
	WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 18750, 18750, 1, 8, 0 };
	waveOutOpen(&wave, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
	char buffer[18750 * 30] = {};
	for (DWORD t = 0; t < sizeof(buffer); ++t)
		buffer[t] = static_cast<char>(t); // plain sawtooth (just time variable)

	WAVEHDR hdr = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
	waveOutPrepareHeader(wave, &hdr, sizeof(WAVEHDR));
	waveOutWrite(wave, &hdr, sizeof(WAVEHDR));
	waveOutUnprepareHeader(wave, &hdr, sizeof(WAVEHDR));
	waveOutClose(wave);
}

void Sound2() {
	HWAVEOUT wave = 0;
	WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 25000, 25000, 1, 8, 0 };
	waveOutOpen(&wave, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
	char buffer[25000 * 30] = {};
	for (DWORD t = 0; t < sizeof(buffer); ++t)
		buffer[t] = static_cast<char>((t^t&t>>12)-t); // modem

	WAVEHDR hdr = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
	waveOutPrepareHeader(wave, &hdr, sizeof(WAVEHDR));
	waveOutWrite(wave, &hdr, sizeof(WAVEHDR));
	waveOutUnprepareHeader(wave, &hdr, sizeof(WAVEHDR));
	waveOutClose(wave);
}

void Sound3() {
	HWAVEOUT wave = 0;
	WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 32000, 32000, 1, 8, 0 };
	waveOutOpen(&wave, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
	char buffer[32000 * 30] = {};
	for (DWORD t = 0; t < sizeof(buffer); ++t)
		buffer[t] = static_cast<char>((t%134|t%168)*2); // something i made with resonance

	WAVEHDR hdr = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
	waveOutPrepareHeader(wave, &hdr, sizeof(WAVEHDR));
	waveOutWrite(wave, &hdr, sizeof(WAVEHDR));
	waveOutUnprepareHeader(wave, &hdr, sizeof(WAVEHDR));
	waveOutClose(wave);
}


void Sound4() {
	HWAVEOUT wave = 0;
	WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 11020, 11020, 1, 8, 0 };
	waveOutOpen(&wave, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
	char buffer[11020 * 30] = {};
	for (DWORD t = 0; t < sizeof(buffer); ++t)
		buffer[t] = static_cast<char>(t^t*(t>>9|16)); // PZI_R5P92-type thing

	WAVEHDR hdr = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
	waveOutPrepareHeader(wave, &hdr, sizeof(WAVEHDR));
	waveOutWrite(wave, &hdr, sizeof(WAVEHDR));
	waveOutUnprepareHeader(wave, &hdr, sizeof(WAVEHDR));
	waveOutClose(wave);
}

void Beeping() {
	HWAVEOUT wave = 0;
	WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 14000, 14000, 1, 8, 0 };
	waveOutOpen(&wave, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
	char buffer[14000 * 30] = {};
	for (DWORD t = 0; t < sizeof(buffer); ++t)
		buffer[t] = static_cast<char>((t*16)&128); // beeeeeeeeeeeeeeeeeeeeeeeeeep

	WAVEHDR hdr = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
	waveOutPrepareHeader(wave, &hdr, sizeof(WAVEHDR));
	waveOutWrite(wave, &hdr, sizeof(WAVEHDR));
	waveOutUnprepareHeader(wave, &hdr, sizeof(WAVEHDR));
	waveOutClose(wave);
}
