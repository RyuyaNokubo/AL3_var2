#pragma once
#include"Model.h"
#include"WorldTransform.h"
#include"Input.h"
#include"DebugText.h"
#include <cassert>
#include"EnemyBullet.h"
#include<memory>
#include<list>
#include"mine.h"

/// <summary>
/// 敵
/// </summary>
class Enemy
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="position">初期座標</param>
	/// <param name="velocity">速度</param>
	void Initialize(Model* model, const Vector3& position, const Vector3& aVelocity, const Vector3& lVelocity);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection">ビュープロジェクション</param>
	void Draw(const ViewProjection& viewProjection);

	//フェーズ
	void approach();
	void leave();

	//接近フェーズ初期化
	void approachInitialize();

	/// <summary>
	/// 弾発射
	/// </summary>
	void Fire();

	//発射間隔
	static const int kFireInterval = 60;

private:
	//ワールド変換データ
	WorldTransform worldTransform_;
	//モデル
	Model* model_ = nullptr;
	//テクスチャハンドル
	uint32_t enemyTextureHandle_ = 0u;
	//速度
	Vector3 approachVelocity_, leaveVelocity_;
	//行動フェーズ
	enum class Phase
	{
		Approach,	//接近する
		Leave,		//離脱する
	};
	//フェーズ
	Phase phase_ = Phase::Approach;
	//弾
	std::list<std::unique_ptr<EnemyBullet>>bullets_;
	//発射タイマー
	int32_t fireTimer = 0;
};
