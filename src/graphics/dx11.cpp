#include "../../src/pch.h"
#include "dx11.h"

#include <spdlog/spdlog.h>

#include <d3d11.h>
#include <dxgi1_3.h>
#ifdef _DEBUG
#include <dxgidebug.h>
#endif

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

#include "plwindow.h"

#define info(...) spdlog::get("GRAPHICS")->info(__VA_ARGS__)
#define warn(...) spdlog::get("GRAPHICS")->warn(__VA_ARGS__)
#define error(...) spdlog::get("GRAPHICS")->error(__VA_ARGS__)
#define critical(...) spdlog::get("GRAPHICS")->critical(__VA_ARGS__)

#define warn_errpr(...) { warn(__VA_ARGS__); print_err(ret); }
#define error_errpr(...) { error(__VA_ARGS__); print_err(ret); }
#define critical_errpr(...) { critical(__VA_ARGS__); print_err(ret); }

#define set_name(obj, name) \
	ret = obj->SetPrivateData({0x429b8c22,0x9188,0x4b0c,0x87,0x42,0xac,0xb0,0xbf,0x85,0xc2,0x00}/*WKPDID_D3DDebugObjectName*/, sizeof(name) - 1, name); \
	if (ret < 0) \
		warn("failed to set name for: {} because: {}", name, ret)

#define release(com) \
	if (com != nullptr) \
	{ \
		com->Release(); \
		com = nullptr; \
	}

void print_err(HRESULT res)
{
	if (FACILITY_WINDOWS == HRESULT_FACILITY(res))
		res = HRESULT_CODE(res);
	char errMsg[512];
	if (FormatMessageA(
		FORMAT_MESSAGE_FROM_SYSTEM,
		nullptr, res, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT) /*TODO: use GetLocaleInfoEx*/, errMsg, 512, nullptr)) {
		error(errMsg);
	}
	else
		error("failed to get a description for error: {}", GetLastError());
}

struct dx11::local
{
	IDXGIFactory2* factory = nullptr;
	ID3D11Device* device = nullptr;

	ID3D11DeviceContext* immediate = nullptr;

	IDXGISwapChain1* swapchain = nullptr;
};

struct dx11_window_buffers : public window_buffers
{
	ID3D11Texture2D* texture = nullptr;
	ID3D11RenderTargetView* renderTarget = nullptr;

	~dx11_window_buffers() override
	{
		release(renderTarget);
		release(texture);
	}
};

struct dx11_command_queue : public command_queue
{
	ID3D11DeviceContext* context = nullptr;

	~dx11_command_queue() override
	{
		release(context);
	}
};

void dx11::init(window* win)
{
	m_local = new local();

	void* hwnd = plwindow::win32_get_hwnd(win);
	if (hwnd == nullptr)
	{
		error("window handle is null!");
		return;
	}

	HRESULT ret = 0;

	uint32_t factoryFlags = 0;
	uint32_t deviceFlags = 0;
#ifdef _WIN32
	factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	ret = CreateDXGIFactory2(factoryFlags, __uuidof(IDXGIFactory2), (void**)&m_local->factory);
	if (ret < 0)
		critical_errpr("failed to create dxgi factory: {}", ret);
	set_name(m_local->factory, "core_factory");

	ret = D3D11CreateDevice(nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		deviceFlags,
		nullptr, 0,
		D3D11_SDK_VERSION,
		&m_local->device,
		nullptr, &m_local->immediate);
	if (ret < 0)
		critical_errpr("failed to create d3d11 device: {}", ret);
	set_name(m_local->device, "core_device");

	DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
	swapchainDesc.Width = 800;
	swapchainDesc.Height = 600;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.SampleDesc = { 1, 0 };
	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchainDesc.BufferCount = 2;
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

	ret = m_local->factory->CreateSwapChainForHwnd(m_local->device, (HWND)hwnd, &swapchainDesc, nullptr, nullptr, &m_local->swapchain);
	if (ret < 0)
		critical_errpr("failed to create swapchain for window: {}", ret);
	set_name(m_local->swapchain, "core_swapChain");
}

