#pragma once
#include"Model.h"
#include"WorldTransform.h"
#include"Input.h"
#include"DebugText.h"
#include <cassert>
#include"EnemyBullet.h"
#include<memory>
#include<list>
#include"mine.h"

//���@�N���X�̑O���錾
class Player;

//GameScene�̑O���錾(����̍�)
class GameScene;

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

	//�t�F�[�Y
	void approach();
	void leave();

	//�ڋ߃t�F�[�Y������
	void approachInitialize();

	/// <summary>
	/// �e����
	/// </summary>
	void Fire();

	//���ˊԊu
	static const int kFireInterval = 60;

	void SetPlayer(Player* player) { player_ = player; }

	//���[���h���W���擾
	Vector3 GetWorldPosition();

	//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�
	void OnCollision();

	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }
	
	bool IsDead()const { return isDead; }

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

	//���˃^�C�}�[
	int32_t fireTimer = 0;

	//�f�X�t���O
	bool isDead = false;

	//���L����
	Player* player_ = nullptr;

	//�Q�[���V�[��
	GameScene* gameScene_ = nullptr;
};
