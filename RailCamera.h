#pragma once
#include"WorldTransform.h"
#include"DebugText.h"
#include"mine.h"

/// <summary>
/// ���[���J����
/// </summary>
class RailCamera
{
public:
	/// <summary>
	/// ������
	/// </summary>
	/// <param name="worldTransform">���[���h�g�����X�t�H�[��</param>
	/// <param name="rad">���W�A��</param>
	void Initialize(WorldTransform worldTransform,Vector3 rad);

	/// <summary>
	/// �X�V
	/// </summary>
	void Update();

	ViewProjection getViewProjection();

	WorldTransform getWorldTransform();

private:
	//���[���h�ϊ��f�[�^
	WorldTransform worldTransform_;
	//�r���[�v���W�F�N�V����
	ViewProjection viewProjection_;
	//�f�o�b�O�e�L�X�g
	DebugText* debugText_ = nullptr;
};