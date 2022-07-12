#pragma once
#include"WorldTransform.h"
#include"DebugText.h"
#include"mine.h"

/// <summary>
/// レールカメラ
/// </summary>
class RailCamera
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="worldTransform">ワールドトランスフォーム</param>
	/// <param name="rad">ラジアン</param>
	void Initialize(WorldTransform worldTransform,Vector3 rad);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	ViewProjection getViewProjection();

	WorldTransform getWorldTransform();

private:
	//ワールド変換データ
	WorldTransform worldTransform_;
	//ビュープロジェクション
	ViewProjection viewProjection_;
	//デバッグテキスト
	DebugText* debugText_ = nullptr;
};