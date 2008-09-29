<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
  <head>
    <title>OSCAR Repository Manager</title>
    <style type="text/css" media="all">
      @import url("/admin/css/base.css");
      @import url("/admin/css/site.css");
    </style>
    <meta name="author" content="Geoffroy Vallee" />
    <meta name="keywords" content="ORNL, system, operating system, research" />
    <meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1" />
    <script src="multifile.js"></script>
  </head>
<body class="composite">
  <div id="banner">
    <span id="bannerLeft"></span>
    <span id="bannerRight">OSCAR Repository Manager</span>
    <div class="clear">
      <hr/>
    </div>
  </div>
  <div id="breadcrumbs">
    <div class="xright">
      <a href="/admin/index.html">Home</a>
      |
      <a href="/admin/upload.php">Upload</a>
      |
      <a href="/admin/create.html">Create Repo</a>
      |
      <a href="/admin/manage.php">Manage Repo</a>
    </dir>
  </div>
  <div class="clear">
    <hr/>
  </div>
  <div id="leftColumn">
    <div id="navcolumn">
      <ul>
        <li class="none">
          <a href="/admin/index.html">Home</a>
        </li>
        <li class="none">
          <a href="/admin/upload.php">Upload Packages</a>
        </li>
        <li class="none">
          <a href="/admin/create.html">Create a New Repository</a>
        </li>
        <li class="none">
          <a href="/admin/manage.php">Manage an Existing Repository</a>
        </li>
      </ul>
    </div>
  </div>
  <div id="bodyColumn">
    <div id="contentBox">
      <div class="section">
        <h1>Upload Binary Packages</h1>
        <br/><br/>
        <form enctype="multipart/form-data" action="process_files.php" method = "post">
          <? include ("list_repos.php"); ?>
          <input id="my_file_element" type="file" name="file_1" >
          <input type="submit">
        </form>
        Files:
        <div id="files_list"></div>
        <script>
          var x = document.getElementsByTagName( 'div' );
          for (var i=0; i<x.length; i++) {
            if (x[i].id == "files_list") {
              var multi_selector = new MultiSelector( x[i], 0 );
              <!-- Pass in the file element -->
              multi_selector.addElement( document.getElementById( 'my_file_element' ) );
            }
          }
        </script>
      </div>
      <div class="section"><p></p></div>
    </div>
  </div>
  <div class="clear">
    <hr/>
  </div>
  <div id="footer">
	<div class="xcenter">
      <a href="http://oscar.openclustergroup.org/"><img alt="OSCAR Logo" src="/admin/images/oscar_header.png"/></a><br/>
<a href="/admin/license.html" alt="license">License</a>|<a href="/admin/authors.html">Authors</a>
      <div class="clear">
        <hr/>
      </div>
    </div>
  </div>
</body>
</html>



