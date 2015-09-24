#pragma comment(lib,"dwmapi")
#include <windows.h>
#define _USE_MATH_DEFINES
#include <math.h>

TCHAR szClassName[] = TEXT("Window");

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HFONT hFont;
	static HPEN hSecHandPen;
	static HPEN hMinHandPen;
	static HPEN hHourHandPen;
	switch (msg)
	{
	case WM_CREATE:
		hFont = CreateFont(32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, TEXT("Consolas"));
		hSecHandPen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_WINDOWTEXT));
		hMinHandPen = CreatePen(PS_SOLID, 4, GetSysColor(COLOR_WINDOWTEXT));
		hHourHandPen = CreatePen(PS_SOLID, 8, GetSysColor(COLOR_WINDOWTEXT));
		SetTimer(hWnd, 0x1234, 1000, 0);
		break;
	case WM_TIMER:
		InvalidateRect(hWnd, 0, 1);
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			const HDC hdc = BeginPaint(hWnd, &ps);
			RECT rect1, rect2;
			GetClientRect(hWnd, &rect1);
			SYSTEMTIME st;
			GetLocalTime(&st);
	
			const double dSecAngle = 2.0 * M_PI * st.wSecond / 60.0;
			const double dMinAngle = 2.0 * M_PI * (st.wMinute + st.wSecond / 60.0) / 60.0;
			const double dHourAngle = 2.0 * M_PI * (st.wHour + st.wMinute / 60.0) / 12.0;
			const double r = ((rect1.right>rect1.bottom) ? rect1.bottom : rect1.right) / 2.0 - 50.0;
			const double dSecHandLength = 0.9 * r;
			const double dMinHandLength = 0.8 * r;
			const double dHourHandLength = 0.7 * r;
	
			const HPEN hOldPen = (HPEN)SelectObject(hdc, hSecHandPen);
			MoveToEx(hdc, rect1.right / 2, rect1.bottom / 2, NULL);
			LineTo(hdc, (int)(rect1.right / 2.0 + dSecHandLength*sin(dSecAngle)), (int)(rect1.bottom / 2.0 - dSecHandLength*cos(dSecAngle)));
	
			SelectObject(hdc, hMinHandPen);
			MoveToEx(hdc, rect1.right / 2, rect1.bottom / 2, NULL);
			LineTo(hdc, (int)(rect1.right / 2.0 + dMinHandLength*sin(dMinAngle)), (int)(rect1.bottom / 2.0 - dMinHandLength*cos(dMinAngle)));
	
			SelectObject(hdc, hHourHandPen);
			MoveToEx(hdc, rect1.right / 2, rect1.bottom / 2, NULL);
			LineTo(hdc, (int)(rect1.right / 2.0 + dHourHandLength*sin(dHourAngle)), (int)(rect1.bottom / 2.0 - dHourHandLength*cos(dHourAngle)));
	
			SelectObject(hdc, hOldPen);
	
			const HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
			TCHAR szMoji[3];
			for (int i = 0; i < 60; ++i)
			{
				rect2.left = (LONG)(rect1.right / 2.0 + r * sin(2.0 * M_PI * i / 60.0));
				rect2.top = (LONG)(rect1.bottom / 2.0 - r * cos(2.0 * M_PI * i / 60.0));
				if (i % 5)
				{
					rect2.right = rect2.left + 4;
					rect2.bottom = rect2.top + 4;
					OffsetRect((LPRECT)&rect2, -2, -2);
					Ellipse(hdc, rect2.left, rect2.top, rect2.right, rect2.bottom);
				}
				else
				{
					rect2.right = rect2.left + rect1.right;
					rect2.bottom = rect2.top + rect1.bottom;
					OffsetRect((LPRECT)&rect2, -rect1.right / 2, -rect1.bottom / 2);
					wsprintf(szMoji, TEXT("%d"), i? i / 5 : 12);
					SetBkMode(hdc, TRANSPARENT);
					DrawText(hdc, szMoji, -1, &rect2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				}
			}
			SelectObject(hdc, hOldFont);
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_DESTROY:
		KillTimer(hWnd, 0x1234);
		DeleteObject(hFont);
		DeleteObject(hSecHandPen);
		DeleteObject(hMinHandPen);
		DeleteObject(hHourHandPen);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("Analog Clock"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
		);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