void dx11::kill()
{
	release(m_local->swapchain);
	release(m_local->immediate);
#ifdef _DEBUG
	if (m_local->device != nullptr)
	{
		ID3D11Debug* _debug = nullptr;
		HRESULT ret = m_local->device->QueryInterface(&_debug);
		if (_debug != nullptr)
		{
			ret = _debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
			if (ret < 0)
				error("failed to report live d3d11 objects: {}", ret);
		}
		else if (ret < 0)
			warn("failed to get d3d11 debug interface");
		release(_debug);
	}
	if (m_local->factory != nullptr)
	{
		IDXGIDebug* _debug = nullptr;
		HRESULT ret = DXGIGetDebugInterface1(0, __uuidof(IDXGIDebug), (void**)&_debug);
		if (_debug != nullptr)
		{
			ret = _debug->ReportLiveObjects({ 0x25cddaa4, 0xb1c6, 0x47e1, 0xac, 0x3e, 0x98, 0x87, 0x5b, 0x5a, 0x2e, 0x2a }/*DXGI_DEBUG_DXGI*/, DXGI_DEBUG_RLO_DETAIL);
			if (ret < 0)
				error("failed to report live dxgi objects: {}", ret);
		}
		else if (ret < 0)
			warn("failed to get dxgi debug interface");
		release(_debug);
	}
#endif
	release(m_local->device);
	release(m_local->factory);

	delete m_local;
	m_local = nullptr;
}

void dx11::present()
{
	HRESULT ret = m_local->swapchain->Present(0, 0);
	if (ret < 0) warn_errpr("failed to present swapchain!");
}

command_queue* dx11::create_cmd_queue()
{
	dx11_command_queue* n = new dx11_command_queue();

	HRESULT ret = 0;

	ret = m_local->device->CreateDeferredContext(0, &n->context);
	if (ret < 0) { error_errpr("failed to create command queue!") return n; };

	return n;
}

void dx11::clear_cmd_queue(command_queue* queue)
{
	dx11_command_queue* cmd = (dx11_command_queue*)queue;
	if (cmd->context == nullptr) return;
	cmd->context->ClearState();
}

void dx11::execute_cmd_queues(command_queue* queue[], int count)
{
	for (size_t i = 0; i < count; i++)
	{
		dx11_command_queue* cmd = (dx11_command_queue*)queue[i];
		if (cmd->context == nullptr) continue;
		ID3D11CommandList* cmdls = nullptr;
		HRESULT ret = cmd->context->FinishCommandList(true, &cmdls);
		if (ret < 0 || cmdls == nullptr)
			warn_errpr("failed to finish command list on queue: {}", i)
		else
			m_local->immediate->ExecuteCommandList(cmdls, false);
		release(cmdls);
	}
}

window_buffers* dx11::create()
{
	dx11_window_buffers* n = new dx11_window_buffers();

	HRESULT ret = 0;

	ret = m_local->swapchain->GetBuffer(0, _uuidof(ID3D11Texture2D), (void**)&n->texture);
	if (ret < 0) { error_errpr("failed to get backbuffer of swapchain for window buffers!"); return n; }
	set_name(n->texture, "wb_backBufferT2D");

	ret = m_local->device->CreateRenderTargetView(n->texture, nullptr, &n->renderTarget);
	if (ret < 0) { error_errpr("failed to create render target view for backbuffer texture!"); return n; }
	set_name(n->renderTarget, "wb_backBufferRTV");

	return n;
}

void dx11::clear_win_buffers(command_queue* queue, window_buffers* bufs, float rgba[4])
{
	dx11_command_queue* cmd = (dx11_command_queue*)queue;
	dx11_window_buffers* win = (dx11_window_buffers*)bufs;
	if (cmd->context == nullptr) return;
	if (win->renderTarget == nullptr) return;
	cmd->context->OMSetRenderTargets(1, &win->renderTarget, nullptr);
	cmd->context->ClearRenderTargetView(win->renderTarget, rgba);
}
