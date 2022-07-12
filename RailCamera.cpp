#include"RailCamera.h"

void RailCamera::Initialize(WorldTransform worldTransform, Vector3 rad)
{
	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	//ワールドトランスフォームの初期設定
	worldTransform_ = worldTransform;
	worldTransform_.rotation_ = rad;
	//ビュープロジェクションの初期化
	viewProjection_.farZ = 100.0f;
	viewProjection_.Initialize();

	debugText_ = DebugText::GetInstance();
}

void RailCamera::Update()
{
	Vector3 move = { 0.0f,0.0f,-0.1f };
	Vector3 Rmove = {rad(0),rad(0),rad(0)};
	worldTransform_.translation_ += move;
	worldTransform_.rotation_ += Rmove;

	//ワールドトランスフォームの更新
	Matrix4 matScale = setScale(worldTransform_);
	Matrix4 matRot = setRot(worldTransform_);
	Matrix4 matTrans = setTrans(worldTransform_);

	worldTransform_.matWorld_ = MathUtility::Matrix4Identity();
	matScale *= matRot;
	matScale *= matTrans;
	worldTransform_.matWorld_ *= matScale;

	worldTransform_.TransferMatrix();

	viewProjection_.eye = worldTransform_.translation_;
	//ワールド前方ベクトル
	Vector3 forward(0, 0, 1);
	//レールカメラの回転を反映
	forward = vec_mat(forward, worldTransform_);
	//視点から前方に適当な距離進んだ位置が注視点
	viewProjection_.target = viewProjection_.eye + forward;
	//ワールド上方ベクトル
	Vector3 up(0, 1, 0);
	//レールカメラの回転を反映(レールカメラの上方ベクトル)
	viewProjection_.up = vec_mat(up, worldTransform_);
	//ビュープロジェクションを更新
	viewProjection_.UpdateMatrix();
	viewProjection_.TransferMatrix();

	debugText_->SetPos(50, 150);
	debugText_->Printf("railCamera:(% f, % f, % f)", viewProjection_.eye.x, viewProjection_.eye.y, viewProjection_.eye.z);
}

ViewProjection RailCamera::getViewProjection()
{
	return viewProjection_;
}

WorldTransform RailCamera::getWorldTransform()
{
	return worldTransform_;
}