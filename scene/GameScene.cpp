#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "AxisIndicator.h"
#include "PrimitiveDrawer.h"
#include <random>
#include<fstream>

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete model_;
	delete skydomeModel_;
	delete debugCamera_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	//ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("mario.jpg");

	//3Dモデルの生成
	model_ = Model::Create();
	skydomeModel_ = Model::CreateFromOBJ("sky", true);

	//レールカメラの数値設定
	WorldTransform railCameraPos;
	railCameraPos.Initialize();
	Vector3 railCameraRot = { 0,0,0 };
	//レールカメラの生成
	RailCamera* newRailCamera = new RailCamera();
	//レールカメラの初期化
	newRailCamera->Initialize(railCameraPos, railCameraRot);
	//レールカメラの登録
	railCamera_.reset(newRailCamera);


	//自キャラの生成
	Player* newPlayer = new Player();
	//自キャラにレールカメラのアドレスを渡す
	newPlayer->SetCamera(railCamera_.get());
	//自キャラの初期化
	newPlayer->Initialize(model_, textureHandle_);
	//自キャラの登録
	player_.reset(newPlayer);


	LoadEnemyPopData();

	//天球の生成
	Skydome* newSkydome = new Skydome();
	//天球の初期化
	newSkydome->Initialize(skydomeModel_);
	//天球の登録
	skydome_.reset(newSkydome);


	//乱数シード生成器
	std::random_device seed_gen;
	//メルセンヌ・ツイスターの乱数エンジン
	std::mt19937_64 engine(seed_gen());
	//乱数範囲の指定
	std::uniform_real_distribution<float> posDist(-10.0f, 10.0f);
	std::uniform_real_distribution<float> rotDist(0.0f, PI);
	std::uniform_real_distribution<float> scaleDist(1.0f, 1.0f);


	//カメラ視点座標を設定
	viewProjection_.eye = { 0,0,-50 };

	////カメラ注視点座標を設定
	//viewProjection_.target = { 10,0,0 };

	////カメラ上方向ベクトルを設定
	//viewProjection_.up = { cosf(PI / 4.0f),sinf(PI / 4.0f),0.0f };

	////カメラ垂直方向視野角を設定
	//viewProjection_.fovAngleY = rad(10.0f);

	////アスペクト比を設定
	//viewProjection_.aspectRatio = 1.0f;

	////ニアクリップ距離を設定
	//viewProjection_.nearZ = 52.0f;
	////ファークリップ距離を設定
	//viewProjection_.farZ = 53.0f;

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();

	//デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	//軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	//軸方向表示が参照するビュープロジェクションを指定する(アドレス渡し)
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);

	////ライン描画が参照するビュープロジェクションを指定する(アドレス渡し)
	//PrimitiveDrawer::GetInstance()->SetViewProjection(&debugCamera_->GetViewProjection());

	//for (int i = 0; i < 102; i++)
	//{
	//	if (i < 51)
	//	{
	//		vec1[i] = Vector3(-50 + i * 2, 0, 50);
	//		vec2[i] = Vector3(-50 + i * 2, 0, -50);
	//	}
	//	else
	//	{
	//		vec1[i] = Vector3(50, 0, -50 + (i - 51) * 2);
	//		vec2[i] = Vector3(-50, 0, -50 + (i - 51) * 2);
	//	}
	//}
	//color[0] = Vector4(100, 100, 100, 10);
	//color[1] = Vector4(255, 0, 0, 10);
}

