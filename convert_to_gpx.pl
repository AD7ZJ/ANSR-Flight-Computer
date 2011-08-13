


print <<EOF;
<?xml version="1.0" standalone="yes"?>
<gpx version="1.0" creator="GPS Visualizer http://www.gpsvisualizer.com/" xmlns="http://www.topografix.com/GPX/1/0" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.topografix.com/GPX/1/0 http://www.topografix.com/GPX/1/0/gpx.xsd">
EOF

$lat = -33.00;
$lon = -128.00;
$elevaion = 427;
$name = "launch";

WayPoint;


WayPoint{
   print "\t<wpt lat=\"$lat\" lon=\"$lon\">\n\t\t<ele>$elevetation</ele>\t\t<name>$name</name>\n\t</wpt>"; 
}
