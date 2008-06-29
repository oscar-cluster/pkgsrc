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

$uploaddir = 'uploads';

echo '<pre>';
echo "Number of args: " . sizeof($_POST) . "<br/>";
for ($i=0; $i < sizeof($_POST); $i++) {
    list($name, $value) = each($_POST);
    $distro_id = $value;
    if ($i != sizeof($_POST) - 1) {
        $distro_id = $distro_id . "-";
    }
    echo "    Target repository: $distro_id<br/>";
}
$file_count = sizeof($_FILES);
for ($i=0; $i<$file_count; $i++) {
    $thisfilename = 'file_'. $i;
    $userfile = $_FILES[$thisfilename]['name'];
    if ($userfile != "") {
        echo "<br/>File id: " . $userfile . "\n";
        $uploadfile = "./$uploaddir/$distro_id/$userfile";
        if (move_uploaded_file($_FILES[$thisfilename]['tmp_name'], $uploadfile)) {
            echo "File is valid, and was successfully uploaded.\n";
            echo "Checking if the file is a valid binary package...<br/>";
            if (is_a_valid_package ($uploadfile)) {
                echo "The package is valid.<br/>";
                $repo_file = "../repos/$distro_id/$userfile";
                if (rename($uploadfile, $repo_file)) {
                    echo "Successfully copied $uploadfile to $repo_file<br/>";
                    prepare_repo ($distro_id);
                } else {
                    echo "ERROR: Impossible to copy the file ($uploadfile) to the repository ($repo_file)<br/>";
                }
             } else {
                echo "The package ($uploadfile) is not valid<br/>";
                unlink ($uploadfile);
            }
        } else {
            echo "Possible file upload attack!\n";
        }
    }
}

echo "<br/><br/>";
echo "<script>function loadIndex() { document.location.href = \"./index.html\" }</script>";
echo "<button onclick=\"loadIndex()\" />Ok</button>";

print "</pre>";

function prepare_repo ($distro_id) {
    echo "<br/>Regenerating the repository's metadata...<br/>";
    $packman_cmd = "/usr/bin/packman --prepare-repo " . getcwd() . "/../repos/$distro_id -v";
    echo "    Executing $packman_cmd...<br/>";
    if (exec ($packman_cmd, $output)) {
        print_full_output ($output);
        echo "    Successfully regenerate repository's meta-data<br/>";
    } else {
        print_full_output ($output);
        echo "    ERROR: Impossible to regenerate repository's meta-data<br/>";
    }
}

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

function print_full_output ($output) {
    $size = sizeof($output);
    echo "Output:<br/>";
    for ($i=0; $i<$size; $i++) {
        echo "    $output[$i]<br/>";
    }
}

?>
