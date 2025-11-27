class Point {
  public:
    Point(int _x, int _y);     // コンストラクタ
    ~Point();                  // デストラクタ
    void set(int _x, int _y);  // 点の位置を指定する
    void setX(int _x);         // 点の横位置を指定する
    void setY(int _y);         // 点の縦位置を指定する
    int getX();                // 点の横位置を得る
    int getY();                // 点の縦位置を得る
    void move(int w, int h);   // 点を横にw／縦にh移動する
  private:
    int x;                     // 横位置
    int y;                     // 縦位置
};

Point::Point(int _x, int _y) {
  set(_x, _y);  // setメンバ関数を実行して点の位置を代入する
}

Point::~Point() {
  // デストラクタで行うことはない
}

void Point::setX(int _x) {
  x = _x;  // メンバ変数のxに引数を代入する
}

void Point::setY(int _y) {
  y = _y;  // メンバ変数のyに引数を代入する
}

void Point::set(int _x, int _y) {
  setX(_x); // 横位置を指定する
  setY(_y); // 縦位置を指定する
}

int Point::getX(void) {
  return x;  // 横位置を返す
}

int Point::getY(void) {
  return y;  // 縦位置を返す
}

void Point::move(int w, int h) {
  x += w;  // 横位置をw移動する
  y += h;  // 縦位置をh移動する
}

void setup() {
  Serial.begin(115200);

  Point p(100, 200);    // Point型のオブジェクトpを宣言する
  p.move(300, 400);     // pの位置を横に300／縦に400移動する
  Serial.println(p.getX());  // pの横位置を表示する
  Serial.println(p.getY());  // pの縦位置を表示する
}

void loop() {
}
