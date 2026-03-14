#include "Camera.h"

namespace AlbinoEngine
{
	Camera::Camera(std::string cameraName)
	{
		// Set camera name.
		this->m_pCameraName = cameraName;

		// float3's.
		this->m_pEye = {0.0f, 0.0f, -3.0f};
		this->m_pLookAt = {0.0f, 0.0f, 0.0f};
		this->m_pUp = {0.0f, 1.0f, 0.0f};

		// Perspective variables.
		this->m_pfov = 75.0f;
		this->m_pAspectRatio = 1.3333f;
		this->m_pZNear = 1.0f;
		this->m_pZFar = 1000.0f;

		this->m_pCameraRotation = { 0.0f, 0.0f, 0.0f };
	}

	Camera::~Camera()
	{

	}

	void Camera::setCameraEye(float xEye, float yEye, float zEye)
	{
		this->m_pEye = { xEye,yEye,zEye };
	}

	void Camera::setCameraLookAt(float xLookAt, float yLookAt, float zLookAt)
	{
		this->m_pLookAt = { xLookAt, yLookAt, zLookAt };
	}

	void Camera::setCameraUpVector(float xUp, float yUp, float zUp)
	{
		this->m_pUp = { xUp,yUp,zUp };
	}

	void Camera::setCameraFov(float fovOfViewDegrees)
	{
		m_pfov = fovOfViewDegrees;
	}

	void Camera::setCameraAspectRation(float aspectRatio)
	{
		m_pAspectRatio = aspectRatio;
	}

	void Camera::setCameraZNear(float zNear)
	{
		m_pZNear = zNear;
	}

	void Camera::setCameraZFar(float zFar)
	{
		m_pZFar = zFar;
	}

	DirectX::XMMATRIX Camera::getViewMatrix() const
	{
		DirectX::XMMATRIX world = getWorldMatrix();
		return DirectX::XMMatrixInverse(nullptr, world);
	}

	void Camera::setCameraPosition(float xPos, float yPos, float zPos)
	{
		m_pCameraPosition = { xPos, yPos, zPos };
	}

	void Camera::moveForward(float delta)
	{
		using namespace DirectX;

		XMVECTOR p = XMLoadFloat3(&m_pCameraPosition);
		p = XMVectorAdd(p, XMVectorScale(getForwardVector(), delta));
		XMStoreFloat3(&m_pCameraPosition, p);
	}

	void Camera::moveBackwards(float delta)
	{
		moveForward(-delta);
	}

	void Camera::moveLeft(float delta)
	{
		moveRight(-delta);
	}

	void Camera::moveRight(float delta)
	{
		using namespace DirectX;
		XMVECTOR p = XMLoadFloat3(&m_pCameraPosition);
		p = XMVectorAdd(p, XMVectorScale(getRightVector(), delta));
		XMStoreFloat3(&m_pCameraPosition, p);
	}

	void Camera::moveUp(float delta)
	{
		m_pCameraPosition.y += delta;
	}

	void Camera::moveDown(float delta)
	{
		m_pCameraPosition.y -= delta;
	}

	void Camera::rotateLeft(float delta)
	{
		m_pCameraRotation.y += delta;
	}

	void Camera::rotateRight(float delta)
	{
		m_pCameraRotation.y -= delta;
	}

	void Camera::rotateYaw(float radians)
	{
		m_pCameraRotation.y += radians;
	}

	void Camera::rotatePitch(float radians)
	{
		m_pCameraRotation.x += radians;

		// clamp pitch to avoid flipping (about +/- 89 degrees)
		const float limit = DirectX::XM_PIDIV2 - 0.01f;
		if (m_pCameraRotation.x > limit) m_pCameraRotation.x = limit;
		if (m_pCameraRotation.x < -limit) m_pCameraRotation.x = -limit;
	}

	void Camera::rotateYawPitch(float yawDelta, float pitchDelta)
	{
		rotateYaw(yawDelta);
		rotatePitch(pitchDelta);
	}
	DirectX::XMMATRIX Camera::getWorldMatrix() const
	{
		using namespace DirectX;

		XMMATRIX rot = XMMatrixRotationRollPitchYaw(
			m_pCameraRotation.x,  // pitch
			m_pCameraRotation.y,  // yaw
			m_pCameraRotation.z	  // roll
		);

		XMMATRIX trans = XMMatrixTranslation(
			m_pCameraPosition.x,
			m_pCameraPosition.y,
			m_pCameraPosition.z
		);

		return rot * trans;
	}

	DirectX::XMMATRIX Camera::getProjMatrix() const
	{
		return DirectX::XMMatrixPerspectiveFovLH(
			DirectX::XMConvertToRadians(this->m_pfov), 
			this->m_pAspectRatio, 
			this->m_pZNear, 
			this->m_pZFar);
	}

	DirectX::XMVECTOR Camera::getForwardVector() const
	{
		using namespace DirectX;

		XMVECTOR forward = XMVectorSet(0, 0, 1, 0);
		XMMATRIX rot = XMMatrixRotationRollPitchYaw(
			m_pCameraRotation.x,
			m_pCameraRotation.y,
			m_pCameraRotation.z);

		return XMVector3Normalize(XMVector3TransformNormal(forward, rot));
	}

	DirectX::XMVECTOR Camera::getRightVector() const
	{
		using namespace DirectX;

		XMVECTOR right = XMVectorSet(1, 0, 0, 0);
		XMMATRIX rot = XMMatrixRotationRollPitchYaw(
			m_pCameraRotation.x, m_pCameraRotation.y, m_pCameraRotation.z);
		return XMVector3Normalize(XMVector3TransformNormal(right, rot));
	}

	DirectX::XMMATRIX Camera::getViewProjMatrix() const
	{
		return getViewMatrix() * getProjMatrix();
	}
}
