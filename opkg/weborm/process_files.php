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

$uploaddir = '/var/www/uploads/';

echo '<pre>';
$file_count = sizeof($_FILES)-1;
echo 'number of files: ' . $file_count . "...\n";
for ($i=0; $i<$file_count; $i++) {
    $thisfilename = 'file_'. $i;
    $userfile = $_FILES[$thisfilename]['name'];
    echo "File id: " . $userfile . "\n";
    $uploadfile = $uploaddir . $userfile;
    if (move_uploaded_file($_FILES[$thisfilename]['tmp_name'], $uploadfile)) {
        echo "File is valid, and was successfully uploaded.\n";
    } else {
        echo "Possible file upload attack!\n";
    }
}

print "</pre>";

?>
