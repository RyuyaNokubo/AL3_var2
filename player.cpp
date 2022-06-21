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
	//デスフラグの立った弾を削除
	bullets_.remove_if([](std::unique_ptr<PlayerBullet>& bullet) {return bullet->IsDead(); });

	//キャラクターの移動ベクトル
	Vector3 move = { 0,0,0 };
	Vector3 rotMove = { 0,0,0 };

	//移動ベクトルを変更する処理
	//速さ
	float speed = 0.2f;
	//回転角度
	float rot = rad(1.0f);

	//キー入力
	if (input_->PushKey(DIK_W) || input_->PushKey(DIK_UP))
		move.y += speed;
	if (input_->PushKey(DIK_S) || input_->PushKey(DIK_DOWN))
		move.y -= speed;
	if (input_->PushKey(DIK_A) || input_->PushKey(DIK_LEFT))
		move.x -= speed;
	if (input_->PushKey(DIK_D) || input_->PushKey(DIK_RIGHT))
		move.x += speed;
	if (input_->PushKey(DIK_Z))
		rotMove.y += rot;
	if (input_->PushKey(DIK_X))
		rotMove.y -= rot;

	worldTransform_.translation_ += move;
	worldTransform_.rotation_ += rotMove;

	//移動限界座標&角度
	const float kMoveLimitX = 35;
	const float kMoveLimitY = 20;

	//範囲を超えない処理
	worldTransform_.translation_.x = MaxNum(worldTransform_.translation_.x, kMoveLimitX);
	worldTransform_.translation_.x = MinNum(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.y = MaxNum(worldTransform_.translation_.y, kMoveLimitY);
	worldTransform_.translation_.y = MinNum(worldTransform_.translation_.y, -kMoveLimitY);

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
		//弾の速度
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, kBulletSpeed);

		//速度ベクトルを自機の向きに合わせて回転させる
		velocity = vec_mat(velocity, worldTransform_);

		//弾を生成し、初期化
		std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();
		newBullet->Initialize(model_, worldTransform_.translation_, velocity);
		//弾を登録する
		bullets_.push_back(std::move(newBullet));
	}
}

Vector3 Player::GetWorldPosition()
{
	//ワールド座標を入れる変数
	Vector3 worldPos;
	//ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.translation_.x;
	worldPos.y= worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;

	return worldPos;
}