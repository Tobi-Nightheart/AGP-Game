#pragma once
#include "pch.h"
#include "Model.h"

struct MODEL_CONSTANT_BUFFER {
	XMMATRIX WorldViewProjection; //64 bytes
};

ID3D11Device* m_pD3DDevice;
ID3D11DeviceContext*  m_pImmediateContext;

ObjFileModel* m_pObject;
ID3D11VertexShader* m_pVShader;
ID3D11PixelShader*  m_pPShader;
ID3D11InputLayout* m_pInputLayout;
ID3D11Buffer* m_pConstantBuffer;
ID3D11ShaderResourceView* m_pTexture0;
ID3D11SamplerState* m_pSampler0;

//Collision 
float m_bounding_sphere_center_x;
float m_bounding_sphere_center_y;
float m_bounding_sphere_center_z;
float m_bounding_sphere_radius;


float m_px, m_py, m_pz;
float m_xAngle, m_yAngle, m_zAngle;
float m_Scale;



Model::Model(ID3D11Device* d3d11Device, ID3D11DeviceContext* d3d11DeviceContext)
{
	m_px = 0.0f;
	m_py = 0.0f;
	m_pz = 0.0f;
	m_xAngle = 0.0f;
	m_yAngle = 0.0f;
	m_zAngle = 0.0f;
	m_Scale = 1.0f;
	m_pD3DDevice = d3d11Device;
	m_pImmediateContext = d3d11DeviceContext;
}

HRESULT Model::LoadObjModel(char* filename) {
	m_pObject = new ObjFileModel(filename, m_pD3DDevice, m_pImmediateContext);
	if (m_pObject->filename == "FILE NOT LOADED") { return S_FALSE; }

	HRESULT hr = S_OK;

	D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice, "assets/texture0.jpg", NULL, NULL, &m_pTexture0, NULL);
	
	//load and compile shaders
	ID3DBlob *VS, *PS, *error;
	hr = D3DX11CompileFromFile("model_shaders.hlsl", 0, 0, "ModelVS", "vs_5_0", 0, 0, 0, &VS, &error, 0);
	if (error != 0)
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))
			return hr;
	}

	hr = D3DX11CompileFromFile("model_shaders.hlsl", 0, 0, "ModelPS", "ps_5_0", 0, 0, 0, &PS, &error, 0);
	if (error != 0)
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))
			return hr;
	}

	//create shader objects
	hr = m_pD3DDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &m_pVShader);
	if (FAILED(hr)) return hr;
	hr = m_pD3DDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &m_pPShader);
	if (FAILED(hr)) return hr;

	//set shader active
	m_pImmediateContext->VSSetShader(m_pVShader, 0, 0);
	m_pImmediateContext->PSSetShader(m_pPShader, 0, 0);

	D3D11_INPUT_ELEMENT_DESC iedesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}

	};

	hr = m_pD3DDevice->CreateInputLayout(iedesc, ARRAYSIZE(iedesc), VS->GetBufferPointer(), VS->GetBufferSize(), &m_pInputLayout);
	if (FAILED(hr))
		return hr;
	m_pImmediateContext->IASetInputLayout(m_pInputLayout);

	D3D11_BUFFER_DESC constant_buffer_desc;
	ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));

	constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	constant_buffer_desc.ByteWidth = 64;
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	hr = m_pD3DDevice->CreateBuffer(&constant_buffer_desc, NULL, &m_pConstantBuffer);
	CalculateModelCenterPoint();
	CalculateBoundingSphereRadius();
	if (FAILED(hr)) return hr;

	D3D11_SAMPLER_DESC sampler_desc;
	ZeroMemory(&sampler_desc, sizeof(sampler_desc));
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

	m_pD3DDevice->CreateSamplerState(&sampler_desc, &m_pSampler0);



	
	return hr;
	
}

void Model::Draw(XMMATRIX* view, XMMATRIX* projection) {
	XMMATRIX world;
	world = XMMatrixScaling(m_Scale, m_Scale, m_Scale);
	world = XMMatrixRotationZ(m_zAngle);
	world = XMMatrixRotationY(m_yAngle);
	world = XMMatrixRotationX(m_xAngle);
	world = XMMatrixTranslation(m_px, m_py, m_pz);

	MODEL_CONSTANT_BUFFER model_cb_values;
	model_cb_values.WorldViewProjection = world * (*view) * (*projection);

	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, 0, &model_cb_values, 0, 0);
	

	//Setting the vs, ps, and input layout
	m_pImmediateContext->VSSetShader(m_pVShader, 0, 0);
	m_pImmediateContext->PSSetShader(m_pPShader, 0, 0);
	m_pImmediateContext->IASetInputLayout(m_pInputLayout);
	m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTexture0);
	m_pImmediateContext->PSSetSamplers(0, 1, &m_pSampler0);

	m_pObject->Draw();
}

