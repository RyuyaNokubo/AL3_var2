#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "AxisIndicator.h"
#include "PrimitiveDrawer.h"
#include <random>

//関数
Matrix4 setScale(const WorldTransform a)
{
	Matrix4 b = MathUtility::Matrix4Identity();

	b.m[0][0] = a.scale_.x;
	b.m[1][1] = a.scale_.y;
	b.m[2][2] = a.scale_.z;
	b.m[3][3] = 1;
	return b;
}

Matrix4 setRot(const WorldTransform a)
{
	Matrix4 x, y, z;
	x = MathUtility::Matrix4Identity();
	y = MathUtility::Matrix4Identity();
	z = MathUtility::Matrix4Identity();

	z.m[0][0] = cos(a.rotation_.z);
	z.m[0][1] = sin(a.rotation_.z);
	z.m[1][0] = -sin(a.rotation_.z);
	z.m[1][1] = cos(a.rotation_.z);

	x.m[1][1] = cos(a.rotation_.x);
	x.m[1][2] = sin(a.rotation_.x);
	x.m[2][1] = -sin(a.rotation_.x);
	x.m[2][2] = cos(a.rotation_.x);

	y.m[0][0] = cos(a.rotation_.y);
	y.m[0][2] = -sin(a.rotation_.y);
	y.m[2][0] = sin(a.rotation_.y);
	y.m[2][2] = cos(a.rotation_.y);

	//各軸の回転行列を合成
	z *= x;
	z *= y;
	return z;
}

Matrix4 setRotX(const WorldTransform a)
{
	Matrix4 x = MathUtility::Matrix4Identity();

	x.m[1][1] = cos(a.rotation_.x);
	x.m[1][2] = sin(a.rotation_.x);
	x.m[2][1] = -sin(a.rotation_.x);
	x.m[2][2] = cos(a.rotation_.x);

	return x;
}

Matrix4 setRotY(const WorldTransform a)
{
	Matrix4 y = MathUtility::Matrix4Identity();

	y.m[0][0] = cos(a.rotation_.y);
	y.m[0][2] = -sin(a.rotation_.y);
	y.m[2][0] = sin(a.rotation_.y);
	y.m[2][2] = cos(a.rotation_.y);

	return y;
}

Matrix4 setRotZ(const WorldTransform a)
{
	Matrix4 z = MathUtility::Matrix4Identity();

	z.m[0][0] = cos(a.rotation_.z);
	z.m[0][1] = sin(a.rotation_.z);
	z.m[1][0] = -sin(a.rotation_.z);
	z.m[1][1] = cos(a.rotation_.z);

	return z;
}

Matrix4 setTrans(const WorldTransform a)
{
	Matrix4 b = MathUtility::Matrix4Identity();

	b.m[3][0] = a.translation_.x;
	b.m[3][1] = a.translation_.y;
	b.m[3][2] = a.translation_.z;

	return b;
}

float rad(float a)
{
	return 3.141592 * a / 180;
}

float MinNum(float a, float aMin)
{
	if (a < aMin)
	{
		return aMin;
	}
	return a;
}

