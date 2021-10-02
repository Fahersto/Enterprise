#include "EP_PCH.h"
#include "Graphics.h"

using Enterprise::Graphics;

std::stack<Graphics::perCameraGlobalUBStruct> Graphics::perCameraGlobalUBDataStack;

void Graphics::PushCamera()
{
	perCameraGlobalUBDataStack.push(perCameraGlobalUBDataStack.top());
}

void Graphics::PushCamera(Math::Vec3 cameraPos, Math::Vec3 cameraRot, Math::Mat4 projectionMat)
{
	Math::Mat4 viewMat = Math::Mat4::Rotation(-cameraRot) * Math::Mat4::Translation(-cameraPos);
	perCameraGlobalUBDataStack.emplace
	(
		viewMat,
		projectionMat,
		projectionMat * viewMat,
		cameraPos
	);
	SetUniformBufferData(perCameraGlobalUB, &perCameraGlobalUBDataStack);
}

void Graphics::SetCamera(Math::Vec3 cameraPos, Math::Vec3 cameraRot, Math::Mat4 projectionMat)
{
	EP_ASSERT(perCameraGlobalUBDataStack.size() > 1); // 0th element is reserved for null camera (identity matrices)

	Math::Mat4 viewMat = Math::Mat4::Rotation(-cameraRot) * Math::Mat4::Translation(-cameraPos);
	perCameraGlobalUBDataStack.top() =
	{
		viewMat,
		projectionMat,
		projectionMat * viewMat,
		cameraPos
	};
	SetUniformBufferData(perCameraGlobalUB, &perCameraGlobalUBDataStack);
}

void Graphics::PopCamera()
{
	EP_ASSERT(perCameraGlobalUBDataStack.size() > 1); // 0th element is reserved for null camera (identity matrices)
	perCameraGlobalUBDataStack.pop();
	SetUniformBufferData(perCameraGlobalUB, &perCameraGlobalUBDataStack.top());
}


void Graphics::SetModelMatrix(Math::Mat4 modelMat)
{
	perDrawGlobalUBData.ep_matrix_m = modelMat;
	perDrawGlobalUBData.ep_matrix_mv = perCameraGlobalUBDataStack.top().ep_matrix_v * modelMat;
	perDrawGlobalUBData.ep_matrix_mvp = perCameraGlobalUBDataStack.top().ep_matrix_vp * modelMat;
	SetUniformBufferData(perDrawGlobalUB, &perDrawGlobalUBData);
}
