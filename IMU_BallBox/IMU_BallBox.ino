/*
 * IMU Ball Box - M5Stack Tab5用ボール物理シミュレーション
 * 
 * デバイスを傾けると、IMUの加速度データに基づいてボールが動きます。
 * スプライトを使用した高速描画で滑らかなアニメーションを実現します。
 */

#include <M5Unified.h>

// ============================================================================
// 定数定義
// ============================================================================
// ボール設定
const int BALL_RADIUS = 20;              // ボールの半径（ピクセル）
const int BALL_CENTER_DOT_RADIUS = 3;   // ボール中心の点の半径（ピクセル）

// 物理パラメータ
const float GRAVITY = 9.8;               // 重力加速度（m/s²）
const float DAMPING = 0.95;              // 減衰係数（摩擦）
const float ACCEL_SCALE = 2.0;           // 加速度スケール（感度調整）
const float GRAVITY_SCALE = 0.1;         // 重力補正スケール
const float BOUNCE_DAMPING = 0.8;        // 壁反発時のエネルギー損失率
const float VELOCITY_LIMIT = 20.0;       // 速度の上限

// UI設定
const int TITLE_Y_OFFSET = 10;           // タイトルのY座標オフセット
const int TITLE_AREA_HEIGHT = 50;        // タイトルエリアの高さ（ボールが入らない領域）
const int DEBUG_INFO_MARGIN = 4;        // デバッグ情報の文字周りのマージン（ドット）
const int DEBUG_INFO_X = 0;            // デバッグ情報のX座標オフセット（左端から）
const int DEBUG_INFO_Y = TITLE_AREA_HEIGHT + 0;  // デバッグ情報のY座標オフセット（タイトル下）
const int DEBUG_INFO_WIDTH = 220;        // デバッグ情報の幅（マージン含む、余裕を持たせる）
const int DEBUG_INFO_HEIGHT = 100;       // デバッグ情報の高さ（4行分のテキスト用、マージン含む、余裕を持たせる）

// 色定義
const uint16_t COLOR_BACKGROUND = BLACK;
const uint16_t COLOR_TITLE = WHITE;
const uint16_t COLOR_BALL = CYAN;
const uint16_t COLOR_BALL_CENTER = WHITE;
const uint16_t COLOR_DEBUG = YELLOW;

// デバッグ設定
const bool DEBUG_ENABLED = false;        // エラー出力の有効/無効
const bool SHOW_DEBUG_INFO = true;       // 画面にセンサー値を表示
const bool SERIAL_DEBUG = false;         // シリアルにデバッグ情報を出力

// 描画設定
const int UPDATE_INTERVAL_MS = 16;        // 更新間隔（16ms = 60fps）

// IMU更新設定
const unsigned long IMU_UPDATE_INTERVAL_US = 5000;  // IMU更新間隔（5ms = 200Hz）
unsigned long lastIMUUpdateTime = 0;     // 前回のIMU更新時刻
int imuUpdateCount = 0;                  // IMU更新回数（デバッグ用）
int imuUpdateFailCount = 0;              // IMU更新失敗回数（デバッグ用）

// ============================================================================
// グローバル変数
// ============================================================================
// スプライト（オフスクリーンバッファ）
LGFX_Sprite sprite(&M5.Display);

// IMUデータ
m5::imu_data_t imuData;

// ボールの状態
struct BallState {
  float x;           // X座標
  float y;           // Y座標
  float prevX;       // 前回のX座標（部分更新用）
  float prevY;       // 前回のY座標（部分更新用）
  float velX;        // X軸方向の速度
  float velY;        // Y軸方向の速度
} ball;

// ============================================================================
// 関数プロトタイプ
// ============================================================================
void initializeHardware(void);
void initializeSprite(void);
void initializeIMU(void);
void initializeBall(void);
void drawInitialScreen(void);
void updatePhysics(void);
void checkBoundaries(void);
void drawBall(void);
void drawDebugInfo(void);

// ============================================================================
// 初期化関数
// ============================================================================
/**
 * ハードウェアの初期化
 */
