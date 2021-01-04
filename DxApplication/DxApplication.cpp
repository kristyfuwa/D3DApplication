#include "DxApplication.h"
#include <QtCore/QTimer>

DxApplication::DxApplication(QWidget *parent)
    : QMainWindow(parent),
	m_eDriverType(D3D_DRIVER_TYPE_NULL),
	m_eFeatureLevel(D3D_FEATURE_LEVEL_11_0),
	m_pD3dDevice(nullptr),
	m_pImmediateContext(nullptr),
	m_pSwapChain(nullptr),
	m_pRenderTargetView(nullptr)
{
	m_pTimer = new QTimer(this);
	connect(m_pTimer, &QTimer::timeout, this, &DxApplication::OnTimer);
	m_pTimer->start(0);
}

DxApplication::~DxApplication()
{
	CleanupDevice();
}

bool DxApplication::Init()
{
	if (!InitD3D())
		return false;
	return true;
}

bool DxApplication::InitD3D()
{
	HRESULT hr = S_OK;

	HWND hWnd = (HWND)winId();
	RECT rcWnd;
	GetWindowRect(hWnd, &rcWnd);

	int width = rcWnd.right-rcWnd.left;
	int height = rcWnd.bottom-rcWnd.top;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[]
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};

	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		m_eDriverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, m_eDriverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &m_pSwapChain, &m_pD3dDevice, &m_eFeatureLevel, &m_pImmediateContext);
		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
		return hr;

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = NULL;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
		return hr;

	hr = m_pD3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr))
		return hr;

	m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_pImmediateContext->RSSetViewports(1, &vp);

	return S_OK;
}

void DxApplication::Render()
{
	// Just clear the backbuffer 
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; //red,green,blue,alpha
	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, ClearColor);
	m_pSwapChain->Present(0, 0);
}

void DxApplication::CleanupDevice()
{
	if (m_pImmediateContext)
		m_pImmediateContext->ClearState();
	if (m_pRenderTargetView)
		m_pRenderTargetView->Release();
	if (m_pSwapChain) 
		m_pSwapChain->Release();
	if (m_pD3dDevice) 
		m_pD3dDevice->Release();
}

void DxApplication::OnTimer()
{
	Render();
}
