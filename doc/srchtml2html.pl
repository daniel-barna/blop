$filename = $ARGV[0];
$dir      = $ARGV[1];

if( $dir eq "")
{
    print "Directory not specified\n";
    exit;
}

$title="";
$mtitle="";
$makelinks = true;
$up="index.html";
$prev="";
$next="";

open(IN, "<$filename");
while( <IN> )
{
    if($_ =~ "==>")
    {
	last;
    }
    ($keyword, $value) = ($_ =~ /([^:]*):[ \t]*(.*)/);
    if($keyword eq "TITLE") {$title = $value;}
    if($keyword eq "MTITLE") {$mtitle = $value;}
    if($keyword eq "NOLINKS") {$makelinks = false;}
    if($keyword eq "UP")      {$up = $value;}
    if($keyword eq "PREV")    {$prev = $value;}
    if($keyword eq "NEXT")    {$next = $value;}
}

if( $title eq "")
{
    print "Missing 'TITLE:' from $filename\n";
    exit;
}

if( $mtitle eq "") {$mtitle = $title;}

$links = "
    <a href=\"$dir/index.html\" title=\"Home\"> 
    <img src=\"$dir/home.png\"  style=\"border-width:0cm;\"> 
    </a> ";

if($prev eq "NONE") {}
elsif($prev ne "")
{
  $links .= "
    <a href=\"$prev\">
    <img src=\"$dir/left.png\"  style=\"border-width:0cm;\" alt=\"Prev: $prev\">
    </a> ";
}
else
{
  $links .= "
    <img src=\"$dir/left.png\"  style=\"border-width:0cm;\">";
}

if($up ne "NONE")
{
    $links .= "
    <a href=\"$up\" title=\"Up\"> 
    <img src=\"$dir/up.png\"    style=\"border-width:0cm;\" alt=\"Up\"> 
    </a> ";
}

if($next eq "NONE") {}
elsif($next ne "")
{
  $links .= "
    <a href=\"$next\">
    <img src=\"$dir/right.png\" style=\"border-width:0cm;\" alt=\"Next: $next\">
    </a> ";
}
else
{
  $links .= "
    <img src=\"$dir/right.png\" style=\"border-width:0cm;\">";
}

print "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\">
<html>
<head>
   <meta name=\"Author\" content=\"Daniel Barna\">
   <meta http-equiv=\"Content-Style-Type\" content=\"text/css\">   
   <title>BLOP -- $title</title>
   <link rel=\"StyleSheet\" href=\"$dir/default.css\">\n";

if($prev ne "")
{
   print "   <link rel=\"Prev\" href=\"$prev\">\n";
}
if($next ne "")
{
   print "   <link rel=\"Next\" href=\"$next\">\n";
}
print "
</head>
<body>
";

if($makelinks) {print $links,"<br>\n";}
print "
<center class=title><b>
$mtitle
</b></center>
<hr>
";

$lineno = 0;
while($_ = <IN>) 
{
  print $_;
}

if($makelinks) {print "<hr>",$links,"\n";}

print "
</body>
</html>
";
