#pragma once
#include"Model.h"
#include"WorldTransform.h"
#include"Input.h"
#include"DebugText.h"
#include <cassert>
#include<memory>
#include<list>
#include"mine.h"

/// <summary>
/// �G
/// </summary>
class Enemy
{
public:
	/// <summary>
	/// ������
	/// </summary>
	/// <param name="model">���f��</param>
	/// <param name="position">�������W</param>
	/// <param name="velocity">���x</param>
	void Initialize(Model* model, const Vector3& position, const Vector3& aVelocity, const Vector3& lVelocity);

	/// <summary>
	/// �X�V
	/// </summary>
	void Update();

	/// <summary>
	/// �`��
	/// </summary>
	/// <param name="viewProjection">�r���[�v���W�F�N�V����</param>
	void Draw(const ViewProjection& viewProjection);

	void approach();
	void leave();

private:
	//���[���h�ϊ��f�[�^
	WorldTransform worldTransform_;
	//���f��
	Model* model_ = nullptr;
	//�e�N�X�`���n���h��
	uint32_t enemyTextureHandle_ = 0u;
	//���x
	Vector3 approachVelocity_, leaveVelocity_;
	//�s���t�F�[�Y
	enum class Phase
	{
		Approach,	//�ڋ߂���
		Leave,		//���E����
	};
	//�t�F�[�Y
	Phase phase_ = Phase::Approach;
};
