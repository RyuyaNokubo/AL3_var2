#include"player.h"


void Player::Initialize(Model* model, uint32_t textureHandle)
{
	//NULL�|�C���^�`�F�b�N
	assert(model);

	//�����Ƃ��Ď󂯎�����f�[�^�������o�ϐ��ɋL�^����
	model_ = model;
	textureHandle_ = textureHandle;

	//�V���O���g���C���X�^���X���擾����
	input_ = Input::GetInstance();
	debugText_ = DebugText::GetInstance();

	//���[���h�ϊ��̏�����
	worldTransform_.Initialize();
}

void Player::Update()
{
	//�L�����N�^�[�̈ړ��x�N�g��
	Vector3 move = { 0,0,0 };
	Vector3 rotMove = { 0,0,0 };

	//�ړ��x�N�g����ύX���鏈��
	//����
	float speed = 0.2f;
	//��]�p�x
	float rot = rad(1.0f);

	//�L�[����
	if (input_->PushKey(DIK_W))
		move.y += speed;
	if (input_->PushKey(DIK_S))
		move.y -= speed;
	if (input_->PushKey(DIK_A))
	{
		move.x -= speed;
		rotMove.y -= rot;
	}
	if (input_->PushKey(DIK_D))
	{
		move.x += speed;
		rotMove.y += rot;
	}

	worldTransform_.translation_ += move;
	worldTransform_.rotation_ += rotMove;

	//�ړ����E���W&�p�x
	const float kMoveLimitX = 35;
	const float kMoveLimitY = 20;
	const float kRotLimit = rad(30.0f);

	//�͈͂𒴂��Ȃ�����
	worldTransform_.translation_.x = MaxNum(worldTransform_.translation_.x, kMoveLimitX);
	worldTransform_.translation_.x = MinNum(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.y = MaxNum(worldTransform_.translation_.y, kMoveLimitY);
	worldTransform_.translation_.y = MinNum(worldTransform_.translation_.y, -kMoveLimitY);
	worldTransform_.rotation_.y = MaxNum(worldTransform_.rotation_.y, kRotLimit);
	worldTransform_.rotation_.y = MinNum(worldTransform_.rotation_.y, -kRotLimit);

	//�x�N�g���̉��Z
	Matrix4 matTrans = setTrans(worldTransform_);
	Matrix4 matRot = setRot(worldTransform_);
	worldTransform_.matWorld_ = MathUtility::Matrix4Identity();
	worldTransform_.matWorld_ *= matRot;
	worldTransform_.matWorld_ *= matTrans;

	//�s��X�V
	worldTransform_.TransferMatrix();

	//�L�����N�^�[�U������
	Attack();

	//�e�X�V
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_)
	{
		bullet->Update();
	}

	//�L�����N�^�[�̍��W����ʕ\�����鏈��
	debugText_->SetPos(50, 10);
	debugText_->Printf("character:(% f, % f, % f)", worldTransform_.translation_.x, worldTransform_.translation_.y, worldTransform_.translation_.z);

}

void Player::Draw(ViewProjection& viewProjection)
{
	model_->Draw(worldTransform_, viewProjection, textureHandle_);

	//�e�`��
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_)
	{
		bullet->Draw(viewProjection);
	}
}

void Player::Attack()
{
	if (input_->TriggerKey(DIK_SPACE))
	{
		//�e�𐶐����A������
		std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();
		newBullet->Initialize(model_, worldTransform_.translation_);
		//�e��o�^����
		bullets_.push_back(std::move(newBullet));
	}
}