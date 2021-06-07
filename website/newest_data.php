<?php 
$servername = "localhost";
$username = "MySQLuser";   # database user with SELECT permission
$password = "password";    # password for this user
$dbname = "id16483562_test";

$conn = new mysqli( $servername, $username, $password, $dbname );
$RESULT_NOW = $conn -> query("SELECT `date`, `temp`, `humi`, `pres` FROM `000_test_czujnik` WHERE `date` BETWEEN DATE_ADD( CURRENT_TIMESTAMP, INTERVAL 7130 HOUR_SECOND ) AND DATE_ADD( CURRENT_TIMESTAMP, INTERVAL 2 HOUR )") -> fetch_assoc();

echo json_encode($RESULT_NOW);

$conn -> close();
?>
