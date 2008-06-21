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

// In PHP versions earlier than 4.1.0, $HTTP_POST_FILES should be used instead
// of $_FILES.

global $HTTP_POST_VARS;
reset($HTTP_POST_VARS);

$http_vars = "";

while(list($name, $value) = each($HTTP_POST_VARS))
    $http_vars .= $name . " = <br />" . $value . "<br /><br />";

?>
