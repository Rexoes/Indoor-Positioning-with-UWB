<!DOCTYPE html>
<html><body>
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

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);
// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
} 

$sql = "SELECT ID, TAG_ID, TAG_POS_X, TAG_POS_Y, TAG_GRID, A0, A1, A2, A3, A4, A5, A6, A7, LOCATION, reading_time FROM UWB_INDOOR_DATASET ORDER BY id DESC";

echo '<table cellspacing="5" cellpadding="5">
      <tr> 
        <td>ID</td> 
        <td>TAG_ID</td> 
        <td>TAG_POS_X</td> 
        <td>TAG_POS_Y</td> 
        <td>TAG_GRID</td>
        <td>A0</td>
        <td>A1</td>
        <td>A2</td>
        <td>A3</td>
        <td>A4</td>
        <td>A5</td>
        <td>A6</td>
        <td>A7</td>
        <td>LOCATION</td>
        <td>TIMESTAMP</td> 
      </tr>';
 
if ($result = $conn->query($sql)) {
    while ($row = $result->fetch_assoc()) {
        $row_ID = $row["ID"];
        $row_TAG_ID = $row["TAG_ID"];
        $row_TAG_POS_X = $row["TAG_POS_X"];
        $row_TAG_POS_Y = $row["TAG_POS_Y"];
        $row_TAG_GRID = $row["TAG_GRID"];
        $row_A0 = $row["A0"];
        $row_A1 = $row["A1"];
        $row_A2 = $row["A2"];
        $row_A3 = $row["A3"];
        $row_A4 = $row["A4"];
        $row_A5 = $row["A5"];
        $row_A6 = $row["A6"];
        $row_A7 = $row["A7"];
        $row_LOCATION = $row["LOCATION"];
        $row_reading_time = $row["reading_time"];
        // Uncomment to set timezone to - 1 hour (you can change 1 to any number)
        //$row_reading_time = date("Y-m-d H:i:s", strtotime("$row_reading_time - 1 hours"));
      
        // Uncomment to set timezone to + 4 hours (you can change 4 to any number)
        //$row_reading_time = date("Y-m-d H:i:s", strtotime("$row_reading_time + 4 hours"));
      
        echo '<tr> 
                <td>' . $row_ID . '</td> 
                <td>' . $row_TAG_ID . '</td> 
                <td>' . $row_TAG_POS_X . '</td> 
                <td>' . $row_TAG_POS_Y . '</td> 
                <td>' . $row_TAG_GRID . '</td>
                <td>' . $row_A0 . '</td>
                <td>' . $row_A1 . '</td>
                <td>' . $row_A2 . '</td>
                <td>' . $row_A3 . '</td>
                <td>' . $row_A4 . '</td>
                <td>' . $row_A5 . '</td>
                <td>' . $row_A6 . '</td>
                <td>' . $row_A7 . '</td>
                <td>' . $row_LOCATION . '</td>
                <td>' . $row_reading_time . '</td> 
              </tr>';
    }
    $result->free();
}

$conn->close();
?> 
</table>
</body>
</html>