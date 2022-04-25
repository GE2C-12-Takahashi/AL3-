﻿#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

using namespace DirectX;

GameScene::GameScene() {}

GameScene::~GameScene() { 

	delete model_;
	
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	//ファイルを指定してテクスチャを読み込む
	textureModelHandle_ = TextureManager::Load("Xion.png");
	//3Dモデルの生成
	model_ = Model::Create();

	//乱数シード生成器
	std::random_device seed_gen;
	//メルセンヌ・ツイスター
	std::mt19937_64 engine(seed_gen());
	//乱数範囲(回転角用)
	std::uniform_real_distribution<float> rotDist(0.0f, XM_2PI);
	//乱数範囲(座標用)
	std::uniform_real_distribution<float> posDist(-10.0f,10.0f);

	for (size_t i = 0; i < _countof(worldTransform_); i++) {

		// X,Y,Z 方向のスケーリングを設定
		worldTransform_[i].scale_ = {1.5f, 1.5f, 1.5f};

		// X,Y,Z 軸周りの回転角を設定
		worldTransform_[i].rotation_ = {rotDist(engine), rotDist(engine), rotDist(engine)};
		//度数法でやるのならこっち↓
		//worldTransform_[i].rotation_ = {XMConvertToRadians(45.0f), XMConvertToRadians(45.0f), 0.0f};

		// X,Y,Z 軸周りの平行移動を設定
		worldTransform_[i].translation_ = {posDist(engine), posDist(engine), posDist(engine)};

		//ワールドトランスフォームの初期化
		worldTransform_[i].Initialize();
	}

	//カメラ垂直方向視野角を設定
	viewProjection_.fovAngleY = XMConvertToRadians(10.0f);

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();
}

void GameScene::Update() {

#pragma region //視点移動処理//

	//視点の移動ベクトル
	//XMFLOAT3 move = {0, 0, 0};

	////視点の移動速度
	//const float kEyeSpeed = 0.2f;

	////押した方向で移動ベクトルを変更
	//if (input_->PushKey(DIK_W)) {
	//	move = {0, 0, kEyeSpeed};
	//} else if (input_->PushKey(DIK_S)) {
	//	move = {0, 0, -kEyeSpeed};
	//}

	////視点移動(ベクトルの加算)
	//viewProjection_.eye.x += move.x;
	//viewProjection_.eye.y += move.y;
	//viewProjection_.eye.z += move.z;

	////行列の再計算
	//viewProjection_.UpdateMatrix();

	////デバッグ用表示
	//debugText_->SetPos(50, 50);
	//debugText_->Printf(
	//  "eye:(%f,%f,%f)", viewProjection_.eye.x, viewProjection_.eye.y, viewProjection_.eye.z);
	//


#pragma endregion

#pragma region //注視点移動処理//

	//
	////注視点の移動ベクトル
	//move = {0, 0, 0};

	////注視点の移動速度
	//const float kTargetSpeed = 0.2f;

	////押した方向で移動ベクトルを変更
	//if (input_->PushKey(DIK_LEFT)) {
	//	move = {-kTargetSpeed, 0, 0};
	//} else if (input_->PushKey(DIK_RIGHT)) {
	//	move = {kTargetSpeed, 0, 0};
	//}

	////注視点移動(ベクトルの加算)
	//viewProjection_.target.x += move.x;
	//viewProjection_.target.y += move.y;
	//viewProjection_.target.z += move.z;

	////行列の再計算
	//viewProjection_.UpdateMatrix();

	////デバッグ用表示
	//debugText_->SetPos(50, 70);
	//debugText_->Printf(
	//  "target:(%f,%f,%f)", viewProjection_.target.x, viewProjection_.target.y,
	//  viewProjection_.target.z);


#pragma endregion

#pragma region //上方向回転処理//

	////上方向の回転速度(ラジアン/Frame)
	//const float kUpRotSpeed = 0.05f;

	////押した方向で移動ベクトルを変更
	//if (input_->PushKey(DIK_SPACE)) {
	//	viewAngle += kUpRotSpeed;
	//	//2πを超えたら0に戻す
	//	viewAngle = fmodf(viewAngle, XM_2PI);
	//}

	////上方向ベクトルを計算(半径1の円周上の座標)
	//viewProjection_.up = {cosf(viewAngle), sinf(viewAngle), 0.0f};

	////行列の再計算
	//viewProjection_.UpdateMatrix();

	////デバッグ用表示
	//debugText_->SetPos(50, 90);
	//debugText_->Printf(
	//  "up:(%f,%f,%f)", viewProjection_.up.x, viewProjection_.up.y, viewProjection_.up.z);

#pragma endregion

#pragma region //垂直方向視野角の変更処理//

	//Fov変更処理
	
	//上キーで視野角が広がる
	if (input_->PushKey(DIK_UP)) {
		viewProjection_.fovAngleY += 0.01f;
		viewProjection_.fovAngleY = min(viewProjection_.fovAngleY, XM_PI);
	}
	//下キーで視野角が狭まる
	else if (input_->PushKey(DIK_DOWN)) {
		viewProjection_.fovAngleY -= 0.01f;
		viewProjection_.fovAngleY =max(viewProjection_.fovAngleY, 0.01f);
	}

	//行列の再計算
	viewProjection_.UpdateMatrix();

	//デバッグ用表示
	debugText_->SetPos(50, 110);
	debugText_->Printf("fovAngleY(Degree):%f", XMConvertToDegrees(viewProjection_.fovAngleY));


#pragma endregion
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

	for (size_t i = 0; i < _countof(worldTransform_); i++) {

		model_->Draw(worldTransform_[i], viewProjection_, textureModelHandle_);
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
