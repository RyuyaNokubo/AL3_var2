#include"Enemy.h"
#include"player.h"

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

	//�ڋ߃t�F�[�Y������
	approachInitialize();
}

void Enemy::Update()
{
	//�f�X�t���O�̗������e���폜
	bullets_.remove_if([](std::unique_ptr<EnemyBullet>& bullet) {return bullet->IsDead(); });

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

	//�e�X�V
	for (std::unique_ptr<EnemyBullet>& bullet : bullets_)
	{
		bullet->Update();
	}
}

void Enemy::Draw(const ViewProjection& viewProjection)
{
	model_->Draw(worldTransform_, viewProjection, enemyTextureHandle_);
	//�e�`��
	for (std::unique_ptr<EnemyBullet>& bullet : bullets_)
	{
		bullet->Draw(viewProjection);
	}
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

	if (--fireTimer <= 0)
	{
		//�e�𔭎�
		Fire();

		//���˃^�C�}�[��������
		fireTimer = kFireInterval;
	}
}

void Enemy::leave()
{
	//�ړ�(�x�N�g�������Z)
	worldTransform_.translation_ += leaveVelocity_;
}

void Enemy::approachInitialize()
{
	//���˃^�C�}�[��������
	fireTimer = 10;
}

void Enemy::Fire()
{
	assert(player_);

	//�e�̑��x
	const float kBulletSpeed = 0.5f;

	Vector3 playerPos = player_->GetWorldPosition();
	Vector3 enemyPos = GetWorldPosition();

	Vector3 velocity = playerPos;
	velocity -= enemyPos;
	velocity = vec_one(velocity);
	velocity *= kBulletSpeed;

	//�e�𐶐����A������
	std::unique_ptr<EnemyBullet> newBullet = std::make_unique<EnemyBullet>();
	newBullet->Initialize(model_, worldTransform_.translation_, velocity);
	//�e��o�^����
	bullets_.push_back(std::move(newBullet));
}

Vector3 Enemy::GetWorldPosition()
{
	//���[���h���W������ϐ�
	Vector3 worldPos;
	//���[���h�s��̕��s�ړ��������擾(���[���h���W)
	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;

	return worldPos;
}

void Enemy::OnCollision()
{

}