void GameScene::Update() {
#ifdef _DEBUG
	if (input_->TriggerKey(DIK_0))
	{
		isDebugCameraActive_ = true;
	}
#endif

	//デスフラグの立った弾を削除
	enemyBullets_.remove_if([](std::unique_ptr<EnemyBullet>& enemyBullet) {return enemyBullet->IsDead(); });
	//デスフラグの立った敵を削除
	enemy_.remove_if([](std::unique_ptr<Enemy>& enemy) {return enemy->IsDead(); });

	if (isDebugCameraActive_ == true)
	{
		//デバッグカメラの更新
		debugCamera_->Update();
	}

	//当たり判定処理
	CheckAllCollisions();

	//行列の再計算
	viewProjection_ = railCamera_->getViewProjection();

	//カメラデバッグ用表示
	debugText_->SetPos(50, 50);
	debugText_->Printf("eye:(%f,%f,%f)", viewProjection_.eye.x, viewProjection_.eye.y, viewProjection_.eye.z);
	debugText_->SetPos(50, 70);
	debugText_->Printf("target:(%f,%f,%f)", viewProjection_.target.x, viewProjection_.target.y, viewProjection_.target.z);
	debugText_->SetPos(50, 90);
	debugText_->Printf("up:(%f,%f,%f)", viewProjection_.up.x, viewProjection_.up.y, viewProjection_.up.z);
	debugText_->SetPos(50, 110);
	debugText_->Printf("fovAngleY(Degree):%f", rad(viewProjection_.fovAngleY));
	debugText_->SetPos(50, 130);
	debugText_->Printf("nearZ:%f", viewProjection_.nearZ);


	UpdateEnemyPopCommands();

	//キャラの更新
	player_->Update();
	for (std::unique_ptr<Enemy>& enemy : enemy_)
		enemy->Update();

	//弾更新
	for (std::unique_ptr<EnemyBullet>& enemyBullet : enemyBullets_)
		enemyBullet->Update();

	skydome_->Update();
	railCamera_->Update();
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	//3Dモデル描画
	//自キャラ
	player_->Draw(viewProjection_);
	//エネミー
	for (std::unique_ptr<Enemy>& enemy : enemy_)
		enemy->Draw(viewProjection_);
	//弾描画
	for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets_)
		bullet->Draw(viewProjection_);
	//天球
	skydome_->Draw(viewProjection_);

	////ライン描画が参照するビュープロジェクションを指定する(アドレス渡し)
	//for (int i = 0; i < 102; i++)
	//{
	//	if (i < 51)
	//		PrimitiveDrawer::GetInstance()->DrawLine3d(vec1[i], vec2[i], color[0]);
	//	else
	//		PrimitiveDrawer::GetInstance()->DrawLine3d(vec1[i], vec2[i], color[1]);
	//}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::CheckAllCollisions()
{
	//判定対象AとBの座標
	Vector3 posA, posB;

	//自弾リストの所得
	const 	std::list<std::unique_ptr<PlayerBullet>>& playerBullets = player_->GetBullets();

#pragma region 自キャラと敵弾の当たり判定

	//自キャラの座標
	posA = player_->GetWorldPosition();

	//自キャラと敵弾全ての当たり判定
	for (const std::unique_ptr<EnemyBullet>& bullet : enemyBullets_)
	{
		//敵弾の座標
		posB = bullet->GetWorldPosition();
		float AtoB = sqrt(pow(posA.x - posB.x, 2) + pow(posA.y - posB.y, 2) + pow(posA.z - posB.z, 2));
		if (AtoB <= 1)
		{
			//自キャラの衝突時コールバックを呼び出す
			player_->OnCollision();
			//敵弾の衝突時コールバックを呼び出す
			bullet->OnCollision();
		}
	}
#pragma endregion

#pragma region 自弾と敵キャラの当たり判定
	for (std::unique_ptr<Enemy>& enemy : enemy_)
	{
		//敵全てと自弾全ての当たり判定
		for (const std::unique_ptr<PlayerBullet>& bullet_ : playerBullets)
		{
			//敵の座標
			posA = enemy->GetWorldPosition();
			//自弾の座標
			posB = bullet_->GetWorldPosition();
			float AtoB = sqrt(pow(posA.x - posB.x, 2) + pow(posA.y - posB.y, 2) + pow(posA.z - posB.z, 2));
			if (AtoB <= 1)
			{
				//敵の衝突時コールバックを呼び出す
				enemy->OnCollision();
				//敵弾の衝突時コールバックを呼び出す
				bullet_->OnCollision();
			}
		}
	}
#pragma endregion

#pragma region 自弾と敵弾の当たり判定

	//自弾と敵弾の当たり判定
	for (const std::unique_ptr<PlayerBullet>& playerBullet_ : playerBullets)
	{
		for (const std::unique_ptr<EnemyBullet>& enemyBullet_ : enemyBullets_)
		{
			//自弾の座標
			posA = playerBullet_->GetWorldPosition();
			//敵弾の座標
			posB = enemyBullet_->GetWorldPosition();
			float AtoB = sqrt(pow(posA.x - posB.x, 2) + pow(posA.y - posB.y, 2) + pow(posA.z - posB.z, 2));
			if (AtoB <= 1)
			{
				//敵の衝突時コールバックを呼び出す
				playerBullet_->OnCollision();
				//敵弾の衝突時コールバックを呼び出す
				enemyBullet_->OnCollision();
			}
		}
	}
#pragma endregion
}

void GameScene::enemyPop(Vector3 position)
{
	//エネミーの数値設定
	Vector3 enemyApproachVelocity = { 0.0f,0.0f,0.0f };
	Vector3 enemyLeaveVelocity = { -0.1f,0.1f,0.0f };
	//エネミーの生成
	std::unique_ptr<Enemy> newEnemy = std::make_unique<Enemy>();
	//エネミーの初期化
	newEnemy->Initialize(model_, position, enemyApproachVelocity, enemyLeaveVelocity);
	//エネミーに自キャラのアドレスを渡す
	newEnemy->SetPlayer(player_.get());
	//エネミーにゲームシーンのアドレスを渡す
	newEnemy->SetGameScene(this);
	//エネミーの登録
	enemy_.push_back(std::move(newEnemy));
}

void GameScene::AddEnemyBullet(std::unique_ptr<EnemyBullet> enemyBullet)
{
	//リストに登録する
	enemyBullets_.push_back(std::move(enemyBullet));
}

void GameScene::LoadEnemyPopData()
{
	//ファイルを開く
	std::ifstream file;
	file.open("Resources/enemyPos.csv");
	assert(file.is_open());

	//ファイルの内容を文字列ストリームにコピー
	enemyPopCommands << file.rdbuf();

	//ファイルを閉じる
	file.close();
}

void GameScene::UpdateEnemyPopCommands()
{
	//待機処理
	if (isEnemyPopWait)
	{
		enemyWaitTimer--;
		if (enemyWaitTimer <= 0)
		{
			//待機完了
			isEnemyPopWait = false;
		}
		return;
	}

	//1行分の文字列を入れる変数
	std::string line;

	//コマンド実行ループ
	while (getline(enemyPopCommands, line))
	{
		//1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		std::string word;
		// , 区切りで行の先頭文字列を取得
		getline(line_stream, word, ',');
		// "//" から始まる行はコメント
		if (word.find("//") == 0)
		{
			//コメント行を飛ばす
			continue;
		}
		//POPコマンド
		if (word.find("POP") == 0)
		{
			//x座標
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());
			//y座標
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());
			//z座標
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			//敵を発生させる
			enemyPop(Vector3(x, y, z));
		}
		//WAITコマンド
		else if (word.find("WAIT") == 0)
		{
			getline(line_stream, word, ',');

			//待ち時間
			int32_t waitTime = atoi(word.c_str());

			//待機開始
			isEnemyPopWait = true;
			enemyWaitTimer = waitTime;

			//コマンドループを抜ける
			break;
		}
	}
}
