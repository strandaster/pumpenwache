<?php

/* 
* Graphic for pump activities
*/

function JSdate($in,$type){
      if($type=='date'){
        //Dates are patterned 'yyyy-MM-dd'
        preg_match('/(\d{4})-(\d{2})-(\d{2})/', $in, $match);
    } elseif($type=='datetime'){
        //Datetimes are patterned 'yyyy-MM-dd hh:mm:ss'
        preg_match('/(\d{4})-(\d{2})-(\d{2})\s(\d{2}):(\d{2}):(\d{2})/', $in, $match);
    }

    $year = (int) $match[1];
    $month = (int) $match[2] - 1; // Month conversion between indexes
    $day = (int) $match[3];

    if ($type=='date'){
        return "Date($year, $month, $day)";
    } elseif ($type=='datetime'){
        $hours = (int) $match[4];
        $minutes = (int) $match[5];
        $seconds = (int) $match[6];
        return "Date($year, $month, $day, $hours, $minutes, $seconds)";    
    }
}

//declare variables for db connection
$servername = "<YOUR_DB_SERVER_NAME>";
$username = "<YOUR_DB_USER_NAME>";
$password = "<YOUR_DB_PASSWORD>";
$dbname = "<YOUR_DB_INSTANCE_NAME>";

//db connection
$con = new mysqli($servername, $username, $password, $dbname);

if ($con->connect_error) {
        die("Connection failed: " . $con->connect_error);
    }
    else
    {
        //echo ("Connect Successfully");
    }
    $query = "SELECT Time, diff/60 as diff FROM diff WHERE (Time >= (now() - interval 60 day)) ORDER BY Time ASC" ; 

    
    $aresult = $con->query($query);
    $number = mysqli_num_rows($aresult);

?>

<!DOCTYPE html>
<html>
<head>
    <script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>
    <script type="text/javascript">
        google.charts.load('current', {'packages':['corechart']});

        google.charts.setOnLoadCallback(drawChart);
        function drawChart(){
            var data = google.visualization.arrayToDataTable([
                [{type: 'datetime', label: 'Zeitpunkt'}, {type: 'number', label: 'Pausenzeit'}], 
                <?php
                    while($row = mysqli_fetch_assoc($aresult)){
                         echo "['".JSDate($row["Time"], datetime)."', ".$row["diff"]."]";
                     if (--$number > 0) {
                         echo ", ";
                      }
                    }
                ?>
               ]);
            
            var date_formatter = new google.visualization.DateFormat({pattern: "dd.MM.YYYY HH:mm:ss"});
            date_formatter.format(data, 0);

            var options = {
                title: 'Pausenzeiten in Stunden',
                curveType: 'function',
                legend: { position: 'bottom' },
                //height: 650,
                hAxis: {
                    format: 'dd.MM.YYYY'
                },
                vAxis: {
                    title: 'Pausenzeit in Stunden'
                },
                pointShape: 'circle',
                pointSize: 5,
                explorer: { 
                    actions: ['dragToZoom', 'rightClickToReset'],
                    axis: 'horizontal',
                    keepInBounds: true}
            };

            var chart = new google.visualization.LineChart(document.getElementById('areachart'));
            chart.draw(data, options);
        }

    </script>
</head>
<body>
     <div id="areachart"></div>
</body>
</html>