#pragma once

#include <windows.h>
#include <tchar.h>
#include <sstream>
#include <d3d11.h>


#include"sprite.h"
#include"Blend.h"
#include "misc.h"
#include "high_resolution_timer.h"
#include "geometric_primitive.h"
#include "skinned_mesh.h"


class framework
{
public:

	CONST HWND hwnd;
	static CONST LONG SCREEN_WIDTH = 1280;
	static CONST LONG SCREEN_HEIGHT = 720;

	
	ID3D11Device*            Device;
	ID3D11DeviceContext*     Context;
	IDXGISwapChain*          SwapChain;
	ID3D11RenderTargetView*  RenderTargetView;
	ID3D11DepthStencilView*  DepthStencilView;

	ID3D11Texture2D* DepthStencil = NULL;

	sprite*sprites[2];

	sprite*particle;

	geometric_primitive* cube;
	geometric_primitive* cylinder;

	skinned_mesh* sdk;


	framework(HWND hwnd) : hwnd(hwnd),
		Device(NULL), Context(NULL), SwapChain(NULL),
		RenderTargetView(NULL), DepthStencilView(NULL)
	{
		
	}
	~framework();
	int run()
	{
		MSG msg = {0};

		if (!initialize( hwnd)) return 0;
		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				timer.tick();
				calculate_frame_stats();
				update(timer.time_interval());
				render(timer.time_interval());
			}
		}
		return static_cast<int>(msg.wParam);
	}

	LRESULT CALLBACK handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		switch (msg)
		{
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
			hdc = BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
			break;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CREATE:
			break;
		case WM_KEYDOWN:
			if (wparam == VK_ESCAPE) PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		case WM_ENTERSIZEMOVE:
			// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
			timer.stop();
			break;
		case WM_EXITSIZEMOVE:
			// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
			// Here we reset everything based on the new window dimensions.
			timer.start();
			break;
		default:
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}
		return 0;

	}

private:
	bool initialize(HWND hwnd);
	void update(float elapsed_time/*Elapsed seconds from last frame*/);
	void render(float elapsed_time/*Elapsed seconds from last frame*/);

private:
	high_resolution_timer timer;
	void calculate_frame_stats()
	{
		// Code computes the average frames per second, and also the 
		// average time it takes to render one frame.  These stats 
		// are appended to the window caption bar.
		static int frames = 0;
		static float time_tlapsed = 0.0f;

		frames++;

		// Compute averages over one second period.
		if ((timer.time_stamp() - time_tlapsed) >= 1.0f)
		{
			float fps = static_cast<float>(frames); // fps = frameCnt / 1
			float mspf = 1000.0f / fps;
			std::ostringstream outs;
			outs.precision(6);
			outs << "FPS: " << fps << " / " << "Frame Time : " << mspf << " (ms)" << "/" << "BlendMode :" << ""<< "/" << "SrcAlpha:" << "";
			SetWindowTextA(hwnd, outs.str().c_str());

			// Reset for next average.
			frames = 0;
			time_tlapsed += 1.0f;
		}

	}

	
};

