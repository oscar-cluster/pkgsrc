B
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
  <head>
    <title>OSCAR Repository Manager</title>
    <style type="text/css" media="all">
      @import url("/admin/css/base.css");
      @import url("/admin/css/site.css");
    </style>
    <meta name="author" content="Geoffroy Vallee" />
    <meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1" />
<!--    <script src="display_packages.js"></script>-->
    <script>
        function newSelection() {
            var x = document.getElementById ('select1');
            if (x != null) {
                var i = x.options.selectedIndex;
                var str = x.options[i].value;
                if (str != "") {
                    var div1 = document.getElementById ('name_repo');
                    div1.innerHTML = str;
                }
            }
        }
    </script>
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
      <div class="section" id="section">
        <h1>Manage an Existing Repository</h1>
        <br/><br/>
        <form action="/admin/manage_repo.php" method="post">
          <? include ("list_repos.php"); ?>
          <input type="submit" />
        </form>
      </div>
      <div id="name_repo"></div>
      <div id="packages_list"></div>
      <div class="section"><p></p></div>
    </div>
  </div>
  <div class="clear">
    <hr/>
  </diV>
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



