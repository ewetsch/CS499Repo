<?php
$requestPayLoad = file_get_contents("php://input");
$parameters = json_decode($requestPayLoad);

$parameters = explode(" ", $parameters);

//Command-Line Arguments:
    //argv[0] --> Executable Name (not important)
    $exe = "./o";
    //argv[1] --> Back Door Debugger Tool (0, or 1)
    $debug_tool = "0";
    //argv[2] --> Ticker Symbol (e.g. "JPM")
    $ticker = $parameters[0];
    //argv[3] --> Current Price (decimal format e.g. "133.37")
            //$cur_price = "133.37";
    //argv[4] --> Target Price (decimal format e.g. "135")
    $tar_price = $parameters[1];
    //argv[5] --> Volatility (decimal format e.g. "0.2115")
        //$vol = "0.2115";
    //argv[6] --> Current Date (date format e.g. "2/3/2020")
            //$cur_date = "2/3/2020";
    //argv[7] --> Target Date (date format e.g. "3/20/2020")
    $tar_date = $parameters[2];
    //argv[8] --> Trading Days (integer format e.g. "33")
                //$trade_days = "33";
    //


//shell_exec("c 1 JPM 133.37 135 0.2115 2/3/2020 3/20/2020 33 2>/dev/null >/dev/null &");
//echo "This thing tried";

//$test = "./c 0 JPM 133.37 135 0.2115 2/3/2020 3/20/2020 33";
//$test = "./c 1 ".$ticker." 133.37 ".$price." 0.2115 2/3/2020 3/20/2020 33";
//$test = "./c 0 ".$ticker." 133.37 ".$tar_price." 0.2115 2/3/2020 ".$tar_date." 33";
$test = $exe." ".$debug_tool." ".$ticker." ".$tar_price." ".$tar_date;

$result=`$test`;

//$jamestest=`./c 0 JPM 133.37 135 0.2115 2/3/2020 3/20/2020 33`;

//echo "<hr>This is James Test<br>";
print_r($result);
?>