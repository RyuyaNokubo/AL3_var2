#include"Enemy.h"

void Enemy::Initialize(Model* model, const Vector3& position, const Vector3& velocity)
{
	//NULLポインタチェック
	assert(model);

	//引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	enemyTextureHandle_ = TextureManager::Load("enemy.png");

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	//引数で受け取った初期座標をセット
	worldTransform_.translation_ = position;
	//引数で受け取った速度をメンバ変数に代入
	velocity_ = velocity;
}

void Enemy::Update()
{
	//座標を移動させる(1フレーム分の移動量を足しこむ)
	worldTransform_.translation_ += velocity_;

	//ワールドトランスフォームの更新
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