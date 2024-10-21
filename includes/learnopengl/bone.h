#pragma once

/* Container for bone data */

#include <vector>
#include <assimp/scene.h>
#include <list>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <learnopengl/assimp_glm_helpers.h>

// 一个骨骼有很多关键帧数据，每帧都不一样
struct KeyPosition
{
	glm::vec3	position;
	float		timeStamp;
};

struct KeyRotation
{
	glm::quat	orientation;
	float		timeStamp;
};

struct KeyScale
{
	glm::vec3	scale;
	float		timeStamp;
};

/*
 *bone结构体最核心的任务就是根据运行时间，不断更新变换矩阵LocalTransform，这个是骨骼的local矩阵
 *
 */
class Bone
{
public:
	// 构造函数，从aiNodeAnim读取密钥并将密钥及其时间戳存储到mPositionKeys、mRotationKeys和mScalingKeys

	// aiNodeAnim就是一个骨骼的所有帧完整的变换数据：positions[]，rotations[]，scalings[]
	// 所有的骨骼构成一个树形结构，一个aiNodeAnim代表的是一个bone上的所有关键帧数据
	Bone(const std::string& name, int ID, const aiNodeAnim* channel):
		m_Name(name),
		m_ID(ID),
		m_LocalTransform(1.0f)
	{
		// 各个关键帧的position
		m_NumPositions = channel->mNumPositionKeys;
		for (int positionIndex = 0; positionIndex < m_NumPositions; ++positionIndex)
		{
			aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
			float timeStamp		  = channel->mPositionKeys[positionIndex].mTime;

			KeyPosition data;
			data.position  = AssimpGLMHelpers::GetGLMVec(aiPosition);
			data.timeStamp = timeStamp;
			m_Positions.push_back(data);
		}

		// 各个关键帧的Rotation
		m_NumRotations = channel->mNumRotationKeys;
		for (int rotationIndex = 0; rotationIndex < m_NumRotations; ++rotationIndex)
		{
			aiQuaternion aiOrientation  = channel->mRotationKeys[rotationIndex].mValue;
			float timeStamp				= channel->mRotationKeys[rotationIndex].mTime;

			KeyRotation data;
			data.orientation = AssimpGLMHelpers::GetGLMQuat(aiOrientation);
			data.timeStamp   = timeStamp;
			m_Rotations.push_back(data);
		}

		// 各个关键帧的Scale
		m_NumScalings = channel->mNumScalingKeys;
		for (int keyIndex = 0; keyIndex < m_NumScalings; ++keyIndex)
		{
			aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
			float timeStamp  = channel->mScalingKeys[keyIndex].mTime;

			KeyScale data;
			data.scale     = AssimpGLMHelpers::GetGLMVec(scale);
			data.timeStamp = timeStamp;
			m_Scales.push_back(data);
		}
	}

	// 根据动画运行时间，不断插值更新矩阵
	void Update(float animationTime)
	{
		glm::mat4 translation	= InterpolatePosition(animationTime);
		glm::mat4 rotation		= InterpolateRotation(animationTime);
		glm::mat4 scale			= InterpolateScaling( animationTime);

		m_LocalTransform = translation * rotation * scale;
	}

	glm::mat4 GetLocalTransform()
	{
		return m_LocalTransform;
	}

	std::string GetBoneName() const
	{
		return m_Name;
	}

	int GetBoneID()
	{
		return m_ID;
	}

	// 根据当前的时间，返回关键帧
	int GetPositionIndex(float animationTime)
	{
		for (int index = 0; index < m_NumPositions - 1; ++index)
		{
			if (animationTime < m_Positions[index + 1].timeStamp)
				return index;
		}
		assert(0);
	}

	int GetRotationIndex(float animationTime)
	{
		for (int index = 0; index < m_NumRotations - 1; ++index)
		{
			if (animationTime < m_Rotations[index + 1].timeStamp)
				return index;
		}
		assert(0);
	}

	int GetScaleIndex(float animationTime)
	{
		for (int index = 0; index < m_NumScalings - 1; ++index)
		{
			if (animationTime < m_Scales[index + 1].timeStamp)
				return index;
		}
		assert(0);
	}


private:

	// 得到两个关键帧之间的时间比例
	float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
	{
		float scaleFactor	= 0.0f;
		float midWayLength	= animationTime - lastTimeStamp;
		float framesDiff	= nextTimeStamp - lastTimeStamp;
		scaleFactor = midWayLength / framesDiff;
		return scaleFactor;
	}

	glm::mat4 InterpolatePosition(float animationTime)
	{
		// 如果只有一帧，直接返回
		if (1 == m_NumPositions)
			return glm::translate(glm::mat4(1.0f), m_Positions[0].position);

		int p0Index = GetPositionIndex(animationTime);
		int p1Index = p0Index + 1;

		// 根据两帧的时间戳，计算时间比例
		float scaleFactor = GetScaleFactor(	m_Positions[p0Index].timeStamp,
											m_Positions[p1Index].timeStamp,
											animationTime);

		// 使用两个关键帧之间的时间比例插值
		glm::vec3 finalPosition = glm::mix(	m_Positions[p0Index].position,
											m_Positions[p1Index].position,
											scaleFactor);

		// 返回矩阵
		return glm::translate(glm::mat4(1.0f), finalPosition);
	}

	glm::mat4 InterpolateRotation(float animationTime)
	{
		if (1 == m_NumRotations)
		{
			auto rotation = glm::normalize(m_Rotations[0].orientation);
			return glm::toMat4(rotation);
		}

		int p0Index = GetRotationIndex(animationTime);
		int p1Index = p0Index + 1;
		float scaleFactor = GetScaleFactor(	m_Rotations[p0Index].timeStamp,
											m_Rotations[p1Index].timeStamp,
											animationTime);

		glm::quat finalRotation = glm::slerp(	m_Rotations[p0Index].orientation,
												m_Rotations[p1Index].orientation,
												scaleFactor);

		finalRotation = glm::normalize(finalRotation);
		return glm::toMat4(finalRotation);
	}

	glm::mat4 InterpolateScaling(float animationTime)
	{
		if (1 == m_NumScalings)
			return glm::scale(glm::mat4(1.0f), m_Scales[0].scale);

		int p0Index = GetScaleIndex(animationTime);
		int p1Index = p0Index + 1;
		float scaleFactor = GetScaleFactor(	m_Scales[p0Index].timeStamp,
											m_Scales[p1Index].timeStamp,
											animationTime);

		glm::vec3 finalScale = glm::mix(	m_Scales[p0Index].scale,
											m_Scales[p1Index].scale,
											scaleFactor);

		return glm::scale(glm::mat4(1.0f), finalScale);
	}

	std::vector<KeyPosition> m_Positions;
	std::vector<KeyRotation> m_Rotations;
	std::vector<KeyScale> m_Scales;
	int m_NumPositions;
	int m_NumRotations;
	int m_NumScalings;

	glm::mat4 m_LocalTransform;
	std::string m_Name;
	int m_ID;
};

