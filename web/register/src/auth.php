<?php

session_start();

if (isset($_COOKIE[User])) {
	$_SESSION["username"]=$_COOKIE[User];
	echo "session set<br>"; 
}

if (!isset($_SESSION["username"])) {
	header("Location: login.php");
	exit();
}

?>
