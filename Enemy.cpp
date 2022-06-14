#include"Enemy.h"

void Enemy::Initialize(Model* model, const Vector3& position, const Vector3& aVelocity, const Vector3& lVelocity)
{
	//NULL�|�C���^�`�F�b�N
	assert(model);

	//�����Ƃ��Ď󂯎�����f�[�^�������o�ϐ��ɋL�^����
	model_ = model;
	enemyTextureHandle_ = TextureManager::Load("enemy.png");

	//���[���h�g�����X�t�H�[���̏�����
	worldTransform_.Initialize();
	//�����Ŏ󂯎�����������W���Z�b�g
	worldTransform_.translation_ = position;
	//�����Ŏ󂯎�������x�������o�ϐ��ɑ��
	approachVelocity_ = aVelocity;
	leaveVelocity_ = lVelocity;
}

void Enemy::Update()
{
	//���W���ړ�������(1�t���[�����̈ړ��ʂ𑫂�����)
	switch (phase_)
	{
	case Phase::Approach:
	default:
		approach();
		break;

	case Phase::Leave:
		leave();
		break;
	}

	//���[���h�g�����X�t�H�[���̍X�V
	Matrix4 matScale = setScale(worldTransform_);
	Matrix4 matRot = setRot(worldTransform_);
	Matrix4 matTrans = setTrans(worldTransform_);

	worldTransform_.matWorld_ = MathUtility::Matrix4Identity();
	matScale *= matRot;
	matScale *= matTrans;
	worldTransform_.matWorld_ *= matScale;

	worldTransform_.TransferMatrix();
}

void Enemy::Draw(const ViewProjection& viewProjection)
{
	model_->Draw(worldTransform_, viewProjection, enemyTextureHandle_);
}

void Enemy::approach()
{
	//�ړ�(�x�N�g���̉��Z)
	worldTransform_.translation_ += approachVelocity_;
	//����̈ʒu�ɓ��B�����痣�E
	if (worldTransform_.translation_.z < 0.0f)
	{
		phase_ = Phase::Leave;
	}
}

void Enemy::leave()
{
	//�ړ�(�x�N�g�������Z)
	worldTransform_.translation_ += leaveVelocity_;
}