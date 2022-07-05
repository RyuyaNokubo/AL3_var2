#include"Skydome.h"

void Skydome::Initialize(Model* model)
{
	//NULL�|�C���^�`�F�b�N
	assert(model);

	model_ = model;

	//���[���h�ϊ��̏�����
	worldTransform_.Initialize();
	worldTransform_.translation_ = { 0,0,0 };
}

void Skydome::Update()
{
	worldTransform_.TransferMatrix();
}

void Skydome::Draw(ViewProjection& viewProjection)
{
	model_->Draw(worldTransform_, viewProjection);
}