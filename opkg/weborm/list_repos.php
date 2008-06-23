<?php
   echo "List of available repositories:<br/>";
   if ($handle = opendir("./repos")) {
       while (($file = readdir($handle)) !== false) {
           if (filetype("./repos/".$file) == "dir" 
               && $file != "."
               && $file != ".."
               && $file != ".svn") {
               echo "<input type=\"radio\" name=\"group1\" value=\"$file\" onChange=\"newSelection(select1)\">$file<br/>";
           }
       }
   }
   closedir($handle);
   echo "<br/><br/>";
?>
