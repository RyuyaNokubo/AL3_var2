#include"RailCamera.h"

void RailCamera::Initialize(WorldTransform worldTransform, Vector3 rad)
{
	//���[���h�g�����X�t�H�[���̏�����
	worldTransform_.Initialize();
	//���[���h�g�����X�t�H�[���̏����ݒ�
	worldTransform_ = worldTransform;
	worldTransform_.rotation_ = rad;
	//�r���[�v���W�F�N�V�����̏�����
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

	//���[���h�g�����X�t�H�[���̍X�V
	Matrix4 matScale = setScale(worldTransform_);
	Matrix4 matRot = setRot(worldTransform_);
	Matrix4 matTrans = setTrans(worldTransform_);

	worldTransform_.matWorld_ = MathUtility::Matrix4Identity();
	matScale *= matRot;
	matScale *= matTrans;
	worldTransform_.matWorld_ *= matScale;

	worldTransform_.TransferMatrix();

	viewProjection_.eye = worldTransform_.translation_;
	//���[���h�O���x�N�g��
	Vector3 forward(0, 0, 1);
	//���[���J�����̉�]�𔽉f
	forward = vec_mat(forward, worldTransform_);
	//���_����O���ɓK���ȋ����i�񂾈ʒu�������_
	viewProjection_.target = viewProjection_.eye + forward;
	//���[���h����x�N�g��
	Vector3 up(0, 1, 0);
	//���[���J�����̉�]�𔽉f(���[���J�����̏���x�N�g��)
	viewProjection_.up = vec_mat(up, worldTransform_);
	//�r���[�v���W�F�N�V�������X�V
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