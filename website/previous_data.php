<?php 
$servername = "localhost";
$username = "MySQLuser";   # database user with SELECT permission
$password = "password";    # password for this user
$dbname = "id16483562_test";

$conn = new mysqli( $servername, $username, $password, $dbname );
$RESULT_SQL = $conn -> query("SELECT `date`, `temp`, `humi`, `pres` FROM `000_test_czujnik` WHERE `date` BETWEEN DATE_ADD( CURRENT_TIMESTAMP, INTERVAL -46 DAY_HOUR ) AND DATE_ADD( CURRENT_TIMESTAMP, INTERVAL 2 HOUR )");

$RESULT = array();

while( $ROW = $RESULT_SQL -> fetch_assoc() ){
    array_push( $RESULT, $ROW );
}
  
echo json_encode($RESULT);
  
$conn -> close();
?>