XMVECTOR Model::GetBoundingSphereWorldSpacePosition()
{
	XMMATRIX world;
	world = XMMatrixScaling(m_Scale, m_Scale, m_Scale);
	world = XMMatrixRotationRollPitchYaw(m_xAngle, m_yAngle, m_zAngle);
	world = XMMatrixTranslation(m_px, m_py, m_pz);

	XMVECTOR offset = XMVectorSet(m_bounding_sphere_center_x, m_bounding_sphere_center_y, m_bounding_sphere_center_z, 0.0f);
	offset = XMVector3Transform(offset, world);
	return offset;
}

bool Model::CheckCollision(Model * model)
{
	//check if they are the same model
	if(model == this)
	return false;

	XMVECTOR homeModel = GetBoundingSphereWorldSpacePosition();
	XMVECTOR foreignModel = model->GetBoundingSphereWorldSpacePosition();

	float homeX, homeY, homeZ, foreignX, foreignY, foreignZ;
	homeX = XMVectorGetX(homeModel);
	homeY = XMVectorGetY(homeModel);
	homeZ = XMVectorGetZ(homeModel);
	foreignX = XMVectorGetX(foreignModel);
	foreignY = XMVectorGetY(foreignModel);
	foreignZ = XMVectorGetZ(foreignModel);

	float distsquared = pow(homeX - foreignX, 2)+ pow(homeY - foreignY, 2)+ pow(homeZ - foreignZ, 2);
	float collisiondist = pow(this->GetBoundingSphereRadius(), 2) + pow(model->GetBoundingSphereRadius(), 2);
	if (distsquared <= collisiondist) {
		return true;
	}
	else return false;

}

void Model::CalculateModelCenterPoint()
{
	float min_x = 0;
	float min_y = 0;
	float min_z = 0;
	float max_x = 0;
	float max_y = 0;
	float max_z = 0;
	for (int i = 0; i < m_pObject->numverts; i++) {
		if (min_x > m_pObject->vertices[i].Pos.x)
			min_x = m_pObject->vertices[i].Pos.x;
		if (min_y > m_pObject->vertices[i].Pos.x)
			min_y = m_pObject->vertices[i].Pos.x;
		if (min_z > m_pObject->vertices[i].Pos.x)
			min_z = m_pObject->vertices[i].Pos.x;
		if (max_x < m_pObject->vertices[i].Pos.x)
			max_x = m_pObject->vertices[i].Pos.x;
		if (max_y < m_pObject->vertices[i].Pos.x)
			max_y = m_pObject->vertices[i].Pos.x;
		if (max_z < m_pObject->vertices[i].Pos.x)
			max_z = m_pObject->vertices[i].Pos.x;
	}
	m_bounding_sphere_center_x = (min_x + max_x) / 2;
	m_bounding_sphere_center_y = (min_y + max_y) / 2;
	m_bounding_sphere_center_z = (min_z + max_z) / 2;
}

void Model::CalculateBoundingSphereRadius()
{
	float maxdist = 0.0f;
	float curr;
	for (int i = 0; i < m_pObject->numverts; i++) {
		curr = pow(m_bounding_sphere_center_x - m_pObject->vertices[i].Pos.x, 2) + pow(m_bounding_sphere_center_y - m_pObject->vertices[i].Pos.y, 2) + pow(m_bounding_sphere_center_z - m_pObject->vertices[i].Pos.z, 2);
		if (curr > maxdist) maxdist = curr;
	}
	m_bounding_sphere_radius = sqrt(maxdist);
}

void Model::IncX(float num)
{
	m_px += num;
}

void Model::IncY(float num)
{
	m_py += num;
}

void Model::IncZ(float num)
{
	m_pz += num;
}

void Model::IncXAngle(float num)
{
	m_xAngle += num;
}

void Model::IncYAngle(float num)
{
	m_yAngle += num;
}

void Model::IncZAngle(float num)
{
	m_zAngle += num;
}

void Model::IncScale(float num)
{
	m_Scale += num;
}

void Model::SetX(float x)
{
	m_px = x;
}

void Model::SetY(float y)
{
	m_py = y;
}

void Model::SetZ(float z)
{
	m_pz = z;
}

void Model::SetXAngle(float xAngle)
{
	m_xAngle = xAngle;
}

void Model::SetYAngle(float yAngle)
{
	m_yAngle = yAngle;
}

void Model::SetZAngle(float zAngle)
{
	m_zAngle = zAngle;
}

void Model::SetScale(float scale)
{
	m_Scale = scale;
}

float Model::GetX()
{
	return m_px;
}

float Model::GetY()
{
	return m_py;
}

float Model::GetZ()
{
	return m_pz;
}

float Model::GetXAngle()
{
	return m_xAngle;
}

float Model::GetYAngle()
{
	return m_yAngle;
}

float Model::GetZAngle()
{
	return m_zAngle;
}

float Model::GetScale()
{
	return m_Scale;
}

float Model::GetBoundingSphereRadius()
{
	return m_bounding_sphere_radius * m_Scale;
}



Model::~Model()
{
	if (m_pTexture0) m_pTexture0->Release();
	delete m_pObject;
}