float MaxNum(float a, float aMax)
{
	if (a > aMax)
	{
		return aMax;
	}
	return a;
}

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

	//乱数シード生成器
	std::random_device seed_gen;
	//メルセンヌ・ツイスターの乱数エンジン
	std::mt19937_64 engine(seed_gen());
	//乱数範囲の指定
	std::uniform_real_distribution<float> posDist(-10.0f, 10.0f);
	std::uniform_real_distribution<float> rotDist(0.0f, PI);
	std::uniform_real_distribution<float> scaleDist(1.0f, 1.0f);

	//ワールドトランスフォームの初期化
	for (WorldTransform& worldTransform : worldTransform_)
	{
		worldTransform.Initialize();
	}

	////カメラ視点座標を設定
	//viewProjection_.eye = { 0,0,-10 };

	////カメラ注視点座標を設定
	//viewProjection_.target = { 10,0,0 };

	////カメラ上方向ベクトルを設定
	//viewProjection_.up = { cosf(PI / 4.0f),sinf(PI / 4.0f),0.0f };

	//カメラ垂直方向視野角を設定
	viewProjection_.fovAngleY = rad(10.0f);

	//アスペクト比を設定
	viewProjection_.aspectRatio = 1.0f;

	//ニアクリップ距離を設定
	viewProjection_.nearZ = 52.0f;
	//ファークリップ距離を設定
	viewProjection_.farZ = 53.0f;

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();

	//デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	//軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	//軸方向表示が参照するビュープロジェクションを指定する(アドレス渡し)
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);

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

	for (WorldTransform& worldTransform : worldTransform_)
	{
		//X,Y,Z方向のスケーリングを設定
		worldTransform.scale_ = { scaleDist(engine),scaleDist(engine),scaleDist(engine) };

		//X,Y,Z軸回りの回転角を設定
		worldTransform.rotation_ = { rotDist(engine),rotDist(engine) ,rotDist(engine) };

		//X,Y,Z軸回りの平行移動を設定
		worldTransform.translation_ = { posDist(engine),posDist(engine) ,posDist(engine) };


		//スケーリング行列を宣言
		Matrix4 matScale = setScale(worldTransform);

		//合成用回転行列を宣言
		Matrix4 matRot = setRot(worldTransform);

		//平行移動行列を宣言
		Matrix4 matTrans = setTrans(worldTransform);

		//単位行列を代入
		//Matrix4 mat1;
		//for (int i = 0; i < 4; i++)
		//	for (int j = 0; j < 4; j++)
		//		if (i == j)
		//			mat1.m[i][j] = 1;
		//		else
		//			mat1.m[i][j] = 0;

		worldTransform.matWorld_ = MathUtility::Matrix4Identity();

		//スケールを乗算して代入
		worldTransform.matWorld_ *= matScale;
		//回転を乗算して代入
		worldTransform.matWorld_ *= matRot;
		//移動量を乗算して代入
		worldTransform.matWorld_ *= matTrans;

		//行列の転送
		worldTransform.TransferMatrix();
	}
}

void GameScene::Update() {
	//デバッグカメラの更新
	debugCamera_->Update();

	////視点の移動ベクトル
	//Vector3 moveZ = { 0,0,0 };
	////注視点の移動ベクトル
	//Vector3 moveX = { 0,0,0 };

	////視点、注視点の移動速さ
	//const float speed = 0.2f;

	////押した方向で移動ベクトルを変更
	//if (input_->PushKey(DIK_W))
	//{
	//	moveZ.z += speed;
	//}
	//else if (input_->PushKey(DIK_S))
	//{
	//	moveZ.z -= speed;
	//}
	//else if (input_->PushKey(DIK_LEFT))
	//{
	//	moveX.x -= speed;
	//}
	//else if (input_->PushKey(DIK_RIGHT))
	//{
	//	moveX.x += speed;
	//}

	////視点移動(ベクトルの加算)
	//viewProjection_.eye += moveZ;

	////注視点移動(ベクトルの加算)
	//viewProjection_.target += moveX;

	////上方向回転処理
	////上方向の回転速さ[ラジアン/frame]
	//const float kUpRotSpeed = 0.05f;

	////押した方向で移動ベクトルを変更
	//if (input_->PushKey(DIK_SPACE))
	//{
	//	viewAngle += kUpRotSpeed;
	//	viewAngle = fmodf(viewAngle, PI * 2.0f);
	//}

	////上方向ベクトルを計算(半径1の円周上の座標)
	//viewProjection_.up = { cosf(viewAngle),sinf(viewAngle),0.0f };

	//Fov変更処理
	{
		//上キーで視野角が広がる
		if (input_->PushKey(DIK_UP))
		{
			viewProjection_.fovAngleY += 0.01f;
			viewProjection_.fovAngleY = MaxNum(viewProjection_.fovAngleY, PI);
		}
		//下キーで視野角が狭まる
		else if (input_->PushKey(DIK_DOWN))
		{
			viewProjection_.fovAngleY -= 0.01f;
			viewProjection_.fovAngleY = MinNum(viewProjection_.fovAngleY, 0.01f);
		}
	}

	//クリップ距離変更処理
	{
		//A,Dキーでニアクリップ距離を増減
		if (input_->PushKey(DIK_A))
		{
			viewProjection_.nearZ+=0.1f;
		}
		else if (input_->PushKey(DIK_D))
		{
			viewProjection_.nearZ-=0.1f;
		}
	}

	//行列の再計算
	viewProjection_.UpdateMatrix();

	//デバッグ用表示
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
	for (WorldTransform& worldTransform : worldTransform_)
	{
		model_->Draw(worldTransform, viewProjection_, textureHandle_);
	}

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
