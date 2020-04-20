document.addEventListener('DOMContentLoaded', function () {
  // initialization of highcart object
  LogNormalChart = Highcharts.chart('lognormal', {
    chart: {
      renderTo: 'lognormal',
      height: 400, 
      width: 700,
      animation: {
        duration: 1000
      }
    },
    title: {
      text: 'Probability Based on Normal Distribution',
      align: 'left'
    },
    subtitle: {
      text: 'Probability Density',
      align: 'left'
    },
    xAxis: {
      title: {
        text: 'Stock Price'
      },
      labels: {
        align: 'center',
        format: '${value:.2f}'
      }
    },
    yAxis: {
      title: {
        text: 'Probability Density'
      },
      labels: {
        align: "center",
        formatter: function () {
          return this.value * 100 + "%";
        }
      },
      tickInterval: 0.05
    },
    plotOptions: {
      areaspline: {
        label: {
          enabled: false
        },
        zoneAxis: 'x' // zones along x axis
      }
    },
    legend: {
      enabled: false
    },
    exporting: {
      enabled: false
    },
    tooltip: {
      formatter: function () {
        // return 2 decimal formatted price on tool tip
        return '<b>$' + parseFloat(this.x).toFixed(2) + '<b>';
      }
    },
    series: [{
      type: 'areaspline'
    }]
  });
});