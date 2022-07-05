#include"Skydome.h"

void Skydome::Initialize(Model* model)
{
	//NULLポインタチェック
	assert(model);

	model_ = model;

	//ワールド変換の初期化
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