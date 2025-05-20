#!/usr/bin/php
<?php
echo "Content-Type: text/html\n\n";
echo "<html><body><h1>Heilbroon 1st May!</h1>\n";
echo "<p>Server name: " . $_SERVER['SERVER_NAME'] . "</p>\n\n";
echo "<p>IP Address: " . $_SERVER['REMOTE_ADDR'] . "</p>\n\n";
echo "</body></html>\n\n";
?>