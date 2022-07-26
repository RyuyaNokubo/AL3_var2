#include"Skydome.h"

void Skydome::Initialize(Model* model)
{
	//NULLポインタチェック
	assert(model);

	model_ = model;

	//ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = { 0,0,0 };
	worldTransform_.scale_ = { 2.0f,2.0f,2.0f };
}

void Skydome::Update()
{
	//ベクトルの加算
	Matrix4 matScale = setScale(worldTransform_);
	Matrix4 matTrans = setTrans(worldTransform_);
	Matrix4 matRot = setRot(worldTransform_);
	worldTransform_.matWorld_ = MathUtility::Matrix4Identity();
	worldTransform_.matWorld_ *= matScale;
	worldTransform_.matWorld_ *= matRot;
	worldTransform_.matWorld_ *= matTrans;
	worldTransform_.TransferMatrix();
}

void Skydome::Draw(ViewProjection& viewProjection)
{
	model_->Draw(worldTransform_, viewProjection);
}