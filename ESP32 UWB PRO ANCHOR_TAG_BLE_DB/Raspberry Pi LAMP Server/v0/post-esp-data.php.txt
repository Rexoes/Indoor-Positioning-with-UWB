<?php

/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-esp8266-mysql-database-php/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

$servername = "localhost";

// REPLACE with your Database name
$dbname = "esp_data";
// REPLACE with Database user
$username = "root";
// REPLACE with Database user password
$password = "057921236";

// Keep this API Key value to be compatible with the ESP32 code provided in the project page. 
// If you change this value, the ESP32 sketch needs to match
$api_key_value = "tPmAT5Ab3j7F9";

$api_key= $TAG_ID = $TAG_POS_X = $TAG_POS_Y = $TAG_GRID = $A1 = $A2 = $A3 = $A4 = $A5 = $A6 = $LOCATION = "";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $api_key = test_input($_POST["api_key"]);
    if($api_key == $api_key_value) {
        $TAG_ID = test_input($_POST["TAG_ID"]);
        $TAG_POS_X = test_input($_POST["TAG_POS_X"]);
        $TAG_POS_Y = test_input($_POST["TAG_POS_Y"]);
        $TAG_GRID = test_input($_POST["TAG_GRID"]);
        $A1 = test_input($_POST["A1"]);
        $A2 = test_input($_POST["A2"]);
        $A3 = test_input($_POST["A3"]);
        $A4 = test_input($_POST["A4"]);
        $A5 = test_input($_POST["A5"]);
        $A6 = test_input($_POST["A6"]);
        $LOCATION = test_input($_POST["LOCATION"]);
        
        // Create connection
        $conn = new mysqli($servername, $username, $password, $dbname);
        // Check connection
        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        } 
        
        $sql = "INSERT INTO UWB_Data (TAG_ID, TAG_POS_X, TAG_POS_Y, TAG_GRID, A1, A2, A3, A4, A5, A6, LOCATION)
        VALUES ('" . $TAG_ID . "', '" . $TAG_POS_X . "', '" . $TAG_POS_Y . "', '" . $TAG_GRID . "', '" . $A1 . "', '" . $A2 . "', '" . $A3 . "', '" . $A4 . "', '" . $A5 . "', '" . $A6 . "', '" . $LOCATION . "')";
        
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

