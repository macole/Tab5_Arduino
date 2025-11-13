#include <M5Unified.h>
#include <M5GFX.h>
M5Canvas canvas(&M5.Display);      // HUD用
M5Canvas ballSprite(&M5.Display);  // ボール用
m5::imu_data_t imuData;

// ボールの状態
float ballX = 0;  // ボールのX座標（画面中央から開始）
float ballY = 0;  // ボールのY座標（画面中央から開始）
float velX = 0;   // X軸方向の速度
float velY = 0;   // Y軸方向の速度
float prevBallX = -1;  // 前フレーム位置（初期未設定）
float prevBallY = -1;  // 前フレーム位置（初期未設定）
int   frameCounter = 0;
const int HUD_UPDATE_INTERVAL = 5;  // HUDは5フレームに1回更新

const int BALL_RADIUS = 19;  // ボールの半径（ピクセル）
const float GRAVITY = 9.8;   // 重力加速度（m/s²）
const float DAMPING = 0.95;  // 減衰係数（摩擦）
// 壁接触のデバウンス用フラグ
bool contactLeft = false;
bool contactRight = false;
bool contactTop = false;
bool contactBottom = false;

void setup() {
  M5.begin();
  M5.Display.setRotation(3);
  
  // 静的背景（タイトル）は直接表示へ描画（1回のみ）
  M5.Display.clear(BLACK);
  M5.Display.setTextDatum(top_center);
  M5.Display.setFont(&fonts::FreeMonoBold12pt7b);
  M5.Display.setTextColor(WHITE, BLACK);
  M5.Display.drawString("IMU Ball Control", M5.Display.width() / 2, 10);

  // HUDスプライト初期化（小領域）
  canvas.setColorDepth(16);
  canvas.createSprite(240, 180);
  canvas.setTextDatum(top_left);
  canvas.setFont(&fonts::FreeMonoBold12pt7b);
  canvas.setTextColor(YELLOW, BLACK);

  // ボールスプライト初期化（直径+マージン）
  ballSprite.setColorDepth(16);
  ballSprite.createSprite(BALL_RADIUS * 2 + 2, BALL_RADIUS * 2 + 2);
  
  // ボールを画面中央に配置
  ballX = M5.Display.width() / 2;
  ballY = M5.Display.height() / 2;
}

void loop() {
  // IMUデータを取得
  M5.Imu.update();
  imuData = M5.Imu.getImuData();

  // 最小更新矩形は物理演算と衝突処理の後に計算する

  // 加速度を使用して速度を更新
  // Y軸: 左右の動き（右が正）
  // X軸: 上下の動き（上が正）
  // 実際のデバイスの動きを反映させるため、符号を調整
  velX -= imuData.accel.y * 3.5;  // 加速度を積分して速度に変換
  velY += imuData.accel.x * 3.5;  // Y軸は上下反転（上が正になるように）
  
  // Z軸（重力）はY方向の動きに影響（デバイスを傾けるとボールが動く）
  //velY += (imuData.accel.z + GRAVITY) * 0.05;  // 重力補正

  // 減衰を適用（摩擦）
  velX *= DAMPING;
  velY *= DAMPING;

  // 速度を使って位置を更新
  ballX += velX;
  ballY += velY;

  // 画面端でボールがはみ出さないように境界チェック（接触立ち上がりでのみ音を鳴らす）
  // X軸（左右）
  if (ballX < BALL_RADIUS) {
    ballX = BALL_RADIUS;
    velX = -velX * 0.8;  // 反発（エネルギー損失あり）
    if (!contactLeft) {
      M5.Speaker.tone(2000, 60);
    }
    contactLeft = true;
  } else {
    contactLeft = false;
  }

  if (ballX > M5.Display.width() - BALL_RADIUS) {
    ballX = M5.Display.width() - BALL_RADIUS;
    velX = -velX * 0.8;
    if (!contactRight) {
      M5.Speaker.tone(2000, 60);
    }
    contactRight = true;
  } else {
    contactRight = false;
  }

  // Y軸（上下、タイトル部分を避ける）
  if (ballY < BALL_RADIUS + 50) {
    ballY = BALL_RADIUS + 50;
    velY = -velY * 0.8;
    if (!contactTop) {
      M5.Speaker.tone(4000, 60);
    }
    contactTop = true;
  } else {
    contactTop = false;
  }

  if (ballY > M5.Display.height() - BALL_RADIUS) {
    ballY = M5.Display.height() - BALL_RADIUS;
    velY = -velY * 0.8;
    if (!contactBottom) {
      M5.Speaker.tone(4000, 60);
    }
    contactBottom = true;
  } else {
    contactBottom = false;
  }

  // 前回位置と今回位置を同時に更新する最小矩形を計算（十分な余白を確保）
  const int margin = BALL_RADIUS + 3;
  int left   = (int)floorf((prevBallX >= 0 ? fminf(prevBallX, ballX) : ballX) - margin);
  int top    = (int)floorf((prevBallY >= 0 ? fminf(prevBallY, ballY) : ballY) - margin);
  int right  = (int)ceilf((prevBallX >= 0 ? fmaxf(prevBallX, ballX) : ballX) + margin);
  int bottom = (int)ceilf((prevBallY >= 0 ? fmaxf(prevBallY, ballY) : ballY) + margin);

  if (left < 0) left = 0;
  if (top < 50) top = 50;  // タイトル領域を避ける
  if (right > (int)M5.Display.width() - 1) right = M5.Display.width() - 1;
  if (bottom > (int)M5.Display.height() - 1) bottom = M5.Display.height() - 1;

  int regionW = right - left + 1;
  int regionH = bottom - top + 1;

  // 最小矩形サイズでスプライトを作成し直し、一括描画→転送（チラつき防止）
  ballSprite.deleteSprite();
  ballSprite.setColorDepth(16);
  ballSprite.createSprite(regionW, regionH);
  ballSprite.fillScreen(BLACK);
  int cx = (int)roundf(ballX) - left;
  int cy = (int)roundf(ballY) - top;
  ballSprite.fillCircle(cx, cy, BALL_RADIUS, CYAN);
  ballSprite.fillCircle(cx, cy, 3, WHITE);
  ballSprite.pushSprite(&M5.Display, left, top);
  prevBallX = ballX;
  prevBallY = ballY;

  // デバッグ情報を表示（右下に小さく）

  // HUDは一定間隔で小領域のみ更新・転送
  if ((frameCounter % HUD_UPDATE_INTERVAL) == 0) {
    canvas.clear(BLACK);
    canvas.setCursor(0, 0);
    canvas.printf(" X:%5.1f\n", ballX);
    canvas.printf(" Y:%5.1f\n", ballY);
    canvas.printf(" vX:%5.1f\n", velX);
    canvas.printf(" vY:%5.1f\n", velY);

    canvas.printf(" AccX = %6.2f  \n", imuData.accel.x);
    canvas.printf(" AccY = %6.2f  \n", imuData.accel.y);
    canvas.printf(" AccZ = %6.2f  \n\n", imuData.accel.z);

    canvas.printf(" GyrX = %6.2f  \n", imuData.gyro.x);
    canvas.printf(" GyrY = %6.2f  \n", imuData.gyro.y);
    canvas.printf(" GyrZ = %6.2f  \n", imuData.gyro.z);
    canvas.pushSprite(&M5.Display, 0, 100);
  }
  frameCounter++;

  delay(20);  // 高速更新
} 