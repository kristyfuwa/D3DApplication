#pragma once

#include <QtWidgets/QMainWindow>
#include <d3dx11.h>
#include <d3d11.h>

class QTimer;

class DxApplication : public QMainWindow
{
    Q_OBJECT

public:
    DxApplication(QWidget *parent = Q_NULLPTR);
	~DxApplication();

	bool Init();
	bool InitD3D();
	void Render();
	void CleanupDevice();

private slots:
	void OnTimer();

private:

	QTimer*						m_pTimer;

	D3D_DRIVER_TYPE				m_eDriverType;
	D3D_FEATURE_LEVEL			m_eFeatureLevel;

	ID3D11Device*				m_pD3dDevice;
	ID3D11DeviceContext*		m_pImmediateContext;
	IDXGISwapChain*				m_pSwapChain;
	ID3D11RenderTargetView*		m_pRenderTargetView;
};
