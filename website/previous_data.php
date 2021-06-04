<?php 
$servername = "localhost";
$username = "MySQLuser";   # database user with SELECT permission
$password = "password";    # password for this user
$dbname = "id16483562_test";

$conn = new mysqli( $servername, $username, $password, $dbname );
$RESULT_SQL = $conn -> query("SELECT `date`, `temp`, `humi`, `pres` FROM `000_test_czujnik` WHERE `date` BETWEEN CURRENT_TIMESTAMP - 2000000 + 0020000 AND CURRENT_TIMESTAMP - 0000000 + 0020000");

$RESULT = array();

while( $ROW = $RESULT_SQL -> fetch_assoc() ){
    array_push( $RESULT, $ROW );
}
  
echo json_encode($RESULT);
  
$conn -> close();
?>
