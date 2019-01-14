#include "camera.h"

//m_camera_rotation in degrees
float m_x, m_y, m_z, m_dx, m_dy, m_dz, m_camera_rotationY, m_camera_rotationX;
XMVECTOR m_position, m_lookat, m_up;

camera::camera()
{
}

camera::camera(float x, float y, float z, float camera_rotationY, float camera_rotationX)
{
	m_x = x;
	m_y = y;
	m_z = z;
	m_camera_rotationY = camera_rotationY;
	m_camera_rotationX = camera_rotationX;
	m_dx = sin(m_camera_rotationY*(XM_PI / 180));
	m_dy = sin(m_camera_rotationX*(XM_PI / 180));
	m_dz = cos(m_camera_rotationY*(XM_PI / 180));
}

//positive values rotate to the right for rotationY and up for rotationX
void camera::Rotate(float camera_rotationY, float camera_rotationX)
{
	if(-90<m_camera_rotationX+ camera_rotationX<90)
	m_camera_rotationX += camera_rotationX;
	m_camera_rotationY += camera_rotationY;
	m_dx = sin(m_camera_rotationY*(XM_PI/180));
	m_dy = sin(m_camera_rotationX*(XM_PI / 180));
	m_dz = cos(m_camera_rotationY*(XM_PI / 180));
}

//postitive values move the camera forward
void camera::Forward(float distance)
{
	m_x -= distance*m_lookat.x;
	m_z -= distance*m_lookat.z;
}


void camera::Up(float up)
{
	m_y = up;
}

void camera::Strafe(float distance)
{
	XMVECTOR orth = GetOrthogonal();
	m_x += distance*orth.x;
	m_z += distance*orth.y;
}

void camera::Jump(float distance)
{
	m_position.y += distance;
}

XMVECTOR camera::GetOrthogonal()
{
	return XMVector3Cross(m_lookat, m_up);
}

XMMATRIX camera::GetViewMatrix()
{
	XMMATRIX view;
	m_position = XMVectorSet(m_x, m_y, m_z, 0.0f);
	m_lookat = XMVectorSet(m_x + m_dx, m_y + m_dy, m_z+ m_dz,0.0f);
	m_up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	view = XMMatrixLookAtLH(m_position, m_lookat, m_up);
	return view;
}

XMVECTOR camera::GetCameraPosition()
{
	return m_position;
}


camera::~camera()
{
}
