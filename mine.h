#pragma once
#include"Matrix4.h"
#include"WorldTransform.h"
#include"AxisIndicator.h"

const float PI = 3.141592;

//ä÷êî
Matrix4 setScale(const WorldTransform a);

Matrix4 setRot(const WorldTransform a);

Matrix4 setRotX(const WorldTransform a);

Matrix4 setRotY(const WorldTransform a);

Matrix4 setRotZ(const WorldTransform a);

Matrix4 setTrans(const WorldTransform a);

float rad(float a);

float MinNum(float a, float aMin);

float MaxNum(float a, float aMax);