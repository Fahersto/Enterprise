#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Enterprise/Graphics.h"

using Enterprise::Graphics;

std::stack<Graphics::perCameraGlobalUBStruct> Graphics::perCameraGlobalUBDataStack;

void Graphics::PushCamera()
{
	perCameraGlobalUBDataStack.push(perCameraGlobalUBDataStack.top());
}

void Graphics::PushCamera(glm::vec3 cameraPos, glm::quat cameraRot, glm::mat4 projectionMat)
{
	glm::mat4 rotmat = glm::mat4_cast(glm::inverse(cameraRot));
	glm::mat4 viewMat = glm::translate(rotmat, -cameraPos);
	perCameraGlobalUBDataStack.emplace
	(
		viewMat,
		projectionMat,
		projectionMat * viewMat,
		cameraPos
	);
	SetUniformBufferData(perCameraGlobalUB, &perCameraGlobalUBDataStack);
}

void Graphics::SetCamera(glm::vec3 cameraPos, glm::quat cameraRot, glm::mat4 projectionMat)
{
	EP_ASSERT(perCameraGlobalUBDataStack.size() > 1); // 0th element is reserved for null camera (identity matrices)

	glm::mat4 rotmat = glm::mat4_cast(glm::inverse(cameraRot));
	glm::mat4 viewMat = glm::translate(rotmat, -cameraPos);
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


void Graphics::SetModelMatrix(glm::mat4 modelMat)
{
	perDrawGlobalUBData.ep_matrix_m = modelMat;
	perDrawGlobalUBData.ep_matrix_mv = perCameraGlobalUBDataStack.top().ep_matrix_v * modelMat;
	perDrawGlobalUBData.ep_matrix_mvp = perCameraGlobalUBDataStack.top().ep_matrix_vp * modelMat;
	SetUniformBufferData(perDrawGlobalUB, &perDrawGlobalUBData);
}
