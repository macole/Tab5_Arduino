#include <M5Unified.h>
#include <M5GFX.h>

m5::imu_data_t imuData;

// スプライト（オフスクリーンバッファ）
LGFX_Sprite sprite(&M5.Display);

// ボールの状態
float ballX = 0;  // ボールのX座標（画面中央から開始）
float ballY = 0;  // ボールのY座標（画面中央から開始）
float velX = 0;   // X軸方向の速度
float velY = 0;   // Y軸方向の速度

const int BALL_RADIUS = 15;  // ボールの半径（ピクセル）
const float GRAVITY = 9.8;   // 重力加速度（m/s²）
const float DAMPING = 0.95;  // 減衰係数（摩擦）

void setup() {
  M5.begin();
  M5.Display.setRotation(3);
  
  // スプライトを画面サイズで初期化
  sprite.createSprite(M5.Display.width(), M5.Display.height());
  sprite.setTextDatum(top_center);
  sprite.setFont(&fonts::FreeMonoBold12pt7b);
  
  // ボールを画面中央に配置
  ballX = M5.Display.width() / 2;
  ballY = M5.Display.height() / 2;
}

void loop() {
  // IMUデータを取得
  M5.Imu.update();
  imuData = M5.Imu.getImuData();

  // 加速度を使用して速度を更新
  // X軸: 左右の動き（右が正）
  // Y軸: 上下の動き（上が正）
  // 実際のデバイスの動きを反映させるため、符号を調整
  velX += imuData.accel.x * 0.1;  // 加速度を積分して速度に変換
  velY -= imuData.accel.y * 0.1;  // Y軸は上下反転（上が正になるように）
  
  // Z軸（重力）はY方向の動きに影響（デバイスを傾けるとボールが動く）
  velY += (imuData.accel.z + GRAVITY) * 0.05;  // 重力補正

  // 減衰を適用（摩擦）
  velX *= DAMPING;
  velY *= DAMPING;

  // 速度を使って位置を更新
  ballX += velX;
  ballY += velY;

  // 画面端でボールがはみ出さないように境界チェック
  // X軸（左右）
  if (ballX < BALL_RADIUS) {
    ballX = BALL_RADIUS;
    velX = -velX * 0.8;  // 反発（エネルギー損失あり）
  } else if (ballX > M5.Display.width() - BALL_RADIUS) {
    ballX = M5.Display.width() - BALL_RADIUS;
    velX = -velX * 0.8;
  }

  // Y軸（上下、タイトル部分を避ける）
  if (ballY < BALL_RADIUS + 50) {
    ballY = BALL_RADIUS + 50;
    velY = -velY * 0.8;
  } else if (ballY > M5.Display.height() - BALL_RADIUS) {
    ballY = M5.Display.height() - BALL_RADIUS;
    velY = -velY * 0.8;
  }

  // === スプライトバッファに描画 ===
  // スプライトを黒でクリア（全画面をリセット）
  sprite.fillScreen(BLACK);
  
  // タイトルを描画
  sprite.setTextColor(WHITE, BLACK);
  sprite.drawString("IMU Ball Control", M5.Display.width() / 2, 10);
  
  // ボールを描画
  sprite.fillCircle((int)ballX, (int)ballY, BALL_RADIUS, CYAN);
  
  // ボールの中心に点を描画（視認性向上）
  sprite.fillCircle((int)ballX, (int)ballY, 3, WHITE);
  
  // デバッグ情報を表示（右下に小さく）
  sprite.setCursor(M5.Display.width() - 150, M5.Display.height() - 60);
  sprite.setTextColor(YELLOW, BLACK);
  sprite.printf("X:%5.1f\n", ballX);
  sprite.printf("Y:%5.1f\n", ballY);
  sprite.printf("vX:%5.1f\n", velX);
  sprite.printf("vY:%5.1f", velY);

  // スプライトバッファを画面に一括転送（チラツキなし）
  sprite.pushSprite(0, 0);

  delay(50);  // 50ms間隔で更新（20fps）
} 