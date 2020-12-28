<?php

/*
* post environment data to database 
*/

$servername = "<YOUR_DB_SERVER_NAME>";

$dbname = "<YOUR_DB_INSTANCE_NAME>";
$username = "<YOUR_DB_USERNAME>";
$password = "<YOUR_DB_PASSWORD>";

$api_key_value = "<YOUR_API_KEY>";

$api_key= $Amps = $temperatur = $feuchtigkeit = $luftdruck= "";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $api_key = test_input($_POST["api_key"]);
    if($api_key == $api_key_value) {
        $Amps = test_input($_POST["Amps"]);
        $temperatur = test_input($_POST["temperatur"]);
        $feuchtigkeit = test_input($_POST["feuchtigkeit"]);
        $luftdruck = test_input($_POST["luftdruck"]);
        
        // Create connection
        $conn = new mysqli($servername, $username, $password, $dbname);
        // Check connection
        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        } 
        
        $sql = "INSERT INTO umwelt (temperatur, feuchtigkeit, luftdruck)
        VALUES ('" . $temperatur . "', '" . $feuchtigkeit . "', '" . $luftdruck .  "')";
        
        if ($conn->query($sql) === TRUE) {
            echo "New record created successfully";
        } 
        else {
            echo "Error: " . $sql . "<br>" . $conn->error;
        }
    
        $conn->close();
    }
    else {
        echo "Wrong API Key provided.";
    }

}
else {
    echo "No data posted with HTTP POST.";
}

function test_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}