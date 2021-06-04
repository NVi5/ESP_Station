<?php 
$servername = "localhost";
$username = "MySQLuser";   # database user with INSERT permission
$password = "password";    # password for this user
$dbname = "id16483562_test";

$conn = new mysqli( $servername, $username, $password, $dbname );

if( $conn -> connect_error ){
  die( "Connection failed: " . $conn -> connect_error );
}

$sql = "INSERT INTO `000_test_czujnik`(`date`, `temp`, `humi`, `pres`) VALUES ( CURRENT_TIMESTAMP + 20000, ?, ?, ? )";
$stmt = $conn -> prepare($sql);
$stmt -> bind_param( "sss",  $_GET['temp'], $_GET['humi'], $_GET['pres'] );
$stmt -> execute();

$conn -> close();
?>