void initializeHardware(void) {
  // シリアル通信はエラー出力用にのみ使用（最小限の出力）
  Serial.begin(115200);
  delay(100);
  
  M5.begin();
  M5.Display.setRotation(3);
  
  // 初期化メッセージを表示
  M5.Display.fillScreen(BLACK);
  M5.Display.setTextColor(WHITE);
  M5.Display.setFont(&fonts::FreeMonoBold12pt7b);
  M5.Display.drawString("Initializing...", M5.Display.width() / 2, M5.Display.height() / 2);
}

/**
 * スプライトの初期化（1280x720、PSRAM使用）
 */
void initializeSprite(void) {
  sprite.setPsram(true);  // 大容量のためPSRAMを使用（1.8MB）
  
  if (!sprite.createSprite(M5.Display.width(), M5.Display.height())) {
    Serial.println("ERROR: Failed to create sprite!");
    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
    if (psramFound()) {
      Serial.printf("Free PSRAM: %d bytes\n", ESP.getFreePsram());
    }
    M5.Display.drawString("Sprite Error!", M5.Display.width() / 2, M5.Display.height() / 2 + 30);
    while(1) delay(1000);
  }
  
  sprite.setTextDatum(top_center);
  sprite.setFont(&fonts::FreeMonoBold12pt7b);
}

/**
 * IMUの初期化
 */
void initializeIMU(void) {
  if (!M5.Imu.begin()) {
    Serial.println("ERROR: IMU initialization failed!");
  } else {
    if (M5.Imu.update()) {
      imuData = M5.Imu.getImuData();
    }
    lastIMUUpdateTime = micros();
  }
}

/**
 * ボールの初期位置を設定
 */
void initializeBall(void) {
  ball.x = M5.Display.width() / 2.0f;
  ball.y = M5.Display.height() / 2.0f;
  ball.prevX = ball.x;
  ball.prevY = ball.y;
  ball.velX = 0.0f;
  ball.velY = 0.0f;
}

/**
 * 初期画面を描画
 */
void drawInitialScreen(void) {
  sprite.fillScreen(COLOR_BACKGROUND);
  sprite.setTextColor(COLOR_TITLE, COLOR_BACKGROUND);
  sprite.drawString("IMU Ball Control", M5.Display.width() / 2, TITLE_Y_OFFSET);
  sprite.pushSprite(0, 0);
}

// ============================================================================
// 物理計算関数
// ============================================================================
/**
 * 物理計算を更新
 */
void updatePhysics(void) {
  // IMUデータを取得（200Hz間隔で更新）
  unsigned long currentTime = micros();
  
  if (currentTime - lastIMUUpdateTime >= IMU_UPDATE_INTERVAL_US) {
    if (M5.Imu.update()) {
      imuData = M5.Imu.getImuData();
      lastIMUUpdateTime = currentTime;
      imuUpdateCount++;
    } else {
      imuUpdateFailCount++;
    }
  }
  
  // 加速度を使用して速度を更新
  ball.velX -= imuData.accel.y * ACCEL_SCALE;  // AccY（左右）を画面X軸にマッピング（符号反転）
  ball.velY += imuData.accel.x * ACCEL_SCALE;  // AccX（上下）を画面Y軸にマッピング（そのまま）
  
  // Z軸（重力）はY方向の動きに影響（デバイスを前後に傾けるとボールが動く）
  // 注意: accel.zは重力を含むため、デバイスが水平な時は約-9.8になる
  // デバイスを前後に傾けると、重力の成分がY軸に分散される
  float gravityEffect = (imuData.accel.z + GRAVITY) * GRAVITY_SCALE;
  ball.velY += gravityEffect;
  
  // 減衰を適用（摩擦）
  ball.velX *= DAMPING;
  ball.velY *= DAMPING;
  
  // 速度の上限を設定（異常な値の防止）
  if (ball.velX > VELOCITY_LIMIT) ball.velX = VELOCITY_LIMIT;
  if (ball.velX < -VELOCITY_LIMIT) ball.velX = -VELOCITY_LIMIT;
  if (ball.velY > VELOCITY_LIMIT) ball.velY = VELOCITY_LIMIT;
  if (ball.velY < -VELOCITY_LIMIT) ball.velY = -VELOCITY_LIMIT;
  
  // 位置を更新
  ball.x += ball.velX;
  ball.y += ball.velY;
}

