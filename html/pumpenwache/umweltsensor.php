<?php
 /*
  * Sensor for Home Assistant 
  */
header('Content-Type: application/json');
//declare variables for db connection
$servername = "<YOUR_DB_SERVERNAME>";
$username = "<YOUR_DB_USERNAME>";
$password = "<YOUR_DB_PASSWORD>";
$dbname = "<YOUR_DB_INSTANCE_NAME>";

//db connection
$con = new mysqli($servername, $username, $password, $dbname);

if ($con->connect_error) {
        die("Connection failed: " . $con->connect_error);
    }

    $query = "SELECT Temperatur, Feuchtigkeit, Luftdruck FROM umwelt ORDER BY Time DESC LIMIT 1"; // select column
    $aresult = $con->query($query);
    $myArray = array();

    $row = mysqli_fetch_row($aresult);
    $myArray["Temperatur"] = $row[0];
    $myArray["Feuchtigkeit"] = $row[1];
    $myArray["Luftdruck"] = $row[2];

    //var_dump($myArray);

    $query = "SELECT TIMESTAMPDIFF(HOUR, (SELECT MAX(TIME) FROM diff), CURRENT_TIMESTAMP) ";
    $lastActPumpeQueryResult = $con->query($query);
    $row = mysqli_fetch_row($lastActPumpeQueryResult);
    $Pumpe["lastActivityPumpe"] = $row[0];
    // var_dump($Pumpe);

    $query = "SELECT TIMESTAMPDIFF(MINUTE, (SELECT MAX(TIME) FROM umwelt), CURRENT_TIMESTAMP)";
    $lastActUmweltQueryResult = $con->query($query);
    $row = mysqli_fetch_row($lastActUmweltQueryResult);
    $Umwelt["lastActivityUmwelt"] = $row[0];
    //var_dump($Umwelt);

    $arrayComplete[] = array_merge($myArray, $Pumpe, $Umwelt);

    print json_encode($arrayComplete,  JSON_NUMERIC_CHECK);
    
   $con->close();
   $aresult->close();
   $lastActUmweltQueryResult->close();
   $lastActPumpeQueryResult->close();
?>
