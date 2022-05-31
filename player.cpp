#include"player.h"


void Player::Initialize(Model* model, uint32_t textureHandle)
{
	//NULLポインタチェック
	assert(model);

	//引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	textureHandle_ = textureHandle;

	//シングルトンインスタンスを取得する
	input_ = Input::GetInstance();
	debugText_ = DebugText::GetInstance();

	//ワールド変換の初期化
	worldTransform_.Initialize();
}

void Player::Update()
{
	//キャラクターの移動ベクトル
	Vector3 move = { 0,0,0 };

	//移動ベクトルを変更する処理
	//速さ
	float speed = 0.2f;

	//キー入力
	if (input_->PushKey(DIK_W))
		move.y += speed;
	if (input_->PushKey(DIK_S))
		move.y -= speed;
	if (input_->PushKey(DIK_A))
		move.x -= speed;
	if (input_->PushKey(DIK_D))
		move.x += speed;

	worldTransform_.translation_ += move;

	//移動限界座標
	const float kMoveLimitX = 35;
	const float kMoveLimitY = 20;

	//範囲を超えない処理
	worldTransform_.translation_.x = MaxNum(worldTransform_.translation_.x,kMoveLimitX);
	worldTransform_.translation_.x = MinNum(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.y = MaxNum(worldTransform_.translation_.y, kMoveLimitY);
	worldTransform_.translation_.y = MinNum(worldTransform_.translation_.y, -kMoveLimitY);

	//ベクトルの加算
	Matrix4 matTrans = setTrans(worldTransform_);
	worldTransform_.matWorld_ = MathUtility::Matrix4Identity();
	worldTransform_.matWorld_ *= matTrans;

	//行列更新
	worldTransform_.TransferMatrix();

	//キャラクターの座標を画面表示する処理
	debugText_->SetPos(50, 10);
	debugText_->Printf("character:(% f, % f, % f)", worldTransform_.translation_.x, worldTransform_.translation_.y, worldTransform_.translation_.z);

}

void Player::Draw(ViewProjection& viewProjection)
{
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}