/**
 * 画面端での境界チェックと反発処理
 */
void checkBoundaries(void) {
  int displayWidth = M5.Display.width();
  int displayHeight = M5.Display.height();
  
  // X軸（左右）の境界チェック
  if (ball.x < BALL_RADIUS) {
    ball.x = BALL_RADIUS;
    ball.velX = -ball.velX * BOUNCE_DAMPING;  // 反発（エネルギー損失あり）
  } else if (ball.x > displayWidth - BALL_RADIUS) {
    ball.x = displayWidth - BALL_RADIUS;
    ball.velX = -ball.velX * BOUNCE_DAMPING;
  }
  
  // Y軸（上下、タイトル部分を避ける）
  int minY = BALL_RADIUS + TITLE_AREA_HEIGHT;
  if (ball.y < minY) {
    ball.y = minY;
    ball.velY = -ball.velY * BOUNCE_DAMPING;
  } else if (ball.y > displayHeight - BALL_RADIUS) {
    ball.y = displayHeight - BALL_RADIUS;
    ball.velY = -ball.velY * BOUNCE_DAMPING;
  }
}

// ============================================================================
// 描画関数
// ============================================================================
/**
 * ボールを描画
 */
void drawBall(void) {
  int prevXInt = (int)ball.prevX;
  int prevYInt = (int)ball.prevY;
  int ballXInt = (int)ball.x;
  int ballYInt = (int)ball.y;
  
  // 前回位置を消去（動的マージンで確実に消去）
  int distanceX = abs(ballXInt - prevXInt);
  int distanceY = abs(ballYInt - prevYInt);
  int dynamicMargin = BALL_RADIUS + 8 + (distanceX + distanceY) / 4;
  if (dynamicMargin > BALL_RADIUS + 20) dynamicMargin = BALL_RADIUS + 20;
  
  int eraseX1 = max(0, prevXInt - dynamicMargin);
  int eraseY1 = max(TITLE_AREA_HEIGHT, prevYInt - dynamicMargin);
  int eraseX2 = min(M5.Display.width(), prevXInt + dynamicMargin);
  int eraseY2 = min(M5.Display.height(), prevYInt + dynamicMargin);
  sprite.fillRect(eraseX1, eraseY1, eraseX2 - eraseX1, eraseY2 - eraseY1, COLOR_BACKGROUND);
  
  // タイトルエリアを再描画（必要な場合）
  if (prevYInt < TITLE_AREA_HEIGHT + BALL_RADIUS || ballYInt < TITLE_AREA_HEIGHT + BALL_RADIUS) {
    sprite.fillRect(0, 0, M5.Display.width(), TITLE_AREA_HEIGHT, COLOR_BACKGROUND);
    sprite.setTextColor(COLOR_TITLE, COLOR_BACKGROUND);
    sprite.drawString("IMU Ball Control", M5.Display.width() / 2, TITLE_Y_OFFSET);
  }
  
  // 新しい位置にボールを描画
  sprite.fillCircle(ballXInt, ballYInt, BALL_RADIUS, COLOR_BALL);
  sprite.fillCircle(ballXInt, ballYInt, BALL_CENTER_DOT_RADIUS, COLOR_BALL_CENTER);
}

/**
 * デバッグ情報を描画
 */
void drawDebugInfo(void) {
  if (!SHOW_DEBUG_INFO) return;
  
  sprite.fillRect(DEBUG_INFO_X, DEBUG_INFO_Y, DEBUG_INFO_WIDTH, DEBUG_INFO_HEIGHT, COLOR_BACKGROUND);
  
  int textX = DEBUG_INFO_X + DEBUG_INFO_MARGIN;
  int textY = DEBUG_INFO_Y + DEBUG_INFO_MARGIN;
  sprite.setCursor(textX, textY);
  sprite.setTextColor(COLOR_DEBUG, COLOR_BACKGROUND);
  sprite.printf("X: %6.1f\n", ball.x);
  sprite.printf("Y: %6.1f\n", ball.y);
  sprite.printf("vX:%6.2f\n", ball.velX);
  sprite.printf("vY:%6.2f", ball.velY);
}

