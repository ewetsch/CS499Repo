<head>
  <h1> ODDS Probability Calculator </h1>
  <script src="https://code.jquery.com/jquery-3.1.1.min.js"></script>
  <script src="https://code.highcharts.com/highcharts.js"></script>
  <script src="https://code.highcharts.com/modules/data.js"></script>
  <script src="https://code.highcharts.com/modules/exporting.js"></script>
  <script src="https://cdnjs.cloudflare.com/ajax/libs/PapaParse/5.1.0/papaparse.min.js"></script>
  <script type="text/javascript" src="./lognormal.js"></script>
  <script type="text/javascript" src="./optionprices.js"></script>
</head>

<body>
  <form name='user_submit' method="POST" class="user_form">
    <table>
      <tr>
        <th>Stock Ticker: <br> <input type='text' name='stock_ticker' id='ticker'> </th>
        <th>Target Price: <br> <input type='number' step="0.01" name='target_price' id='price'> </th>
        <th>Target Date: <br> <input type='date' name='target_date' id='date'> </th>
        <th>Submit: <br> <input type='button' onclick='test_func();runOptions()' value='Enter'> </th><br>
      </tr>

    </table>
  </form>

  <div id="demo"></div>

  <!-- tesing purposes -->
  <!-- <input type='button' onclick='temp()' value='Test'>
  <script>
    function temp() {
      
    }
  </script> -->

  <script>
    function test_func() {
      var ticker = document.getElementById('ticker').value;
      var price = document.getElementById('price').value;
      var date = document.getElementById('date').value;
      var formData = JSON.stringify(ticker + " " + price + " " + date)

      if (ticker == "" || price == "" || date == "") {
        alert("Please fill out all fields!")
        return false;
      }
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          response_hol = this.responseText;
          response_hol = response_hol.split("$PL1T");
          table_data = response_hol[1].replace(/,/g, ': ');
          table_data = table_data.split("\n");
          document.getElementById("Name").innerHTML = table_data[1];
          document.getElementById("ticker_data").innerHTML = table_data[2];
          document.getElementById("cur_price_data").innerHTML = table_data[3];
          document.getElementById("target_price_data").innerHTML = table_data[4];
          document.getElementById("volatility_data").innerHTML = table_data[5];
          document.getElementById("current_date_data").innerHTML = table_data[6];
          document.getElementById("targer_date_data").innerHTML = table_data[7];
          document.getElementById("prob_below_data").innerHTML = table_data[8];
          document.getElementById("prob_above_data").innerHTML = table_data[9];
          document.getElementById("LogNormData.csv").innerHTML = response_hol[0];
          //document.getElementById("demo").innerHTML = response_hol[1];
          console.log(table_data);

          // GRAPH CODE
          var parsed = Papa.parse(document.getElementById('LogNormData.csv').innerHTML);
          // sort to get each 20th line (in original data this is +/- sd)
          let indexes = parsed.data.filter((e, index) => {
            return index % 20 === 0;
          });
          // takes the x value of each x,y pair to serve as x-axis tick values
          var xticks = [];
          indexes.forEach(x => {
            // parsed from string to float so that highchart obj can round label vals
            xticks.push(parseFloat(x[0]));
          });
          //changes data from csv to array of arrays [[x,y],[x,y],...]
          var data = [];
          parsed.data.forEach(element => {
            var point = [];
            point.push(parseFloat(element[0]));
            point.push(parseFloat(element[1]));
            data.push(point);
          });
          LogNormalChart.update({
            plotOptions: {
              areaspline: {
                zones: [{
                  value: document.getElementById("price").value, //Target value
                  color: 'black', // color of line in zone 1
                  fillColor: '#c00000' // color of fill in zone 1
                }, {
                  color: 'black', // color of line in zone 2
                  fillColor: 'transparent' // color of fill in zone 2
                }]
              }
            }
          });
          //set the data
          LogNormalChart.series[0].setData(data);
          //set the xaxis
          LogNormalChart.xAxis[0].tickPositions = xticks;
          //redraw the xaxis
          LogNormalChart.xAxis[0].redraw();
        }
      };
      xhttp.open("POST", "/calculations/LogNorm.php");
      xhttp.send(formData);
    }
  </script>

  <script>
    function runOptions() {
      var ticker = document.getElementById('ticker').value;
      var price = document.getElementById('price').value;
      var date = document.getElementById('date').value;
      var formData = JSON.stringify(ticker + " " + price + " " + date)

      if (ticker == "" || price == "" || date == "") {
        alert("Please fill out all fields!")
        return false;
      }
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          response_hol = this.responseText;
          response_hol = response_hol.split("$PL1T");
          table_data = response_hol[1].replace(/,/g, ': ');
          table_data = table_data.split("\n");
          document.getElementById("NameO").innerHTML = table_data[1];
          document.getElementById("ticker_dataO").innerHTML = table_data[2];
          document.getElementById("cur_price_dataO").innerHTML = table_data[3];
          document.getElementById("target_price_dataO").innerHTML = table_data[4];
          document.getElementById("volatility_dataO").innerHTML = table_data[5];
          document.getElementById("current_date_dataO").innerHTML = table_data[6];
          document.getElementById("targer_date_dataO").innerHTML = table_data[7];
          document.getElementById("prob_below_dataO").innerHTML = table_data[8];
          document.getElementById("prob_above_dataO").innerHTML = table_data[9];
          document.getElementById("OptionPricesData.csv").innerHTML = response_hol[0];
          //document.getElementById("demo").innerHTML = response_hol[1];
          console.log(table_data);

          // GRAPH CODE
          var optionparsed = Papa.parse(document.getElementById('OptionPricesData.csv').innerHTML);
          var normalparsed = Papa.parse(document.getElementById('LogNormData.csv').innerHTML);
          console.log(optionparsed.data);
          let indexes = normalparsed.data.filter((e, index) => {
            return index % 20 === 0;
          });
          // takes the x value of each x,y pair to serve as x-axis tick values
          var xticks = [];
          indexes.forEach(x => {
            // parsed from string to float so that highchart obj can round label vals
            xticks.push(parseFloat(x[0]));
          });
          var model = [];
          var option = [];
          for (let i = 0; i < 162; i++) {
            var optionelement = optionparsed.data[i];
            var normalelement = normalparsed.data[i];
            optionpoint = [];
            modelpoint = [];
            optionpoint.push(parseFloat(normalelement[0]));
            optionpoint.push(parseFloat(optionelement[0]));
            modelpoint.push(parseFloat(normalelement[0]));
            modelpoint.push(parseFloat(normalelement[1]));
            option.push(optionpoint);
            model.push(modelpoint);
          }
          OptionPricesChart.update({
            plotOptions: {
              areaspline: {
                zones: [{
                  value: document.getElementById("price").value, //Target value
                  color: 'black', // color of line in zone 1
                  fillColor: '#c00000' // color of fill in zone 1
                }, {
                  color: 'black', // color of line in zone 2
                  fillColor: 'transparent' // color of fill in zone 2
                }]
              }
            }
          });
          //set the data
          OptionPricesChart.series[0].setData(option);
          OptionPricesChart.series[1].setData(model);
          //set the xaxis
          OptionPricesChart.xAxis[0].tickPositions = xticks;
          //redraw the xaxis
          OptionPricesChart.xAxis[0].redraw();
        }
      };
      xhttp.open("POST", "/calculations/Options.php");
      xhttp.send(formData);

    }
  </script>

  <style>
    table,
    th,
    td {
      border: 1px solid black;
      border-collapse: collapse;
    }

    th,
    td {
      padding: 5px;
      text-align: left;
    }
  </style>

  <!--LogNormal Display-->
  <table>
    <tr>
      <th colspan="8" id="Name"></th>
    </tr>
    <tr>
      <td id="ticker_data">
      <td id="cur_price_data">
    </tr>
    <tr>
      <td id="target_price_data">
      <td id="volatility_data">
    </tr>
    <tr>
      <td id="current_date_data">
      <td id="targer_date_data">
    </tr>
    <tr>
      <td id="prob_below_data">
      <td id="prob_above_data">
    </tr>

  </table>
  <div id="lognormal"></div>

  <!--Options Display-->
  <table>
    <tr>
      <th colspan="8" id="NameO"></th>
    </tr>
    <tr>
      <td id="ticker_dataO">
      <td id="cur_price_dataO">
    </tr>
    <tr>
      <td id="target_price_dataO">
      <td id="volatility_dataO">
    </tr>
    <tr>
      <td id="current_date_dataO">
      <td id="targer_date_dataO">
    </tr>
    <tr>
      <td id="prob_below_dataO">
      <td id="prob_above_dataO">
    </tr>

  </table>
  <div id="optionprices"></div>

</body>

<pre id="LogNormData.csv" style="display: none"></pre>
<pre id="OptionPricesData.csv" style="display: none"></pre>