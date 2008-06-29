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


echo "Number of args: " . sizeof($_POST) . "<br/>";
for ($i=0; $i < sizeof($_POST); $i++) {
    list($name, $value) = each($_POST);
    $distro_id = $value;
    if ($i != sizeof($_POST) - 1) {
        $distro_id = $distro_id . "-";
    }
    echo "    Target repository: $distro_id<br/>";
}

echo "<br/>Available packages, select package you want to delete:<br/>";
echo "<form action=\"delete_packages.php\" method=\"post\">";
if ($handle = opendir("./repos/$distro_id")) {
       while (($file = readdir($handle)) !== false) {
           if (filetype("./repos/$distro_id/$file") == "file") {
               echo "<input type=\"radio\" name=\"group1\" value=\"$distro_id/$file\" \">$file<br/>";
           }
       }
   }
   closedir($handle);
   echo "<br/><br/>";
   echo "<input type=\"submit\" /></form>"; 
?>
