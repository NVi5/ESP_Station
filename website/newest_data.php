<?php 
$servername = "localhost";
$username = "MySQLuser";   # database user with SELECT permission
$password = "password";    # password for this user
$dbname = "id16483562_test";

$conn = new mysqli( $servername, $username, $password, $dbname );
$RESULT_NOW = $conn -> query("SELECT `date`, `temp`, `humi`, `pres` FROM `000_test_czujnik` WHERE `date` BETWEEN CURRENT_TIMESTAMP - 0000100 + 0020000 AND CURRENT_TIMESTAMP - 0000000 + 0020000") -> fetch_assoc();

echo json_encode($RESULT_NOW);

$conn -> close();
?>
