#pragma once
#include "objfilemodel.h"

class Model
{
private:
	void CalculateModelCenterPoint();
	void CalculateBoundingSphereRadius();

public:
	Model(ID3D11Device* d3d11Device, ID3D11DeviceContext* d3d11DeviceContext);
	~Model();
	HRESULT LoadObjModel(char* filename);
	void Draw(XMMATRIX * view, XMMATRIX * projection);
	XMVECTOR GetBoundingSphereWorldSpacePosition();
	bool CheckCollision(Model* model);
	void IncX(float num);
	void IncY(float num);
	void IncZ(float num);
	void IncXAngle(float num);
	void IncYAngle(float num);
	void IncZAngle(float num);
	void IncScale(float num);
	void SetX(float x);
	void SetY(float y);
	void SetZ(float z);
	void SetXAngle(float xAngle);
	void SetYAngle(float yAngle);
	void SetZAngle(float zAngle);
	void SetScale(float scale);
	float GetX();
	float GetY();
	float GetZ();
	float GetXAngle();
	float GetYAngle();
	float GetZAngle();
	float GetScale();
	float GetBoundingSphereRadius();
};

