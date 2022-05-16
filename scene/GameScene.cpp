#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "AxisIndicator.h"
#include "PrimitiveDrawer.h"

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete model_;
	delete debugCamera_;
}

const float PI = 3.141592;

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	//ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("mario.jpg");

	//3Dモデルの生成
	model_ = Model::Create();

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();

	//デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	//軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	//軸方向表示が参照するビュープロジェクションを指定する(アドレス渡し)
	AxisIndicator::GetInstance()->SetTargetViewProjection(&debugCamera_->GetViewProjection());

	//ライン描画が参照するビュープロジェクションを指定する(アドレス渡し)
	PrimitiveDrawer::GetInstance()->SetViewProjection(&debugCamera_->GetViewProjection());

	for (int i = 0; i < 102; i++)
	{
		if (i < 51)
		{
			vec1[i] = Vector3(-50 + i * 2, 0, 50);
			vec2[i] = Vector3(-50 + i * 2, 0, -50);
		}
		else
		{
			vec1[i] = Vector3(50, 0, -50 + (i - 51) * 2);
			vec2[i] = Vector3(-50, 0, -50 + (i - 51) * 2);
		}
	}
	color[0] = Vector4(100, 100, 100, 10);
	color[1] = Vector4(255, 0, 0, 10);

	//X,Y,Z方向のスケーリングを設定
	worldTransform_.scale_ = { 5.0f,5.0f,5.0f };

	//X,Y,Z軸回りの回転角を設定
	worldTransform_.rotation_ = { PI / 4,PI / 4,0 };

	//X,Y,Z軸回りの平行移動を設定
	worldTransform_.translation_ = { 10,10,10 };


	//スケーリング行列を宣言
	Matrix4 matScale;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			matScale.m[i][j] = 0;

	matScale.m[0][0] = worldTransform_.scale_.x;
	matScale.m[1][1] = worldTransform_.scale_.y;
	matScale.m[2][2] = worldTransform_.scale_.z;
	matScale.m[3][3] = 1;


	//合成用回転行列を宣言
	Matrix4 matRot;

	//各軸用回転行列を宣言
	Matrix4 matRotX, matRotY, matRotZ;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			if (i == j)
			{
				matRotX.m[i][j] = 1;
				matRotY.m[i][j] = 1;
				matRotZ.m[i][j] = 1;
			}
			else
			{
				matRotX.m[i][j] = 0;
				matRotY.m[i][j] = 0;
				matRotZ.m[i][j] = 0;
			}
		}
	matRotZ.m[0][0] = cos(worldTransform_.rotation_.z);
	matRotZ.m[0][1] = sin(worldTransform_.rotation_.z);
	matRotZ.m[1][0] = -sin(worldTransform_.rotation_.z);
	matRotZ.m[1][1] = cos(worldTransform_.rotation_.z);

	matRotX.m[1][1] = cos(worldTransform_.rotation_.x);
	matRotX.m[1][2] = sin(worldTransform_.rotation_.x);
	matRotX.m[2][1] = -sin(worldTransform_.rotation_.x);
	matRotX.m[2][2] = cos(worldTransform_.rotation_.x);

	matRotY.m[0][0] = cos(worldTransform_.rotation_.y);
	matRotY.m[0][2] = -sin(worldTransform_.rotation_.y);
	matRotY.m[2][0] = sin(worldTransform_.rotation_.y);
	matRotY.m[2][2] = cos(worldTransform_.rotation_.y);

		//各軸の回転行列を合成
	matRotZ *= matRotX;
	matRotZ *= matRotY;
	matRot = matRotZ;


	//平行移動行列を宣言
	Matrix4 matTrans = MathUtility::Matrix4Identity();
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			if (i == j)
				matTrans.m[i][j] = 1;
			else
				matTrans.m[i][j] = 0;
	matTrans.m[3][0] = worldTransform_.translation_.x;
	matTrans.m[3][1] = worldTransform_.translation_.y;
	matTrans.m[3][2] = worldTransform_.translation_.z;


	//単位行列を代入
	Matrix4 mat1;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			if (i == j)
				mat1.m[i][j] = 1;
			else
				mat1.m[i][j] = 0;

	worldTransform_.matWorld_ = mat1;

	//スケールを乗算して代入
	worldTransform_.matWorld_ *= matScale;
	//回転を乗算して代入
	worldTransform_.matWorld_ *= matRot;
	//移動量を乗算して代入
	worldTransform_.matWorld_ *= matTrans;

	//行列の転送
	worldTransform_.TransferMatrix();
}

void GameScene::Update() {
	//デバッグカメラの更新
	debugCamera_->Update();
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
	model_->Draw(worldTransform_, debugCamera_->GetViewProjection(), textureHandle_);

	//ライン描画が参照するビュープロジェクションを指定する(アドレス渡し)
	for (int i = 0; i < 102; i++)
	{
		if (i < 51)
			PrimitiveDrawer::GetInstance()->DrawLine3d(vec1[i], vec2[i], color[0]);
		else
			PrimitiveDrawer::GetInstance()->DrawLine3d(vec1[i], vec2[i], color[1]);
	}

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
