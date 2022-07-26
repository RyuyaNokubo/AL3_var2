#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "DebugText.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "DebugCamera.h"
#include"player.h"
#include"Enemy.h"
#include"EnemyBullet.h"
#include"Skydome.h"
#include"RailCamera.h"
#include"mine.h"
#include<sstream>

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
  /// <summary>
  /// コンストクラタ
  /// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	enum PartId {
		Root,	//大元
		Spine,	//脊髄
		Chest,	//胸
		Head,	//頭
		ArmL,	//左腕
		ArmR,	//右腕
		Hip,	//尻
		LegL,	//左足
		LegR,	//右足

		kNumPartId
	};

	/// <summary>
	/// 衝突判定と応答
	/// </summary>
	void CheckAllCollisions();

	void enemyPop(Vector3 position);

	//敵弾リストを所得
	const std::list<std::unique_ptr<EnemyBullet>>& GetEnemyBullets() { return enemyBullets_; }
	
	/// <summary>
	/// 敵弾を追加する
	/// </summary>
	/// <param name="enemyBullet">敵弾</param>
	void AddEnemyBullet(std::unique_ptr<EnemyBullet>enemyBullet);

	/// <summary>
	/// 敵発生データの読み込み
	/// </summary>
	void LoadEnemyPopData();

	/// <summary>
	/// 敵発生コマンドの更新
	/// </summary>
	void UpdateEnemyPopCommands();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	DebugText* debugText_ = nullptr;

	//テクスチャハンドル
	uint32_t textureHandle_ = 0;

	//3Dモデル
	Model* model_ = nullptr;
	Model* skydomeModel_ = nullptr;

	//ビュープロジェクション
	ViewProjection viewProjection_;

	//デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;
	bool isDebugCameraActive_ = false;

	Vector3 vec1[102];
	Vector3 vec2[102];
	Vector4 color[2];

	float viewAngle = 0.0f;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
	//自キャラ
	std::unique_ptr<Player>player_;

	//エネミー
	std::list<std::unique_ptr<Enemy>>enemy_;

	//敵発生コマンド
	std::stringstream enemyPopCommands;

	//敵待機フラグ
	bool isEnemyPopWait = false;

	//敵待機タイマー
	int enemyWaitTimer = 0;

	//エネミー弾
	std::list<std::unique_ptr<EnemyBullet>>enemyBullets_;

	//天球
	std::unique_ptr<Skydome>skydome_;

	//レールカメラ
	std::unique_ptr<RailCamera>railCamera_;
};
