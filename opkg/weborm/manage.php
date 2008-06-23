<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
  <head>
    <title>OSCAR Repository Manager</title>
    <style type="text/css" media="all">
      @import url("css/base.css");
      @import url("css/site.css");
    </style>
    <link rel="stylesheet" href="./css/print.css" type="text/css" media="print"/>
    <meta name="author" content="Geoffroy Vallee" />
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
      <a href="index.html">Home</a>
      |
      <a href="upload.php">Upload</a>
      |
      <a href="create.html">Create Repo</a>
      |
      <a href="manage.php">Manage Repo</a>
    </dir>
  </div>
  <div class="clear">
    <hr/>
  </div>
  <div id="leftColumn">
    <div id="navcolumn">
      <ul>
        <li class="none">
          <a href="index.html">Home</a>
        </li>
        <li class="none">
          <a href="upload.php">Upload Packages</a>
        </li>
        <li class="none">
          <a href="create.html">Create a New Repository</a>
        </li>
        <li class="none">
          <a href="manage.php">Manage an Existing Repository</a>
        </li>
      </ul>
    </div>
  </div>
  <div id="bodyColumn">
    <div id="contentBox">
      <div class="section">
        <h1>Manage an Existing Repository</h1>
        <br/><br/>
        <form enctype="multipart/form-data" action="manage_repo.php" method = "post">
          <? include ("list_repos.php"); ?>
          <input type="submit">
        </form>
      </div>
      <div class="section"><p></p></div>
    </div>
  </div>
  <div class="clear">
    <hr/>
  </div>
  <div id="footer">
	<div class="xcenter">
      <a href="http://oscar.openclustergroup.org/"><img alt="OSCAR Logo" src="images/oscar_header.png"/></a><br/>
<a href="license.html" alt="license">License</a>|<a href="authors.html">Authors</a>
      <div class="clear">
        <hr/>
      </div>
    </div>
  </div>
</body>
</html>



