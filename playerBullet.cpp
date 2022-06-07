#include"playerBullet.h"


void PlayerBullet::Initialize(Model* model, const Vector3& position)
{
	//NULLポインタチェック
	assert(model);

	model_ = model;
	//テクスチャ読み込み
	textureHandle_ = TextureManager::Load("black.png");

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	//引数で受け取った初期座標をセット
	worldTransform_.translation_ = position;

}

void PlayerBullet::Update()
{
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

void PlayerBullet::Draw(const ViewProjection& viewProjection)
{
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}