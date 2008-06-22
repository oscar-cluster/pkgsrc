<?php
//if (!isset($_SERVER['PHP_AUTH_USER'])) {
//    header('WWW-Authenticate: Basic realm="My Realm"');
//    header('HTTP/1.0 401 Unauthorized');
//    echo 'Text to send if user hits Cancel button';
//    exit;
//} else {
//    echo "<p>Hello {$_SERVER['PHP_AUTH_USER']}.</p>";
//    echo "<p>You entered {$_SERVER['PHP_AUTH_PW']} as your password.</p>";
//}

$http_vars = "";
$distro_id = "";

if (sizeof($_POST) != 3) {
    echo "Wrong number of arguments (" . sizeof($_POST) . ", instead of 3)<br/>";
}
echo "Number of args: " . sizeof($_POST) . "<br/>";
for ($i=0; $i < sizeof($_POST); $i++) {
    list($name, $value) = each($_POST);
    $distro_id = $distro_id . $value;
    if ($i != sizeof($_POST) - 1) {
        $distro_id = $distro_id . "-";
    }
}
echo $distro_id . "<br/>";

$dir = "./repos/".$distro_id;
echo "Creating " . $dir . "...<br/>";
if (mkdir($dir)) {
    echo "Repository successfully created<br/>";
} else {
    echo "ERROR: Impossible to create the repository";
}

?>
