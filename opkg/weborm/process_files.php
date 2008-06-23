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

$uploaddir = 'uploads/';

echo '<pre>';
$file_count = sizeof($_FILES);
echo 'number of files: ' . $file_count . "...\n";
for ($i=0; $i<$file_count; $i++) {
    $thisfilename = 'file_'. $i;
    $userfile = $_FILES[$thisfilename]['name'];
    echo "File id: " . $userfile . "\n";
    $uploadfile = "./" . $uploaddir . $userfile;
    if (move_uploaded_file($_FILES[$thisfilename]['tmp_name'], $uploadfile)) {
        echo "File is valid, and was successfully uploaded.\n";
        echo "Checking if the file is a valid binary package...<br/>";
        if (is_a_valid_package ($uploadfile)) {
            echo "The package is valid.<br/>";
            
        } else {
            echo "The package ($uploadfile) is not valid<br/>";
        }
    } else {
        echo "Possible file upload attack!\n";
    }
}

print "</pre>";

function is_a_valid_package ($file_path) {
    $str = "";
    if (exec ("file $file_path | awk ' { print $2 } '", $output)) {
	$str = $output[0];
        echo "    File type: $str<br/>";
    }
    if ($str == "Debian" || $str == "RPM") {
        return true;
    } else {
        return false;
    }
}

?>
