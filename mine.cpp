#include"mine.h"


//�֐�
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

	//�e���̉�]�s�������
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

void setWorldTransform(WorldTransform a)
{
	Matrix4 matScale = setScale(a);
	Matrix4 matRot = setRot(a);
	Matrix4 matTrans = setTrans(a);

	a.matWorld_ = MathUtility::Matrix4Identity();
	matScale *= matRot;
	matScale *= matTrans;
	a.matWorld_ *= matScale;
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

Vector3 vec_mat(Vector3 a, WorldTransform b)
{
	Vector3 c;
	c.x = a.x * b.matWorld_.m[0][0] +
		a.y * b.matWorld_.m[1][0] +
		a.z * b.matWorld_.m[2][0];
	c.y = a.x * b.matWorld_.m[0][1] +
		a.y * b.matWorld_.m[1][1] +
		a.z * b.matWorld_.m[2][1];
	c.z = a.x * b.matWorld_.m[0][2] +
		a.y * b.matWorld_.m[1][2] +
		a.z * b.matWorld_.m[2][2];

	return c;
}