// ============================================================================
// メイン関数
// ============================================================================
void setup() {
  initializeHardware();
  initializeSprite();
  initializeIMU();
  initializeBall();
  drawInitialScreen();
}

void loop() {
  updatePhysics();
  checkBoundaries();
  drawBall();
  drawDebugInfo();
  
  // DMA部分更新（変更領域のみ転送）
  int ballXInt = (int)ball.x;
  int ballYInt = (int)ball.y;
  int prevXInt = (int)ball.prevX;
  int prevYInt = (int)ball.prevY;
  
  // 更新領域を計算（動的マージン）
  int distanceX = abs(ballXInt - prevXInt);
  int distanceY = abs(ballYInt - prevYInt);
  int baseMargin = BALL_RADIUS + 8;
  int dynamicMargin = baseMargin + (distanceX + distanceY) / 4;
  if (dynamicMargin > BALL_RADIUS + 25) dynamicMargin = BALL_RADIUS + 25;
  
  int prevUpdateX1 = prevXInt - dynamicMargin;
  int prevUpdateY1 = prevYInt - dynamicMargin;
  int prevUpdateX2 = prevXInt + dynamicMargin;
  int prevUpdateY2 = prevYInt + dynamicMargin;
  
  int currUpdateX1 = ballXInt - baseMargin;
  int currUpdateY1 = ballYInt - baseMargin;
  int currUpdateX2 = ballXInt + baseMargin;
  int currUpdateY2 = ballYInt + baseMargin;
  
  int updateX1 = min(prevUpdateX1, currUpdateX1);
  int updateY1 = min(prevUpdateY1, currUpdateY1);
  int updateX2 = max(prevUpdateX2, currUpdateX2);
  int updateY2 = max(prevUpdateY2, currUpdateY2);
  
  updateX1 = max(0, updateX1);
  updateY1 = max(TITLE_AREA_HEIGHT, updateY1);
  updateX2 = min(M5.Display.width(), updateX2);
  updateY2 = min(M5.Display.height(), updateY2);
  
  // ボール領域をDMA転送
  if (updateX2 > updateX1 && updateY2 > updateY1) {
    void* spriteBuffer = sprite.getBuffer();
    if (spriteBuffer) {
      uint16_t* src = (uint16_t*)spriteBuffer;
      int spriteWidth = sprite.width();
      int updateWidth = updateX2 - updateX1;
      int updateHeight = updateY2 - updateY1;
      
      for (int y = 0; y < updateHeight; y++) {
        uint16_t* row = src + (updateY1 + y) * spriteWidth + updateX1;
        M5.Display.pushImageDMA(updateX1, updateY1 + y, updateWidth, 1, row);
      }
      M5.Display.waitDMA();
    }
  }
  
  // デバッグ情報領域をDMA転送
  if (SHOW_DEBUG_INFO) {
    void* spriteBuffer = sprite.getBuffer();
    if (spriteBuffer) {
      uint16_t* src = (uint16_t*)spriteBuffer;
      int spriteWidth = sprite.width();
      
      for (int y = 0; y < DEBUG_INFO_HEIGHT; y++) {
        uint16_t* row = src + (DEBUG_INFO_Y + y) * spriteWidth + DEBUG_INFO_X;
        M5.Display.pushImageDMA(DEBUG_INFO_X, DEBUG_INFO_Y + y, DEBUG_INFO_WIDTH, 1, row);
      }
      M5.Display.waitDMA();
    }
  }
  
  if (UPDATE_INTERVAL_MS > 0) {
    delay(UPDATE_INTERVAL_MS);
  }
  
  // 前回位置を更新（DMA転送後に実行）
  ball.prevX = ball.x;
  ball.prevY = ball.y;
}
