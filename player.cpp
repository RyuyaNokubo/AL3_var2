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

	//�ړ��x�N�g����ύX���鏈��
	//����
	float speed = 0.2f;

	//�L�[����
	if (input_->PushKey(DIK_W))
		move.y += speed;
	if (input_->PushKey(DIK_S))
		move.y -= speed;
	if (input_->PushKey(DIK_A))
		move.x -= speed;
	if (input_->PushKey(DIK_D))
		move.x += speed;

	worldTransform_.translation_ += move;

	//�ړ����E���W
	const float kMoveLimitX = 35;
	const float kMoveLimitY = 20;

	//�͈͂𒴂��Ȃ�����
	worldTransform_.translation_.x = MaxNum(worldTransform_.translation_.x,kMoveLimitX);
	worldTransform_.translation_.x = MinNum(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.y = MaxNum(worldTransform_.translation_.y, kMoveLimitY);
	worldTransform_.translation_.y = MinNum(worldTransform_.translation_.y, -kMoveLimitY);

	//�x�N�g���̉��Z
	Matrix4 matTrans = setTrans(worldTransform_);
	worldTransform_.matWorld_ = MathUtility::Matrix4Identity();
	worldTransform_.matWorld_ *= matTrans;

	//�s��X�V
	worldTransform_.TransferMatrix();

	//�L�����N�^�[�̍��W����ʕ\�����鏈��
	debugText_->SetPos(50, 10);
	debugText_->Printf("character:(% f, % f, % f)", worldTransform_.translation_.x, worldTransform_.translation_.y, worldTransform_.translation_.z);

}

void Player::Draw(ViewProjection& viewProjection)
{
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}