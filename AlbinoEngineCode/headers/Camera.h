#pragma once
#include <DirectXMath.h>
#include <string>

namespace AlbinoEngine
{
	class Camera
	{
	public:
		Camera(std::string cameraName);

		~Camera();

		void setCameraEye(float xEye, float yEye, float zEye);

		void setCameraLookAt(float xLookAt, float yLookAt, float zLookAt);

		void setCameraUpVector(float xUp, float yUp, float zUp);

		void setCameraFov(float fovOfViewDegrees);

		void setCameraAspectRation(float aspectRatio);

		void setCameraZNear(float zNear);

		void setCameraZFar(float zFar);

		void setCameraPosition(float xPos, float yPos, float zPos);

		void moveLeft(float delta);

		void moveRight(float delta);

		void moveForward(float delta);

		void moveBackwards(float delta);

		void moveUp(float delta);

		void moveDown(float delta);

		void rotateLeft(float delta);

		void rotateRight(float delta);


		void rotateYaw(float radians);
		void rotatePitch(float radians);
		void rotateYawPitch(float yawDelta, float pitchDelta);

		DirectX::XMVECTOR getForwardVector() const;
		DirectX::XMVECTOR getRightVector() const;
		

		DirectX::XMMATRIX getViewMatrix() const;

		DirectX::XMMATRIX getProjMatrix() const;

		DirectX::XMMATRIX getWorldMatrix() const;

		DirectX::XMMATRIX getViewProjMatrix() const;

		const DirectX::XMFLOAT3& getCameraPosition() const { return m_pCameraPosition; }
	protected:
		std::string m_pCameraName;
		// view variables.
		DirectX::XMFLOAT3 m_pEye;	  // The position of the camera in view space.
		DirectX::XMFLOAT3 m_pLookAt;  // What the camera is looking at.
		DirectX::XMFLOAT3 m_pUp; // The up-vector.

		DirectX::XMFLOAT3 m_pCameraPosition;
		DirectX::XMFLOAT3 m_pCameraRotation;
		// world transform vectors.
		float m_pfov;			
		float m_pAspectRatio;
		float m_pZNear;
		float m_pZFar;
	};
}