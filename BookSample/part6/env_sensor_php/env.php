<?php
$dbname = 'データベース名';
$host = 'ホスト名';
$user = 'ユーザー名';
$tbname = 'env';
$pass = 'パスワード';
try {
  $dbh = new PDO("mysql:dbname=${dbname};host=${host}", $user, $pass);
}
catch (PDOException $e) {
  echo 'Connection failed ' . $e->getMessage();
  exit();
}
$date = sprintf("%04d-%02d-%02d %02d:%02d:%02d",
                $_POST['year'] + 0, $_POST['month'] + 0, $_POST['day'] + 0,
                $_POST['hour'] + 0, $_POST['min'] + 0, $_POST['sec'] + 0);
$temperature = $_POST['temperature'] + 0.0;
$humidity = $_POST['humidity'] + 0.0;
$pressure = $_POST['pressure'] + 0.0;
$air = $_POST['air'] + 0.0;
$sql = <<<HERE
INSERT INTO ${tbname} (date, temparature, humidity, pressure, air)
VALUES ( '${date}', ${temperature}, ${humidity}, ${pressure}, ${air});
HERE;
$res = $dbh->exec($sql);
$sql = <<<HERE
DELETE FROM env WHERE date < DATE_SUB(now(), INTERVAL 180 DAY);
HERE;
$res = $dbh->exec($sql);
header('Content-type: text/plain');
print('ok');
?>

