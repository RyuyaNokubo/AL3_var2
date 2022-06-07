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
	Vector3 rotMove = { 0,0,0 };

	//移動ベクトルを変更する処理
	//速さ
	float speed = 0.2f;
	//回転角度
	float rot = rad(1.0f);

	//キー入力
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

	//移動限界座標&角度
	const float kMoveLimitX = 35;
	const float kMoveLimitY = 20;
	const float kRotLimit = rad(30.0f);

	//範囲を超えない処理
	worldTransform_.translation_.x = MaxNum(worldTransform_.translation_.x, kMoveLimitX);
	worldTransform_.translation_.x = MinNum(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.y = MaxNum(worldTransform_.translation_.y, kMoveLimitY);
	worldTransform_.translation_.y = MinNum(worldTransform_.translation_.y, -kMoveLimitY);
	worldTransform_.rotation_.y = MaxNum(worldTransform_.rotation_.y, kRotLimit);
	worldTransform_.rotation_.y = MinNum(worldTransform_.rotation_.y, -kRotLimit);

	//ベクトルの加算
	Matrix4 matTrans = setTrans(worldTransform_);
	Matrix4 matRot = setRot(worldTransform_);
	worldTransform_.matWorld_ = MathUtility::Matrix4Identity();
	worldTransform_.matWorld_ *= matRot;
	worldTransform_.matWorld_ *= matTrans;

	//行列更新
	worldTransform_.TransferMatrix();

	//キャラクター攻撃処理
	Attack();

	//弾更新
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_)
	{
		bullet->Update();
	}

	//キャラクターの座標を画面表示する処理
	debugText_->SetPos(50, 10);
	debugText_->Printf("character:(% f, % f, % f)", worldTransform_.translation_.x, worldTransform_.translation_.y, worldTransform_.translation_.z);

}

void Player::Draw(ViewProjection& viewProjection)
{
	model_->Draw(worldTransform_, viewProjection, textureHandle_);

	//弾描画
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_)
	{
		bullet->Draw(viewProjection);
	}
}

void Player::Attack()
{
	if (input_->TriggerKey(DIK_SPACE))
	{
		//弾を生成し、初期化
		std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();
		newBullet->Initialize(model_, worldTransform_.translation_);
		//弾を登録する
		bullets_.push_back(std::move(newBullet));
	}
}