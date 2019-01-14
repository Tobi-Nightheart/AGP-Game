#pragma once
class Skybox
{
public:
	Skybox(ID3D11Device* device, ID3D11DeviceContext* context);
	HRESULT Initialize();
	void Draw(XMMATRIX* view, XMMATRIX* projection, camera* cam);
	~Skybox();
};

