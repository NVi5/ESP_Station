function all_newest_data_call(){
  return $.ajax({
	url: "newest_data.php",
	dataType: "json"
  });
}

function previous_data_call(){
  return $.ajax({
	url: "previous_data.php",
	dataType: "json"
  });
}

function reload_current_values(){
  all_newest_data_call().done( function(data){
	
	//var date = 'OSTATNIA AKTUALIZACJA<div class="number">' + data.date + '</div>',
	var	temp = 'TEMPERATURE [°C]<div class="number">' + data.temp + '</div>',
		humi = 'HUMIDITY [%]<div class="number">' + data.humi + '</div>',
		pres = 'PRESSURE [hPa]<div class="number">' + data.pres + '</div>';
	
	//document.getElementById("top").innerHTML = date;
	document.getElementById("temp").innerHTML = temp;
	document.getElementById("humi").innerHTML = humi;
	document.getElementById("pres").innerHTML = pres;
  } );
}

function create_charts(chart_type){
  previous_data_call().done( function(data){
	
	var data_sql = [], row, full_name = 'Live ';
	
	switch(chart_type){
	    case 'temp':
	        full_name += 'Temperature';
	        break;
        case 'humi':
            full_name += 'Humidity';
            break;
        case 'pres':
            full_name += 'Pressure';
            break;
	    default:
	        full_name += "RANDOM";
	}
	
	full_name += ' Data';
	
	while( data.length > 0 ){
	  row = data.pop();
	  data_sql.push([
		Date.parse( row['date'] ), 
		parseFloat(row[chart_type])
	  ]);
	}
	
Highcharts.stockChart( 'chart_' + chart_type, {
  chart: {
    events: {
      load: function () {
        var series = this.series[0];
		setInterval( function(){
		  var x, y;
		  all_newest_data_call().done( function(data){
			x = Date.parse(data['date']);
			y = parseFloat(data[chart_type]);
		    series.addPoint( [ x, y ], true, true );
		  } );
		}, 5000 );
      }
    }
  },

  time: {
    useUTC: false
  },

  rangeSelector: {
    buttons: [{
      count: 60,
      type: 'minute',
      text: '1H'
    }, {
      count: 720,
      type: 'minute',
      text: '12H'
    }, {
      type: '2880',
      text: '2DAYS'
    }],
    inputEnabled: false,
    selected: 0
  },

  title: {
    text: full_name
  },

  exporting: {
    enabled: false
  },

  series: [{
    name: chart_type + ' data',
    data: ( function () {
      // generate an array of random data
      var data = [], row;
	  
	while( data_sql.length > 0 ){
	  row = data_sql.pop();
	  data.push([
		row['0'], 
		row['1']
	]);}

      return data;
    }())
  }]
});

  } );
}

$(function(){
  create_charts('temp');
  create_charts('humi');
  create_charts('pres');
  
  // set newest data and their refresh time
  setInterval( reload_current_values, 5000 );
  reload_current_values();
});