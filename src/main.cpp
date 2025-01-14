#include <windows.h>
#include <shellapi.h>
#include <stdexcept>
#include "../include/resource.h"

#define ID_TRAY_ICON 1
#define WM_TRAY_ICON (WM_USER + 1)
#define CAPS_LOCK_ON_ICON L"capslock-icon-on.ico"
#define CAPS_LOCK_OFF_ICON L"capslock-icon-off.ico"

HINSTANCE hInstance;
NOTIFYICONDATA nid;
HICON onIcon;
HICON offIcon;

bool isCapslockOn();
void toggleCapslock();
void updateTrayIconTooltip();
HICON loadIconFromBitmap(HINSTANCE hInstance, int bitmapID);

// Window procedure to handle messages
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Main function
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	hInstance = hInst;

	onIcon = loadIconFromBitmap(hInst, IDB_BITMAP2);
	offIcon = loadIconFromBitmap(hInst, IDB_BITMAP1);
	if (!onIcon || !offIcon)
		return 1;

	// Register a window class
	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInst;
	wc.lpszClassName = L"CapsLockNotifier";
	wc.hIcon = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE));
	RegisterClass(&wc);

	// Create a hidden window
	HWND hwnd = CreateWindowEx(0, L"CapsLockNotifier", L"CapsLockNotifier", 0, 0, 0, 0, 0, NULL, NULL , hInst, NULL);

	// Set up the tray icon
	nid = {};
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hwnd;
	nid.uID = ID_TRAY_ICON;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = WM_TRAY_ICON;
	nid.hIcon = isCapslockOn() ? onIcon : offIcon;
	
	wcscpy_s(nid.szTip, L"Caps Lock is OFF");
	Shell_NotifyIcon(NIM_ADD, &nid);

	// Message loop
	MSG msg;
	while (true) {
		updateTrayIconTooltip(); // Update tooltip in real time
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) return 0;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		Sleep(100); // Small delay to avoid high CPU usage
	}
}

bool isCapslockOn() {
	return GetKeyState(VK_CAPITAL) & 0x0001;
}

// simulates pressing and releasing capslock
void toggleCapslock() {
	keybd_event(VK_CAPITAL, 0, KEYEVENTF_EXTENDEDKEY | 0, 0);
	keybd_event(VK_CAPITAL, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
	updateTrayIconTooltip();
}

void updateTrayIconTooltip() {
	if (isCapslockOn()) {
		wcscpy_s(nid.szTip, L"Caps Lock is ON");
		nid.hIcon = onIcon;
	}
	else {
		wcscpy_s(nid.szTip, L"Caps Lock is OFF");
		nid.hIcon = offIcon;
	}
	Shell_NotifyIcon(NIM_MODIFY, &nid);
}

// Loads bitmap from resources and converts it to an icon
HICON loadIconFromBitmap(HINSTANCE hInstance, int bitmapID) {
	// Load the bitmap from resources
	HBITMAP hBitmap = static_cast<HBITMAP>(LoadImage(hInstance, MAKEINTRESOURCE(bitmapID), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION));
	if (!hBitmap)
		return NULL;

	// Get bitmap dimensions
	BITMAP bm;
	GetObject(hBitmap, sizeof(bm), &bm);

	// Create a monochrome mask bitmap
	HBITMAP hMask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);
	if (!hMask) {
		DeleteObject(hBitmap);
		return NULL;
	}

	// Create an icon from the bitmap
	ICONINFO iconInfo = {};
	iconInfo.fIcon = TRUE;
	iconInfo.hbmMask = hMask;
	iconInfo.hbmColor = hBitmap;

	HICON hIcon = CreateIconIndirect(&iconInfo);

	// Clean up
	DeleteObject(hBitmap);
	DeleteObject(hMask);

	return hIcon;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_TRAY_ICON:
		switch (lParam) {
		case WM_LBUTTONUP:
			toggleCapslock();
			break;
		case WM_RBUTTONUP:
			// Create a popup menu
			HMENU hMenu = CreatePopupMenu();
			AppendMenu(hMenu, MF_STRING, 1, L"Exit");

			// Get the cursor position
			POINT pt;
			GetCursorPos(&pt);

			// Display the menu at the cursor position
			SetForegroundWindow(hwnd); // Required to keep the menu open
			int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, 0, hwnd, NULL);

			// Handle menu commands
			if (cmd == 1)
				PostQuitMessage(0); // Exit the application

			// Destroy the menu
			DestroyMenu(hMenu);
			break;
		}
		break;
	case WM_DESTROY:
		Shell_NotifyIcon(NIM_DELETE, &nid);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}