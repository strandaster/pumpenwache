<?php

/*
* post pump activity to the database
*/

$servername = "<YOUR_DB_SERVER_NAME>";

$dbname = "<YOUR_DB_INSTANCE_NAME>";
$username = "<YOUR_DB_USERNAME>";
$password = "<YOUR_DB_PASSWORD>";

$api_key_value = "<YOUR_API_KEY>";

$api_key= $Amps = "";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $api_key = test_input($_POST["api_key"]);
    if($api_key == $api_key_value) {
        $Amps = test_input($_POST["Amps"]);
        
        // Create connection
        $conn = new mysqli($servername, $username, $password, $dbname);
        // Check connection
        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        } 
        
        $sql = "INSERT INTO dataV2 (Amps)
        VALUES ('" . $Amps .  "')";
        
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