#pragma once
#include <d3d11.h>
#include <math.h>
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>


class camera
{
public:
	camera();
	camera(float x, float y, float z, float camera_rotationY, float camera_rotationX);
	void Rotate(float camera_rotationY, float camera_rotationX);
	void Forward(float distance);
	void Up(float up);
	void Strafe(float distance);
	void Jump(float distance);
	XMVECTOR GetOrthogonal();
	XMMATRIX GetViewMatrix();
	XMVECTOR GetCameraPosition();
	~camera();
};

