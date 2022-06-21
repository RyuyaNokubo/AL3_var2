#include"Enemy.h"

void Enemy::Initialize(Model* model, const Vector3& position, const Vector3& aVelocity, const Vector3& lVelocity)
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
	approachVelocity_ = aVelocity;
	leaveVelocity_ = lVelocity;

	//接近フェーズ初期化
	approachInitialize();
}

void Enemy::Update()
{
	//座標を移動させる(1フレーム分の移動量を足しこむ)
	switch (phase_)
	{
	case Phase::Approach:
	default:
		approach();
		break;

	case Phase::Leave:
		leave();
		break;
	}

	//ワールドトランスフォームの更新
	Matrix4 matScale = setScale(worldTransform_);
	Matrix4 matRot = setRot(worldTransform_);
	Matrix4 matTrans = setTrans(worldTransform_);

	worldTransform_.matWorld_ = MathUtility::Matrix4Identity();
	matScale *= matRot;
	matScale *= matTrans;
	worldTransform_.matWorld_ *= matScale;

	worldTransform_.TransferMatrix();

	//弾更新
	for (std::unique_ptr<EnemyBullet>& bullet : bullets_)
	{
		bullet->Update();
	}
}

void Enemy::Draw(const ViewProjection& viewProjection)
{
	model_->Draw(worldTransform_, viewProjection, enemyTextureHandle_);
	//弾描画
	for (std::unique_ptr<EnemyBullet>& bullet : bullets_)
	{
		bullet->Draw(viewProjection);
	}
}

void Enemy::approach()
{
	//移動(ベクトルの加算)
	worldTransform_.translation_ += approachVelocity_;
	//既定の位置に到達したら離脱
	if (worldTransform_.translation_.z < 0.0f)
	{
		phase_ = Phase::Leave;
	}

	if (--fireTimer <= 0)
	{
		//弾を発射
		Fire();

		//発射タイマーを初期化
		fireTimer = kFireInterval;
	}
}

void Enemy::leave()
{
	//移動(ベクトルを加算)
	worldTransform_.translation_ += leaveVelocity_;
}

void Enemy::approachInitialize()
{
	//発射タイマーを初期化
	fireTimer = 10;
}

void Enemy::Fire()
{
	//弾の速度
	const float kBulletSpeed = -1.0f;
	Vector3 velocity(0, 0, kBulletSpeed);

	//速度ベクトルを自機の向きに合わせて回転させる
	velocity = vec_mat(velocity, worldTransform_);

	//弾を生成し、初期化
	std::unique_ptr<EnemyBullet> newBullet = std::make_unique<EnemyBullet>();
	newBullet->Initialize(model_, worldTransform_.translation_, velocity);
	//弾を登録する
	bullets_.push_back(std::move(newBullet));
}