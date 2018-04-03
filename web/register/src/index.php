<?php

include("auth.php"); //include auth.php file on all secure pages
?>

<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<title>Welcome Home</title>
<link rel="stylesheet" href="css/style.css" />
</head>
<body>
<div class="form">
<p>Welcome <?php echo $_SESSION['username']; ?>!</p>
<?php if ($_SESSION['username']=="admin") echo "TG18{Se55ion_C00kie_Shou1d_Be_Secured!!}" ?>
<p>This is secure area.</p>
<p><a href="dashboard.php">Dashboard</a></p>
<a href="logout.php">Logout</a>


<br /><br /><br /><br />

</div>
</body>
</html>
