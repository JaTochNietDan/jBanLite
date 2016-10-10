<?php

//-----------------------------------------
// Point this to the exact path your jban file!
//-----------------------------------------
$SQL_DatabaseLocation 	= "jban.db";

//-----------------------------------------
// Fill in your ban table name here
//-----------------------------------------
$SQL_Table		= "bans";

//-----------------------------------------
// Fill in your server information
//-----------------------------------------
$Server_Name = 'My Server';

//-----------------------------------------
// Don't edit unless you know what you're doing!
//-----------------------------------------

$SQL_DB;
$OrderReplace = array("ban_timestamp", "user_banned", "user_banner", "ban_reason", "ban_timestamp", "ban_timeleft");

try 
{
    $SQL_DB = new PDO("sqlite:".$SQL_DatabaseLocation);
}
catch(PDOException $e)
{
    echo $e->getMessage();
    echo "<br><br>Database -- NOT -- loaded successfully .. ";
    die( "<br><br>Query Closed !!! $error");
}

?>