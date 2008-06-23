<?php
echo "Number of args: " . sizeof($_POST) . "<br/>";
for ($i=0; $i < sizeof($_POST); $i++) {
    list($name, $value) = each($_POST);
    $package = getcwd() . "/repos/$value";
    echo "    Deleting: $package<br/>";
    if (unlink ($package)) {
        echo "    Successfully delete $value<br/>";
    } else {
        echo "    ERROR: Impossible to delete $value<br/>";
    }
}

echo "<br/>Regenerating the repository's metadata...<br/>";
$packman_cmd = "/usr/bin/packman --prepare-repo " . dirname ($package) . " -v";
echo "    Executing $packman_cmd...<br/>";
if (exec ($packman_cmd, $output)) {
    echo "    Successfully regenerate repository's meta-data<br/>";
} else {
    echo "    ERROR: Impossible to regenerate repository's meta-data<br/>";
}

?>
